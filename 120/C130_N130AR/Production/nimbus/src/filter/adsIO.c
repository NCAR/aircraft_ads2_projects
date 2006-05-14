/*
-------------------------------------------------------------------------
OBJECT NAME:	adsIO.c

FULL NAME:		ADS Record IO routines

ENTRY POINTS:	FindFirstLogicalRecord(char *record, long starttime)
				FindNextLogicalRecord(char *record, long endtime)
				FindNextDataRecord(char *record)

STATIC FNS:		none

DESCRIPTION:	These routines locate data records that start with the
				ID = 0x8681.  (i.e. skips all PMS2D records).

INPUT:			Pointer to where to place the record
				Time of first record desired / Time last record desired

OUTPUT:			Length of record or ERR

REFERENCES:		tapeIO.c

REFERENCED BY:	lrloop.c, hrloop.c

COPYRIGHT:		University Corporation for Atmospheric Research, 1992
-------------------------------------------------------------------------
*/

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "nimbus.h"
#include "decode.h"
#include "ctape.h"
#include "raf.h"

#define ADS_WORD	0x4144
#define HDR_WORD	0x5448
#define SDI_WORD	SDIWRD

#define FIRST_DATA_RECORD	((long)3)

static char	phys_rec[MX_PHYS] = "";
static long	lrlen, lrppr, currentLR;

extern int	infd;
extern long	LITTON51_start;

static long		FindNextDataRecord(char buff[]);


/* -------------------------------------------------------------------- */
long FindFirstLogicalRecord(
	char	record[],	/* First Data Record, for start time	*/
	long	starttime)	/* User specified start time		*/
{
	long		rectime, nbytes;
	static int	firstTime = TRUE;

	if (firstTime)
		{
		firstTime = FALSE;

		get_lrlen(&lrlen);
		get_lrppr(&lrppr);

		if (!DiskData)
			TapeAssign(infd);

		if ((DiskData ? lseek(infd, 0L, SEEK_SET)
					  : TapeSeek(FIRST_DATA_RECORD)) == ERR)
			return(ERR);

		if ((nbytes = FindNextDataRecord(phys_rec)) <= 0)
			return(nbytes);

		if (starttime == BEG_OF_TAPE)
			return(FindNextLogicalRecord(record, starttime));
		}
	else
		currentLR = 0;


	rectime = (long)HdrBlkTimeToSeconds((struct Hdr_blk  *)phys_rec);

	/*		     12:00:00			   23:59:59	*/
	if (rectime < 43200L && starttime > 86399L)
		rectime += 86399L;

	while (starttime > (rectime + lrppr))
		{
		if ((nbytes = FindNextDataRecord(phys_rec)) <= 0)
			return(nbytes);

		rectime = HdrBlkTimeToSeconds((struct Hdr_blk  *)phys_rec);

		/*	     12:00:00		   23:59:59	*/
		if (rectime < 43200L && starttime > 86399L)
			rectime += 86399L;

		FlushXEvents();
		}


	/* Cover the case if start time is before first record on file
	 */
	if (starttime < rectime)
		starttime = rectime;


	while ((nbytes = FindNextLogicalRecord(record, starttime)) > 0)
		{
		rectime = HdrBlkTimeToSeconds((struct Hdr_blk *)record);

		/*	     12:00:00		   23:59:59	*/
		if (rectime < 43200L && starttime > 86399L)
			rectime += 86399L;

		if (rectime >= starttime)
			break;
		}

	return(nbytes);

}	/* END FINDFIRSTLOGICALRECORD */

/* -------------------------------------------------------------------- */
long FindNextLogicalRecord(
	char	record[],
	long	endtime)
{
	int	nbytes, rectime;

	rectime = HdrBlkTimeToSeconds(&phys_rec[currentLR * lrlen]);

	if (endtime != END_OF_TAPE)
		{
		/*	     12:00:00		 23:59:59	*/
		if (rectime < 43200L && endtime > 86399L)
			rectime += 86399L;

		if (rectime > endtime)
			return(0);	/* End Of Time Segment	*/
		}

	memcpy(record, &phys_rec[currentLR * lrlen], lrlen);

	if (++currentLR >= lrppr)
		{
		if ((nbytes = FindNextDataRecord(phys_rec)) <= 0)
			return(nbytes);

		currentLR = 0;
		}

	/* Lag the Litton 51 INS one second.
	 */
	if (LITTON51_present)
		memcpy( &record[LITTON51_start],
			&phys_rec[currentLR * lrlen + LITTON51_start],
			sizeof(struct Ins_blk));

	return(lrlen);

}	/* END FINDNEXTLOGICALRECORD */

/* -------------------------------------------------------------------- */
#define ONE_WORD	sizeof(short)

static long FindNextDataRecord(char buff[])
{
	long	nbytes;

	do
		{
		if (DiskData)
			{
			long	len;
			int		size = ONE_WORD;

			if ((nbytes = read(infd, buff, size)) == 0 && GetNextADSfile())
				nbytes = read(infd, buff, size);

			if (nbytes <= 0)
				break;

			switch (*((ushort *)buff))
				{
				case SDI_WORD:
					size = lrppr * lrlen - ONE_WORD;
					break;

				case ADS_WORD:
					size = 18;
					break;

				case HDR_WORD:
					get_thdrlen(&len);
					size = len - ONE_WORD;
					break;

				case PMS2DC1: case PMS2DC2: /* PMS2D */
				case PMS2DP1: case PMS2DP2:
				case PMS2DH1: case PMS2DH2:	/* HVPS */
					size = PMS2_RECSIZE - ONE_WORD;
					break;

				case PMS2DG1: case PMS2DG2: /* GrayScale */
					size = 32000;
					break;

				default:
					size = 0;
				}

			nbytes += read(infd, &buff[ONE_WORD], size);
			}
		else
			nbytes = TapeRead(buff);

		if (AsyncFileEnabled && IsThisAnAsyncRecord(buff))
			WriteAsyncData(buff);
		}
	while (nbytes > 0 && *((ushort *)buff) != SDI_WORD);

	currentLR = 0;
	return(nbytes);

}	/* END FINDNEXTDATARECORD */

/* -------------------------------------------------------------------- */
IsThisAnAsyncRecord(short buff[])
{
	if (buff[0] == PMS2DC1 || buff[0] == PMS2DC2 ||
		buff[0] == PMS2DP1 || buff[0] == PMS2DP2 ||
		buff[0] == PMS2DG1 || buff[0] == PMS2DG2 ||
		buff[0] == PMS2DH1 || buff[0] == PMS2DH2)
		return(TRUE);
	else
		return(FALSE);

}	/* END ISTHISANASYNCRECORD */

/* END RECORDIO.C */
