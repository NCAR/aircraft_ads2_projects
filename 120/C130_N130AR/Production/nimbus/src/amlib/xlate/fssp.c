/*
-------------------------------------------------------------------------
OBJECT NAME:	fssp.c

FULL NAME:	Translate PMS1D to new record format (including Version 2)

ENTRY POINTS:	xlfreset(), xlfrange(), xlfstrob()

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
void xlfreset(varp, p, np)
RAWTBL		*varp;
struct P16_blk	*p;
NR_TYPE		*np;
{
	register	i;

	for (i = 0; i < varp->SampleRate; ++i)
		np[i] = p[i].aux[3];

}	/* END XLFRESET */

/* -------------------------------------------------------------------- */
void xlfrange(varp, p, np)
RAWTBL		*varp;
struct P16_blk	*p;
NR_TYPE		*np;
{
	register	i;

	for (i = 0; i < varp->SampleRate; ++i)
		if ((np[i] = p[i].rng_flg) < 0.0 || np[i] > 3.0)
			np[i] = 0.0;

}	/* END XLFRANGE */

/* -------------------------------------------------------------------- */
void xlfrange2(varp, p, np)
RAWTBL			*varp;
struct P16v2_blk	*p;
NR_TYPE			*np;
{
	register	i;

	for (i = 0; i < varp->SampleRate; ++i)
		if ((np[i] = p[i].rng_flg) < 0.0 || np[i] > 3.0)
			np[i] = 0.0;

}	/* END XLFRANGE2 */

/* -------------------------------------------------------------------- */
void xlfstrob(varp, p, np)
RAWTBL		*varp;
struct P16_blk	*p;
NR_TYPE		*np;
{
	register	i;

	for (i = 0; i < varp->SampleRate; ++i)
		np[i] = p[i].aux[2];

}	/* END XLFSTROB */

/* END FSSP.C */
