/*
-------------------------------------------------------------------------
OBJECT NAME:	200x.c

FULL NAME:	Compute derived paramters for PMS1D 200X probe.

ENTRY POINTS:	sc200x(), sc200cInit()

STATIC FNS:	none

DESCRIPTION:	

OUTPUT:		

REFERENCES:	pms1d.c (ComputePMS1DParams(), ComputeDOF(),
			 ComputeConcentrations())

REFERENCED BY:	ComputeDerived()

NOTES:		Calculations taken from Bulletin 24 dated 1/89.

COPYRIGHT:	University Corporation for Atmospheric Research, 1992
-------------------------------------------------------------------------
*/

#include <math.h>

#include "nimbus.h"
#include "amlib.h"

#define FIRST_BIN	1
#define LAST_BIN	(BINS_16-2)
#define MAX_200X	2

#define NDIODES		15
#define RESOLUTION	20
#define MAG		10.0
#define RESPONSE_TIME	0.35
#define ARM_DISTANCE	61.0	/* in mm	*/

#define DENS		1.0
#define PLWFAC		0.000000001

static NR_TYPE	total_concen[MAX_200X], dbar[MAX_200X], plwc[MAX_200X],
		disp[MAX_200X];

static float	radius[BINS_16],
		dia[BINS_16],
		esw[BINS_16];	/* Effective Sample Width	*/


void	ComputePMS1DParams(), ComputeDOF();


/* -------------------------------------------------------------------- */
void c200xInit()
{
	register int	i;

	ComputePMS1DParams(radius, esw, RESOLUTION, MAG, NDIODES,
						FIRST_BIN, LAST_BIN);

	for (i = FIRST_BIN; i < LAST_BIN; ++i)
		dia[i] = (float)i * RESOLUTION;

}	/* END CONSTRUCTOR */

/* -------------------------------------------------------------------- */
void sc200x(varp)
DERTBL	*varp;
{
	register	i;
	NR_TYPE		*actual;
	NR_TYPE		*concentration;
	NR_TYPE		tasx;		/* True Air Speed		*/
	float		dof[BINS_16],	/* Depth Of Field		*/
			sv[BINS_16];	/* Sample Volume		*/

	/* NOTE: -1 index offset to compensate for undersizeing.
	 */
        actual	= GetVector(varp, 0, 16) - 1;
	tasx	= GetSample(varp, 1);

	if (FeedBack == HIGH_RATE_FEEDBACK)
		{
		if (SampleOffset >= varp->OutputRate)
			return;

		tasx /= varp->OutputRate;
		concentration = &HighRateData[varp->HRstart +
						(SampleOffset * varp->Length)];
		}
	else
		concentration = &AveragedData[varp->LRstart];


	ComputeDOF(radius, tasx, dof, FIRST_BIN, LAST_BIN, RESPONSE_TIME);

	for (i = FIRST_BIN; i < LAST_BIN; ++i)
		{
		if (dof[i] > ARM_DISTANCE)
			dof[i] = ARM_DISTANCE;

		sv[i] = tasx * (dof[i] * esw[i]);
		}

#define PLWC
#define DBZ

#include "pms1d_cv"

	concentration[0] = 0.0;

}	/* END SC200X */

/* -------------------------------------------------------------------- */
void sconcx(varp)
DERTBL	*varp;
{
        PutSample(varp, total_concen[varp->ProbeCount]);
}

/* -------------------------------------------------------------------- */
void sdispx(varp)
DERTBL	*varp;
{
        PutSample(varp, disp[varp->ProbeCount]);
}

/* -------------------------------------------------------------------- */
void splwcx(varp)
DERTBL	*varp;
{
        PutSample(varp, plwc[varp->ProbeCount]);
}

/* -------------------------------------------------------------------- */
void sdbarx(varp)
DERTBL	*varp;
{
        PutSample(varp, dbar[varp->ProbeCount]);
}

/* END 200X.C */
