/*
-------------------------------------------------------------------------
OBJECT NAME:	amlib.c  (new INDOEX version)

FULL NAME:	

ENTRY POINTS:	SetComputeFunctions()
				slatc()
				slonc()
				svewc()
				svnsc()

STATIC FNS:		none

DESCRIPTION:	Skeleton to test/fix GPS-correction code

INPUT:			GPS raw measurements, IRS raw measurements

OUTPUT:			"Corrected" position and ground speed measurements

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
void slatc(), slonc(), svewc(), svnsc();


/* STEP 1: List variables to pass through unaltered by default.  HOUR,
 * MINUTE & SECOND are mandatory.  NULL must be last.
 */
char *passThrough[] = 
  {
  "HOUR",
  "MINUTE",
  "SECOND",
  "LAT",
  "LON",
  "VEW",
  "VNS",
  "ROLL",
  "GLAT",
  "GLON",
  "GVEW",
  "GVNS",
  "GSTAT",
  "GMODE",
  "XLATC",
  "XLONC",
  "XVEWC",
  "XVNSC",
  "LATC",
  "LONC",
  "VEWC",
  "VNSC",
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
	"LATCX", "deg",	1,	1,	   slatc,	"Reworked Corrected Latitude",
	"LONCX", "deg",	1,	1,	   slonc,	"Reworked Corrected Longitude",
	"VEWCX", "M/s",	1,	1,	   svewc,	"Reworked Corrected GS East Comp.",
	"VNSCX", "M/s",	1,	1,	   svnsc,	"Reworked Corrected GS North Comp.",
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

/* #######   NIMBUS gpsc.c file merged in here */

/*
-------------------------------------------------------------------------
OBJECT NAME:	gpsc.c

FULL NAME:	Corrected IRS/GPS

ENTRY POINTS:	slatc(), slonc(), svnsc(), svewc()

STATIC FNS:	matrix_inversion()

DESCRIPTION:	Create a corrected set of Reference variables by combining
		the IRS and GPS data.

INPUT:		LAT LON GLAT GLON VNS VEW GVNS GVEW ROLL GSTAT GMODE

OUTPUT:		LATC LONC VNSC VEWC

REFERENCES:	none

REFERENCED BY:	compute.c

COPYRIGHT:	University Corporation for Atmospheric Research, 1993
-------------------------------------------------------------------------
*/

#include <values.h>
/* #include "nimbus.h" */
/* #include "amlib.h"  */

static NR_TYPE
		UPFCTR	= 0.999444,
/*		FCTRF	= 0.997,           */
		FCTRF	= 0.5,
		CDM	= 111120.0,
/* #######                     Stuff with which I want to play               *
		TAUP	= 100.0,
		ROLL_MAX = 40.0;
		ROLL_MAX = 90.0;
 */
		TAUP	= 600.0,
		ROLL_MAX = 24.0;
/* #######  ---------------------------------------------------------------  */

/* #######  NIMBUS stuff not automatically handled by this skeleton program  */
#define LOW_RATE_FEEDBACK 0
#define nFeedBackTypes 2
#define ProcessingRate 25
#define DEG_RAD 0.01745329252
/*  #######  --------------------------------------------------------------  */

#define NCF	3

static NR_TYPE	latc[nFeedBackTypes], lonc[nFeedBackTypes],
		vnsc[nFeedBackTypes], vewc[nFeedBackTypes];

static NR_TYPE	filter();
static double	invert();

/* #######   Skeleton program needs the following line(s)             */
static NR_TYPE latcx, loncx, vewcx, vnscx;
static int    FeedBack = 0;
/* #######   Skeleton program needs the preceding line(s)             */

/* -------------------------------------------------------------------- */
void slatc(varp)
/* #######   Skeleton program avoiding the following line(s)
DERTBL	*varp;		/* Information about current variable	*/
/* #######   Skeleton program avoiding the preceding line(s)          */
/* #######   Skeleton program needs the following line(s)             */
VARTBL  *varp;      /* Information about current variable   */
{

    static bool first_time = TRUE;
    static int  inVarID[18];
/* #######   Skeleton program needs the preceding line(s)             */

  int		i, j, k;
  NR_TYPE	alat, alon, vew, vns, roll, glat, glon, gvew, gvns;
  NR_TYPE	dx, dy, dgps;
  NR_TYPE	omegat, sinwt, coswt, gvnsf, gvewf, vnsf, vewf;
  long		gstat, gmode;

/* #######   Skeleton program needs the following line(s)             */
  NR_TYPE	hr, min, sec;
  NR_TYPE	olatc, olonc, ovewc, ovnsc;
  int		ihr, imin, isec, seconds;
  NR_TYPE	fgstat, fgmode;
/* #######   Skeleton program needs the preceding line(s)             */

  double	det;

  NR_TYPE  *tmp;  /* for getting values from the project's Defaults file */

  static bool	firstTime[nFeedBackTypes] = { TRUE, TRUE },
		matrix_updated[nFeedBackTypes] = { FALSE, FALSE };

  static NR_TYPE	old_glat[nFeedBackTypes], old_glon[nFeedBackTypes];

  static NR_TYPE	DELT[nFeedBackTypes], factorp[nFeedBackTypes],
			dlat[nFeedBackTypes], dlon[nFeedBackTypes],
			dvy[nFeedBackTypes], dvx[nFeedBackTypes],
			time[nFeedBackTypes];

  static double	h[NCF][NCF], hi[NCF][NCF], zf[nFeedBackTypes][4][6];
  static double	am[2][NCF], bm[2][NCF], c[2][NCF], cp[2][NCF];
  static int	countr = 0, goodGPS = 0, gps_is_flat = 0;

/* #######   Skeleton program needs the following line(s)             */

/* ########   Define diagnostic flag  */
  bool diag = TRUE;
  static NR_TYPE dx1, dy1, dla, dlo;
/* ########                           */

  if (first_time)
  {
    /* Obtain IDs of input variables.
     */
    inVarID[ 0] = ncvarid(InputFile, "LAT");
    inVarID[ 1] = ncvarid(InputFile, "LON");
    inVarID[ 2] = ncvarid(InputFile, "VEW");
    inVarID[ 3] = ncvarid(InputFile, "VNS");
    inVarID[ 4] = ncvarid(InputFile, "ROLL");
    inVarID[ 5] = ncvarid(InputFile, "GLAT");
    inVarID[ 6] = ncvarid(InputFile, "GLON");
    inVarID[ 7] = ncvarid(InputFile, "GVEW");
    inVarID[ 8] = ncvarid(InputFile, "GVNS");
    inVarID[ 9] = ncvarid(InputFile, "GSTAT");
    inVarID[10] = ncvarid(InputFile, "GMODE");
    inVarID[11] = ncvarid(InputFile, "HOUR");
    inVarID[12] = ncvarid(InputFile, "MINUTE");
    inVarID[13] = ncvarid(InputFile, "SECOND");

    inVarID[14] = ncvarid(InputFile, "XLATC");
    inVarID[15] = ncvarid(InputFile, "XLONC");
    inVarID[16] = ncvarid(InputFile, "XVEWC");
    inVarID[17] = ncvarid(InputFile, "XVNSC");

/*    Diagnostic prints begin  */
    if (diag)
    {
      printf ("Changed equations originally on lines 494-497.\n Changed FCTRF to 0.5 to shorten time constant.\n");
    }
/*    Diagnostic prints end    */
    first_time = FALSE;

  }

  /* Get current values of inputs.
   */
  mindex[0] = CurrentInputRecordNumber;
  ncvarget1(InputFile, inVarID[ 0], mindex, (void *)&alat);
  ncvarget1(InputFile, inVarID[ 1], mindex, (void *)&alon);
  ncvarget1(InputFile, inVarID[ 2], mindex, (void *)&vew);
  ncvarget1(InputFile, inVarID[ 3], mindex, (void *)&vns);
  ncvarget1(InputFile, inVarID[ 4], mindex, (void *)&roll);
  ncvarget1(InputFile, inVarID[ 5], mindex, (void *)&glat);
  ncvarget1(InputFile, inVarID[ 6], mindex, (void *)&glon);
  ncvarget1(InputFile, inVarID[ 7], mindex, (void *)&gvew);
  ncvarget1(InputFile, inVarID[ 8], mindex, (void *)&gvns);
  ncvarget1(InputFile, inVarID[ 9], mindex, (void *)&fgstat);
  ncvarget1(InputFile, inVarID[10], mindex, (void *)&fgmode);
  ncvarget1(InputFile, inVarID[11], mindex, (void *)&hr);
  ncvarget1(InputFile, inVarID[12], mindex, (void *)&min);
  ncvarget1(InputFile, inVarID[13], mindex, (void *)&sec);

  ncvarget1(InputFile, inVarID[14], mindex, (void *)&olatc);
  ncvarget1(InputFile, inVarID[15], mindex, (void *)&olonc);
  ncvarget1(InputFile, inVarID[16], mindex, (void *)&ovewc);
  ncvarget1(InputFile, inVarID[17], mindex, (void *)&ovnsc);

  gstat = (long)fgstat;
  gmode = (long)fgmode;

  ihr = (int)hr;
  imin = (int)min;
  isec = (int)sec;
/* #######   Skeleton program needs the preceding line(s)             */

/* #######   Skeleton program avoiding the following line(s)
  alat	= GetSample(varp, 0);
  alon	= GetSample(varp, 1);
  vns	= GetSample(varp, 4);
  vew	= GetSample(varp, 5);
  roll	= GetSample(varp, 8);
  glat	= GetSample(varp, 2);
  glon	= GetSample(varp, 3);
  gvns	= GetSample(varp, 6);
  gvew	= GetSample(varp, 7);
  gstat	= (long)GetSample(varp, 9);
  gmode	= (long)GetSample(varp, 10);
   #######   Skeleton program avoiding the preceding line(s)          */


  if (firstTime[FeedBack])
    {
/*  Get constant values from Defaults file, if available  */

/* #######   Skeleton program avoiding the following line(s)
  (Use values within the function instead of those in the Defaults file.)

    if ((tmp = GetDefaultsValue("GPS_ROLL_MAX", varp->name)) == NULL)
  {
    sprintf(buffer, "Value set to %f in AMLIB function slatc.\n", ROLL_MAX);
    LogMessage(buffer);
  }
  else
    ROLL_MAX = tmp[0];
    if ((tmp = GetDefaultsValue("GPS_TAUP", varp->name)) == NULL)
  {
    sprintf(buffer, "Value set to %f in AMLIB function slatc.\n", TAUP);
    LogMessage(buffer);
  }
  else
    TAUP = tmp[0];
    if ((tmp = GetDefaultsValue("GPS_UPFCTR", varp->name)) == NULL)
  {
    sprintf(buffer, "Value set to %f in AMLIB function slatc.\n", UPFCTR);
    LogMessage(buffer);
  }
  else
    UPFCTR = tmp[0];
    if ((tmp = GetDefaultsValue("GPS_FCTRF", varp->name)) == NULL)
  {
    sprintf(buffer, "Value set to %f in AMLIB function slatc.\n", FCTRF);
    LogMessage(buffer);
  }
  else
    FCTRF = tmp[0];
   #######   Skeleton program avoiding the preceding line(s)          */

    if (FeedBack == LOW_RATE_FEEDBACK)
      DELT[FeedBack]	= 1.0;
    else
      DELT[FeedBack]  = 1.0 / ProcessingRate;

    old_glat[FeedBack]	= glat;
    old_glon[FeedBack]	= glon;

    memset((char *)zf, 0, sizeof(zf));
    memset((char *)am, 0, sizeof(am));
    memset((char *)bm, 0, sizeof(bm));
    memset((char *)h, 0, sizeof(h));

    factorp[FeedBack] = 0.002 * (1000.0 / DELT[FeedBack]) * M_PI / TAUP;

    time[FeedBack] = dvy[FeedBack] = dvx[FeedBack] = dlat[FeedBack] =
      dlon[FeedBack] = 0.0;


    firstTime[FeedBack] = FALSE;
    }


  /* If no IRS, then bail out.
   */
  if (alat == 0.0 && alon == 0.0)
    {
    latc[FeedBack] = lonc[FeedBack] = vewc[FeedBack] = vnsc[FeedBack] = 0.0;

/*    Diagnostic prints begin  */
    if (diag)
    {
      printf ("Disabled:  no IRS.\n");
    }
/*    Diagnostic prints end    */

/* #######   Skeleton program needs the following line(s)             */
    latcx = latc[FeedBack];
    loncx = lonc[FeedBack];
    vewcx = vewc[FeedBack];
    vnscx = vnsc[FeedBack];
    mindex[0] = CurrentOutputRecordNumber;
    ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&latcx);
/* #######   Skeleton program needs the preceding line(s)             */
/* #######   Skeleton program avoiding the following line(s)
    PutSample(varp, latc[FeedBack]);
   #######   Skeleton program avoiding the preceding line(s)          */
    }

  time[FeedBack] += DELT[FeedBack];

  dx	= glat - old_glat[FeedBack];
  dy	= glon - old_glon[FeedBack];
  dgps	= dx*dx + dy*dy;
  old_glat[FeedBack] = glat;
  old_glon[FeedBack] = glon;

  /* Check GPS status, only do this on the Low-rate pass.
   */
  if (FeedBack == LOW_RATE_FEEDBACK)
    {
    ++goodGPS;

    if (gstat == 0x0b00)	/* 3 satellites is also considered good	*/
      gstat = 0;


    /* Bad Positions for using GPS?
     */
    if (gstat > 0 || gmode < 4 || fabs(glat) > 90.0 || fabs(glon) > 180.0 || fabs(roll) > ROLL_MAX)
      {

/*    Diagnostic print begins  */
      if (diag)
      {
        printf("Disabled due to GPS or Roll.\n");
      }
/*    Diagnostic print ends    */

      goodGPS = 0;
      }

    if (dgps == 0.0)
      {
      if (++gps_is_flat > 2)	/* > 2 seconds	*/
        {

/*   Diagnostic print begins  */
        if (diag)
          {
            printf("Disabled:  GPS is flat.\n");
          }
/*    Diagnostic print ends    */

        goodGPS = 0;
        }
      }
    else
      gps_is_flat = 0;
    }

/*   Diagnostic print begins  */
      printf ("      matrix_updated (top)= %d\n", matrix_updated[FeedBack]);
/*    Diagnostic print ends    */

  if (goodGPS < 10)	/* < 10 seconds		*/
    {
/*    if (countr > 1800.0)	   30 minutes of operation.	*/ 
    if (countr > 1800)	/* 30 minutes of operation.	*/
      {
      if (FeedBack == LOW_RATE_FEEDBACK && matrix_updated[FeedBack])
        {
        matrix_updated[FeedBack] = FALSE;
        /*  invert information matrix to obtain error matrix  */
        h[0][1] = h[1][0];
        h[0][2] = h[2][0];
        h[1][2] = h[2][1];

        memcpy(hi, h, sizeof(h));

        det = invert(hi);

        /* Beware of matrix that cannot be inverted.
         */
        if (det == 0.0)
          {
          printf("GPS determinant is zero, resetting countr.\n");
          countr = 0;
          goto label546;
          }

        /* Now multiply times am to get matrix of coefficients.
         */
        for (k = 0; k < 2; ++k)
          for (i = 0; i < NCF; ++i)
            {
            c[k][i] = cp[k][i] = 0.0;

            for (j = 0; j < NCF; ++j)
              {
/*   C exchanges primary and secondary indices wrt Fortran
              c[k][i] += hi[i][j] * am[k][j];
              cp[k][i] += hi[i][j] * bm[k][j];
 */
              c[k][i] += hi[j][i] * am[k][j];
              cp[k][i] += hi[j][i] * bm[k][j];
              }
            }
        }

      omegat = 2.0 * M_PI * time[FeedBack] / 5040.0;
      sinwt = sin(omegat);
      coswt = cos(omegat);

      /* Want to avoid sharp transition to fit because that would
       * produce spike affecting power spectra.  Instead, slowly
       * adjust from last value toward fit value, time constant of
       * about 5 minutes chosen to be small compared to Schuller
       * period but large compared to wavelengths inportant in
       * spectral analysis.
       */
/*      gpsflg = 1.0;
*/
/*    Try new stuff here    */
      dvy[FeedBack]  *= FCTRF;
      dvx[FeedBack]  *= FCTRF;
      dlat[FeedBack] *= FCTRF;
      dlon[FeedBack] *= FCTRF;
/*  Print what's happening to the stuff I dropped  */
      dy1 = (1-FCTRF)*(c[0][0]+c[0][1]*sinwt+c[0][2]*coswt);
      dx1 = (1-FCTRF)*(c[1][0]+c[1][1]*sinwt+c[1][2]*coswt);
      dla = (1-FCTRF)*(cp[0][0]+cp[0][1]*sinwt+cp[0][2]*coswt);
      dlo = (1-FCTRF)*(cp[1][0]+cp[1][1]*sinwt+cp[1][2]*coswt);
/*   Diagnostic print begins  */
      printf ("     dx = %f, dy = %f, dla = %f, dlo = %f\n", dx1, dy1, dla, dlo);
/*    Diagnostic print ends    */
/*
      dvy[FeedBack] = dvy[FeedBack]*FCTRF+(1-FCTRF)*(c[0][0]+c[0][1]*sinwt+c[0][2]*coswt);
      dvx[FeedBack] = dvx[FeedBack]*FCTRF+(1-FCTRF)*(c[1][0]+c[1][1]*sinwt+c[1][2]*coswt);
      dlat[FeedBack]= dlat[FeedBack]*FCTRF+(1-FCTRF)*(cp[0][0]+cp[0][1]*sinwt+cp[0][2]*coswt);
      dlon[FeedBack]= dlon[FeedBack]*FCTRF+(1-FCTRF)*(cp[1][0]+cp[1][1]*sinwt+cp[1][2]*coswt);
 */
      }
    else
      {
      /* No good fit accumulated yet, so just let last correction
       * factors slowly decay.
       */
/*      gpsflg = 0.001;
*/
      dvy[FeedBack]	*= FCTRF;
      dvx[FeedBack]	*= FCTRF;
      dlat[FeedBack]	*= FCTRF;
      dlon[FeedBack]	*= FCTRF;
      }
    }
  else
    {
    /* Good GPS comes here.
     */
    gvnsf	= filter((double)gvns, zf[FeedBack][0]);
    gvewf	= filter((double)gvew, zf[FeedBack][1]);
    vnsf	= filter((double)vns, zf[FeedBack][2]);
    vewf	= filter((double)vew, zf[FeedBack][3]);

    dvy[FeedBack]	= gvnsf - vnsf;
    dvx[FeedBack]	= gvewf - vewf;
    dlat[FeedBack]	= glat - alat;
    dlon[FeedBack]	= glon - alon;

    if (FeedBack == LOW_RATE_FEEDBACK) /* Only do this in the Low-rate pass */
      {
      omegat = 2.0 * M_PI * time[FeedBack] / 5040.0;
      sinwt = sin(omegat);
      coswt = cos(omegat);

      am[0][0] = UPFCTR * am[0][0] + dvy[FeedBack];
      am[0][1] = UPFCTR * am[0][1] + dvy[FeedBack] * sinwt;
      am[0][2] = UPFCTR * am[0][2] + dvy[FeedBack] * coswt;
      am[1][0] = UPFCTR * am[1][0] + dvx[FeedBack];
      am[1][1] = UPFCTR * am[1][1] + dvx[FeedBack] * sinwt;
      am[1][2] = UPFCTR * am[1][2] + dvx[FeedBack] * coswt;
      bm[0][0] = UPFCTR * bm[0][0] + dlat[FeedBack];
      bm[0][1] = UPFCTR * bm[0][1] + dlat[FeedBack] * sinwt;
      bm[0][2] = UPFCTR * bm[0][2] + dlat[FeedBack] * coswt;
      bm[1][0] = UPFCTR * bm[1][0] + dlon[FeedBack];
      bm[1][1] = UPFCTR * bm[1][1] + dlon[FeedBack] * sinwt;
      bm[1][2] = UPFCTR * bm[1][2] + dlon[FeedBack] * coswt;

      /* Note: only one information matrix needed: independant of dx,dy
       */
      h[0][0] = UPFCTR * h[0][0] + 1.0;
      h[1][0] = UPFCTR * h[1][0] + sinwt;
      h[2][0] = UPFCTR * h[2][0] + coswt;
      h[1][1] = UPFCTR * h[1][1] + sinwt * sinwt;
      h[2][1] = UPFCTR * h[2][1] + sinwt * coswt;
      h[2][2] = UPFCTR * h[2][2] + coswt * coswt;

      matrix_updated[FeedBack] = TRUE;
      ++countr;
      }
    }

/*    all values come here.  apply correction factors, determined
       in various ways above  */

label546:
vnsc[FeedBack] = vns + dvy[FeedBack];
vewc[FeedBack] = vew + dvx[FeedBack];

/*  another difference between C and FORTRAN  */
/* if (fabs(lonc[FeedBack] - alon) > 5 || fabs(latc[FeedBack] - alat) > 5) */
if (fabs(lonc[FeedBack] - alon) > .5 || fabs(latc[FeedBack] - alat) > .5)
    {
      lonc[FeedBack] = alon;
      latc[FeedBack] = alat;
    }

  lonc[FeedBack] += vewc[FeedBack] / (CDM * cos(alat * DEG_RAD));
  latc[FeedBack] += vnsc[FeedBack] / CDM;
  lonc[FeedBack] += factorp[FeedBack] * (alon+dlon[FeedBack]-lonc[FeedBack]);
  latc[FeedBack] += factorp[FeedBack] * (alat+dlat[FeedBack]-latc[FeedBack]);

/* #######   Skeleton program needs the following line(s)             */
  latcx = latc[FeedBack];
  loncx = lonc[FeedBack];
  vewcx = vewc[FeedBack];
  vnscx = vnsc[FeedBack];

/*    Diagnostic prints begin  */
  if (diag)
  {
    printf (" %02d:%02d:%02d", ihr, imin, isec);
    printf (" LAT = %f", alat);
    printf ("   LON = %f", alon);
    printf ("   VEW = %f", vew);
    printf ("   VNS = %f", vns);
    printf (" ROLL = %f\n", roll);
    printf ("         GLAT = %f", glat);
    printf ("  GLON = %f", glon);
    printf ("  GVEW = %f", gvew);
    printf ("  GVNS = %f", gvns);
    printf (" GSTAT = %d", gstat);
    printf (" GMODE = %d\n", gmode);
/*
    printf ("         LATC = %f", olatc);
    printf ("  LONC = %f", olonc);
    printf ("  VEWC = %f", ovewc);
    printf ("  VNSC = %f \n", ovnsc);
 */
    printf ("        LATCX = %f", latcx);
    printf (" LONCX = %f", loncx);
    printf (" VEWCX = %f", vewcx);
    printf (" VNSCX = %f \n", vnscx);

    printf ("         dlat = %f", dlat[FeedBack]);
    printf ("  dlon = %f", dlon[FeedBack]);
    printf ("   dvx = %f", dvx[FeedBack]);
    printf ("   dvy = %f\n", dvy[FeedBack]);

    printf ("      goodGPS = %d", goodGPS);
    printf (" countr = %d", countr);
    printf (" matrix_updated (bot) = %d\n", matrix_updated[FeedBack]);
  }
/*    Diagnostic prints end    */

  mindex[0] = CurrentOutputRecordNumber;
  ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&latcx);
