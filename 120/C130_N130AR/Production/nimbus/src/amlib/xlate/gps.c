/*
-------------------------------------------------------------------------
OBJECT NAME:	gps.c

FULL NAME:	Translate Trimble GPS block

ENTRY POINTS:	xlgstat(), xlglon(), xlglat(), xlgalt(), xlgtimp(),
		xlgvew(), xlgvns(), xlgvzi(), xlgtimv, xlgmode()

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


#define ONE_EIGHTY_OVER_PI	57.29577951

/* -------------------------------------------------------------------- */
void xlgstat(varp, input, output)
RAWTBL		*varp;
struct Gps_blk	*input;
NR_TYPE		*output;
{
	*output = (NR_TYPE)input->ghealth;
}

/* -------------------------------------------------------------------- */
void xlglat(varp, input, output)
RAWTBL		*varp;
struct Gps_blk	*input;
NR_TYPE		*output;
{
	*output = (NR_TYPE)input->glat * ONE_EIGHTY_OVER_PI;
}

/* -------------------------------------------------------------------- */
void xlglon(varp, input, output)
RAWTBL		*varp;
struct Gps_blk	*input;
NR_TYPE		*output;
{
	*output = (NR_TYPE)input->glon * ONE_EIGHTY_OVER_PI;
}

/* -------------------------------------------------------------------- */
void xlgalt(varp, input, output)
RAWTBL		*varp;
struct Gps_blk	*input;
NR_TYPE		*output;
{
	*output = (NR_TYPE)input->galt;
}

/* -------------------------------------------------------------------- */
void xlgtimp(varp, input, output)
RAWTBL		*varp;
struct Gps_blk	*input;
NR_TYPE		*output;
{
	*output = (NR_TYPE)input->postime;
}

/* -------------------------------------------------------------------- */
void xlgvew(varp, input, output)
RAWTBL		*varp;
struct Gps_blk	*input;
NR_TYPE		*output;
{
	*output = (NR_TYPE)input->veleast;
}

/* -------------------------------------------------------------------- */
void xlgvns(varp, input, output)
RAWTBL		*varp;
struct Gps_blk	*input;
NR_TYPE		*output;
{
	*output = (NR_TYPE)input->velnrth;
}

/* -------------------------------------------------------------------- */
void xlgvzi(varp, input, output)
RAWTBL		*varp;
struct Gps_blk	*input;
NR_TYPE		*output;
{
	*output = (NR_TYPE)input->velup;
}

/* -------------------------------------------------------------------- */
void xlgtimv(varp, input, output)
RAWTBL		*varp;
struct Gps_blk	*input;
NR_TYPE		*output;
{
	*output = (NR_TYPE)input->veltime;
}

/* -------------------------------------------------------------------- */
void xlgmode(varp, input, output)
RAWTBL		*varp;
struct Gps_blk	*input;
NR_TYPE		*output;
{
	*output = (NR_TYPE)input->gpsmode;
}

/* END GPS.C */
