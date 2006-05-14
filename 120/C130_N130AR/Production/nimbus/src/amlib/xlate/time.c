/*
-------------------------------------------------------------------------
OBJECT NAME:	time.c

FULL NAME:	Translate time to new record format

ENTRY POINTS:	xlhour(), xlmin(), xlsec()

STATIC FNS:	none

DESCRIPTION:	

INPUT:		

OUTPUT:		

REFERENCES:	none

REFERENCED BY:	rec_decode.c

COPYRIGHT:	University Corporation for Atmospheric Research, 1992
-------------------------------------------------------------------------
*/

#include "nimbus.h"
#include "amlib.h"


/* -------------------------------------------------------------------- */
void xlhour(varp, input, output)
RAWTBL		*varp;
struct Hdr_blk	*input;
NR_TYPE		*output;
{
	*output = (NR_TYPE)input->hour;

}	/* END XLHOUR */

/* -------------------------------------------------------------------- */
void xlmin(varp, input, output)
RAWTBL		*varp;
struct Hdr_blk	*input;
NR_TYPE		*output;
{
	*output = (NR_TYPE)input->minute;

}	/* END XLMIN */

/* -------------------------------------------------------------------- */
void xlsec(varp, input, output)
RAWTBL		*varp;
struct Hdr_blk	*input;
NR_TYPE		*output;
{
	*output = (NR_TYPE)input->sec;

}	/* END XLSEC */

/* END TIME.C */
