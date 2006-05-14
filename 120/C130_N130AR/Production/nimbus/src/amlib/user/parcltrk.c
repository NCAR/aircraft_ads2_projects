/*  Parcel tracking in three dimensions from Ron Ruth's f77 code of 4-91 */

#include <math.h>
#include "nimbus.h"
#include "amlib.h"

#define NPARCEL 2		/* Number of parcels handled here */
#define INTRVL  5		/* Number of samples in mean winds */
#define CD2NMI 60.		/* Degrees Latitude --> nmi conversion */
#define CM2FT 3.2808398		/* Meters --> feet conversion */
#define CM2NMI 5.396E-4		/* Meters --> nmi conversion */
#define DEG 57.29577952		/* Radians --> degrees conversion */
#define RAD .01745329

static int reset[NPARCEL];	/* reset flags */
static int intvl_fill[NPARCEL];	/* one half of the INTRVL */
static float pdst[NPARCEL];	/* distance to parcel (n.mi. from a/c) */
static float prad[NPARCEL];	/* angle to parcel (deg from a/c) */
static float pheight[NPARCEL];	/* altitude to parcel (ft above a/c) */


parcel_reset(num)
int num;		/* number of parcel to reset - counting from zero */
{
reset[num] = 1;		/* 1 = reset, 0 = no reset */
/* number of points to advance before a mean wind can be computed at the start of the track */
intvl_fill[num] = (INTRVL+1)/2;
}


/* -------------------------------------------------------------------- */
void sptdst1(varp)
DERTBL	*varp;
{
static int ptime[NPARCEL];		/* number of seconds tracking parcel */
static float pu[NPARCEL];		/* e-w wind values at start of track*/
static float pv[NPARCEL];		/* n-s wind values at start of track*/
static float pw[NPARCEL];		/* vert wind values at start of track*/
static float plat[NPARCEL];		/* latitude values at start of track*/
static float plon[NPARCEL];		/* longitude values at start of track*/
static float palt[NPARCEL];		/* altitude values at start of track*/
static float u[INTRVL];			/* current e-w wind values */
static float v[INTRVL];			/* current n-s wind values */
static float w[INTRVL];			/* current vertical wind values */
static int count = 0;			/* count times routine called*/

	float	north, east;
	int	i, j, indx;

indx = count % INTRVL;			/* mod of the count */
u[indx] = GetSample(varp, 3);
v[indx] = GetSample(varp, 4);
w[indx] = GetSample(varp, 5);

for(i=0;i<NPARCEL;i++)
	ptime[i]++;			/* incr tracking time */

for(i=0;i<NPARCEL;i++)
	{
	if(intvl_fill[i]>0)		/* save initial posn,winds */
		{
		pu[i] = 0;
		pv[i] = 0;
		pw[i] = 0;
		for(j=0;j<INTRVL;j++)
		   {
		   pu[i] += u[j];
		   pv[i] += v[j];
		   pw[i] += w[j];
		   }
		pu[i] = pu[i]/INTRVL;
		pv[i] = pv[i]/INTRVL;
		pw[i] = pw[i]/INTRVL;
		intvl_fill[i]--;
		}
	if(reset[i]>0)
		{
		plat[i] = GetSample(varp, 0);
		plon[i] = GetSample(varp, 1);
		palt[i] = GetSample(varp, 2);
		reset[i] = 0;
		ptime[i] = 0;
		}

	/* Distance, radial and elevation to parcel */
	north = (plat[i] - GetSample(varp, 0)) *
		CD2NMI + ptime[i] * pv[i] * CM2NMI;

	east = cos((double)(RAD * 0.5 * (plat[i] +
			GetSample(varp, 0)))) *
		(plon[i] - GetSample(varp, 1)) *
		CD2NMI + ptime[i] * pu[i] * CM2NMI;

	pdst[i] = sqrt((double)(north*north + east*east));

	if (north != 0.0 || east != 0.0)
		prad[i] = atan2((double)-east, (double)-north) * DEG + 180.0;
	else
		prad[i] = 0.0;

	pheight[i] = (palt[i] - GetSample(varp, 2)) *
			CM2FT + ptime[i] * pw[i] * CM2FT;
	}
count++;	

PutSample(varp, pdst[0]);
}

/* -------------------------------------------------------------------- */
void sptrad1(varp)
DERTBL	*varp;
{
	PutSample(varp, prad[0]);
}

/* -------------------------------------------------------------------- */
void sptalt1(varp)
DERTBL	*varp;
{
	PutSample(varp, pheight[0]);
}

/* -------------------------------------------------------------------- */
void sptdst2(varp)
DERTBL	*varp;
{
	PutSample(varp, pdst[1]);
}

/* -------------------------------------------------------------------- */
void sptrad2(varp)
DERTBL	*varp;
{
	PutSample(varp, prad[1]);
}

/* -------------------------------------------------------------------- */
void sptalt2(varp)
DERTBL	*varp;
{
	PutSample(varp, pheight[1]);
}
