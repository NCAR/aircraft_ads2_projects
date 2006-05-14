/*
-------------------------------------------------------------------------
OBJECT NAME:	palt.c

FULL NAME:	NACA pressure altitude (M)

ENTRY POINTS:	spalt()
		spaltf()

STATIC FNS:	none

DESCRIPTION:	

INPUT:		

OUTPUT:		

REFERENCES:	none

REFERENCED BY:	compute.c

COPYRIGHT:	University Corporation for Atmospheric Research, 1992
-------------------------------------------------------------------------
*/

#include <math.h>
#include "nimbus.h"
#include "amlib.h"

#define ASTG	1013.246 /* Default altimeter setting (mb)		*/
#define SFCT	288.15	 /* Default mean surface temperature setting (K)*/

/* -------------------------------------------------------------------- */
void spalt(varp)
DERTBL	*varp;
{
	NR_TYPE	psxc, palt;

	psxc = GetSample(varp, 0);

	palt = 153.77 * SFCT *
		(1.0 - pow((double)psxc / ASTG, (double)0.190284));

	PutSample(varp, palt);

}	/* END SPALT */

/* -------------------------------------------------------------------- */
void spaltf(varp)
DERTBL	*varp;
{
	PutSample(varp, GetSample(varp, 0) * 3.2808399);

}	/* END SPALTF */

/* END PALT.C */
