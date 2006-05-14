/*
-------------------------------------------------------------------------
OBJECT NAME:	amlib.c

FULL NAME:	

ENTRY POINTS:	SetComputeFunctions()
		sthdg()
		sconcn()
		suvb()
		sswb()
		sws()
		swsc()
		swd()
		swdc()
		sconcf()
		sui()
		suic()
		svi()
		svic()

STATIC FNS:	none

DESCRIPTION:	Template for AMLIB std & user directories.

INPUT:			

OUTPUT:			

REFERENCES:	none

REFERENCED BY:	compute.c

COPYRIGHT:	University Corporation for Atmospheric Research, 1993,1994
-------------------------------------------------------------------------
*/

#include "define.h"
#include "math.h"


/* STEP 4: To appease the compiler, place all AMLIB function names here.
 * By convention, AMLIB functions start with the letter 's'.  These should
 * be the same names you used in steps 2 & 3.
 */
void	sthdg();
void	sconcn();
void	suvb();
void	sswb();
void	sws();
void	swsc();
void	swd();
void	swdc();
void	sconcf();
void	sui();
void	suic();
void	svi();
void	svic();


/* STEP 1: List variables to pass through unaltered by default.  HOUR,
 * MINUTE & SECOND are mandatory.  NULL must be last.
 */
char	*passThrough[] = 
	{
	"HOUR",
	"MINUTE",
	"SECOND",
	"GVEW",
	"GVNS",
	NULL
	};


/* STEP 2: New variables get added here.  Also add existing variables that
 * you would like to enter additional processing for.  Remove the examples
 * given below, leaving the "NULL, NULL" line in.  Make sure to leave off
 * the parentheses from the function names.
 *
 * Attributes: (Units, Rate, Vector Len, & Title) only need to be defined
 * for variables that do NOT exist in the input file.
 */
DERVAR	derivftns[] =
	{
/*			Out  Vector			*/
/*  Name	Units Rate Length  Function Title	*/
/* ------------------------------------------------------------------------ */
  "GHDG",	"deg",	1,	1,	   sthdg,	"",

  "CONCN",	"N/cm3",	1,	1,	   sconcn,	"",

  "UVB",	"W/m2",	1,	1,	   suvb,	"",

  "SWB",	"W/m2",	1,	1,	   sswb,	"",

  "WS",		"m/s",	1,	1,	   sws,	"",

  "WSC",	"m/s",	1,	1,	   swsc,	"",

  "WD",	"deg",	1,	1,	   swd,	"",

  "WDC",	"deg",	1,	1,	   swdc,	"",

  "CONCF_IBL",	"N/cm3",	1,	1,	sconcf,	"",

  "UI",		"m/s",	1,	1,	   sui,	"",

  "UIC",	"m/s",	1,	1,	   suic,	"",

  "VI",	"m/s",	1,	1,	   svi,	"",

  "VIC",	"m/s",	1,	1,	   svic,	"",

  NULL,		NULL,	0,	0,	   NULL

  };


/* STEP 3: Create AMLIB functions here.  Use these examples as templates.
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
void sthdg(varp)
VARTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	gvew, gvns, ghdg;

	static bool	first_time = TRUE;
	static int	inVarID[2];
	static NR_TYPE	gtot, gsin, gvew2, gvns2, gtot2;

	if (first_time)
		{
		/* Obtain ID's of input variables.
		 */
		inVarID[0] = ncvarid(InputFile, "GVEW");
		inVarID[1] = ncvarid(InputFile, "GVNS");
     
		first_time = FALSE;
		}

	/* Get current values of inputs.
	 */
	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[0], mindex, (void *)&gvew);
	ncvarget1(InputFile, inVarID[1], mindex, (void *)&gvns);


	gvew2 = gvew*gvew;

	gvns2 = gvns*gvns;

	gtot2 = gvew2 + gvns2;

	gtot =  sqrt((double)gtot2);

	gsin = gvew/gtot; 

	ghdg =  asin((double)gsin)*57.3;


	/* Write out new variable.
	 */
	mindex[0] = CurrentOutputRecordNumber;
	ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&ghdg);
}	/* END STHDG */

/*------------------------------------------------------------------*/
void sconcn(varp)
VARTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	gvew, gvns, concn;

	static bool	first_time = TRUE;
	static int	inVarID[2];

	if (first_time)
		{
		/* Obtain ID's of input variables.
		 */
		inVarID[0] = ncvarid(InputFile, "GVEW");
		inVarID[1] = ncvarid(InputFile, "GVNS");
     
		first_time = FALSE;
		}

	/* Get current values of inputs.
	 */
	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[0], mindex, (void *)&gvew);
	ncvarget1(InputFile, inVarID[1], mindex, (void *)&gvns);


	concn = -32767.;



	/* Write out new variable.
	 */
	mindex[0] = CurrentOutputRecordNumber;
	ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&concn);
}	/* END SCONCN */


