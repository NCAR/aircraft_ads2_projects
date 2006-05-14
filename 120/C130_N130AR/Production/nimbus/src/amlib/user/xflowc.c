/*
-------------------------------------------------------------------------
OBJECT NAME:	xflowc.c

FULL NAME:	CVI External Sampler Flow Rate (vlpm)

ENTRY POINTS:	sxflowc()

STATIC FNS:	none

DESCRIPTION:	

INPUT:		

OUTPUT:		

REFERENCES:	none

REFERENCED BY:	compute.c

COPYRIGHT:	University Corporation for Atmospheric Research, 1992
-------------------------------------------------------------------------
*/

#include "nimbus.h"
#include "amlib.h"


/* -------------------------------------------------------------------- */
void sxflowc(varp)
DERTBL	*varp;
{
	NR_TYPE	xflow, psxc, atx, xflowc;

	xflow	= GetSample(varp, 0);
	psxc	= GetSample(varp, 1);
	atx	= GetSample(varp, 2);

	if (psxc <= 0.0)
		psxc = 0.0001;

	xflowc = xflow * (1013.25 / psxc) * ((atx + 273.15) / 294.26);

	PutSample(varp, xflowc);

}	/* END SXFLOWC */

/* END XFLOWC.C */
