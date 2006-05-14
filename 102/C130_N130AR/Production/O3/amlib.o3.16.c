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

	if( hr == 3 ) { teo3c = -32767.0; }

	if( hr == 4 ){
	   if( (t >= 0) && (t <= 840) ){ teo3c = -32767.0; }
	}

	if( hr == 5 ){
	   if( (t >= 810) && (t <= 1050) ||
		(t >= 2323) && (t <= 2325) ||
		(t >= 2425) && (t <= 2433) ||
		(t >= 2447) && (t <= 2452) ||
		(t >= 2881) && (t <= 2893) ||
		(t >= 2899) && (t <= 2941) ||
		(t >= 2945) && (t <= 2954) ||
		(t >= 2996) && (t <= 3130) ||
	   	(t >= 3205) && (t <= 3216) ||
	   	(t >= 3205) && (t <= 3216) ||
	   	(t >= 3258) && (t <= 3284) ||
	   	(t >= 3307) && (t <= 3313) ||
	   	(t >= 3394) && (t <= 3400) ||
	   	(t >= 3437) && (t <= 3468) ||
	   	(t >= 3541) && (t <= 3600) ){ teo3c = -32767.0; }
	}

	if( hr == 6 ){
	   if( (t >= 0) && (t <= 74) ||
	   	(t >= 155) && (t <= 159) ||
	   	(t >= 348) && (t <= 352) ||
		(t >= 757) && (t <= 775) ||
	   	(t >= 851) && (t <= 856) ||
	   	(t >= 858) && (t <= 870) ||
	   	(t >= 1154) && (t <= 1157) ||
		(t >= 2216) && (t <= 3090) ||
	   	(t >= 3301) && (t <= 3600) ){ teo3c = -32767.0; }
	}

	if( hr == 7 ){ teo3c = -32767.0; }

	if( hr == 8 ){
	   if( (t >= 0) && (t <= 2160) ||
	   	(t >= 3000) && (t <= 3240) ){ teo3c = -32767.0; }
	}

	if( hr == 9 ){
	   if( (t >= 1350) && (t <= 1620) ||
	   	(t >= 1650) && (t <= 1710) ||
	   	(t >= 1800) && (t <= 2070) ||
	   	(t >= 3463) && (t <= 3468) ){ teo3c = -32767.0; }
	}

	if( hr == 11 ){
	   if( (t >= 3420) && (t <= 3600) ||
	   	(t >= 3360) && (t <= 3540) ){ teo3c = -32767.0; }
	}

	if( hr == 12 ){
	   if( (t >= 0) && (t <= 60) ||
	   	(t >= 360) && (t <= 480) ||
	   	(t >= 1052) && (t <= 1060) ||
	   	(t >= 1011) && (t <= 1048) ||
	   	(t == 1131) ||
	   	(t >= 1196) && (t <= 1197) ||
	   	(t == 1402) ||
		(t >= 2580) && (t <= 2670) ||
		(t >= 2880) && (t <= 3600) ){ teo3c = -32767.0; }
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
