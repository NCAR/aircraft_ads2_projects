/*  Cut and Paste the next line.
:set tabstop=5
-------------------------------------------------------------------------
OBJECT NAME:	amlib.c

FULL NAME:	

ENTRY POINTS:	SetComputeFunctions()
				scoup()
				slwcc()
				slwcc1()
				sgerb()
				stsurf()

STATIC FNS:		none

DESCRIPTION:	Template for AMLIB std & user directories.

INPUT:			

OUTPUT:			

REFERENCES:		none

REFERENCED BY:	compute.c

COPYRIGHT:		University Corporation for Atmospheric Research, 1993,1994
-------------------------------------------------------------------------
*/

#include "define.h"


/* Step 4: To appease the compiler, place all AMLIB function names here.
 * By convention, AMLIB functions start with the letter 's'.  These should
 * be the same names you used in steps 2 & 3.
 */
void	scoup(), slwcc(), slwcc1(), sgerb(), stsurf();


/* Step 1: List variables to pass through unaltered by default.  HOUR,
 * MINUTE & SECOND are mandatory.  NULL must be last.
 */
char	*passThrough[] = 
	{
	"HOUR",
	"MINUTE",
	"SECOND",
	"OAT",
	"ATX",
	"PLWCF_RPC",
	"PLWCC",
	"PLWCC1",
	"XGLWC",
	"HGM232",
	"RSTB1",
	NULL
	};


/* Step 2: New variables get added here.  Also add existing variables that
 * you would like to enter additional processing for.  Remove the examples
 * given below, leaving the "NULL, NULL" line in.  Make sure to leave off
 * the parentheses from the function names.
 */
DERVAR	derivftns[] =
	{
	"OPHT",		scoup,
	"XLWCC",	slwcc,
	"XLWCC1",	slwcc1,
	"XGERB",	sgerb,
	"XTSURF",	stsurf,
	NULL,		NULL
	};


/* Step 3: Create AMLIB functions here.  Use these examples as templates.
 * First example is one of modifying an existing variable (ADIFR).  The
 * second example is of a new variable, using 2 existing inputs.  The third
 * example is more complex, using high-rate data.  You want one function
 * for each variable output, do NOT have one function outputting several
 * variables.  However you may use the same function for multiple variables.
 * Each of these functions will be called once per second of data, so if
 * you are dealing with high-rate data or vector data, make sure you handle
 * all the samples for a given second.
 */

static long	mindex[3] = {0, 0, 0};

/* -------------------------------------------------------------------- */
void scoup(varp)
VARTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	atx, oat, hgm232;

	static bool	first_time = TRUE;
	static int	inVarID[3], n0;
	static NR_TYPE	opht, corc0;

	if (first_time)
		{
		/* Obtain ID's of input variables.
		 */
		inVarID[0] = ncvarid(InputFile, "ATX");
		inVarID[1] = ncvarid(InputFile, "OAT");
		inVarID[2] = ncvarid(InputFile, "HGM232");

	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[1], mindex, (void *)&oat);
	opht = oat;
	corc0 = 0.0;

		first_time = FALSE;
		}

	/* Get current values of inputs.
	 */
	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[0], mindex, (void *)&atx);
	ncvarget1(InputFile, inVarID[1], mindex, (void *)&oat);
	ncvarget1(InputFile, inVarID[2], mindex, (void *)&hgm232);

	if ( hgm232 <= 0.0 )
		{
		n0 = 0;
		atx = opht;
		}

	n0 = n0 + 1;

	if ( n0 == 2 )
		{
		opht = oat;
		}

	corc0 = corc0 + 0.004*( atx - opht );

	opht = oat + corc0;

	/* Write out new variable.
	 */
	mindex[0] = CurrentOutputRecordNumber;
	ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&opht);

}	/* END SCOUP */

/* -------------------------------------------------------------------- */

void slwcc(varp)
VARTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	plwcc, plwcf_rpc, hgm232;

	static bool	first_time = TRUE;
	static int	inVarID[3], n1;
	static NR_TYPE	xlwcc, corc1;

	if (first_time)
		{
		/* Obtain ID's of input variables.
		 */
		inVarID[0] = ncvarid(InputFile, "PLWCC");
		inVarID[1] = ncvarid(InputFile, "PLWCF_RPC");
		inVarID[2] = ncvarid(InputFile, "HGM232");

	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[0], mindex, (void *)&plwcc);
	xlwcc = plwcc;
	corc1 = 0.0;

		first_time = FALSE;
		}

	/* Get current values of inputs.
	 */
	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[0], mindex, (void *)&plwcc);
	ncvarget1(InputFile, inVarID[1], mindex, (void *)&plwcf_rpc);
	ncvarget1(InputFile, inVarID[2], mindex, (void *)&hgm232);

	if ( hgm232 <= 0.0 )
		{
		n1 = 0;
		plwcf_rpc = xlwcc;
		}

	if ( plwcc <= -0.5 )
		{
		n1 = 0;
		plwcf_rpc = xlwcc;
		}

	n1 = n1 + 1;

	if ( n1 == 2 )
		{
		xlwcc = plwcc;
		}

	corc1 = corc1 + 0.004*( plwcf_rpc - xlwcc );

	xlwcc = plwcc + corc1;

	/* Write out new variable.
	 */
	mindex[0] = CurrentOutputRecordNumber;
	ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&xlwcc);

}	/* END SLWCC */
  