/* -------------------------------------------------------------------- */

void suvb(varp)
VARTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	gvew, gvns, uvb;

	static bool	first_time = TRUE;
	static int	inVarID[2];

	if (first_time)
		{
		/* Obtain ID's of input variables.
		 */
		inVarID[0] = ncvarid(InputFile, "GVEW");
		inVarID[1] = ncvarid(InputFile, "GVNS");
     
		first_time = FALSE;
		}

	/* Get current values of inputs.
	 */
	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[0], mindex, (void *)&gvew);
	ncvarget1(InputFile, inVarID[1], mindex, (void *)&gvns);


	uvb = -32767.;


	/* Write out new variable.
	 */
	mindex[0] = CurrentOutputRecordNumber;
	ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&uvb);
}	/* END SUVB */

/*--------------------------------------------------------------------*/

void sswb(varp)
VARTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	gvew, gvns, swb;

	static bool	first_time = TRUE;
	static int	inVarID[2];

	if (first_time)
		{
		/* Obtain ID's of input variables.
		 */
		inVarID[0] = ncvarid(InputFile, "GVEW");
		inVarID[1] = ncvarid(InputFile, "GVNS");
     
		first_time = FALSE;
		}

	/* Get current values of inputs.
	 */
	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[0], mindex, (void *)&gvew);
	ncvarget1(InputFile, inVarID[1], mindex, (void *)&gvns);


	swb  = -32767.;



	/* Write out new variable.
	 */
	mindex[0] = CurrentOutputRecordNumber;
	ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&swb);
}	/* END SSWB */


/* -------------------------------------------------------------------- */

void sws(varp)
VARTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	gvew, gvns, ws;

	static bool	first_time = TRUE;
	static int	inVarID[2];

	if (first_time)
		{
		/* Obtain ID's of input variables.
		 */
		inVarID[0] = ncvarid(InputFile, "GVEW");
		inVarID[1] = ncvarid(InputFile, "GVNS");
     
		first_time = FALSE;
		}

	/* Get current values of inputs.
	 */
	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[0], mindex, (void *)&gvew);
	ncvarget1(InputFile, inVarID[1], mindex, (void *)&gvns);


	ws = -32767.;



	/* Write out new variable.
	 */
	mindex[0] = CurrentOutputRecordNumber;
	ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&ws);
}	/* END SWS */


/* -------------------------------------------------------------------- */

void swsc(varp)
VARTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	gvew, gvns, wsc;

	static bool	first_time = TRUE;
	static int	inVarID[2];

	if (first_time)
		{
		/* Obtain ID's of input variables.
		 */
		inVarID[0] = ncvarid(InputFile, "GVEW");
		inVarID[1] = ncvarid(InputFile, "GVNS");
     
		first_time = FALSE;
		}

	/* Get current values of inputs.
	 */
	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[0], mindex, (void *)&gvew);
	ncvarget1(InputFile, inVarID[1], mindex, (void *)&gvns);


	wsc = -32767.;



	/* Write out new variable.
	 */
	mindex[0] = CurrentOutputRecordNumber;
	ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&wsc);
}	/* END SWSC */


/* -------------------------------------------------------------------- */

void swd(varp)
VARTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	gvew, gvns, wd;

	static bool	first_time = TRUE;
	static int	inVarID[2];

	if (first_time)
		{
		/* Obtain ID's of input variables.
		 */
		inVarID[0] = ncvarid(InputFile, "GVEW");
		inVarID[1] = ncvarid(InputFile, "GVNS");
     
		first_time = FALSE;
		}

	/* Get current values of inputs.
	 */
	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[0], mindex, (void *)&gvew);
	ncvarget1(InputFile, inVarID[1], mindex, (void *)&gvns);


	wd = -32767.;



	/* Write out new variable.
	 */
	mindex[0] = CurrentOutputRecordNumber;
	ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&wd);
}	/* END SWD */


/* -------------------------------------------------------------------- */

void swdc(varp)
VARTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	gvew, gvns, wdc;

	static bool	first_time = TRUE;
	static int	inVarID[2];

	if (first_time)
		{
		/* Obtain ID's of input variables.
		 */
		inVarID[0] = ncvarid(InputFile, "GVEW");
		inVarID[1] = ncvarid(InputFile, "GVNS");
     
		first_time = FALSE;
		}

	/* Get current values of inputs.
	 */
	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[0], mindex, (void *)&gvew);
	ncvarget1(InputFile, inVarID[1], mindex, (void *)&gvns);


	wdc = -32767.;



	/* Write out new variable.
	 */
	mindex[0] = CurrentOutputRecordNumber;
	ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&wdc);
}	/* END SWDC */


