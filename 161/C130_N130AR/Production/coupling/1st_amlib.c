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
	"ATX",
	"PLWCF_LPC",
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
	"XOAT",		"C",	1,	1,	scoup,	"",
	"PLWCC",	"gm-3",	1,	1,	slwcc,	"",
	"PLWCC1",	"gm-3",	1,	1,	slwcc1,	"",
	"XGLWC",	"gm-3",	1,	1,	sgerb,	"",
	"XTSURF",	"C",	1,	1,	stsurf,	"",
	NULL,		NULL,	0,	0,	NULL
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

static long	mindex[4] = {0, 0, 0};

/* -------------------------------------------------------------------- */
void scoup(varp)
VARTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	atx, oat, hgm232, roll, diff1;

	static bool	first_time = TRUE;
	static int	inVarID[5], n0;
	static NR_TYPE	opht, corc0, diff0, diff2, delta0;

	if (first_time)
		{
		/* Obtain ID's of input variables.
		 */
		inVarID[0] = ncvarid(InputFile, "ATX");
		inVarID[1] = ncvarid(InputFile, "OAT");
		inVarID[2] = ncvarid(InputFile, "HGM232");
		inVarID[3] = ncvarid(InputFile, "ROLL");
		inVarID[4] = ncvarid(InputFile, "DIFF1");

	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[1], mindex, (void *)&oat);
	opht = oat;
	corc0 = 0.0;
	n0 = 0;
	diff0 = 0.0;
	diff2 = 0.0;
	delta0 = 0.0;

		first_time = FALSE;
		}

	/* Get current values of inputs.
	 */
	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[0], mindex, (void *)&atx);
	ncvarget1(InputFile, inVarID[1], mindex, (void *)&oat);
	ncvarget1(InputFile, inVarID[2], mindex, (void *)&hgm232);
	ncvarget1(InputFile, inVarID[3], mindex, (void *)&roll);
	ncvarget1(InputFile, inVarID[4], mindex, (void *)&diff1);

	delta0 = diff1 - diff2;

	if ( hgm232 <= 0.0 || oat >= 50.0 )
		{
		n0 = 0;
		atx = opht;
		}

	if ( delta0 <= -2.1 || delta0 >= 2.1 )
		{
		n0 = 0;
		}

	n0 = n0 + 1;

	if ( n0 == 2 )
		{
		opht = oat;
		corc0 = (atx - opht);
		}

	if ( roll >= -5.0 && roll<= 5.0 )
		{
	corc0 = corc0 + 0.004*( atx - opht );
		}

	opht = oat + corc0;

	diff2 = diff0;
	diff0 = diff1;

	/* Write out new variable.
	 */

	if (oat <= -327.0)
		{

	corc0 = 0.0;
	n0 = 0;
	diff0 = 0.0;
	diff2 = 0.0;
	delta0 = 0.0;

	opht = -32767.0;
		}

	oat = opht;

	if (oat <= -100.0 || oat >= 50.0)
		{

	oat = -32767.0;

		}
	mindex[0] = CurrentOutputRecordNumber;
	ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&oat);

}	/* END SCOUP */

/* -------------------------------------------------------------------- */

void slwcc(varp)
VARTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	plwcc, plwcf_lpc, hgm232;

	static bool	first_time = TRUE;
	static int	inVarID[3], n1;
	static NR_TYPE	xlwcc, corc1, flag1;

	if (first_time)
		{
		/* Obtain ID's of input variables.
		 */
		inVarID[0] = ncvarid(InputFile, "PLWCC");
		inVarID[1] = ncvarid(InputFile, "PLWCF_LPC");
		inVarID[2] = ncvarid(InputFile, "HGM232");

	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[0], mindex, (void *)&plwcc);
	xlwcc = plwcc;
	corc1 = 0.0;
	n1 = 0;
	flag1 = 0.0;

		first_time = FALSE;
		}

	/* Get current values of inputs.
	 */
	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[0], mindex, (void *)&plwcc);
	ncvarget1(InputFile, inVarID[1], mindex, (void *)&plwcf_lpc);
	ncvarget1(InputFile, inVarID[2], mindex, (void *)&hgm232);

	if ( hgm232 <= 0.0 )
		{
		n1 = 0;
		plwcf_lpc = xlwcc;
		}

	if ( plwcc <= -32. )
		{
		flag1 = 1.0;
		xlwcc = plwcf_lpc;
		plwcc = plwcf_lpc;
		n1 = 1;
		}

	if ( plwcc <= -0.5 )
		{
		n1 = 0;
		plwcf_lpc = xlwcc;
		}

	n1 = n1 + 1;

	if ( n1 == 2 )
		{
		xlwcc = plwcc;
		corc1 = (plwcf_lpc - xlwcc);
		}

	if ( plwcf_lpc <= 0.01 )

		{
	corc1 = corc1 + 0.004*( plwcf_lpc - xlwcc );
		}

	xlwcc = plwcc + corc1;

	/* Write out new variable.
	 */

	plwcc = xlwcc;

	if ( flag1 == 1.0 )
		{
		flag1 = 0.0;
		plwcc = -32767.0;
		}

	mindex[0] = CurrentOutputRecordNumber;
	ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&plwcc);

}	/* END SLWCC */
  
/* -------------------------------------------------------------------- */

