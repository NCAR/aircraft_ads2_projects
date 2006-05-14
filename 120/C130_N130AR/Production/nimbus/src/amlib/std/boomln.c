/*
-------------------------------------------------------------------------
OBJECT NAME:	boomln.c

FULL NAME:	Retrieve Boom Length for given Aircraft

ENTRY POINTS:	GetBoomLength()

STATIC FNS:	none

DESCRIPTION:	Boom Length is defined as the distance from the Inertial
		system to the tip of the Radome.

INPUT:		none

OUTPUT:		float Boom_Length

REFERENCES:	GetAircraft (hdr_api)

REFERENCED BY:	irs.c, ins.c

COPYRIGHT:	University Corporation for Atmospheric Research, 1992
-------------------------------------------------------------------------
*/

#include "nimbus.h"
#include "ctape.h"
#include "decode.h"
#include "raf.h"

extern int Aircraft;

/* -------------------------------------------------------------------- */
float GetBoomLength()
{
	FILE	*fp;
	float	boomln;
	int     rc;
	char    aircraft[8], tailnum[8];

	switch (Aircraft)
		{
		case C130:
			sprintf(aircraft, "N%03dAR", Aircraft);
			break;

		default:
			sprintf(aircraft, "N%03dD", Aircraft);
		}

	fp = OpenProjectFile(BOOMLN, "r", EXIT);

	do
		{
		rc = fscanf(fp, "%s %f", tailnum, &boomln);
		}
	while (rc > 0 && strcmp(tailnum, aircraft) != 0);

	if (rc == EOF)
		{
		fprintf(stderr, "Aircraft %s does not exist in %s.",
		aircraft, BOOMLN);
		fprintf(stderr, "  This is fatal.\n");
		exit(1);
		}

	fclose(fp);

	return(boomln);

}	/* END GETBOOMLENGTH */

/* END BOOMLN.C */
