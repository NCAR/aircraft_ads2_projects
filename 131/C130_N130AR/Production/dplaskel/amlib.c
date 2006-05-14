/* #######   Skeleton program needs the following line(s)             */
/*
-------------------------------------------------------------------------
OBJECT NAME:	amlib.c

FULL NAME:	

ENTRY POINTS:	SetComputeFunctions()
				sdpla()
				srhla()
				sdpla1()
				srhla1()
				sdpdt()

STATIC FNS:		none

DESCRIPTION:	Calculate Dew Point and Relative Humidity
					from Lyman Alpha Absolute Humidity

INPUT:			Derived RHOLA, ATX, PSXC

OUTPUT:			Dew point (DPLA) and relative humidity (RHLA) from Lyman Alpha

REFERENCES:		none

REFERENCED BY:	compute.c

COPYRIGHT:		University Corporation for Atmospheric Research, 2001
-------------------------------------------------------------------------
*/

#include "define.h"
#include <math.h>


/* STEP 4: To appease the compiler, place all AMLIB function names here.
 * By convention, AMLIB functions start with the letter 's'.  These should
 * be the same names you used in steps 2 & 3.
 */
void sdpla(), srhla(), sdpla1(), srhla1();

/*  Test routine  */
void sdpdt();

double esubt(double, double);
static const double Ts = 373.16, To = 273.16;


/* STEP 1: List variables to pass through unaltered by default.  HOUR,
 * MINUTE & SECOND are mandatory.  NULL must be last.
 */
