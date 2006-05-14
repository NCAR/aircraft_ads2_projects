/*
-------------------------------------------------------------------------
OBJECT NAME:	uvhyg.c

FULL NAME:	Translate UV Hygrometer data block

ENTRY POINTS:	xluvhsmp(), xluvhref(), xluvkrsp(), xluvkrrf(), xluvchsp(),
		xluvchrf(), xluvpvol(), xluvtvol(), xlmruv

DESCRIPTION:	
		UVHSMP         hydrogen sample
		UVHREF         hydrogen reference
		UVKRSP         krypton sample
		UVKRRF         krypton reference
		UVCHSP         chopper sample
		UVCHRF         chopper reference
		UVPVOL         sample volume pressure
		UVTVOL         sample volume temperature
		MRUV           calculated humidity

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
void xluvhsmp(varp, p, np)
RAWTBL		*varp;
struct Uv_blk	*p;
NR_TYPE		*np;
{
	register	i;

	for (i = 0; i < varp->SampleRate; ++i)
		np[i] = p->uvsmp[i].hydr_smp;
}

/* -------------------------------------------------------------------- */
void xluvhref(varp, p, np)
RAWTBL		*varp;
struct Uv_blk	*p;
NR_TYPE		*np;
{
	register	i;

	for (i = 0; i < varp->SampleRate; ++i)
		np[i] = p->uvsmp[i].hydr_ref;
}

/* -------------------------------------------------------------------- */
void xluvkrsp(varp, p, np)
RAWTBL		*varp;
struct Uv_blk	*p;
NR_TYPE		*np;
{
	register	i;

	for (i = 0; i < varp->SampleRate; ++i)
		np[i] = p->uvsmp[i].kryp_smp;
}

/* -------------------------------------------------------------------- */
void xluvkrrf(varp, p, np)
RAWTBL		*varp;
struct Uv_blk	*p;
NR_TYPE		*np;
{
	register	i;

	for (i = 0; i < varp->SampleRate; ++i)
		np[i] = p->uvsmp[i].kryp_ref;
}

/* -------------------------------------------------------------------- */
void xluvchsp(varp, p, np)
RAWTBL		*varp;
struct Uv_blk	*p;
NR_TYPE		*np;
{
	register	i;

	for (i = 0; i < varp->SampleRate; ++i)
		np[i] = p->uvsmp[i].chop_smp;
}

/* -------------------------------------------------------------------- */
void xluvchrf(varp, p, np)
RAWTBL		*varp;
struct Uv_blk	*p;
NR_TYPE		*np;
{
	register	i;

	for (i = 0; i < varp->SampleRate; ++i)
		np[i] = p->uvsmp[i].chop_ref;
}

/* -------------------------------------------------------------------- */
void xluvpvol(varp, p, np)
RAWTBL		*varp;
struct Uv_blk	*p;
NR_TYPE		*np;
{
	register	i;

	for (i = 0; i < varp->SampleRate; ++i)
		np[i] = p->uvsmp[i].pressure;
}

/* -------------------------------------------------------------------- */
void xluvtvol(varp, p, np)
RAWTBL		*varp;
struct Uv_blk	*p;
NR_TYPE		*np;
{
	register	i;

	for (i = 0; i < varp->SampleRate; ++i)
		np[i] = p->uvsmp[i].temp;
}

/* -------------------------------------------------------------------- */
void xlmruv(varp, p, np)
RAWTBL		*varp;
struct Uv_blk	*p;
NR_TYPE		*np;
{
	register	i;

	for (i = 0; i < varp->SampleRate; ++i)
		np[i] = p->uvsmp[i].humidity;
}

/* END UVHYG.C */
