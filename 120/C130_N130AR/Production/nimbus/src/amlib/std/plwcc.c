/**************************** PLWCC *******************************  PLWCC
 ** Corrected PMS liquid water content (g/m3)
 	Input:
 		plwc - raw liquid water content
 		tasx - derived true air speed
 		atx - derived ambient temperature
 		psxc - derived static pressure
 	Output:
 		plwcc - corrected PMS liquid water content
*/

#include <math.h>
#include "nimbus.h"
#include "amlib.h"
#include "raf.h"
 
static float	twire;

NR_TYPE	kinglwcc();

extern int	Aircraft;

/* -------------------------------------------------------------------- */
void plwccInit()
{
	twire = (GetDefaultsValue("TWIRE_PMS"))[0];

}	/* END PLWCCINIT */

/* -------------------------------------------------------------------- */
void splwcc(varp)
DERTBL	*varp;
{
	NR_TYPE	plwc, tasx, atx, psxc;

	plwc	= GetSample(varp, 0);
	tasx	= GetSample(varp, 1);
	atx	= GetSample(varp, 2);
	psxc	= GetSample(varp, 3);
 
	if (Aircraft == C130)
		tasx *= 0.90;

	PutSample(varp, kinglwcc(plwc, tasx, atx, psxc, twire));

}	/* END SPLWCC */

/* END PLWCC.C */
