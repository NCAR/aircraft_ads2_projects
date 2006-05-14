/*
-------------------------------------------------------------------------
OBJECT NAME:	akrd.c

FULL NAME:	Attack Angle of the Radome

ENTRY POINTS:	sakrd()

STATIC FNS:	none

DESCRIPTION:	

INPUT:		

OUTPUT:		

REFERENCES:	none

REFERENCED BY:	ComputeDerived()

COPYRIGHT:	University Corporation for Atmospheric Research, 1992
-------------------------------------------------------------------------
*/

#include <math.h>
#include "raf.h"
#include "nimbus.h"
#include "amlib.h"

extern int	Aircraft;

/* -------------------------------------------------------------------- */
void sakrd(varp)
DERTBL	*varp;
{
	NR_TYPE	qcxc, adifr, pitch, thedot, pfactor;
	NR_TYPE	akrd;

	adifr	= GetSample(varp, 0);
	qcxc	= GetSample(varp, 1);


	switch (Aircraft)
		{
		case C130:
			akrd = (((adifr / qcxc) + 0.37467) / (0.06297));

			break;

		case ELECTRA:
			akrd = ((adifr / qcxc) + 0.4095) / 0.07155;
			break;

		case KINGAIR:
			akrd = ((adifr / qcxc) + 0.01414) / 0.08485;
			break;

		case SABRELINER:
			{
			double	xmach2;

			xmach2 = GetSample(varp, 2);
			akrd = adifr / (qcxc * (0.06773-0.01578*sqrt(xmach2) +
							0.01843*xmach2));
			}
			break;

		default:
			akrd = 0.0;
		}

	PutSample(varp, akrd);

}	/* END SAKRD */

/* END AKRD.C */
