/*
-------------------------------------------------------------------------
OBJECT NAME:	amlib.c

FULL NAME:	

ENTRY POINTS:	SetComputeFunctions()
		steo3c()

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


/* STEP 4: To appease the compiler, place all AMLIB function names here.
 * By convention, AMLIB functions start with the letter 's'.  These should
 * be the same names you used in steps 2 & 3.
 */
void	steo3c();


/* STEP 1: List variables to pass through unaltered by default.  HOUR,
 * MINUTE & SECOND are mandatory.  NULL must be last.
 */
char	*passThrough[] = 
	{
	"HOUR",
	"INTEO3",
	"MINUTE",
	"SECOND",
	"TEO3",
	"TET",
	"TEP",
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
  "TEO3C",	"ppbv",	1,	1,	   steo3c,	"Ozone Mixing Ratio",
/*  "XNOYM",	"ppm",	50,	1,	   sxnoym,	"Nitro Measured", */
  NULL,		NULL,	0,	0,	   NULL
  };


/* STEP3: Create AMLIB functions here.  Use these examples as templates.
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
void steo3c(varp)
VARTBL	*varp;		/* Information about current variable	*/
{
/* O3 special edits 
   Bad Data Flag is -32767.0
*/
	NR_TYPE	teo3c;

	static int	inVarID[4];
	static	bool	first_time = TRUE;
	float		hr,min,sec,t;

	if (first_time)
		{
		/* Obtain ID's of input variables.
		 */
		inVarID[0] = ncvarid(InputFile, "TEO3C");
		inVarID[1] = ncvarid(InputFile, "HOUR");
		inVarID[2] = ncvarid(InputFile, "MINUTE");
		inVarID[3] = ncvarid(InputFile, "SECOND");

		}

	/* Get current values of inputs.
	 */
	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[0], mindex, (void *)&teo3c);
	ncvarget1(InputFile, inVarID[1], mindex, (void *)&hr);
	ncvarget1(InputFile, inVarID[2], mindex, (void *)&min);
	ncvarget1(InputFile, inVarID[3], mindex, (void *)&sec);

	

	if( hr == 16 ){
		if( (min == 55) && (sec >= 59) ){ teo3c = -32767.0; }
		if( (min == 56) && (sec <= 9) ){ teo3c = -32767.0; }
	}

	if( hr == 17 ){
		if( (min == 21) && (sec >= 50) ){ teo3c = -32767.0; }
		if( (min == 22) && (sec <= 0) ){ teo3c = -32767.0; }
	}
	if( hr == 18 ){
		if( min == 6 ){ teo3c = -32767.0; }
		if( (min == 7) && (sec <= 0) ){ teo3c = -32767.0; }
		if( min == 17 ) {
			if( (sec >= 20) && (sec <= 45) ){ teo3c = -32767.0; }
		}
		if( min == 18 ) {
			if( (sec >= 6) && (sec <= 7) ){ teo3c = -32767.0; }
		}
	}

	/* Write out new variable.
	 */
	mindex[0] = CurrentOutputRecordNumber;

	ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&teo3c);

}	/* END steo3c */

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
