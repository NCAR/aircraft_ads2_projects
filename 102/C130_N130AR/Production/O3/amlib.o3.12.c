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
	   if( (t >= 0) && (t <= 480) ||
	   	(t >= 578) && (t <= 604) ||
	   	(t >= 638) && (t <= 648) ||
	   	(t >= 657) && (t <= 683) ||
	   	(t >= 717) && (t <= 727) ||
	   	(t >= 902) && (t <= 928) ||
	   	(t >= 962) && (t <= 972) ||
	   	(t >= 3360) && (t <= 3600) ){ teo3c = -32767.0; }
	}

	if( hr == 5 ){
	   if( (t >= 0) && (t <= 180) ||
	   	(t == 314) ||
	   	(t >= 520) && (t <= 548) ||
	   	(t >= 582) && (t <= 592) ||
	   	(t >= 917) && (t <= 918) ||
	   	(t == 919) ||
	   	(t == 921) ||
	   	(t == 929) ||
	   	(t >= 942) && (t <= 1001) ||
	   	(t == 1020) ||
	   	(t >= 1038) && (t <= 1064) || 
	   	(t >= 1098) && (t <= 1108) || 
		(t >= 2280) && (t <= 2520) ){ teo3c = -32767.0; }
	}

	if( hr == 6 ){
	   if( (t >= 48) && (t <= 73) ||
	   	(t >= 107) && (t <= 117) ||
		(t >= 214) && (t <= 242) ||
	   	(t >= 330) && (t <= 480) ||
	   	(t >= 634) && (t <= 660) ||
	   	(t >= 694) && (t <= 704) ||
	   	(t >= 1080) && (t <= 1114) || 
	   	(t >= 1147) && (t <= 1158) || 
	   	(t >= 1980) && (t <= 2010) || 
	   	(t >= 3519) && (t <= 3545) ||
	   	(t >= 3579) && (t <= 3589) ){ teo3c = -32767.0; }
	}

	if( hr == 7 ){
	   if( (t >= 177) && (t <= 202) ||
		(t >= 236) && (t <= 246) ||
	   	(t >= 927) && (t <= 954) ||
		(t >= 988) && (t <= 998) ||
	   	(t >= 1024) && (t <= 1034) ||
	   	(t >= 1560) && (t <= 1570) ||
	   	(t >= 1710) && (t <= 1722) ||
	   	(t >= 1920) && (t <= 1930) ||
		(t >= 2404) && (t <= 2427) ){ teo3c = -32767.0; }
	}

	if( hr == 8 ){
	   if( (t >= 344) && (t <= 409) ||
		(t >= 120) && (t <= 240) ||
		(t >= 442) && (t <= 475) ||
	   	(t >= 3360) && (t <= 3600) ){ teo3c = -32767.0; }
	}

	if( hr == 9 ){
	   if( (t >= 0) && (t <= 60) ||
		(t >= 120) && (t <= 240) ){ teo3c = -32767.0; }
	}

	if( hr == 10 ){
	   if( (t >= 3120) && (t <= 3600) ){ teo3c = -32767.0; }
	}

	if( hr == 11 ){
	   if( (t >= 180) && (t <= 300) ||
		(t >= 2460) && (t <= 2640) ||
	   	(t >= 2760) && (t <= 2820) ){ teo3c = -32767.0; }
	}

	if( hr == 12 ){
	   if( (t >= 1380) && (t <= 3600) ){ teo3c = -32767.0; }
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
