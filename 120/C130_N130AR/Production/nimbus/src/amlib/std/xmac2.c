/*    XMAC2 = mach number squared calculation function              XMAC2 */

#include <math.h>
#include "nimbus.h"

NR_TYPE xmac2(a)
NR_TYPE	a;
{
	return(5.0 * (pow((double)(a+1.), (double)0.28571) - 1.0));
}
