/*
-------------------------------------------------------------------------
OBJECT NAME:	pms1_ps.c

FULL NAME:	Translate PMS1D Particle Spacing to new record format

ENTRY POINTS:	xlpartspace()

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
void xlpartspace(varp, p, np)
RAWTBL			*varp;
struct P1dSpc_blk	*p;
NR_TYPE			*np;
{
	register	i, j;

	for (i = 0; i < varp->SampleRate; ++i)
		for (j = 0; j < varp->Length; ++j)
			np[(i*varp->Length)+j] = (ushort)p[i].bins[j];

}	/* END XLPARTSPACE */

/* END PMS1_PS.C */
