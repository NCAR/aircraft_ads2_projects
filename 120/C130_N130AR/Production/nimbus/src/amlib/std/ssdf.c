/*******SSDF  SIDESLIP ANGLE OF THE BOOM (deg)                         SSDF
 
 	Input:
 		bdif - raw boom differential pressure
 		qcb - raw boom dynamic pressure
		xmach2 - derived mach number
 	Output:
 		ssdf - sideslip angle of the boom (deg)
*/

#include <math.h>
#include "nimbus.h"
#include "amlib.h"

/* -------------------------------------------------------------------- */
void sssdf(varp)
DERTBL	*varp;
{
	NR_TYPE	qcb, bdif;
	NR_TYPE	ssdf;
	NR_TYPE	gr, divr;
	double	xmach2;

	qcb	= GetSample(varp, 0);
	bdif	= GetSample(varp, 1);
	xmach2	= GetSample(varp, 2);

	if (qcb < 0.001)
		qcb = 0.001;

	gr = 0.086577797 - 0.03560256 * sqrt(xmach2) + 0.00006143 * xmach2;

	if ((divr = gr * qcb) == 0.0)
		divr = 0.00079;

	ssdf = bdif / divr;

	if (ssdf > 20.0)
		ssdf = 20.0;

	if (ssdf < -20.0)
		ssdf = -20.0;

	ssdf -= 0.7;

	PutSample(varp, ssdf);

}