char *passThrough[] = 
  {
  "HOUR",
  "MINUTE",
  "SECOND",
  "RHODT",
  "RHUM",
  "RHOLA",
  "RHOLA1",
  "DPXC",
  "PSXC",
  "ATX",
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
DERVAR  derivftns[] =
  {
/*			Out  Vector			*/
/*  Name	Units Rate Length  Function Title	*/
/* ------------------------------------------------------------------------ */
	"DPLA",  "C",	1,	1,	   sdpla,	"Dew Point (Lyman Alpha)",
	"RHLA",  "%",	1,	1,	   srhla,	"Relative Humidity (Lyman Alpha)",
	"DPLA1", "C",	1,	1,	   sdpla1,	"Dew Point (Lyman Alpha)",
	"RHLA1", "%",	1,	1,	   srhla1,	"Relative Humidity (Lyman Alpha)",
	"DPDT",  "C",	1,	1,	   sdpdt,	"Dew Point from RHODT",
	NULL,	NULL,	0,	0,	   NULL
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

static long mindex[3] = {0, 0, 0};

/* #######   Skeleton program needs the preceding line(s)             */
/*
-------------------------------------------------------------------------
OBJECT NAME:	dpla.c

FULL NAME:	Dew Point from Lyman Alpha Absolute Humidity

ENTRY POINTS:	sdpla(), srhla(), sdpla1(), srhla1(),

DESCRIPTION:	Calculate Dew Point and Relative Humidity
                  from Lyman Alpha Absolute Humidity

INPUT:		Derived:  RHOLA, ATX, PSXC

OUTPUT:		DPLA

REFERENCES:	none

REFERENCED BY:	compute.c

COPYRIGHT:	University Corporation for Atmospheric Research, 2001
-------------------------------------------------------------------------
*/

#include <values.h>
/* #######   Skeleton program avoiding the following line(s)
#include "nimbus.h"
#include "amlib.h"
   #######   Skeleton program avoiding the preceding line(s)          */

static NR_TYPE dpla, rhla;
static NR_TYPE dpla1, rhla1;

/*  Test variable  */
static NR_TYPE dpdt;

/* #######   Skeleton program needs the following line(s)             */
/*  NIMBUS stuff not automatically handled by this skeleton program  */
#define LOW_RATE_FEEDBACK 0
#define nFeedBackTypes 2
#define ProcessingRate 25
#define DEG_RAD 0.01745329252
#define MISSING_VALUE -32767.

static int    FeedBack = 0;
/* #######   Skeleton program needs the preceding line(s)             */

/* -------------------------------------------------------------------- */
void sdpla(varp)
/* #######   Skeleton program avoiding the following line(s)
DERTBL  *varp;      /* Information about current variable   */
/* #######   Skeleton program avoiding the preceding line(s)          */
/* #######   Skeleton program needs the following line(s)             */
VARTBL  *varp;      /* Information about current variable   */
  {

  static bool first_time = TRUE;
  static int  inVarID[11];
  NR_TYPE     hr, min, sec;
  int         ihr, imin, isec, seconds;
  double      ela, z;
/* #######   Skeleton program needs the preceding line(s)             */

  NR_TYPE     *tmp;  /* for getting values from the project's Defaults file */

  static bool    firstTime[nFeedBackTypes] = { TRUE, TRUE };
  static NR_TYPE DELT[nFeedBackTypes], time[nFeedBackTypes];

  NR_TYPE        rhola, rhola1, atx, psxc;
  NR_TYPE        eatx, edpc;

/*  Test variable  */
  NR_TYPE        rhodt;

/* #######   Skeleton program needs the following line(s)             */

  static bool diag = FALSE;  /* Define diagnostic flag  */

  if (first_time)
    {
    /* Obtain IDs of input variables.
     */
    inVarID[ 0] = ncvarid(InputFile, "RHOLA");
    inVarID[ 1] = ncvarid(InputFile, "RHOLA1");
    inVarID[ 2] = ncvarid(InputFile, "ATX");
    inVarID[ 3] = ncvarid(InputFile, "PSXC");
    inVarID[ 4] = ncvarid(InputFile, "RHODT");
    inVarID[ 5] = ncvarid(InputFile, "HOUR");
    inVarID[ 6] = ncvarid(InputFile, "MINUTE");
    inVarID[ 7] = ncvarid(InputFile, "SECOND");

/*    Diagnostic print begins  */
    if (diag)
      {
      printf ("Printing diagnostics.\n");
      }
/*    Diagnostic print ends    */
    first_time = FALSE;

    }

  /* Get current values of inputs.
   */
  mindex[0] = CurrentInputRecordNumber;
  ncvarget1(InputFile, inVarID[ 0], mindex, (void *)&rhola);
  ncvarget1(InputFile, inVarID[ 1], mindex, (void *)&rhola1);
  ncvarget1(InputFile, inVarID[ 2], mindex, (void *)&atx);
  ncvarget1(InputFile, inVarID[ 3], mindex, (void *)&psxc);
  ncvarget1(InputFile, inVarID[ 4], mindex, (void *)&rhodt);
  ncvarget1(InputFile, inVarID[ 5], mindex, (void *)&hr);
  ncvarget1(InputFile, inVarID[ 6], mindex, (void *)&min);
  ncvarget1(InputFile, inVarID[ 7], mindex, (void *)&sec);

  ihr = (int)hr;
  imin = (int)min;
  isec = (int)sec;
/* #######   Skeleton program needs the preceding line(s)             */

/* #######   Skeleton program avoiding the following line(s)
  rhola		= GetSample(varp, 0);
  rhola1	= GetSample(varp, 1);
  atx		= GetSample(varp, 2);
  psxc 		= (long)GetSample(varp, 3);
  rhodt		= GetSample(varp, 4);
/* #######   Skeleton program avoiding the preceding line(s)

/*  If RHOLA and/or ATX are missing, so is DPLA  */
  if ( (rhola == MISSING_VALUE) || (atx == MISSING_VALUE) )
    {
    dpla = MISSING_VALUE;
    }
  else
    {
/* Dew point calculation  */
    if (rhola < 0.001) rhola = 0.001;
    ela = (atx+273.15)*rhola/1322.3;
    if (ela > 0.0)
      {
      z = log(ela);
/*  Use commented code below if frost point is desired below 0C.
        if (atx >= 0.0)
        {
 */
        dpla = 237.3*z/(17.27-z);
/*
        }
      else
        {
        dpla = 273.0*z/(22.51-z);
        }
 *  Use commented code above if frost point is desired below 0C.  */
      }
    else
      {
      dpla = MISSING_VALUE;
      if (diag)
/*    Diagnostic prints begin  */
        {
        printf (" %02d:%02d:%02d", ihr, imin, isec);
        printf ("  ela = %f, atx = %f, rhola = %f, dpla missing.\n", ela, atx, rhola);
        }
/*    Diagnostic prints end    */
      }

/* Relative humidity calculation  */
    if (dpla == MISSING_VALUE)
      {
      rhla = MISSING_VALUE;
      }
    else
      {
      edpc = esubt(dpla,psxc);
      eatx = esubt(atx,psxc);
      if (eatx < 0.0001) eatx = 0.0001;
      rhla = 100.*edpc/eatx;
      }
    }

/*  If RHOLA1 and/or ATX are missing, so is DPLA1  */
  if ( (rhola1 == MISSING_VALUE) || (atx == MISSING_VALUE) )
    {
    dpla1 = MISSING_VALUE;
    }
  else
    {
/* Dew point calculation  */
    if (rhola1 < 0.001) rhola1 = 0.001;
    ela = (atx+273.15)*rhola1/1322.3;
    if (ela > 0.0)
      {
      z = log(ela);
/*  Use commented code below if frost point is desired below 0C.
        if (atx >= 0.0)
        {
 */
        dpla1 = 237.3*z/(17.27-z);
/*
        }
      else
        {
        dpla1 = 273.0*z/(22.51-z);
        }
 *  Use commented code above if frost point is desired below 0C.  */
      }
    else
      {
      dpla1 = MISSING_VALUE;
/*    Diagnostic prints begin  */
      if (diag)
        {
        printf (" %02d:%02d:%02d", ihr, imin, isec);
        printf ("  ela = %f, atx = %f, rhola1 = %f, dpla1 missing.\n", ela, atx, rhola1);
        }
/*    Diagnostic prints end    */
      }

/* Relative humidity calculation  */
    if (dpla1 == MISSING_VALUE)
      {
      rhla1 = MISSING_VALUE;
      }
    else
      {
      edpc = esubt(dpla1,psxc);
      eatx = esubt(atx,psxc);
      if (eatx < 0.0001) eatx = 0.0001;
      rhla1 = 100.*edpc/eatx;
      }
    }

/* Test dew point calculation using reference  */
    if (rhodt < 0.001) rhodt = 0.001;
    ela = (atx+273.15)*rhodt/1322.3;
    if (ela > 0.0)
      {
      z = log(ela);
/*  Use commented code below if frost point is desired below 0C.
      if (atx >= 0.0)
        {
 */
        dpdt = 237.3*z/(17.27-z);
/*
        }
      else
        {
        dpdt = 273.0*z/(22.51-z);
        }
 *  Use commented code above if frost point is desired below 0C.  */
      }
    else
      {
      dpdt = MISSING_VALUE;
      }

/* #######   Skeleton program needs the following line(s)             */
/*    Diagnostic prints begin  */
  if (diag)
    {
    printf (" %02d:%02d:%02d", ihr, imin, isec);
    printf (" RHLA = %f", rhla);
    printf (" RHLA1 = %f", rhla1);
    printf (" DPLA = %f", dpla);
    printf (" DPLA1 = %f", dpla1);

    printf ("  DPDT = %f", dpdt);
    printf ("  PSXC = %f", psxc);
    printf ("   ATX = %f\n", atx);
    }
/*    Diagnostic prints end    */

  mindex[0] = CurrentOutputRecordNumber;
  ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&dpla);
/* #######   Skeleton program needs the preceding line(s)             */
/* #######   Skeleton program avoiding the following line(s)
  PutSample(varp, dpla[FeedBack]);
   #######   Skeleton program avoiding the preceding line(s)          */

  }	/* END SDPLA */

/* -------------------------------------------------------------------- */
/* #######   Skeleton program avoiding the following line(s)
void srhla(DERTBL *varp)
{
  PutSample(varp, rhla[FeedBack]);
}
   #######   Skeleton program avoiding the preceding line(s)          */
/* #######   Skeleton program needs the following line(s)             */
void srhla(varp)
VARTBL  *varp;      /* Information about current variable   */
{
  mindex[0] = CurrentOutputRecordNumber;
  ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&rhla);
}
/* #######   Skeleton program needs the preceding line(s)             */

