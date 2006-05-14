/*
-------------------------------------------------------------------------
OBJECT NAME:	func.c

FULL NAME:	

ENTRY POINTS:	sfunc()

STATIC FNS:	

DESCRIPTION:	Template for AMLIB std & user directories.

INPUT:		

OUTPUT:		

REFERENCES:	

REFERENCED BY:	compute.c

COPYRIGHT:	University Corporation for Atmospheric Research, 1995
-------------------------------------------------------------------------
*/

#include "nimbus.h"
#include "amlib.h"


/* -------------------------------------------------------------------- */
void sfunc(varp)
DERTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	x1, x2, x3...xn, y;

	x1 = GetSample(varp, 0);
	x2 = GetSample(varp, 1);
	....
	xn = GetSample(varp, n-1);


	y = ..............;


	PutSample(varp, y);

}	/* END SFUNC */

/* END FUNC.C */