/* -------------------------------------------------------------------- */

void sconcf(varp)
VARTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	gvew, gvns, concf_ibl;

	static bool	first_time = TRUE;
	static int	inVarID[2];

	if (first_time)
		{
		/* Obtain ID's of input variables.
		 */
		inVarID[0] = ncvarid(InputFile, "GVEW");
		inVarID[1] = ncvarid(InputFile, "GVNS");
     
		first_time = FALSE;
		}

	/* Get current values of inputs.
	 */
	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[0], mindex, (void *)&gvew);
	ncvarget1(InputFile, inVarID[1], mindex, (void *)&gvns);


	concf_ibl = -32767.;



	/* Write out new variable.
	 */
	mindex[0] = CurrentOutputRecordNumber;
	ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&concf_ibl);
}	/* END SCONCF */


/* -------------------------------------------------------------------- */

void sui(varp)
VARTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	gvew, gvns, ui;

	static bool	first_time = TRUE;
	static int	inVarID[2];

	if (first_time)
		{
		/* Obtain ID's of input variables.
		 */
		inVarID[0] = ncvarid(InputFile, "GVEW");
		inVarID[1] = ncvarid(InputFile, "GVNS");
     
		first_time = FALSE;
		}

	/* Get current values of inputs.
	 */
	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[0], mindex, (void *)&gvew);
	ncvarget1(InputFile, inVarID[1], mindex, (void *)&gvns);


	ui = -32767.;



	/* Write out new variable.
	 */
	mindex[0] = CurrentOutputRecordNumber;
	ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&ui);
}	/* END SUI */


/* -------------------------------------------------------------------- */

void suic(varp)
VARTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	gvew, gvns, uic;

	static bool	first_time = TRUE;
	static int	inVarID[2];

	if (first_time)
		{
		/* Obtain ID's of input variables.
		 */
		inVarID[0] = ncvarid(InputFile, "GVEW");
		inVarID[1] = ncvarid(InputFile, "GVNS");
     
		first_time = FALSE;
		}

	/* Get current values of inputs.
	 */
	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[0], mindex, (void *)&gvew);
	ncvarget1(InputFile, inVarID[1], mindex, (void *)&gvns);


	uic = -32767.;



	/* Write out new variable.
	 */
	mindex[0] = CurrentOutputRecordNumber;
	ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&uic);
}	/* END SUIC */


/* -------------------------------------------------------------------- */

void svi(varp)
VARTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	gvew, gvns, vi;

	static bool	first_time = TRUE;
	static int	inVarID[2];

	if (first_time)
		{
		/* Obtain ID's of input variables.
		 */
		inVarID[0] = ncvarid(InputFile, "GVEW");
		inVarID[1] = ncvarid(InputFile, "GVNS");
     
		first_time = FALSE;
		}

	/* Get current values of inputs.
	 */
	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[0], mindex, (void *)&gvew);
	ncvarget1(InputFile, inVarID[1], mindex, (void *)&gvns);


	vi = -32767.;



	/* Write out new variable.
	 */
	mindex[0] = CurrentOutputRecordNumber;
	ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&vi);
}	/* END SVI */


/* -------------------------------------------------------------------- */

void svic(varp)
VARTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	gvew, gvns, vic;

	static bool	first_time = TRUE;
	static int	inVarID[2];

	if (first_time)
		{
		/* Obtain ID's of input variables.
		 */
		inVarID[0] = ncvarid(InputFile, "GVEW");
		inVarID[1] = ncvarid(InputFile, "GVNS");
     
		first_time = FALSE;
		}

	/* Get current values of inputs.
	 */
	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[0], mindex, (void *)&gvew);
	ncvarget1(InputFile, inVarID[1], mindex, (void *)&gvns);


	vic = -32767.;



	/* Write out new variable.
	 */
	mindex[0] = CurrentOutputRecordNumber;
	ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&vic);
}	/* END SVIC */


/* -------------------------------------------------------------------- */
/* Leave this function alone.
 */
void SetComputeFunctions()
{
  int	i, indx;

  for (i = 0; derivftns[i].Compute; ++i)
    if ((indx = SearchTable(Variable,nVariables,derivftns[i].Name)) != ERR)
      Variable[indx]->Compute = derivftns[i].Compute;

}	/* END SETCOMPUTEFUNCTIONS */

/* END AMLIB.C */
