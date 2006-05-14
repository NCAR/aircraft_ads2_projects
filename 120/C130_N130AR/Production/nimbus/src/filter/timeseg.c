/*
-------------------------------------------------------------------------
OBJECT NAME:	timeseg.c

FULL NAME:		Time Segment Routines

ENTRY POINTS:	GetUserTimeIntervals()		Used by StartProcesing()
				NextTimeInterval()			Used by StartProcesing()
				CheckForTimeGap()			lrloop.c & hrloop.c
				UpdateTime()				lrloop.c & hrloop.c
				FormatTimeSegmentsForOutputFile()	netcdf.c

STATIC FNS:		none

DESCRIPTION:	Functions should be used in the order shown above.
				User specified time intervals may not correspond to
				what is actually output (e.g. User requests 00:00:00 -
				15:00:00, and ADS actually started recording at 13:00:00).

INPUT:			struct Hdr_blk

OUTPUT:		

REFERENCES:		none

REFERENCED BY:	cb_main.c, oppo.c, mrf.c

COPYRIGHT:		University Corporation for Atmospheric Research, 1993
-------------------------------------------------------------------------
*/

#include <Xm/TextF.h>

#include "nimbus.h"
#include "decode.h"
#include "gui.h"
#include "header.h"

#define NEW_SEG		(-1)

static int	timeIndex[3];

static int	nTimeIntervals;

/* User specified time intervals, stored as int's	*/
static long	prev_time,
			UserBtim[MAX_TIME_SLICES*4],
			UserEtim[MAX_TIME_SLICES*4];

/* Actual time intervals in output file.			*/
static int	currentTimeSegment;
static int	BtimeInt[MAX_TIME_SLICES*4][3],
			EtimeInt[MAX_TIME_SLICES*4][3];


/* -------------------------------------------------------------------- */
void GetUserTimeIntervals() /* From TimeSliceWindow	*/
{
	int		i;
	int		hour, minute, second;
	char	*bp, *ep;

	nTimeIntervals = 0;
	currentTimeSegment = (-1);

	for (i = 0; i < MAX_TIME_SLICES; ++i)
		{
		bp = XmTextFieldGetString(ts_text[i]);
		ep = XmTextFieldGetString(ts_text[i+MAX_TIME_SLICES]);

		if (strlen(bp) == 0 && strlen(ep) == 0)
			continue;

		sscanf(bp, "%02d:%02d:%02d", &hour, &minute, &second);
		UserBtim[nTimeIntervals] = (hour * 3600) + (minute * 60) + second;

		sscanf(ep, "%02d:%02d:%02d", &hour, &minute, &second);
		UserEtim[nTimeIntervals] = (hour * 3600) + (minute * 60) + second;

		++nTimeIntervals;
		}

	if (nTimeIntervals == 0)
		{
		UserBtim[0] = BEG_OF_TAPE;
		UserEtim[0] = END_OF_TAPE;

		++nTimeIntervals;
		}

	/* And while we're here let's get the indices for the time variables,
	 * HOUR, MINUTE, SECOND, used by UpdateTime().
	 */
	timeIndex[0] = raw[SearchTable((char **)raw, nraw, "HOUR")]->SRstart;
	timeIndex[1] = raw[SearchTable((char **)raw, nraw, "MINUTE")]->SRstart;
	timeIndex[2] = raw[SearchTable((char **)raw, nraw, "SECOND")]->SRstart;

}	/* END GETUSERTIMEINTERVALS */

/* -------------------------------------------------------------------- */
NextTimeInterval(long *start, long *end)
{
	if (++currentTimeSegment >= nTimeIntervals)
		return(FALSE);

	prev_time = NEW_SEG;

	*start	= UserBtim[currentTimeSegment];
	*end	= UserEtim[currentTimeSegment];

	BtimeInt[currentTimeSegment][0] = NEW_SEG;

	return(TRUE);

}	/* END NEXTTIMEINTERVAL */

