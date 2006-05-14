/*******       Dynamic Pressure  (Default) (mb)                         QSRXC 
  	Input:
  		qcc - derived dynamic pressure
  	Output:
  		qcrxc - derived generic dynamic pressure (mb)
*/

#include "nimbus.h"
#include "amlib.h"

/* -------------------------------------------------------------------- */
NR_TYPE sqcrxc(indata, varp)
NR_TYPE	indata[];
DERTBL	*varp;
{
	return(indata[varp->depend_index[0]]);

}