/* #######   Skeleton program needs the preceding line(s)             */
/* #######   Skeleton program avoiding the following line(s)
  PutSample(varp, latc[FeedBack]);
   #######   Skeleton program avoiding the preceding line(s)          */

}	/* END SLATC */

/* -------------------------------------------------------------------- */
/* #######   Skeleton program avoiding the following line(s)
void slonc(DERTBL *varp)
{
  PutSample(varp, lonc[FeedBack]);
}
   #######   Skeleton program avoiding the preceding line(s)          */
/* #######   Skeleton program needs the following line(s)             */
void slonc(varp)
VARTBL  *varp;      /* Information about current variable   */
{
  mindex[0] = CurrentOutputRecordNumber;
  ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&loncx);
}
/* #######   Skeleton program needs the preceding line(s)             */

/* -------------------------------------------------------------------- */
/* #######   Skeleton program avoiding the following line(s)
void svewc(DERTBL *varp)
{
  PutSample(varp, vewc[FeedBack]);
}
   #######   Skeleton program avoiding the preceding line(s)          */
/* #######   Skeleton program needs the following line(s)             */
void svewc(varp)
VARTBL  *varp;      /* Information about current variable   */
{
  mindex[0] = CurrentOutputRecordNumber;
  ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&vewcx);
}
/* #######   Skeleton program needs the preceding line(s)             */

