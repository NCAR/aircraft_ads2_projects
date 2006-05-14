/*
-------------------------------------------------------------------------
OBJECT NAME:	prcltrk1.c

FULL NAME:	Parcel Tracking

ENTRY POINTS:	sptdrdst(), sptdrtim(), sptdrrad()

STATIC FNS:	none

DESCRIPTION:	Parcel Tracking, Dead Reckoning.

INPUT:		

OUTPUT:		

REFERENCES:	none

REFERENCED BY:	compute.c

COPYRIGHT:	University Corporation for Atmospheric Research, 1995
-------------------------------------------------------------------------
*/

#include <math.h>
#include "nimbus.h"
#include "amlib.h"

#define MAX_PTDR	5

static NR_TYPE	decl, x[MAX_PTDR], y[MAX_PTDR];
static NR_TYPE	deltaT[MAX_PTDR];
static NR_TYPE	bearing[MAX_PTDR];
static NR_TYPE	timeToReturn[MAX_PTDR];

/* -------------------------------------------------------------------- */
void parcel_reset(num)
int	num;
{
	if (num >= 3)
		ResetDeadReckoning(num - 3);
	else
		ResetLagrangian(num);

}	/* END PARCEL_RESET */

/* -------------------------------------------------------------------- */
void deadReckInit()
{
	decl = (GetDefaultsValue("DECLINATION"))[0];

}	/* END DEADRECKINIT */

/* -------------------------------------------------------------------- */
void ResetDeadReckoning(num)
int	num;
{
	x[num] = y[num] = deltaT[num] = 0.0;

}	/* END RESETDEADRECKONING */

/* -------------------------------------------------------------------- */
void sptdrdst(varp)
DERTBL	*varp;		/* Information about current variable	*/
{
	int		indx;
	NR_TYPE		tasx, thdg, sslip, d;

	tasx = GetSample(varp, 0);
	thdg = GetSample(varp, 1);
	sslip = GetSample(varp, 2);

	indx = varp->ProbeCount;
	deltaT[indx] += 1.0;

	x[indx] += tasx * 0.00054 * sin(thdg - sslip) * deltaT[indx];
	y[indx] += tasx * 0.00054 * cos(thdg - sslip) * deltaT[indx];

	d = sqrt(x[indx]*x[indx] + y[indx]*y[indx]);
	bearing[indx] = atan2(x[indx], y[indx]) * 180.0 / M_PI + 180.0 - decl;
	timeToReturn[indx] = (d / 0.00054) / (60.0 * tasx);

	if (bearing[indx] < 0.0)
		bearing[indx] += 360.0;
	else
	if (bearing[indx] > 360.0)
		bearing[indx] -= 360.0;
		
	PutSample(varp, d);

}	/* END SPTDRDST */

/* -------------------------------------------------------------------- */
void sptdrrad(varp)
DERTBL	*varp;		/* Information about current variable	*/
{
	PutSample(varp, bearing[varp->ProbeCount]);

}	/* END SPTDRRAD */

/* -------------------------------------------------------------------- */
void sptdrtim(varp)
DERTBL	*varp;		/* Information about current variable	*/
{
	PutSample(varp, timeToReturn[varp->ProbeCount]);

}	/* END SPTDRRAD */

/* END PRCLTRK1.C */
