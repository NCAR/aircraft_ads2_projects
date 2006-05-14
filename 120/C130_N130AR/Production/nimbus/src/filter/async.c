/*
-------------------------------------------------------------------------
OBJECT NAME:	async.c

FULL NAME:		NetCDF IO

ENTRY POINTS:	InitAsyncModule()
				WriteAsyncData()

STATIC FNS:		locateAsyncVar()
				flush2dQueue()

DESCRIPTION:	Writes the async data.  A queueing method was chosen since
				2d data can be early, late, and the phys records will have
				C, P, & H interspersed.  We need it all sorted out, so one
				q exists for each probe/variable.  The Async file itself is
				nothing more than a heap file, with the variables in the
				netCDF file providing startPositions, record lengths, and
				count of sequential records for that second.

INPUT:			Any ADS non-synchronous record

OUTPUT:			none

REFERENCES:		queue.c

REFERENCED BY:	adsIO.c

COPYRIGHT:		University Corporation for Atmospheric Research, 1995
-------------------------------------------------------------------------
*/

#include <errno.h>
#include <stdlib.h>
#include <time.h>

#include "nimbus.h"
#include "decode.h"
#include "netcdf.h"
#include "header.h"
#include "raf.h"
#include "queue.h"

#define MAX_ASYNC_VARS	32

static FILE		*AsyncFP;

static RAWTBL	*AsyncVar[MAX_ASYNC_VARS];
static Queue	*AsyncQueue[MAX_ASYNC_VARS];

static void flush2dQueue(int indx, int syncTime);
static locateAsyncVar(ushort record[]);

static int ncid, recDim;

/* -------------------------------------------------------------------- */
void InitAsyncModule(char fileName[])
{
	int		i, cnt = 0;
	char	tempName[512];

	for (i = 0; i < nraw; ++i)
		{
		if (cnt == MAX_ASYNC_VARS)
			{
			fprintf(stderr,
				"async.c: MAX_ASYNC_VARS exceeded, fix and recompile.\n");
			exit(1);
			}

		if (raw[i]->ProbeType & PROBE_PMS2D)
			{
			AsyncVar[cnt] = raw[i];
			AsyncQueue[cnt++] = CreateQueue();
			}
		}

	AsyncVar[cnt] = NULL;


	strcpy(tempName, fileName);
	strcpy(strchr(tempName, '.')+1, "async");

	if ((AsyncFP = fopen(tempName, "w+")) == NULL)
		{
		sprintf(buffer, "Async: Unable to destroy/create %s.\n", tempName);
		LogMessage(buffer);
		LogMessage("Disabling Async data processing, and continuing.\n");
		AsyncFileEnabled = FALSE;
		}

}	/* END INITASYNCMODULE */

/* -------------------------------------------------------------------- */
void WriteAsyncData(char record[])
{
	P2d_rec	*p2dp;
	void	*dp;
	int		i, indx, p2d_time, syncTime;

	syncTime = GetPreviousTime();

	switch (*((ushort *)record))
		{
		case PMS2DC1: case PMS2DC2: /* PMS2D */
		case PMS2DP1: case PMS2DP2:
		case PMS2DH1: case PMS2DH2: /* HVPS */

			for (i = 0; i < P2DLRPR; ++i)
				{
				p2dp = (P2d_rec *)&record[i * sizeof(P2d_rec)];

				if ((indx = locateAsyncVar((ushort *)p2dp)) == ERR)
					continue;

				dp = (void *)GetMemory(sizeof(P2d_rec));
				memcpy((char *)dp, (char *)p2dp, sizeof(P2d_rec));
				EnQueue(AsyncQueue[indx], dp);

				/* ok, lets see if we are ready to flush the data.
				 */
				p2d_time = (long)HdrBlkTimeToSeconds(p2dp);

				/* Add 2 sec in case of early data (2d clock can walk)
				 */
				if (p2d_time > (syncTime+2))
					flush2dQueue(indx, syncTime);
				}

			break;

		case PMS2DG1: case PMS2DG2:
			break;
		}

}	/* END WRITEASYNCDATA */

/* -------------------------------------------------------------------- */
static void flush2dQueue(int indx, int syncTime)
{
	int		first_p2d_time, p2d_time, startPos, nRecords, diff;
	NR_TYPE	info[3];
	P2d_rec	*p2dp;
	RAWTBL	*rp = AsyncVar[indx];
	Queue	*q = AsyncQueue[indx];

	static long		start[] = {0, 0};	/* for netCDF ncvarput/get	*/
	static long		count[] = {1, 3};

	SyncNetCDF();
	ncdiminq(ncid, recDim, (char *)NULL, &nRecords);

	startPos = ftell(AsyncFP);

	if ((p2dp = FrontQueue(q)) == NULL)
		return;

	p2d_time = first_p2d_time = (long)HdrBlkTimeToSeconds(p2dp);

	diff = syncTime - p2d_time;
	start[0] = nRecords - 1 - diff;
	ncvarget(ncid, rp->varid, start, count, (void *)info);

	do
		{
		fwrite(p2dp, sizeof(P2d_rec), 1, AsyncFP);

		if (info[0] == 0)
			info[0] = startPos, info[1] = sizeof(P2d_rec);

		info[2]++;

		DeQueue(q);
		p2dp = FrontQueue(q);
		p2d_time = (long)HdrBlkTimeToSeconds(p2dp);
		}
	while (first_p2d_time == p2d_time);

	ncvarput(ncid, rp->varid, start, count, (void *)info);

}	/* END FLUSHQUEUE */

/* -------------------------------------------------------------------- */
static locateAsyncVar(ushort record[])
{
	int		indx = 0, probeCnt;
	char	target[NAMELEN];

	switch (record[0])
		{
		case PMS2DC1:
			strcpy(target, "2D-C");
			probeCnt = 0;
			break;

		case PMS2DC2:
			strcpy(target, "2D-C");
			probeCnt = 1;
			break;

		case PMS2DP1:
			strcpy(target, "2D-P");
			probeCnt = 0;
			break;

		case PMS2DP2:
			strcpy(target, "2D-P");
			probeCnt = 1;
			break;

		case PMS2DH1:
			strcpy(target, "2D-H");
			probeCnt = 0;
			break;

		case PMS2DH2:
			strcpy(target, "2D-H");
			probeCnt = 1;
			break;

		default:
			return(ERR);
		}


	for (indx = 0; AsyncVar[indx]; ++indx)
		{
		if (strncmp(AsyncVar[indx]->name, target, strlen(target)) == 0 &&
			AsyncVar[indx]->ProbeCount == probeCnt)
			return(indx);
		}

	return(ERR);

}	/* END LOCATEASYNCVAR */

/* END ASYNC.C */
