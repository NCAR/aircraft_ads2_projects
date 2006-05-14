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

	if( hr == 4 ){
	   if( (t >= 0) && (t <= 1860) ||
	   	(t == 2440) ||
	   	(t == 3142) ||
	   	(t == 3210) ||
	   	(t == 3234) ){ teo3c = -32767.0; }
	}

	if( hr == 5 ){
	   if( (t == 468) ||
		(t == 562) ||
	   	(t == 786) ||
		(t >= 902) && (t <= 904) ||
		(t >= 912) && (t <= 917) ||
	   	(t == 1247) ||
	   	(t == 2048) ||
		(t == 2596) ||
	   	(t >= 2700) && (t <= 3300) ||
	   	(t >= 3582) ){ teo3c = -32767.0; }
	}

	if( hr == 6 ){
	   if( (t == 799) ||
		(t == 967) ||
	   	(t == 1075) ||
	   	(t == 1502) ||
	   	(t == 1579) ||
	   	(t == 1826) ||
	   	(t >= 2400) && (t <= 2430) ||
	   	(t == 3543) ){ teo3c = -32767.0; }
	}

	if( hr == 7 ){
	   if( (t == 90) ||
	   	(t == 154) ||
	   	(t == 363) ||
	   	(t == 399) ||
	   	(t == 748) ||
	   	(t == 940) ||
		((t >= 1231) && (t <= 1233)) ||
		(t == 1416) ||
		(t == 1684) ||
		(t >= 2670) && (t <= 3180) ||
	   	(t == 3266) ){ teo3c = -32767.0; }
	}

	if( hr == 8 ){
	   if( (t == 550) ||
		(t == 617) ||
	   	(t == 1077) ||
		(t >= 2280) && (t <= 2520) ||
		(t >= 2880) && (t <= 3060) ||
	   	(t == 3575) ){ teo3c = -32767.0; }
	}

	if( hr == 9 ){
	   if( (t == 43) ||
	   	(t == 552) ||
	   	(t >= 780) && (t <= 1020) ||
	   	(t >= 1350) && (t <= 1500) ||
		(t == 1766) ||
		(t == 2767) ||
		(t == 2783) ){ teo3c = -32767.0; }
	}

	if( hr == 10 ){
	   if( (t == 286) ||
	   	(t >= 360) && (t <= 540) ||
	   	(t == 1050) ||
	   	(t == 1450) ||
	   	(t == 1557) ||
		(t == 1916) ||
		(t >= 2960) && (t <= 2983) ||
	   	(t == 3560) ){ teo3c = -32767.0; }
	}

	if( hr == 11 ){
	   if( (t >= 720) && (t <= 960) ||
		(t >= 1050) && (t <= 1380) ||
	   	(t >= 1454) && (t <= 1460) ||
		(t == 2076) ||
		(t == 2495) ||
	   	(t == 2550) ){ teo3c = -32767.0; }
	}

	if( hr == 12 ){
	   if( (t >= 300) && (t <= 840) ||
		(t == 854) ||
		(t >= 1320) && (t <= 1680) ||
		(t >= 2040) && (t <= 2220) ||
		(t == 2664) ||
		(t == 2674) ||
		(t == 2870) ||
	   	(t >= 3175) && (t <= 3600) ){ teo3c = -32767.0; }
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