/* -------------------------------------------------------------------- */
/* #######   Skeleton program avoiding the following line(s)
void svnsc(DERTBL *varp)
{
  PutSample(varp, vnsc[FeedBack]);
}
   #######   Skeleton program avoiding the preceding line(s)          */
/* #######   Skeleton program needs the following line(s)             */
void svnsc(varp)
VARTBL  *varp;      /* Information about current variable   */
{
  mindex[0] = CurrentOutputRecordNumber;
  ncvarput1(OutputFile, varp->outVarID, mindex, (void *)&vnscx);
}
/* #######   Skeleton program needs the preceding line(s)             */


/* -------------------------------------------------------------------- */
/* Version 2 (A Cooper, 5/28/92, coded by CJW 7/30/93)
 * This function returns the low-pass-filtered value of the time series
 * 'x', if the function is called sequentially for each element in x.
 * The filter is a three-pole Butterworth lowpass filter with
 * cutoff frequency (i.e., attentuation by 0.707) at T/tau where T is
 * the sampling frequency and tau is the data constant below.  (For
 * a sequence sampled once per second and tau=600, the cutoff frequency
 * is at 1/600 = (10 min)**{-1}.)  The algorithm used is described
 * in Bosic, p. 49.
 * The array zf is used to save values for the next call, to make it
 * easy to filter more than one variable.
 */
