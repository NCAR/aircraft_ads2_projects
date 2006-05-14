/*
-------------------------------------------------------------------------
OBJECT NAME:	amlib.c  (new INDOEX version reverted to MAP project coding)

FULL NAME:	

ENTRY POINTS:	SetComputeFunctions()
				saltc()
				sdalt()

STATIC FNS:		none

DESCRIPTION:	Skeleton to fix GPS Altitude

INPUT:			raw GPS measurements, Reference altitude measurement

OUTPUT:			"Corrected" aircraft altitude

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
void saltc(), sdalt();


/* STEP 1: List variables to pass through unaltered by default.  HOUR,
 * MINUTE & SECOND are mandatory.  NULL must be last.
 */
char *passThrough[] = 
  {
  "HOUR",
  "MINUTE",
  "SECOND",
  "GALT",
  "ALT",
  "PALT",
  "HGM232",
  "GSTAT",
  "GMODE",
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
	"ALTC", "M",	1,	1,	   saltc,	"Corrected GPS Altitude",
	"DALT", "M",	1,	1,	   sdalt,	"Calculated Altitude Difference",
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

/*
-------------------------------------------------------------------------
OBJECT NAME:	altc.c

FULL NAME:	Corrected GPS Altitude

ENTRY POINTS:	saltc(), sdalt()

DESCRIPTION:	Create a corrected altitude variable by combining
				the GPS altitude data with IRS altitude when needed

INPUT:		GALT ALT GSTAT GMODE

OUTPUT:		ALTC

REFERENCES:	none

REFERENCED BY:	compute.c

COPYRIGHT:	University Corporation for Atmospheric Research, 1993
-------------------------------------------------------------------------
*/

#include <values.h>
/* #include "nimbus.h" */
/* #include "amlib.h"  */

static NR_TYPE
/*	time constant for exponential average */
/*		Tau     = 300.0;    * 5 minutes */
/*		Tau     = 10.0;   /* 10 seconds */
		Tau     = 1.0;   /* none */
/*  Regression equation to convert from ALT to GALT  */
#define Coef1 0.9786647
#define Coef2 -46.2877565
/*  Number of past calculations to save  */
#define SAVE 10
#define WAIT 10

/* #######  ---------------------------------------------------------------  */

/* #######  NIMBUS stuff not automatically handled by this skeleton program  */
#define LOW_RATE_FEEDBACK 0
#define nFeedBackTypes 2
#define ProcessingRate 25
#define DEG_RAD 0.01745329252
#define MISSING_VALUE -32767.
/*  #######  --------------------------------------------------------------  */

/* #######   Skeleton program needs the following line(s)             */
static NR_TYPE altc, dalt;
static int    FeedBack = 0;
/* #######   Skeleton program needs the preceding line(s)             */

/* -------------------------------------------------------------------- */
void saltc(varp)
/* #######   Skeleton program avoiding the following line(s)
DERTBL  *varp;      /* Information about current variable   */
/* #######   Skeleton program avoiding the preceding line(s)          */
/* #######   Skeleton program needs the following line(s)             */
VARTBL  *varp;      /* Information about current variable   */
  {

  static bool first_time = TRUE;
  static int  inVarID[9];
/* #######   Skeleton program needs the preceding line(s)             */

  int		i, j, k;
  long		gstat, gmode;
  NR_TYPE	galt, alt;
/*     Following probably not needed   */
  NR_TYPE	palt, hgm232;

/* #######   Skeleton program needs the following line(s)             */
  NR_TYPE	hr, min, sec;
  int		ihr, imin, isec, seconds;
  NR_TYPE	fgstat, fgmode;
/* #######   Skeleton program needs the preceding line(s)             */

  NR_TYPE  *tmp;  /* for getting values from the project's Defaults file */

  static bool    firstTime[nFeedBackTypes] = { TRUE, TRUE };
  static NR_TYPE DELT[nFeedBackTypes], time[nFeedBackTypes];
  static NR_TYPE dalt_sav[SAVE];
  static int     badgalt = 0;
  static int     isav = 0;
  static int     countr = 0, goodGPS = 0;

/* #######   Skeleton program needs the following line(s)             */

/* ########   Define diagnostic flag  */
  static bool diag = TRUE;
/* ########   Define diagnostic variable  */
  NR_TYPE newalt;
/* ########                           */

  if (first_time)
    {
    /* Obtain IDs of input variables.
     */
    inVarID[ 0] = ncvarid(InputFile, "GALT");
    inVarID[ 1] = ncvarid(InputFile, "ALT");
    inVarID[ 2] = ncvarid(InputFile, "GSTAT");
    inVarID[ 3] = ncvarid(InputFile, "GMODE");
    inVarID[ 4] = ncvarid(InputFile, "PALT");
    inVarID[ 5] = ncvarid(InputFile, "HGM232");
    inVarID[ 6] = ncvarid(InputFile, "HOUR");
    inVarID[ 7] = ncvarid(InputFile, "MINUTE");
    inVarID[ 8] = ncvarid(InputFile, "SECOND");

/*    Diagnostic prints begin  */
    if (diag)
      {
      printf ("Printing diagnostics.\n");
      }
/*    Diagnostic prints end    */
    first_time = FALSE;

    }

  /* Get current values of inputs.
   */
  mindex[0] = CurrentInputRecordNumber;
  ncvarget1(InputFile, inVarID[ 0], mindex, (void *)&galt);
  ncvarget1(InputFile, inVarID[ 1], mindex, (void *)&alt);
  ncvarget1(InputFile, inVarID[ 2], mindex, (void *)&fgstat);
  ncvarget1(InputFile, inVarID[ 3], mindex, (void *)&fgmode);
  ncvarget1(InputFile, inVarID[ 4], mindex, (void *)&palt);
  ncvarget1(InputFile, inVarID[ 5], mindex, (void *)&hgm232);
  ncvarget1(InputFile, inVarID[ 6], mindex, (void *)&hr);
  ncvarget1(InputFile, inVarID[ 7], mindex, (void *)&min);
  ncvarget1(InputFile, inVarID[ 8], mindex, (void *)&sec);

  gstat = (long)fgstat;
  gmode = (long)fgmode;

  ihr = (int)hr;
  imin = (int)min;
  isec = (int)sec;
/* #######   Skeleton program needs the preceding line(s)             */

/* #######   Skeleton program avoiding the following line(s)
  galt		= GetSample(varp, 0);
  alt		= GetSample(varp, 1);
  gstat		= (long)GetSample(varp, 2);
  gmode		= (long)GetSample(varp, 3);

  palt		= GetSample(varp, 4);
  hgm232	= GetSample(varp, 5);
   #######   Skeleton program avoiding the preceding line(s)          */


/*  If ALT and/or GALT are missing, so is ALTC  */
  if ( (galt == MISSING_VALUE) || (alt == MISSING_VALUE) )
    {
    altc = MISSING_VALUE;
    dalt = 0.;
    }
  else
    {
    if (firstTime[FeedBack])
      {
/*  Get constant values from Defaults file, if available  */

/* #######   Skeleton program avoiding the following line(s)
  (Use values within the function instead of those in the Defaults file.)

      if ((tmp = GetDefaultsValue("GPS_GALT_TAU", varp->name)) == NULL)
        {
        sprintf(buffer, "Value set to %f in AMLIB function slatc.\n", GALT_TAU);
        LogMessage(buffer);
        }
      else
        Tau = tmp[0];
   #######   Skeleton program avoiding the preceding line(s)          */

      if (FeedBack == LOW_RATE_FEEDBACK)
        DELT[FeedBack]	= 1.0;
      else
        DELT[FeedBack]  = 1.0 / ProcessingRate;

/*  Initialize  */
      newalt = alt*Coef1 + Coef2;
      dalt = galt - newalt;
      for (i=0; i<SAVE; i++) dalt_sav[i] = dalt;
      Tau = 1.0/Tau;

      firstTime[FeedBack] = FALSE;
      }

/* Check GPS status; only do this on the Low-rate pass.  */
    if (FeedBack == LOW_RATE_FEEDBACK)
      {
      ++goodGPS;

      if (gstat == 0x0b00)	/* 3 satellites is also considered good	*/
        gstat = 0;

      newalt = alt*Coef1 + Coef2;

    /* Bad GPS Status ?  */
      if (gstat > 0 || gmode < 4 )
        {
        goodGPS = 0;

/*    Diagnostic print begins  */
        if (diag)
          {
          printf("GPS Disabled due to GSTAT/GMODE.\n");
          }
/*    Diagnostic print ends    */
        }

/*  Check for bad GALT  */
/*    Devise a tood test for only one occurrence (lock out for awhile?)
 *     if GALT goes bad one second before GMODE tells me.  I will need
 *     to monitor second-to-second changes for something drastic until
 *     GMODE lets me know.  Should I use GMODE to reset?
 *     What's the best test to find a bad GALT?  (It does tend to approach
 *     zero.)
 */
      if ( badgalt == 0 && (fabs(galt-(newalt+dalt_sav[isav])) > 50. ) )
        {
        goodGPS = 0;
        ++ badgalt;

/*    Diagnostic print begins  */
        if (diag)
          {
          printf("GPS Disabled due to bad GALT.\n");
          }
/*    Diagnostic print ends    */
        }
      }

    if (goodGPS < WAIT)     /* < WAIT seconds of good data before continuing */
      {
/*  Ignore this test for now
      if (countr > 1800.0)      * 30 minutes of operation * 
        {
 */
        if (FeedBack == LOW_RATE_FEEDBACK)
          {

      /* Want to avoid sharp transition to fit because that would
       * produce spike affecting power spectra.  Instead, slowly
       * adjust from last value toward fit value, time constant of
       * about 5 minutes chosen to be small compared to Schuller
       * period but large compared to wavelengths important in
       * spectral analysis.
       */

      /* Above is a noble goal, but for now, let's just do a simple
       * substitution when the data are bad and return when it's not.
       */

          newalt = alt*Coef1 + Coef2;
/*  Former method
          altc = newalt + dalt;
 */
/*  New method  */
          dalt = dalt_sav[0];
          for (i=1; i<SAVE; i++) dalt += dalt_sav[i];
/*   Omit the most recent one, since sometimes GSTAT and GMODE flag late  */
          dalt -= dalt_sav[isav];
          dalt /= (SAVE-1);
          altc = newalt + dalt;

          }
/*
        }
 */
      }
  else
      {
    /* Good GPS comes here.
     */
      badgalt = 0;
      altc = galt;
      newalt = alt*Coef1 + Coef2;

/* Old method (exponential average dalt)
      dalt = galt - (newalt*Tau + dalt*(1.0 - Tau));
 */
/* New method (calculate a running average dalt) */
      dalt = galt - newalt;
      ++isav;
      if (isav >= SAVE) isav = 0;
      dalt_sav[isav] = dalt;
      ++countr;
      }
    }

/*    Diagnostic prints begin  */
  if (diag)
    {
    printf (" %02d:%02d:%02d", ihr, imin, isec);
    printf (" GALT = %f", galt);
    printf (" ALTC = %f", altc);
    printf (" newALT = %f", newalt);
    printf ("  ALT = %f", alt);
    printf (" DALT = %f\n", dalt);

/*
    printf ("  PALT = %f", palt);
    printf ("HGM232 = %f", hgm232);
 */

    printf ("      goodGPS = %d", goodGPS);
    printf (" countr = %d\n", countr);
    }
/*    Diagnostic prints end    */

  mindex[0] = CurrentOutputRecordNumber;
  ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&altc);
/* #######   Skeleton program needs the preceding line(s)             */
/* #######   Skeleton program avoiding the following line(s)
  PutSample(varp, altc[FeedBack]);
   #######   Skeleton program avoiding the preceding line(s)          */

  }	/* END SALTC */

/* -------------------------------------------------------------------- */
/* #######   Skeleton program avoiding the following line(s)
void sdalt(DERTBL *varp)
{
  PutSample(varp, dalt[FeedBack]);
}
   #######   Skeleton program avoiding the preceding line(s)          */
/* #######   Skeleton program needs the following line(s)             */
void sdalt(varp)
VARTBL  *varp;      /* Information about current variable   */
{
  mindex[0] = CurrentOutputRecordNumber;
  ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&dalt);
}
/* #######   Skeleton program needs the preceding line(s)             */

/* -------------------------------------------------------------------- */

/* END ALTC.C */

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
