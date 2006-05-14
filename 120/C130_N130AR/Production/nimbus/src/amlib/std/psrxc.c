/*******       Static Pressure  (Default) (mb)                         PSRXC 
  	Input:
  		psc - derived static pressure
  	Output:
  		psrxc - derived generic static pressure (mb)
*/

#include "nimbus.h"
#include "amlib.h"

/* -------------------------------------------------------------------- */
NR_TYPE spsrxc(indata, varp)
NR_TYPE	indata[];
DERTBL	*varp;
{
	return(indata[varp->depend_index[0]]);

}