static NR_TYPE filter(x, zf)
double	x;
double	zf[];
{
  static double	a[nFeedBackTypes], a2[nFeedBackTypes],
		a3[nFeedBackTypes], a4[nFeedBackTypes];
  static bool	firstTime[nFeedBackTypes] = { TRUE, TRUE };

  if (firstTime[FeedBack])
    {
    double	TAU;
    double	b1, b2, c, c2, e, f;

    if (FeedBack == LOW_RATE_FEEDBACK)
      TAU = 600.0;
    else
      TAU = 600.0 * ProcessingRate;

    a[FeedBack]	= 2.0 * M_PI / TAU;
    b1		= sqrt(3.0) / 2.0;
    b2		= sqrt(1.0 / 3.0);
    c		= exp(-0.5 * a[FeedBack]);
    c2		= c * 2.0;
    e		= a[FeedBack] * b1;
    f		= c * (cos(e) + b2 * sin(e));
    a2[FeedBack]= a[FeedBack] * f;
    a3[FeedBack]= 2.0 * exp(-a[FeedBack] / 2.0) * cos(e);
    a4[FeedBack]= exp(-a[FeedBack]);

    firstTime[FeedBack] = FALSE;
    }

  zf[2] = -a[FeedBack] * x + a2[FeedBack] * zf[5] + a3[FeedBack] * zf[3] - a4[FeedBack] * zf[4];
  zf[1] = a[FeedBack] * x + a4[FeedBack] * zf[1];
  zf[4] = zf[3];
  zf[3] = zf[2];
  zf[5] = x;

  return(zf[1] + zf[2]);

}	/* END FILTER */

