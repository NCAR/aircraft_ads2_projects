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
	"MINUTE",
	"SECOND",
	"TEO3",
	"INTEO3",
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

	static int	inVarID[7];
	static	bool	first_time = TRUE;
	float		hr,min,sec,teo3,t,p;

	if (first_time)
		{
		/* Obtain ID's of input variables.
		 */
		inVarID[0] = ncvarid(InputFile, "TEO3C");
		inVarID[1] = ncvarid(InputFile, "HOUR");
		inVarID[2] = ncvarid(InputFile, "MINUTE");
		inVarID[3] = ncvarid(InputFile, "SECOND");
		inVarID[4] = ncvarid(InputFile, "TEO3");
		inVarID[5] = ncvarid(InputFile, "TET");
		inVarID[6] = ncvarid(InputFile, "TEP");

		}

	/* Get current values of inputs.
	 */
	mindex[0] = CurrentInputRecordNumber;
	ncvarget1(InputFile, inVarID[0], mindex, (void *)&teo3c);
	ncvarget1(InputFile, inVarID[1], mindex, (void *)&hr);
	ncvarget1(InputFile, inVarID[2], mindex, (void *)&min);
	ncvarget1(InputFile, inVarID[3], mindex, (void *)&sec);
	ncvarget1(InputFile, inVarID[4], mindex, (void *)&teo3);
	ncvarget1(InputFile, inVarID[5], mindex, (void *)&t);
	ncvarget1(InputFile, inVarID[6], mindex, (void *)&p);

	
	p -= 20;
	if( hr == 7 ){
		if ( (min >= 26) && (min <= 38) ){
			teo3c = teo3*1013/p*(273.15+t)/273.15;
			if( min == 26 && (sec >= 34 && sec <= 55) ){ 
				teo3c = -32767.0;
			}
		}
	}
	if( hr == 6 ){
		if( (min == 54) && (sec == 38 || sec == 39 ) ){
			teo3c = -32767.0;
		}
	}
	if( hr == 8 ){
		if( (min == 56) && (sec == 55 || sec == 56 ) ){
			teo3c = -32767.0;
		}
		if( (min == 58) && (sec >= 37 && sec <= 53 ) ){
			teo3c = -32767.0;
		}
	}
	if( hr == 9 ){
		if( (min == 50) && (sec >= 50 && sec <= 55 ) ){
			teo3c = -32767.0;
		}
		if( (min == 51) && (sec >= 33 && sec <= 34 ) ){
			teo3c = -32767.0;
		}
		if( (min == 54) && (sec >= 38 && sec <= 39 ) ){
			teo3c = -32767.0;
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
