/*
-------------------------------------------------------------------------
OBJECT NAME:	260x.c

FULL NAME:	Compute derived paramters for PMS1D 260X probe.

ENTRY POINTS:	sc260x()

STATIC FNS:	none

DESCRIPTION:	

OUTPUT:		

REFERENCES:	pms1d.c (ComputePMS1DParams(), ComputeDOF(),
			 ComputeConcentrations())

REFERENCED BY:	Compute()

NOTES:		Calculations taken from Bulletin 24 dated 1/89.

COPYRIGHT:	University Corporation for Atmospheric Research, 1992
-------------------------------------------------------------------------
*/

#include <math.h>

#include "nimbus.h"
#include "amlib.h"

#define FIRST_BIN	1
#define LAST_BIN	(BINS_64-1)
#define MAX_260X	2

#define NDIODES		64
#define RESOLUTION	10
#define MAG		20.0
#define RESPONSE_TIME	0.35
#define ARM_DISTANCE	61.0	/* in mm	*/

#define DENS		1.0
#define PLWFAC		0.000000001

static NR_TYPE	total_concen[MAX_260X], dbar[MAX_260X], plwc[MAX_260X],
		disp[MAX_260X];

static float	radius[BINS_64],
		dia[BINS_64],
		esw[BINS_64];	/* Effective Sample Width	*/

void	ComputePMS1DParams(), ComputeDOF();


/* -------------------------------------------------------------------- */
void c260xInit()
{
	register int	i;

	ComputePMS1DParams(radius, esw, RESOLUTION, MAG, NDIODES,
						FIRST_BIN, LAST_BIN);

	for (i = FIRST_BIN; i < LAST_BIN; ++i)
		dia[i] = (float)i * RESOLUTION;

}	/* END CONSTRUCTOR */

/* -------------------------------------------------------------------- */
void sc260x(varp)
DERTBL	*varp;
{
	register	i;
	NR_TYPE		*actual, tasx;
	NR_TYPE		*concentration;
	float		dof[BINS_64],	/* Depth Of Field		*/
			sv[BINS_64];	/* Sample Volume		*/

	/* NOTE: -1 index offset to compensate for undersizeing.
	 */
	actual	= GetVector(varp, 0, 64) - 1;
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

		sv[i] = tasx * (dof[i] * esw[i]) * 0.001;
		}

#define PLWC
#define DBZ

#include "pms1d_cv"

	concentration[0] = 0.0;

}	/* END SC260X */

/* -------------------------------------------------------------------- */
void sconc6(varp)
DERTBL	*varp;
{
	PutSample(varp, total_concen[varp->ProbeCount]);
}

/* -------------------------------------------------------------------- */
void sdisp6(varp)
DERTBL	*varp;
{
	PutSample(varp, disp[varp->ProbeCount]);
}

/* -------------------------------------------------------------------- */
void splwc6(varp)
DERTBL	*varp;
{
	PutSample(varp, plwc[varp->ProbeCount]);
}

/* -------------------------------------------------------------------- */
void sdbar6(varp)
DERTBL	*varp;
{
	PutSample(varp, dbar[varp->ProbeCount]);
}

/* END 260X.C */