/* -------------------------------------------------------------------- */
CheckForTimeGap(
	struct Hdr_blk	*ADShdr,
	int				initMode)
{
	long	i, j;
	long	new_time;


	new_time = (long)HdrBlkTimeToSeconds(ADShdr);

	/* If everthing is peachy, then bail out
	 */
	if (prev_time+1 == new_time || prev_time == NEW_SEG ||
		(new_time == 0 && prev_time == 86399L))		/* Midnight crossover */
		{
		prev_time = new_time;
		return(FALSE);
		}


	if (prev_time == new_time)
		{
		sprintf(buffer,
			"Duplicate time stamp @ %02d:%02d:%02d, processing anyways.\n",
			ADShdr->hour, ADShdr->minute, ADShdr->sec);

		LogMessage(buffer);
		return(FALSE);
		}


	/* Locate current time segment
	 */
	for (i = 0; i < nTimeIntervals; ++i)
		{
		if (prev_time > UserBtim[i] && prev_time <= UserEtim[i])
			break;

		if (UserEtim[i] == END_OF_TAPE)
			break;
		}

	if (i == nTimeIntervals)
		{
		LogMessage("Impossible, no time segment currently active.\n");
		return(GAP_FOUND);
		}


	/* Insert new time segment.
	 */
	if (new_time <= UserEtim[i] || UserEtim[i] == END_OF_TAPE)
		{
		LogMessage("Break in time sequence, ");

		if (initMode)
			{
			LogMessage("moving up start time.\n");
			--currentTimeSegment;
			UserBtim[i] = new_time + 2;
			return(GAP_FOUND);
			}

		LogMessage("adding new time interval.\n");

		for (j = nTimeIntervals; j > i; --j)
			{
			UserEtim[j] = UserEtim[j-1];
			UserBtim[j] = UserBtim[j-1];
			}

		UserEtim[i] = prev_time;
		UserBtim[i+1] = new_time;

		if (ProcessingRate == LOW_RATE)
			UserBtim[i+1] += 2;
		else
			UserBtim[i+1] += 14;

		++nTimeIntervals;

		return(GAP_FOUND);
		}


	LogMessage("CheckForTimeGap: you shouldn't receive this message.\n");
	return(GAP_FOUND);

}	/* END CHECKFORTIMEGAP */

/* -------------------------------------------------------------------- */
void UpdateTime(NR_TYPE *record)
{
	int		hour = record[timeIndex[0]],
			minute = record[timeIndex[1]],
			second = record[timeIndex[2]];

	if (BtimeInt[currentTimeSegment][0] == NEW_SEG)
		{
		BtimeInt[currentTimeSegment][0] = hour;
		BtimeInt[currentTimeSegment][1] = minute;
		BtimeInt[currentTimeSegment][2] = second;
		}

	EtimeInt[currentTimeSegment][0] = hour;
	EtimeInt[currentTimeSegment][1] = minute;
	EtimeInt[currentTimeSegment][2] = second;

	if (Interactive && second == 0)
		{
		sprintf(buffer, "%02d:%02d:00", hour, minute);
		XmTextFieldSetString(timeDisplayText, buffer);
		FlushXEvents();
		}

}	/* END UPDATETIME */

/* -------------------------------------------------------------------- */
void FormatTimeSegmentsForOutputFile(char *buff)
{
	int		i;
	char	temp[32];

	buff[0] = '\0';

	for (i = 0; i < nTimeIntervals; ++i)
		{
		if (i > 0)
			strcat(buff, ", ");

		sprintf(temp, "%02d:%02d:%02d-%02d:%02d:%02d",
			BtimeInt[i][0], BtimeInt[i][1], BtimeInt[i][2],
			EtimeInt[i][0], EtimeInt[i][1], EtimeInt[i][2]);

		strcat(buff, temp);
		}

}	/* END FORMATTIMESEGMENTSFOROUTPUTFILE */

/* -------------------------------------------------------------------- */
GetPreviousTime()
{
	return(prev_time);

}	/* END GETPREVIOUSTIME */

/* END TIMESEG.C */
