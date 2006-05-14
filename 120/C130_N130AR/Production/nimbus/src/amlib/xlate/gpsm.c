/*
-------------------------------------------------------------------------
OBJECT NAME:	gpsm.c

FULL NAME:	Translate Magnavox GPS block

ENTRY POINTS:	xlgmtime()

DESCRIPTION:	

INPUT:		GPS block pointer

OUTPUT:		

REFERENCES:	none

REFERENCED BY:	rec_decode.c

COPYRIGHT:	University Corporation for Atmospheric Research, 1992
-------------------------------------------------------------------------
*/

#include "nimbus.h"
#include "amlib.h"


/* -------------------------------------------------------------------- */
void xlgmtime(varp, gpstime, output)
RAWTBL	*varp;
char	*gpstime;
NR_TYPE	*output;
{
	*output = (10000 * (gpstime[0] * 10 + gpstime[1])) +
		  (100 * (gpstime[2] * 10 + gpstime[3])) +
		  (gpstime[4] * 10 + gpstime[5]);
}

/* -------------------------------------------------------------------- */
void xlgmlat(varp, input, output)
RAWTBL			*varp;
struct GpsMag_blk	*input;
NR_TYPE			*output;
{
        *output = (NR_TYPE)input->glat;
}

/* -------------------------------------------------------------------- */
void xlgmlon(varp, input, output)
RAWTBL			*varp;
struct GpsMag_blk	*input;
NR_TYPE			*output;
{
        *output = (NR_TYPE)input->glon;
}

/* -------------------------------------------------------------------- */
void xlgmalt(varp, input, output)
RAWTBL			*varp;
struct GpsMag_blk	*input;
NR_TYPE			*output;
{
        *output = (NR_TYPE)input->galt;
}

/* -------------------------------------------------------------------- */
void xlgmgeoh(varp, input, output)
RAWTBL			*varp;
struct GpsMag_blk	*input;
NR_TYPE			*output;
{
        *output = (NR_TYPE)input->geo_height;
}

/* -------------------------------------------------------------------- */
void xlgmvew(varp, input, output)
RAWTBL			*varp;
struct GpsMag_blk	*input;
NR_TYPE			*output;
{
        *output = (NR_TYPE)input->veleast;
}

/* -------------------------------------------------------------------- */
void xlgmvns(varp, input, output)
RAWTBL			*varp;
struct GpsMag_blk	*input;
NR_TYPE			*output;
{
        *output = (NR_TYPE)input->velnrth;
}

/* -------------------------------------------------------------------- */
void xlgmmode(varp, input, output)
RAWTBL			*varp;
struct GpsMag_blk	*input;
NR_TYPE			*output;
{
        *output = (NR_TYPE)input->gpsmode;
}

/* END GPSM.C */
