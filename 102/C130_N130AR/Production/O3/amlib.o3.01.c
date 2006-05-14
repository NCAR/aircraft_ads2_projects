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

	t = (min*60) + sec;

	if( hr == 6 ){ teo3c = -32767.0; }

	if( hr == 7 ){
	   if( (t >= 0) && (t <= 600) ||
	   	(t >= 1980) && (t <= 2580) ||
	   	(t >= 2640) && (t <= 2880) ){ teo3c = -32767.0; }
	}

	if( hr == 8 ){
	   if( (t >= 600) && (t <= 720) ||
	   	(t >= 1740) && (t <= 1980) ||
	   	(t >= 1200) && (t <= 1440) ){ teo3c = -32767.0; }
	}

	if( hr == 9 ){
	   if( (t >= 0) && (t <= 120) ||
	   	(t >= 840) && (t <= 1320) ||
	   	(t >= 1680) && (t <= 1920) ||
	   	(t >= 3120) && (t <= 3360) ||
		(t >= 3480) && (t <= 3600) ){ teo3c = -32767.0; }
	}

	if( hr == 10 ){
	   if( (t >= 60) && (t <= 240) ||
	   	(t >= 3536) && (t <= 3537) ||
	   	(t >= 3120) && (t <= 3180) ){ teo3c = -32767.0; }
	}

	if( hr == 11 ){
	   if( (t >= 3420) && (t <= 3600) ){ teo3c = -32767.0; }
	}

	if( hr == 12 ){
	   if( (t >= 0) && (t <= 300) ||
		(t >= 2640) && (t <= 2880) ||
	   	(t >= 3120) && (t <= 3240) ||
		(t >= 3420) && (t <= 3600) ){ teo3c = -32767.0; }
	}

	if( hr == 13 ){
	   if( (t >= 0) && (t <= 180) ||
	   	(t >= 540) && (t <= 600) ||
	   	(t >= 2040) && (t <= 2760) ||
		(t >= 2940) && (t <= 3060) ||
	   	(t >= 3120) && (t <= 3540) ){ teo3c = -32767.0; }
	}

	if( hr == 14 ){
	   if( (t >= 480) && (t <= 660) ||
		(t >= 1380) && (t <= 1740) ||
	   	(t >= 1980) && (t <= 2340) ||
	   	(t >= 2460) && (t <= 2540) ){ teo3c = -32767.0; }
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
