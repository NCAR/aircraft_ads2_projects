/*  Skeleton program to "fix" the high-rate FSSP data for 4-870 WISP-94 */
/*   Written by Chris Webster and Ron Ruth                February 1995 */

/*  Cut and Paste the next line.
:set ts=4
-------------------------------------------------------------------------
OBJECT NAME:	amlib.c

FULL NAME:	

ENTRY POINTS:	SetComputeFunctions()
				sfsspc()

STATIC FNS:		none

DESCRIPTION:	Template for AMLIB std & user directories.

INPUT:			

OUTPUT:			

REFERENCES:		none

REFERENCED BY:	compute.c

COPYRIGHT:		University Corporation for Atmospheric Research, 1993,1994
--------------------------------------------------------------------------
*/

#include "define.h"

/*  FSSP Probe constants used for this project */
#define M_PI	3.1415926536
#define PLWFAC	0.000001
#define DOF		2.81
#define BDIA	0.19

/*  FSSP cell size array for all four ranges */
float	fssp_csiz[4][16] = { 0.0,3.5,6.5,9.5,12.5,15.5,18.5,21.5,24.5, 27.5,30.5,33.5,36.5,39.5,42.5,45.5, 0.0,3.0,5.0,7.0,9.0,11.0,13.0,15.0,17.0,19.0, 21.0,23.0,25.0,27.0,29.0,31.0,0.0,1.,2.,3.,4.,5.,6.,7.,8.,9.,10.,11.,12.,13.,14.,15., 0.0,.5,1.,1.5,2.,2.5,3.,3.5,4.,4.5,5.,5.5,6.,6.5,7.,7.5 };

/* Step 4: To appease the compiler, place all AMLIB function names here.
 * By convention, AMLIB functions start with the letter 's'.  These should
 * be the same names you used in steps 2 & 3.
 */
void scfsspc();
void sconcfc();
void sdbarfc();
void sdispfc();
void splwcfc();
void sfactc();
void sfstbc();
void sbmfrc();

/* Step 1: List variables to pass through unaltered by default.  HOUR,
 * MINUTE & SECOND are mandatory.  NULL must be last.
 */
char	*passThrough[] = 
	{
	"HOUR",
	"MINUTE",
	"SECOND",
	NULL
	};

/* Step 2: New variables get added here.  Also add existing variables that
 * you would like to enter additional processing for.  Remove the examples
 * given below, leaving the "NULL, NULL" line in.  Make sure to leave off
 * the parentheses from the function names.
 */
DERVAR	derivftns[] =
	{
	"CFSSP_IBL",	scfsspc,
	"CONCF_IBL",	sconcfc,
	"DISPF_IBL",	sdispfc,
	"DBARF_IBL",	sdbarfc,
	"PLWCF_IBL",	splwcfc,
	"FACT_IBL",		sfactc,
	"FSTB_IBL",		sfstbc,
	"FBMFR_IBL",	sbmfrc,
	NULL,			NULL
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

static NR_TYPE	concf[10], fbmfr[10], dbar[10], disp[10], plwc[10],
				fact[10], fstb[10], fbmfr[10];

/* -------------------------------------------------------------------- */
void scfsspc(varp)
/*  New per-cell concentrations */
VARTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	cfssp[10][16], afssp[10][16], tas[25], range[10];
	NR_TYPE	dbar2, conc, sv, *dia, total_act, total_stb;

	long	start[3], count[3];
	int		i, cell, irange;

	static bool	first_time = TRUE;
	static int	inVarID[8];
	static NR_TYPE sa;

	if (first_time)
		{
		/* Obtain ID's of input variables.
		 */
		inVarID[0] = ncvarid(InputFile, "AFSSP_IBL");
		inVarID[1] = ncvarid(InputFile, "TASX");
		inVarID[2] = ncvarid(InputFile, "FACT_IBL");
		inVarID[3] = ncvarid(InputFile, "FSTB_IBL");
		inVarID[4] = ncvarid(InputFile, "FRNG_IBL");
		sa = DOF * BDIA;

		first_time = FALSE;
		}

	start[0] = CurrentInputRecordNumber, start[1] = start[2] = 0;
	count[0] = 1, count[1] = 10, count[2] = 16;
	ncvarget(InputFile, inVarID[0], start, count, (void *)afssp);

	count[0] = 1, count[1] = 10,	count[2] = 0;
	ncvarget(InputFile, inVarID[2], start, count, (void *)fact);
	ncvarget(InputFile, inVarID[3], start, count, (void *)fstb);
	ncvarget(InputFile, inVarID[4], start, count, (void *)range);

	count[0] = 1, count[1] = 25,	count[2] = 0;
	ncvarget(InputFile, inVarID[1], start, count, (void *)tas);

	/* Average Strobes, and Activity, since they are what was f***** up.
	 */
	total_act = total_stb = 0.0;

	for (i = 0; i < 10; ++i)
		{
		total_act += fact[i];
		total_stb += fstb[i];
		}

	/*  According to Darrel Baumgardner (email 7 Feb 1995), the
	 *   activity value should be divided by the sample period,
         *   since it is the fraction of the time period that the
         *   probe was busy.  In this case, if I avoid the divide
         *   by 10 in the next loop, I've effected the same thing. */
	for (i = 0; i < 10; ++i)
		{
		fact[i] = total_act;
	/*	fact[i] = (total_act / 10.) / 0.1 */;
		fstb[i] = total_stb / 10;
		}

	/* Generate new FBMFR.
	 */
	for (i = 0; i < 10; ++i)
		{
		total_act = 0.0;

		for (cell = 1; cell < 16; ++cell)
			total_act += afssp[i][cell];

		if (fstb[i] != 0.0)
			fbmfr[i] = total_act / fstb[i];

		if (fstb[i] < 1.0 || total_act <= 0.0)
			fbmfr[i] = 0.4;
		}


	/* Generate new FSSP derived parameters.
	 */
	for (i = 0; i < 10; ++i)
		{
		sv = sa * tas[i * 2]*0.1 * fbmfr[i] * (1.0 - (fact[i] * 0.71));
/*                                  ----  because of sample rate data */

		dia = fssp_csiz[(int)(range[i]+.5)];

		plwc[i] = disp[i] = dbar[i] = dbar2 = concf[i] = 0.0;

		for (cell = 1; cell < 15; ++cell)
			if (sv == 0.0)
				cfssp[i][cell] = 0.0;
			else
				{
				cfssp[i][cell] = conc = afssp[i][cell] / sv;
				dbar[i]		+= dia[cell] * conc;
				dbar2		+= dia[cell] * dia[cell] * conc;
				plwc[i]		+= dia[cell] * dia[cell] * dia[cell]* conc;
				concf[i]	+= conc;
				}

		plwc[i] *= M_PI / 6.0 * PLWFAC;

		if (concf[i] > 0.0001)
			{
			dbar[i] /= concf[i];
	
			disp[i] = (NR_TYPE)sqrt(fabs((double)(dbar2 /
				concf[i] - dbar[i] * dbar[i]))) / dbar[i];
			}
		}

	start[0] = CurrentOutputRecordNumber;
	count[0] = 1, count[1] = 10,	count[2] = 16;
	ncvarput(OutputFile, varp->outVarID, start, count, (void *)cfssp);

}