/* -------------------------------------------------------------------- */

void slwcc1(varp)
VARTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	plwcc1, plwcf_rpc, hgm232;

	static bool	first_time = TRUE;
	static int	inVarID[3], n2;
	static NR_TYPE	xlwcc1, corc2;

	if (first_time)
		{
		/* Obtain ID's of input variables.
		 */
		inVarID[0] = ncvarid(InputFile, "PLWCC1");
		inVarID[1] = ncvarid(InputFile, "PLWCF_RPC");
		inVarID[2] = ncvarid(InputFile, "HGM232");

	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[0], mindex, (void *)&plwcc1);
	xlwcc1 = plwcc1;
	corc2 = 0.0;

		first_time = FALSE;
		}

	/* Get current values of inputs.
	 */
	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[0], mindex, (void *)&plwcc1);
	ncvarget1(InputFile, inVarID[1], mindex, (void *)&plwcf_rpc);
	ncvarget1(InputFile, inVarID[2], mindex, (void *)&hgm232);

	if ( hgm232 <= 0.0 )
		{
		n2 = 0;
		plwcf_rpc = xlwcc1;
		}

	n2 = n2 + 1;

	if ( n2 == 2 )
		{
		xlwcc1 = plwcc1;
		}

	corc2 = corc2 + 0.004*( plwcf_rpc - xlwcc1 );

	xlwcc1 = plwcc1 + corc2;

	/* Write out new variable.
	 */
	mindex[0] = CurrentOutputRecordNumber;
	ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&xlwcc1);

}	/* END SLWCC1 */

/* -------------------------------------------------------------------- */

void sgerb(varp)
VARTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	xglwc, plwcf_rpc, hgm232;

	static bool	first_time = TRUE;
	static int	inVarID[3], n3;
	static NR_TYPE	xgerb, corc3;

	if (first_time)
		{
		/* Obtain ID's of input variables.
		 */
		inVarID[0] = ncvarid(InputFile, "XGLWC");
		inVarID[1] = ncvarid(InputFile, "PLWCF_RPC");
		inVarID[2] = ncvarid(InputFile, "HGM232");

	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[0], mindex, (void *)&xglwc);
	xgerb = xglwc;
	corc3 = 0.0;

		first_time = FALSE;
		}

	/* Get current values of inputs.
	 */
	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[0], mindex, (void *)&xglwc);
	ncvarget1(InputFile, inVarID[1], mindex, (void *)&plwcf_rpc);
	ncvarget1(InputFile, inVarID[2], mindex, (void *)&hgm232);

	if ( hgm232 <= 0.0 )
		{
		n3 = 0;
		plwcf_rpc = xgerb;
		}

	n3 = n3 + 1;

	if ( n3 == 2 )
		{
		xgerb = xglwc;
		}

	corc3 = corc3 + 0.004*( plwcf_rpc - xgerb );

	xgerb = xglwc + corc3;

	/* Write out new variable.
	 */
	mindex[0] = CurrentOutputRecordNumber;
	ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&xgerb);

}	/* END SGERB */


/* -------------------------------------------------------------------- */

void stsurf(varp)
VARTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	rstb1, hgm232;

	static bool	first_time = TRUE;
	static int	inVarID[2];
	static NR_TYPE 	xtsurf;

	if (first_time)
		{
		/* Obtain ID's of input variables.
		 */
		inVarID[0] = ncvarid(InputFile, "RSTB1");
		inVarID[1] = ncvarid(InputFile, "HGM232");

		first_time = FALSE;
		}

	/* Get current values of inputs.
	 */
	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[0], mindex, (void *)&rstb1);
	ncvarget1(InputFile, inVarID[1], mindex, (void *)&hgm232);

	xtsurf = rstb1 * (1.00 + hgm232 * (7.297e-05 + hgm232 * (5.2869e-09  - 
		hgm232 * 1.15666e-12)));

	if ( hgm232 >= 6000. )
		{
		xtsurf = rstb1 * 1.374;
		}

	/* Write out new variable.
	 */
	mindex[0] = CurrentOutputRecordNumber;
	ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&xtsurf);

}	/* END STSURF */

/* Leave this function alone.
 */
void SetComputeFunctions()
{
	register int	i, indx;

	for (i = 0; derivftns[i].Compute; ++i)
		if ((indx = SearchTable(Variable,nVariables,derivftns[i].Name)) != ERR)
			Variable[indx]->Compute = derivftns[i].Compute;

}	/* END SETCOMPUTEFUNCTIONS */

/* END AMLIB.C */
