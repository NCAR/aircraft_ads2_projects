/*
-------------------------------------------------------------------------
OBJECT NAME:	globals.c

FULL NAME:		Global Variable Definitions

ENTRY POINTS:	none

DESCRIPTION:	

INPUT:			none

OUTPUT:			none

REFERENCES:		none

REFERENCED BY:	none

NOTE:			Globals also exist in nimbus.c, memalloc.c & sh_mem.c.

COPYRIGHT:		University Corporation for Atmospheric Research, 1992
-------------------------------------------------------------------------
*/

#include "nimbus.h"
#include "decode.h"

char	buffer[4096];			/* Generic, volatile string space		*/
char	*ProjectDirectory, *ProjectNumber, *ProjectName;

SDITBL  *sdi[MAX_SDI];			/* SDI (aka raw) variable list	*/
RAWTBL	*raw[MAX_RAW];			/* Alphabeticly sorted pointers	*/
DERTBL	*derived[MAX_DERIVE];		/* Alphabeticly sorted pointers	*/
DERTBL	*ComputeOrder[MAX_DERIVE];	/* Compute Order for aindx	*/
DEFAULT	*Defaults[MAX_DEFAULTS];

int		nsdi, nraw, nderive, nDefaults;

bool	DiskData,				/* cb_main.c & adsIO.c		*/
		LITTON51_present,		/* hdr_decode.c & adsIO.c	*/
		PauseFlag,
		AsyncFileEnabled,
		LoadProductionSetupFile,
		Interactive,
		ProductionRun,
		QCenabled;

int		Mode,			/* RealTime or PostProcessing (winput or nimbus)	*/
		ProcessingRate,
		FeedBack, SampleOffset,	/* Used by amlib fns, set in hrloop.c	*/
		PauseWhatToDo,
		Aircraft, FlightNumber,
		infd;					/* cb_main.c & adsIO.c		*/

/* Data record pointers
 */
char	*ADSrecord;
ushort	*bits;
NR_TYPE	*volts;
NR_TYPE	*SampledData, *AveragedData, *HighRateData;

long	nFloats;	/* Contains number of floats used in New Record		*/
long	LITTON51_start;		/* hdr_decode.c & adsIO.c		*/

/* END GLOBALS.C */
