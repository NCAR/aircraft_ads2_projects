/*
-------------------------------------------------------------------------
OBJECT NAME:	hgm.c

FULL NAME:	Translate HGM SDI variables to new record format

ENTRY POINTS:	xlhgm(), xlhgme(), xlchgme()

STATIC FNS:	none

DESCRIPTION:	

INPUT:		

OUTPUT:		Averaged value

REFERENCES:	angcr()

REFERENCED BY:	rec_decode.c

COPYRIGHT:	University Corporation for Atmospheric Research, 1992
-------------------------------------------------------------------------
*/

#include "raf.h"
#include "nimbus.h"
#include "amlib.h"
#include "gust.h"

static NR_TYPE	chgme[250];	/* 250 = Max Rate	*/

extern int	Aircraft;

NR_TYPE angcr();

/* -------------------------------------------------------------------- */
void xlhgm(varp, input, output)
RAWTBL	*varp;
short	input[];
NR_TYPE	*output;
{
	int	i;
	NR_TYPE		hgm, hgmv;

	if (Aircraft == KINGAIR)
		for (i = 0; i < varp->SampleRate; ++i)
			{
			hgmv = (input[i * varp->ADSoffset] - varp->convertOffset) * varp->convertFactor;

			hgm  = 480.0 - 50.0 * hgmv;

			if (hgmv < -0.4)
				hgm = 366.6667 - 333.3333 * hgmv;

			if (atoi(ProjectNumber) == 201)
				{
				hgm = 264.0 - 29.98 * hgmv;

				if (hgmv < -7.3)
					hgm = -948.0 - 196.0 * hgmv;
				}

			output[i] = hgm * 0.3048;
			}
	else
		/* This is hard wired for 2 cal coe's */
		for (i = 0; i < varp->SampleRate; ++i)
			output[i] = varp->cof[0] + varp->cof[1] *
				(input[i * varp->ADSoffset] - varp->convertOffset) * varp->convertFactor;

}	/* END XLHGM */

/* -------------------------------------------------------------------- */
void xlhgme(varp, input, output)
RAWTBL	*varp;
short	input[];
NR_TYPE	*output;
{
	register	i;
	NR_TYPE		hgme;
	float		angle, start;

	angle	= 14.4;
	start	= 0.0;

	for (i = 0; i < varp->SampleRate; ++i)
		{
		hgme	= ~input[i * varp->ADSoffset] * RESOLV * 0.04;
		hgme	= angcr(hgme, chgme[i], angle, start);

		/* 180 removed to fix readout offset
		if (hgme > 180.0)
			hgme -= 180.0;
*/
		output[i] = hgme * 84.666667;
		}

}	/* END XLHGME */

/* -------------------------------------------------------------------- */
void xlchgme(varp, input, output)
RAWTBL	*varp;
short	input[];
NR_TYPE	*output;
{
	register	i;

	for (i = 0; i < varp->SampleRate; ++i)
		output[i] = chgme[i] = ~input[i * varp->ADSoffset] * RESOLV;

}	/* END XLCHGME */

/* END HGM.C */