/* -------------------------------------------------------------------- */
static double invert(array)
double	array[NCF][NCF];
{
  int		i, j, indx[NCF];
  double	y[NCF][NCF], col[NCF], det;

  if (ludcmp(array, NCF, indx, &det) == ERR)
    return(0.0);

  for (i = 0; i < NCF; ++i)
    det *= array[i][i];

  for (j = 0; j < NCF; ++j)
    {
    for (i = 0; i < NCF; ++i)
      col[i] = 0.0;

    col[j] = 1.0;
    lubksub(array, NCF, indx, col);

    for (i = 0; i < NCF; ++i)
      y[i][j] = col[i];
    }

  memcpy(array, y, sizeof(y));

  return(det);

}

/* -------------------------------------------------------------------- */
ludcmp(a, N, indx, det)
double	a[NCF][NCF];
int	N;
int	indx[];
double	*det;
{
  int		i, imax, j, k;
  double	big, dum, sum, temp;
  double	vv[NCF];

  *det = 1.0;

  for (i = 0; i < N; ++i)
    {
    big = 0.0;

    for (j = 0; j < N; ++j)
      if ((temp = fabs(a[i][j])) > big)
        big = temp;

    if (big == 0.0)
      {
      fprintf(stderr, "Singular matrix in LUdcmp.\n");
      return(ERR);
    }

    vv[i] = 1.0 / big;
    }

  for (j = 0; j < N; ++j)
    {
    for (i = 0; i < j; ++i)
      {
      sum = a[i][j];

      for (k = 0; k < i; ++k)
        sum -= a[i][k] * a[k][j];

      a[i][j] = sum;
      }

    big = 0.0;

    for (i = j; i < N; ++i)
      {
      sum = a[i][j];

      for (k = 0; k < j; ++k)
        sum -= a[i][k] * a[k][j];

      a[i][j] = sum;

      if ((dum = vv[i] * fabs(sum)) >= big)
        {
        big = dum;
        imax = i;
        }
      }

    if (j != imax)
      {
      for (k = 0; k < N; ++k)
        {
        dum = a[imax][k];
        a[imax][k] = a[j][k];
        a[j][k] = dum;
        }

      *det = -(*det);
      vv[imax] = vv[j];
      }

    indx[j] = imax;

    if (a[j][j] == 0.0)
      a[j][j] = MINFLOAT;

    if (j != N-1)
      {
      dum = 1.0 / a[j][j];

      for (i = j+1; i < N; ++i)
        a[i][j] *= dum;
      }
    }

}	/* END LUDCMP */

/* -------------------------------------------------------------------- */
lubksub(a, N, indx, b)
double	a[NCF][NCF];
int	N;
int	indx[];
double	b[];
{
  int		i, ii = -1, ip, j;
  double	sum;

  for (i = 0; i < N; ++i)
    {
    ip = indx[i];
    sum = b[ip];
    b[ip] = b[i];

    if (ii != -1)
      for (j = ii; j < i; ++j)
        sum -= a[i][j] * b[j];
    else
      if (sum != 0.0)
        ii = i;

    b[i] = sum;
    }

  for (i = N - 1; i >= 0; --i)
    {
    sum = b[i];

    for (j = i+1; j < N; ++j)
      sum -= a[i][j] * b[j];

    b[i] = sum / a[i][i];
    }

}	/* END LUBKSUB */

/* END GPSC.C */

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