void slwcc1(varp)
VARTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	plwcc1, plwcf_lpc, hgm232;

	static bool	first_time = TRUE;
	static int	inVarID[3], n2;
	static NR_TYPE	xlwcc1, corc2, flag2;

	if (first_time)
		{
		/* Obtain ID's of input variables.
		 */
		inVarID[0] = ncvarid(InputFile, "PLWCC1");
		inVarID[1] = ncvarid(InputFile, "PLWCF_LPC");
		inVarID[2] = ncvarid(InputFile, "HGM232");

	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[0], mindex, (void *)&plwcc1);
	xlwcc1 = plwcc1;
	corc2 = 0.0;
	n2 = 0;
	flag2 = 0.0;

		first_time = FALSE;
		}

	/* Get current values of inputs.
	 */
	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[0], mindex, (void *)&plwcc1);
	ncvarget1(InputFile, inVarID[1], mindex, (void *)&plwcf_lpc);
	ncvarget1(InputFile, inVarID[2], mindex, (void *)&hgm232);


	if ( hgm232 <= 0.0 )
		{
		n2 = 0;
		plwcf_lpc = xlwcc1;
		}

	if ( plwcc1 <= -32. )
		{
		flag2 = 1.0;
		xlwcc1 = plwcf_lpc;
		plwcc1 = plwcf_lpc;
		n2 = 1;
		}

	if ( plwcc1 <= -0.5 )
		{
		n2 = 0;
		plwcf_lpc = xlwcc1;
		}

	n2 = n2 + 1;

	if ( n2 == 2 )
		{
		xlwcc1 = plwcc1;
		corc2 = (plwcf_lpc - plwcc1);
		}

	if ( plwcf_lpc <= 0.01 )

		{
	corc2 = corc2 + 0.004*( plwcf_lpc - xlwcc1 );

		}

	xlwcc1 = plwcc1 + corc2;

	/* Write out new variable.
	 */

	plwcc1 = xlwcc1;

	if ( flag2 == 1.0 )
		{
		flag2 = 0.0;
		plwcc1 = -32767.0;
		}

	mindex[0] = CurrentOutputRecordNumber;
	ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&plwcc1);

}	/* END SLWCC1 */

/* -------------------------------------------------------------------- */

void sgerb(varp)
VARTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	xglwc, plwcf_lpc, hgm232;

	static bool	first_time = TRUE;
	static int	inVarID[3], n3;
	static NR_TYPE	xgerb, corc3, flag3;

	if (first_time)
		{
		/* Obtain ID's of input variables.
		 */
		inVarID[0] = ncvarid(InputFile, "XGLWC");
		inVarID[1] = ncvarid(InputFile, "PLWCF_LPC");
		inVarID[2] = ncvarid(InputFile, "HGM232");

	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[0], mindex, (void *)&xglwc);
	xgerb = xglwc;
	corc3 = 0.0;
	n3 = 0;
	flag3 = 0.0;

		first_time = FALSE;
		}

	/* Get current values of inputs.
	 */
	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[0], mindex, (void *)&xglwc);
	ncvarget1(InputFile, inVarID[1], mindex, (void *)&plwcf_lpc);
	ncvarget1(InputFile, inVarID[2], mindex, (void *)&hgm232);

	if ( hgm232 <= 0.0 )
		{
		n3 = 0;
		plwcf_lpc = xgerb;
		}

	n3 = n3 + 1;

	if ( xglwc <= -32. )
		{
		flag3 = 1.0;
		xglwc = plwcf_lpc;
		n3 = 1;
		}

	if ( n3 == 2 )
		{
		xgerb = xglwc;
		corc3 = (plwcf_lpc - xgerb);
		}

	if ( plwcf_lpc <= 0.01 )

		{
	corc3 = corc3 + 0.004*( plwcf_lpc - xgerb );
		}

	xgerb = xglwc + corc3;

	/* Write out new variable.
	 */

	if ( flag3 == 1.0 )

		{
	flag3 = 0.0;
	xgerb = -32767.0;
		}

	xglwc = xgerb;
	mindex[0] = CurrentOutputRecordNumber;
	ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&xglwc);

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
		inVarID[1] = ncvarid(InputFile, "ALTX");

		first_time = FALSE;
		}

	/* Get current values of inputs.
	 */
	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[0], mindex, (void *)&rstb1);
	ncvarget1(InputFile, inVarID[1], mindex, (void *)&hgm232);

	/* coeffs for INDOEX profile A:

		1.000 / 6.740e-05 / 7.910e-09 / -1.3953e-12 /

		max altitude & above factor = 1.377


	xtsurf = rstb1 * (1.00 + hgm232 * (6.740e-05 + hgm232 * (7.910e-09  - 
		hgm232 * 1.3953e-12)));

	*/


	/* coeffs for  INDOEX profile B:

		0.999 / 8.892e-05 / -6.0674e-09 / -7.1807e-14 /

		max altitude & above factor = 1.300

	xtsurf = rstb1 * (0.999 + hgm232 * (8.892e-05 + hgm232 * (-6.0674e-09  - 
		hgm232 * 7.1807e-14)));

	*/


	/* coeffs for EPIC2001 profile A:

		0.988 / 7.517404e-05 / -3.120012e-09 / -1.440100e-13 /


	   coeffs for EPIC2001 profile B:

		0.9937 / 8.1126406e-05 / -3.530953e-10 / -3.0735328e-13 /

	*/

	xtsurf= rstb1*(0.9937 + hgm232 *(8.1126406e-05 + hgm232 *(-3.530953e-10

		- hgm232 * 3.0735328e-13)));


	/* coeffs for ACE-Asia:

		0.989 / 3.905e-05/   

		max altitude & above factor = 1.22


	xtsurf = (rstb1 * (0.989 + hgm232 * 3.905e-05));

	if ( hgm232 >= 6000. )
		{
		xtsurf = rstb1 * 1.22;
		}

	*/

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