/* -------------------------------------------------------------------- */

/* END SRHLA */

/* -------------------------------------------------------------------- */
/* #######   Skeleton program avoiding the following line(s)
void sdpdt(DERTBL *varp)
{
  PutSample(varp, dpdt[FeedBack]);
}
   #######   Skeleton program avoiding the preceding line(s)          */
/* #######   Skeleton program needs the following line(s)             */
void sdpdt(varp)
VARTBL  *varp;      /* Information about current variable   */
{
  mindex[0] = CurrentOutputRecordNumber;
  ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&dpdt);
}
/* #######   Skeleton program needs the preceding line(s)             */

/* -------------------------------------------------------------------- */

/* END SDPDT */

/* -------------------------------------------------------------------- */
/* #######   Skeleton program avoiding the following line(s)
void sdpdla1(DERTBL *varp)
{
  PutSample(varp, dpla1[FeedBack]);
}
   #######   Skeleton program avoiding the preceding line(s)          */
/* #######   Skeleton program needs the following line(s)             */
void sdpla1(varp)
VARTBL  *varp;      /* Information about current variable   */
{
  mindex[0] = CurrentOutputRecordNumber;
  ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&dpla1);
}
/* #######   Skeleton program needs the preceding line(s)             */

/* -------------------------------------------------------------------- */

/* END SDPLA1 */

/* -------------------------------------------------------------------- */
/* #######   Skeleton program avoiding the following line(s)
void srhla1(DERTBL *varp)
{
  PutSample(varp, rhla1[FeedBack]);
}
   #######   Skeleton program avoiding the preceding line(s)          */
/* #######   Skeleton program needs the following line(s)             */
void srhla1(varp)
VARTBL  *varp;      /* Information about current variable   */
{
  mindex[0] = CurrentOutputRecordNumber;
  ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&rhla1);
}
/* #######   Skeleton program needs the preceding line(s)             */

/* -------------------------------------------------------------------- */

/* END SRHLA1 */

/* #######   Skeleton program needs the following line(s)             */
/* -------------------------------------------------------------------- */
/* Leave this function alone.  */

double esubt(temperature, pressure)
double  temperature;
double  pressure;
{
    double  Tk = temperature + To;
    double  fw, ew;

    if (Tk < 1.0)
        Tk = 1.0;

    ew = pow(10.0, (
        -7.90298 * ((Ts / Tk) - 1.0)
        +5.02808 * log10(Ts / Tk)
        -1.3816e-7 * (pow(10.0, 11.334 * (1.0 - Tk / Ts)) - 1.0)
        +8.1328e-3 * (pow(10.0, -3.49149 * (Ts / Tk - 1.0)) - 1.0))
        +log10(1013.246) );

    /* Arden Buck's pressure enhancement factor.
     */
    fw = 1.0007 + (3.46e-6 * pressure);

    return(ew * fw);

}   /* END ESUBT */
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
/* #######   Skeleton program needs the preceding line(s)             */