/* -------------------------------------------------------------------- */
void sconcfc(varp)
/*  New total concentration */
VARTBL	*varp;		/* Information about current variable	*/
{
	long	start[3], count[3];

	start[0] = CurrentOutputRecordNumber;
	start[1] = start[2] = 0;
	count[0] = 1, count[1] = 10,	count[2] = 0;
	ncvarput(OutputFile, varp->outVarID, start, count, (void *)concf);
}

/* -------------------------------------------------------------------- */
void sdbarfc(varp)
/*  New mean diameter */
VARTBL	*varp;		/* Information about current variable	*/
{
	long	start[3], count[3];

	start[0] = CurrentOutputRecordNumber;
	start[1] = start[2] = 0;
	count[0] = 1, count[1] = 10,	count[2] = 0;
	ncvarput(OutputFile, varp->outVarID, start, count, (void *)dbar);
}

/* -------------------------------------------------------------------- */
void sdispfc(varp)
/*  New dispersion */
VARTBL	*varp;		/* Information about current variable	*/
{
	long	start[3], count[3];

	start[0] = CurrentOutputRecordNumber;
	start[1] = start[2] = 0;
	count[0] = 1, count[1] = 10,	count[2] = 0;
	ncvarput(OutputFile, varp->outVarID, start, count, (void *)disp);
}

/* -------------------------------------------------------------------- */
void splwcfc(varp)
/*  New liquid water content */
VARTBL	*varp;		/* Information about current variable	*/
{
	long	start[3], count[3];

	start[0] = CurrentOutputRecordNumber;
	start[1] = start[2] = 0;
	count[0] = 1, count[1] = 10,	count[2] = 0;
	ncvarput(OutputFile, varp->outVarID, start, count, (void *)plwc);
}

/* -------------------------------------------------------------------- */
void sfactc(varp)
/*  New activity */
VARTBL	*varp;		/* Information about current variable	*/
{
	long	start[3], count[3];

	start[0] = CurrentOutputRecordNumber;
	start[1] = start[2] = 0;
	count[0] = 1, count[1] = 10,	count[2] = 0;
	ncvarput(OutputFile, varp->outVarID, start, count, (void *)fact);
}

/* -------------------------------------------------------------------- */
void sfstbc(varp)
/*  New strobes */
VARTBL	*varp;		/* Information about current variable	*/
{
	long	start[3], count[3];

	start[0] = CurrentOutputRecordNumber;
	start[1] = start[2] = 0;
	count[0] = 1, count[1] = 10,	count[2] = 0;
	ncvarput(OutputFile, varp->outVarID, start, count, (void *)fstb);
}

/* -------------------------------------------------------------------- */
void sbmfrc(varp)
/*  New beam fraction */
VARTBL	*varp;		/* Information about current variable	*/
{
	long	start[3], count[3];

	start[0] = CurrentOutputRecordNumber;
	start[1] = start[2] = 0;
	count[0] = 1, count[1] = 10,	count[2] = 0;
	ncvarput(OutputFile, varp->outVarID, start, count, (void *)fbmfr);
}

/* -------------------------------------------------------------------- */
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
