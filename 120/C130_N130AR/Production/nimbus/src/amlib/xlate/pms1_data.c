/*
-------------------------------------------------------------------------
OBJECT NAME:	pms1_data.c

FULL NAME:	Translate PMS1D to new record format (including Version 2)

ENTRY POINTS:	xlP16bin(), xlP16v2bin() xlP260bin(), xlP32v2bin(),
		xlP300bin(), xlP64v2bin()

STATIC FNS:	none

DESCRIPTION:	This sums up the data bins from SampleRate to 1hz.

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
void xlP16bin(varp, p, np)
RAWTBL		*varp;
struct P16_blk	*p;
NR_TYPE		*np;
{
	register	i, j;

	for (i = 0; i < varp->SampleRate; ++i)
		for (j = 0; j < varp->Length; ++j)
			np[(i*varp->Length)+j] = (ushort)p[i].bins[j];

}	/* END XLP16BIN */

/* -------------------------------------------------------------------- */
void xlP16v2bin(varp, p, np)
RAWTBL			*varp;
struct P16v2_blk	*p;
NR_TYPE			*np;
{
	register	i, j;

	for (i = 0; i < varp->SampleRate; ++i)
		for (j = 0; j < varp->Length; ++j)
			np[(i*varp->Length)+j] = (ushort)p[i].bins[j];

}	/* END XLP16V2BIN */

/* -------------------------------------------------------------------- */
void xlP260bin(varp, p, np)
RAWTBL		*varp;
struct P260_blk	*p;
NR_TYPE		*np;
{
	register	i, j;

	for (i = 0; i < varp->SampleRate; ++i)
		for (j = 0; j < varp->Length; ++j)
			np[(i*varp->Length)+j] = (ushort)p[i].bins[j];

}	/* END XLP260BIN */

/* -------------------------------------------------------------------- */
void xlP32v2bin(varp, p, np)
RAWTBL			*varp;
struct P32v2_blk	*p;
NR_TYPE			*np;
{
	register	i, j;

	for (i = 0; i < varp->SampleRate; ++i)
		for (j = 0; j < varp->Length; ++j)
			np[(i*varp->Length)+j] = (ushort)p[i].bins[j];

}	/* END XLP32V2BIN */

/* -------------------------------------------------------------------- */
void xlP300bin(varp, p, np)
RAWTBL		*varp;
struct P300_blk	*p;
NR_TYPE		*np;
{
	register	i, j;

	for (i = 0; i < varp->SampleRate; ++i)
		for (j = 0; j < varp->Length; ++j)
			np[(i*varp->Length)+j] = (ushort)p[i].bins[j];

}	/* END XLP300BIN */

/* -------------------------------------------------------------------- */
void xlP64v2bin(varp, p, np)
RAWTBL			*varp;
struct P64v2_blk	*p;
NR_TYPE			*np;
{
	register	i, j;

	for (i = 0; i < varp->SampleRate; ++i)
		for (j = 0; j < varp->Length; ++j)
			np[(i*varp->Length)+j] = (ushort)p[i].bins[j];

}	/* END XLP64V2BIN */

/* END PMS1_DATA.C */
