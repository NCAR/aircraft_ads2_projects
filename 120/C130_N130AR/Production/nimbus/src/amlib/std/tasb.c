/*******       AIRCRAFT TRUE AIRSPEED BOOM  (M/S)                     * TASB
                REQUIRES --- TTF,RECFF, TAS FCN., XMAC2 ftn.
 	Input:
 		qcbc - derived dynamic pressure
 		psbc - derived static pressure
 		ttf - raw temperature
 	Output:
 		tasb - derived true airspeed
 	Include:
 		xmac2 - mach number function
 		tas - true airspeed function
 		recff - recovery factor
*/

#include "nimbus.h"
#include "amlib.h"

extern NR_TYPE xmac2();
extern NR_TYPE tas();
extern NR_TYPE recff;

/* -------------------------------------------------------------------- */
void stasb(varp)
DERTBL	*varp;
{
	NR_TYPE	bmach2;
	NR_TYPE	ttf, qcbc, psbc, tasb;

	ttf	= GetSample(varp, 0);
	qcbc	= GetSample(varp, 1);
	psbc	= GetSample(varp, 2);

	bmach2	= xmac2(qcbc / psbc);
	tasb	= tas(ttf, recff, bmach2);

	PutSample(varp, tasb);

}	/* END STASB */

/* END TASB.C */
