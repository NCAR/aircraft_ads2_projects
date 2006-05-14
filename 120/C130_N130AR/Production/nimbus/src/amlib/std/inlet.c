/*
-------------------------------------------------------------------------
OBJECT NAME:	inlet.c

FULL NAME:	Community Inlet

ENTRY POINTS:	sCItas()

STATIC FNS:	none

DESCRIPTION:	Routines for Community Inlet Parameters.

INPUT:		

OUTPUT:		

REFERENCES:	tas(), xmac2()

REFERENCED BY:	compute.c

COPYRIGHT:	University Corporation for Atmospheric Research, 1995
-------------------------------------------------------------------------
*/

#include "nimbus.h"
#include "amlib.h"

extern NR_TYPE xmac2();
extern NR_TYPE tas();
extern NR_TYPE recfra;

/* -------------------------------------------------------------------- */
void sCItas(varp)
DERTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	tt, qc, ps;

	qc = GetSample(varp, 0);
	ps = GetSample(varp, 1);	/* Static Pressure	*/
	tt = GetSample(varp, 2);	/* Total Temperature	*/

	PutSample(varp, tas(tt, recfra, xmac2(qc / ps)));

}	/* END SCITAS */

/* END INLET.C */
