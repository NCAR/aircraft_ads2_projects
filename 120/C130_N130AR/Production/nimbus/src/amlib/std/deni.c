/*
-------------------------------------------------------------------------
OBJECT NAME:	deni.c

FULL NAME:	

ENTRY POINTS:	deiInit()
		sdei()
		sdni()

STATIC FNS:	none

DESCRIPTION:	Compute distance north and east of the initial latitude
		and longitude in km.

INPUT:		

OUTPUT:		

REFERENCES:	

REFERENCED BY:	compute.c

COPYRIGHT:	University Corporation for Atmospheric Research, 1993
-------------------------------------------------------------------------
*/

#include <math.h>
#include "nimbus.h"
#include "amlib.h"

#define RAD	0.01745329

static NR_TYPE startlat, startlon;

/* -------------------------------------------------------------------- */
void deiInit()
{
	startlat = (GetDefaultsValue("DENI_START_LAT"))[0];
	startlon = (GetDefaultsValue("DENI_START_LON"))[0];

}	/* END DEIINIT */

/* -------------------------------------------------------------------- */
void sdei(varp)
DERTBL	*varp;
{
	NR_TYPE		lat, lon;

	lat = GetSample(varp, 0);
	lon = GetSample(varp, 1);

	PutSample(varp, (lon - startlon) * 111.17 * cos((double)(RAD*lat)));

}	/* END SDEI */

/* -------------------------------------------------------------------- */
void sdni(varp)
DERTBL	*varp;
{
	NR_TYPE	lat;

	lat = GetSample(varp, 0);

	PutSample(varp, (lat - startlat) * 111.17);

}	/* END SDNI */

/* END DENI.C */
