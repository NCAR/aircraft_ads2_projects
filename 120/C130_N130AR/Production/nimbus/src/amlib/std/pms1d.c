/*
-------------------------------------------------------------------------
OBJECT NAME:	pms1d.c

FULL NAME:	

ENTRY POINTS:	ComputePMS1DParams()
		ComputeDOF()

STATIC FNS:	none

DESCRIPTION:	Functions used by all PMS1D probes.

INPUT:		

OUTPUT:		

REFERENCES:	none

REFERENCED BY:	sfssp(), sasas(), s200x(), s200y(), s260x(), sf300()

NOTES:		Calculations taken from Bulletin 24 dated 1/89.

COPYRIGHT:	University Corporation for Atmospheric Research, 1992
-------------------------------------------------------------------------
*/

#include <math.h>

#include "nimbus.h"
#include "amlib.h"

#define PI		M_PI
#define DIODE_DIAMETER	0.2


/* -------------------------------------------------------------------- */
void ComputePMS1DParams(radius, esw, RESOLUTION, MAG, NDIODES, FirstBin,LastBin)
float	radius[];
float	esw[];
int	RESOLUTION;
float	MAG;
int	NDIODES;
int	FirstBin;
int	LastBin;
{
	register	i;

	for (i = FirstBin; i < LastBin; ++i)
		{
		radius[i]	= (float)i * RESOLUTION / 2000;
		esw[i]		= (DIODE_DIAMETER * (NDIODES - i - 1) / MAG);
		}

}	/* END COMPUTEPMS1DPARAMS */

/* -------------------------------------------------------------------- */
/* This procedure was taken from a piece of code Darrel gave me called
 * subroutine sub1d().  I'm isolating it, because it looks to be in a
 * state of flux.
 */
void ComputeDOF(radius, tasx, dof, FirstBin, LastBin, RESPONSE_TIME)
float	radius[];
float	tasx;
float	dof[];
int	FirstBin;
int	LastBin;
float	RESPONSE_TIME;
{
	register	i;
	float		szz;
	float		timex;
	float		frac, f, frc;
	float		z;

	for (i = FirstBin; i < LastBin; ++i)
		{
		/* Determine size in terms of # of array elements
		 */
		szz = (float)i * 0.2;
		/* Diminish this radius by an amount that accounts for
		 * the fact that the particle is not on the diode array
		 * 100% of the time 50% shadowed. This amount is approximately
		 * one half diode array width wide.
		 */
		szz -= 0.1;

		/* Calculate amount of time particle will be on the array
		 */
		timex = szz / (tasx * 1000.0) * 1.0e6;

		/* This is the fraction of total amplitude caused by response
		 * time
		 */
		frac = (double)1.0 - exp((double)-timex / RESPONSE_TIME);

		/* Have to meet 70% occultation for the 1D.
		 */
		if ((frc = 0.7 / frac) > 1.0)
			frc = 1.0;

		/* Cautionary note to users of this routine, the relationship
		 * between dof and Z was derived from laboratory studies which
		 * are somewhat prelimnary in nature and need additional
		 * verification, A new algorithm should be available by 9/1/92
		 */
		f	= frc * 100.0;
		z	= 30.46 - 0.628 * f + 0.003246 * f * f;
		dof[i]	= 1580.0 * z * radius[i] * radius[i];

		if (frc == 1.0)
			dof[i] = 0.0;
		}

}	/* END COMPUTEDOF */

/* END PMS1D.C */
