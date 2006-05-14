/*   TAS = TRUE AIRSPEED CALCULATION FUNCTION (m/s)                     TAS */

#include <math.h>
#include "nimbus.h"

NR_TYPE tas(t, r, xm2)
NR_TYPE	t, r, xm2;
{
	double	term;
	NR_TYPE	out;

	term = (t + 273.16) * (401.86 / (1.0 / xm2 + 0.2 * r));

	if (term < 0.0)
		term = -term;

	out = sqrt(term);
	return(out);
}
