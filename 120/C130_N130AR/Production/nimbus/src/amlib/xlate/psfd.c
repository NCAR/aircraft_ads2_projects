/*
-------------------------------------------------------------------------
OBJECT NAME:	psfd.c

FULL NAME:	Translate PSFD into new record format

ENTRY POINTS:	xlpsfd()

STATIC FNS:	none

DESCRIPTION:	

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
void xlpsfd(varp, input, output)
RAWTBL	*varp;
ushort	input[];
NR_TYPE	*output;
{
	register	i;
	unsigned long	psfd;

	for (i = 0; i < varp->SampleRate; ++i)
		{
		psfd = (~input[varp->ADSoffset * i - 1] & 0x0001) << 16;
		psfd |= ~input[varp->ADSoffset * i] & 0x0000ffff;

		output[i] = 0.033864 * psfd;
		}

}	/* END XLPSFD */

/* -------------------------------------------------------------------- */
void xlpsfd_ADS2(varp, input, output)
RAWTBL	*varp;
ushort	input[];
NR_TYPE	*output;
{
	register	i;
	unsigned long	psfd;
	ushort		*p1_input, *p2_input;

	static bool	firstTime = TRUE;
	static SDITBL	*psfd1, *psfd2;

	if (firstTime)
		{
		int	indx1, indx2;

		indx1 = SearchTable((char **)sdi, nsdi, "PSFD1");
		indx2 = SearchTable((char **)sdi, nsdi, "PSFD2");

		if ((int)psfd1 == ERR || (int)psfd2 == ERR)
			{
			LogMessage("PSFD: can't find PSFD1 or PSFD2, fatal.\n");
			sleep(2);
			exit(1);
			}

		psfd1 = sdi[indx1];
		psfd2 = sdi[indx2];

		firstTime = FALSE;
		}

	p1_input = input;
	p2_input = &input[-(psfd1->ADSstart << 1) + (psfd2->ADSstart << 1)];

	for (i = 0; i < varp->SampleRate; ++i)
		{
		psfd = (p2_input[psfd2->ADSoffset * i - 1] & 0x0001) << 16;
		psfd |= p1_input[psfd1->ADSoffset * i] & 0x0000ffff;

		output[i] = 0.033864 * psfd;
		}

}	/* END XLPSFD */

/* END PSFD.C */
