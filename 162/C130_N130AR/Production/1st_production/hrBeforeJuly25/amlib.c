/*  SCMS Skeleton Processor
	High rate data processor
	Darrel Baumgardner: February 23, 1996
	1)	Calculates FSSP-100 activity from DFSTRB and DFRSTS
	2)	Corrects concf, cfsp01-15,  and plwcf for activity
	3)	Calculates FSSP-300 activity from total counts
	4)	Corrects conc3 and cfs301-30  for activity
	5)	Calculates egg temperatures with new cal coeffs.
-------------------------------------------------------------------------
OBJECT NAME:	amlib.c

FULL NAME:	

ENTRY POINTS:	SetComputeFunctions()
			sfssp100Activity()
			sfssp300Activity()
			sfssp100Conc()
			sfssp300Conc()
			sfssp100Spectra()
			sfssp300Spectra()
			sfsspLwc()
			seggTemps()
			seggTemp1()
			seggTemp2()
			seggTemp3()

STATIC FNS:		none

DESCRIPTION:	Template for AMLIB std & user directories.

INPUT:			

OUTPUT:			

REFERENCES:		none

REFERENCED BY:	compute.c

COPYRIGHT:		University Corporation for Atmospheric Research, 1993,1994
-------------------------------------------------------------------------
*/

#include "define.h"
#include <math.h>



/* Step 4: To appease the compiler, place all AMLIB function names here.
 * By convention, AMLIB functions start with the letter 's'.  These should
 * be the same names you used in steps 2 & 3.
 */
void	sfssp100Conc(),
		sfssp100Activity(),
     	sfssp300Activity(),
		sfssp100BeamFrac(),
     	sfssp300Conc(),
     	sfssp100Spectra(),
     	sfssp300Spectra(),
     	sfsspLwc(),
	seggTemps(),
	seggTemp1(),
	seggTemp2(),
	seggTemp3();



/* Step 1: List variables to pass through unaltered by default.  HOUR,
 * MINUTE & SECOND are mandatory.  NULL must be last.
 */
char	*passThrough[] = 
	{
	"HOUR",
	"MINUTE",
	"SECOND",
	NULL
	};


/* Step 2: New variables get added here.  Also add existing variables that
 * you would like to enter additional processing for.  Remove the examples
 * given below, leaving the "NULL, NULL" line in.  Make sure to leave off
 * the parentheses from the function names.
 */
DERVAR	derivftns[] =
	{
/*  'Name'  'Rate' 'Vector Length'  'function'	*/
    "CF300_LPO",	10, 	32, 	sfssp300Spectra,
    "CFSSP_IBR",	10,	16,	sfssp100Spectra,
    "CONC3_LPO",	10,	1, 	sfssp300Conc,
    "CONCF_IBR",	10, 	1, 	sfssp100Conc,
    "FACT_IBR",		10,	1,	sfssp100Activity,
    "FACT_LPO", 	10,	1, 	sfssp300Activity,
    "FBMFR_IBR",	10,	1,	sfssp100BeamFrac,
    "PLWCF_IBR",	10,	1, 	sfsspLwc,
/*  "XEGGT1",		25,	1,	seggTemp1,
    "XEGGT2",		25,	1,	seggTemp2,
    "XEGGT3",		25,	1,	seggTemp3,  */
	NULL,		0,	0,	NULL
	};


/* Step 3: Create AMLIB functions here.  Use these examples as templates.
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
NR_TYPE xthmp3[25],xthmp4[25],xthmp5[25], fbmfr_ibr[10], fact_ibr[10],
        concf_ibr[10],conc3_lpo[10], fact_lpo[10],plwcf_ibr[10];
/* -------------------------------------------------------------------- */
void sfssp100Spectra(varp)
VARTBL  *varp;          /* Information about current variable   */
{
#define TAU1 0.0000090
#define TAU2 0.0000020
#define DOF 2.81
#define BEAMDIAMETER 0.19
    NR_TYPE	dfstrb[25],dfrsts[25],tasx[25];
    NR_TYPE   afssp_ibr[10][16], cfssp_ibr[10][16];
    float size[16] = {0.0,3.7,7.1,11.4,14.7,17.4,20.2,24.3,28.6,
                      31.7,36.1,40.7,43.5,46.9,49.0,50.5};
    float   sum,beamFraction, activityFraction,correctionFraction;
    float   tas,conc,lwc,totalConc,totalLwc,totalStrobe,fastReset;
	
    long    start[3], count[3];
	int i, j, k;

	static bool	first_time = TRUE;
	static int	inVarID[7];
	float transitTime, sampleVolume;

	if (first_time)
		{
		/* Obtain ID's of input variables.
		 */
		inVarID[0] = ncvarid(InputFile, "DFSTRB");
		inVarID[1] = ncvarid(InputFile, "DFRSTS");
		inVarID[2] = ncvarid(InputFile, "TASX");
		inVarID[3] = ncvarid(InputFile, "FBMFR_IBR");
		inVarID[4] = ncvarid(InputFile, "CONCF_IBR");
		inVarID[5] = ncvarid(InputFile, "PLWCF_IBR");
		inVarID[6] = ncvarid(InputFile, "AFSSP_IBR");

		first_time = FALSE;
		}

	/* Get current values of inputs.  */
        start[0] = CurrentInputRecordNumber;
        start[1] = start[2] = 0;
        count[0] = 1; count[1] = 25; count[2] = 1;

        ncvarget(InputFile, inVarID[0], start, count, (void *)dfstrb);
        ncvarget(InputFile, inVarID[1], start, count, (void *)dfrsts);
        ncvarget(InputFile, inVarID[2], start, count, (void *)tasx);

        count[0] = 1; count[1] = 10; count[2] = 1;
        ncvarget(InputFile, inVarID[3], start, count, (void *)fbmfr_ibr);
        ncvarget(InputFile, inVarID[4], start, count, (void *)concf_ibr);
        ncvarget(InputFile, inVarID[5], start, count, (void *)plwcf_ibr);

        count[0] = 1; count[1] = 10; count[2] = 16;
        ncvarget(InputFile, inVarID[6], start, count, (void *)afssp_ibr);
        ncvarget(InputFile, varp->inVarID, start, count, (void *)cfssp_ibr);

/*Go through all 10 samples of the FSSP data. Use every 2.5 TAS,DFSTRB and
  DFRST points to parse 25 hertz data down to 10 Hz */
        totalConc = 0.0;
	totalLwc  = 0.0;
	for (i = 0; i < 10; i++)
	{
	   j = (float)i*2.5;
/*Average the  airspeed, total strobes and fast resets over j-1,j,j+1 */
	   if (j > 0)
	   {
	      tas = (tasx[j-1] + tasx[j] + tasx[j+1])/3.0;
	      totalStrobe = (dfstrb[j-1] + dfstrb[j] + dfstrb[j+1])/3.0;
	      fastReset = (dfrsts[j-1] + dfrsts[j] + dfrsts[j+1])/3.0;
	   }
	   else
	   {
	      tas = (tasx[j] + tasx[j+1])/2.0;
	      totalStrobe = (dfstrb[j] + dfstrb[j+1])/2.0;
	      fastReset = (dfrsts[j] + dfrsts[j+1])/2.0;
           }
/*Compute the average transit time*/
	   transitTime = .148e-3/tas ;

/*	Compute the activity */
/*	Before July 25  */
	   fact_ibr[i] = 2.5*totalStrobe * (TAU1 + transitTime) 
                   + 2.5*(4 * totalStrobe) * (transitTime + TAU2) ;
  
  
/*	After July 25
	   fact_ibr[i] = 2.5*totalStrobe * (TAU1 + transitTime) 
                   + 2.5*(fastReset - totalStrobe) * (transitTime + TAU2) ;

*/

	   if ((fact_ibr[i] <= 0)||(fact_ibr[i] > 1))
	      fact_ibr[i] = 0.0;
	   activityFraction = 1.0 - 5.50*fact_ibr[i];
	   fact_ibr[i] *= 10. ;
	   if (activityFraction < 0)
	      activityFraction = 1.0;

/* Calculate the beam fraction */


	   sum = 0;
	   for (k = 1; k < 16 ; k++)
	      sum += afssp_ibr[i][k];

	   if (dfstrb[j] > 0)
	      beamFraction = sum/(2.5*totalStrobe);
   	   else
	      beamFraction = 0.6;
	   if ((beamFraction > 1)||(beamFraction < .2))
	      beamFraction = 0.6;

 
	   sampleVolume =  
		0.1*BEAMDIAMETER*DOF*tasx[j]*beamFraction*activityFraction ;
	   if (sampleVolume <= 0)
	      sampleVolume = 1.0;
	   conc = concf_ibr[i];
	   lwc = plwcf_ibr[i];
	   concf_ibr[i]=0;
	   plwcf_ibr[i]=0;
	   for (k = 1; k < 16 ; k++)
	   {
/*	      printf(" %4.0f",(float)afssp_ibr[i][k]);		*/
	      cfssp_ibr[i][k] = (float)afssp_ibr[i][k]/sampleVolume;
	      concf_ibr[i] += cfssp_ibr[i][k];
	      plwcf_ibr[i] += cfssp_ibr[i][k] * size[k]*size[k]*size[k];
	   }
/*	   printf("\n");	*/
	   plwcf_ibr[i] *= 0.523599e-6;
	   fbmfr_ibr[i] = beamFraction ;
/*
	   printf("i,j,dfstrb,dfsrst,sum,bf=%2i %2i %4.0f %4.0f %4.0f %4.2f\n",
	      i,j,totalStrobe,fastReset,sum,beamFraction);
	   printf("SV,actv,oldC,newC,oldL,newL= %3.1f %4.2f %5.1f %5.1f %5.3f %5.3f\n",
		   sampleVolume, activityFraction,conc,concf_ibr[i],
		   lwc,plwcf_ibr[i]);
*/
	   totalConc += concf_ibr[i];
	   totalLwc  += plwcf_ibr[i];
	}
	totalConc /= 10.;
	totalLwc  /= 10.;
/*	printf("1s Concentration, LWC = %f %f\n\n",totalConc,totalLwc); */
        start[0] = CurrentOutputRecordNumber;
        start[1] = start[2] = 0;
        count[0] = 1; count[1] = 10; count[2] = 1;

    ncvarput(OutputFile, varp->outVarID, start, count, (void *)cfssp_ibr);

}
/* -------------------------------------------------------------------- */
/* -------------------------------------------------------------------- */
void sfssp300Spectra(varp)
VARTBL  *varp;          /* Information about current variable   */
{
	NR_TYPE tasx[25],cf300_lpo[10][32], af300_lpo[10][32];
	static bool	first_time = TRUE;
	static int	inVarID[4];
	float totalCounts;
       float transitTime, sampleVolume;
       long    start[3], count[3];
       int i,j,k;
#define TAU 0.0000050

	if (first_time)
		{
		/* Obtain ID's of input variables.
		 */
		inVarID[0] = ncvarid(InputFile, "AF300_LPO");
		inVarID[1] = ncvarid(InputFile, "TASX");
		inVarID[2] = ncvarid(InputFile, "FACT_LPO");
		inVarID[3] = ncvarid(InputFile, "CONC3_LPO");

		first_time = FALSE;
		}

	/* Get current values of inputs.
	 */
        /* Get current values of inputs.  */
        start[0] = CurrentInputRecordNumber;
        start[1] = start[2] = 0;
        count[0] = 1; count[1] = 25; count[2] = 1;
 
        ncvarget(InputFile, inVarID[1], start, count, (void *)tasx);
 
        count[0] = 1; count[1] = 10; count[2] = 1;
        ncvarget(InputFile, inVarID[3], start, count, (void *)conc3_lpo);
        ncvarget(InputFile, inVarID[2], start, count, (void *)fact_lpo);
 
        count[0] = 1; count[1] = 10; count[2] = 32;
        ncvarget(InputFile, inVarID[0], start, count, (void *)af300_lpo);
        ncvarget(InputFile, varp->inVarID, start, count, (void *)cf300_lpo);

/* Step through all 10 samples */
	for (i = 0; i < 10; i++)
	{
/*	Compute the activity */
	   totalCounts = 0.0;
	   for (j=1; j<30; j++)
	      totalCounts += af300_lpo[i][j];
              j = (float)i*2.5;
/*Compute the average transit time*/
           transitTime = .148e-3/tasx[j] ;

	   fact_lpo[i] = totalCounts * (TAU + transitTime) ;
	   if ((fact_lpo[i] <= 0)||(fact_lpo[i] > 0.8))
	      fact_lpo[i] = 0.0;

	   sampleVolume = 0.05 * tasx[j]*(1.0 - 5.5*fact_lpo[i])*0.1;

	   conc3_lpo[i] = 0.0;
/*	Note, Don't sum the last three channels. Something looked odd these */
           for (k = 1; k < 30 ; k++)
	   {
              cf300_lpo[i][k] = af300_lpo[i][k]/sampleVolume;
	      conc3_lpo[i] += cf300_lpo[i][k];
	   }
	}
        start[0] = CurrentOutputRecordNumber;
        start[1] = start[2] = 0;
        count[0] = 1; count[1] = 10; count[2] = 1;

        ncvarput(OutputFile, varp->outVarID, start, count, (void *)cf300_lpo);
}
/* -------------------------------------------------------------------- */
/* -------------------------------------------------------------------- */
void sfssp100Activity(varp)
VARTBL  *varp;          /* Information about current variable   */
{
    long    start[3], count[3];


	/* Write out new variable.*/

        count[0] = 1; count[1] = 10; count[2] = 1;
        start[0] = CurrentOutputRecordNumber;
        start[1] = start[2] = 0;

        ncvarput(OutputFile, varp->outVarID, start, count, (void *)fact_ibr);


}

/* -------------------------------------------------------------------- */
/* -------------------------------------------------------------------- */
void sfssp100BeamFrac(varp)
VARTBL  *varp;          /* Information about current variable   */
{
    long    start[3], count[3];


	/* Write out new variable.*/

        count[0] = 1; count[1] = 10; count[2] = 1;
        start[0] = CurrentOutputRecordNumber;
        start[1] = start[2] = 0;

        ncvarput(OutputFile, varp->outVarID, start, count, (void *)fbmfr_ibr);

}
/* -------------------------------------------------------------------- */
/* -------------------------------------------------------------------- */
void sfssp300Activity(varp)
VARTBL	*varp;		/* Information about current variable	*/
{
    long    start[3], count[3];


	/* Write out new variable.
	 */
        count[0] = 1; count[1] = 10; count[2] = 1;
        start[0] = CurrentOutputRecordNumber;
        start[1] = start[2] = 0;

        ncvarput(OutputFile, varp->outVarID, start, count, (void *)fact_lpo);

}

/* -------------------------------------------------------------------- */
/* -------------------------------------------------------------------- */
void sfssp100Conc(varp)
VARTBL    *varp;         /* Information about current variable   */
{
    long    start[3], count[3];

 
     count[0] = 1; count[1] = 10; count[2] = 1;
        start[0] = CurrentOutputRecordNumber;
        start[1] = start[2] = 0;
     ncvarput(OutputFile, varp->outVarID, start, count, (void *)concf_ibr);
 
}
 
/* -------------------------------------------------------------------- */
void sfssp300Conc(varp)
VARTBL    *varp;         /* Information about current variable   */
{
    long    start[3], count[3];

 
 
     /* Write out new variable.
      */
     count[0] = 1; count[1] = 10; count[2] = 1;
        start[0] = CurrentOutputRecordNumber;
        start[1] = start[2] = 0;
     ncvarput(OutputFile, varp->outVarID, start, count, (void *)conc3_lpo);
 
}

/* -------------------------------------------------------------------- */
/* -------------------------------------------------------------------- */
void sfsspLwc(varp)
VARTBL    *varp;         /* Information about current variable   */
{
    long    start[3], count[3];

 
 
     /* Write out new variable.
      */
     count[0] = 1; count[1] = 10; count[2] = 1;
        start[0] = CurrentOutputRecordNumber;
        start[1] = start[2] = 0;
     ncvarput(OutputFile, varp->outVarID, start, count, (void *)plwcf_ibr);
 
}
 
/* -------------------------------------------------------------------- */
/* -------------------------------------------------------------------- */
void seggTemps(varp)
VARTBL  *varp;          /* Information about current variable   */
{
        NR_TYPE xtc3[25],xtc4[25],xtc5[25],xrf3[25],psxc[25],qcxc[25];
        double  factor, delpeg, facteg, taseg2;
        double  tiso3c, tiso4c, tiso5c, tiso3, tiso4, tiso5;
        double  t3emf, t4emf, t5emf, tiemf3, tiemf4, tiemf5;
        double  tem3, tem4, tem5, thm3t, thm5t, delt3, delt5;
        double  tasfac, pratio;
	double rbycp = 0.2859;
	float fact;
    long    start[3], count[3];


 
        static bool     first_time = TRUE;
        static int      inVarID[6];
	int i;
 
        if (first_time)
                {
                /* Obtain ID's of input variables.
                 */
                inVarID[0] = ncvarid(InputFile, "XTC3");
                inVarID[1] = ncvarid(InputFile, "XTC4");
                inVarID[2] = ncvarid(InputFile, "XTC5");
                inVarID[3] = ncvarid(InputFile, "XRF3");
                inVarID[4] = ncvarid(InputFile, "PSXC");
                inVarID[5] = ncvarid(InputFile, "QCXC");
	}

        start[0] = CurrentInputRecordNumber;
        start[1] = start[2] = 0;
        count[0] = 1; count[1] = 25; count[2] = 1;
 
        ncvarget(InputFile, inVarID[0], start, count, (void *)xtc3);
        ncvarget(InputFile, inVarID[1], start, count, (void *)xtc4);
        ncvarget(InputFile, inVarID[2], start, count, (void *)xtc5);
        ncvarget(InputFile, inVarID[3], start, count, (void *)xrf3);
        ncvarget(InputFile, inVarID[4], start, count, (void *)psxc);
        ncvarget(InputFile, inVarID[5], start, count, (void *)qcxc);
   for (i = 0; i< 25; i++)
   {

        pratio = 1.0 + (double)qcxc[i] / (double)psxc[i];
        factor = pow(pratio, rbycp) - 1.0;
	fact = factor;

/* Thermocouple temperature computation procedure                   */
/*         (a).  get temperature of internal reference block (TISO) */
/*               from VOUT3                                         */
/*         (b).  convert from measured voltages to thermocouple emf */
/*               according to UW calibration                        */
/*         (c).  calculate emf contribution from Cu-Ch and Cu-Con   */
/*               junctions at reference block (E2 from TISO)        */
/*         (d).  correct from measured emf (due to exposed Con-Ch   */
/*               junction + reference-block junctions) to get       */
/*               contribution from exposed junction                 */
/*         (e).  invert from junction potential to get temperature  */
/*         (f).  apply standard dyn-heating correction with r=0.70  */
 
/* Start Prototype II calculations */
 
/*         (a): *****FIRST apply gain and offset form ICL7652 ampl  */
/*              in probe (note: on 900421 used first guess as gain of 10,000 */
/*              & offset of 250 uA - 20k res  from nominal values   */
/*              before RAF calibration was available).              */
 
        tiso3c = (xrf3[i] * 10.0 + 250.0) - 273.15;

        /* *****NEXT: correct AD590 sensors per calibration in NCAR         */
        /* bath on 900220 NOTE: numbers stamped on AD590's have following   */
        /* relationship to parameter names:     Sensor #5 = TISO3       */
        /*                                      Sensor #6 = TISO4       */
        /*                                      Sensor #7 = TISO5       */
 
        tiso3 = 0.990000 * tiso3c + 0.800000;
        tiso4 = tiso3;          /* Hardwired 4 and 5 to 3 5-10-93 */
        tiso5 = tiso3;
 
        /*...... (b): */
        /* *****RANGE: -1 TO +1 mV (EMF) = +10 to -10 VDC (negative slope)  */
        /*             NOTE: preliminary calibration used on 900421         */
 
        t3emf =  0.1 * xtc3[i];
        t4emf =  0.1 * xtc4[i];
        t5emf =  0.1 * xtc5[i];
        /*...... (c): */
        /*          (calibration from thermocouple tables) */
 
        tiemf3=(5.86959E1 + (4.31109E-2 + (5.72204E-5
           + (-5.40207E-7 + (1.54259E-9 + (- 2.485E-12
           +(2.33897E-15 + (- 1.194629E-18 +2.556E-22*tiso3)*tiso3)
           *tiso3)*tiso3)*tiso3)*tiso3)*tiso3)*tiso3)*tiso3;
 
        tiemf4=(5.86959E1 + (4.31109E-2 + (5.72204E-5
           + (-5.40207E-7 + (1.54259E-9 + (- 2.485E-12
           +(2.33897E-15 + (- 1.194629E-18 +2.556E-22*tiso4)*tiso4)
           *tiso4)*tiso4)*tiso4)*tiso4)*tiso4)*tiso4)*tiso4;
 
        tiemf5=(5.86959E1 + (4.31109E-2 + (5.72204E-5
           + (-5.40207E-7 + (1.54259E-9 + (- 2.485E-12
           +(2.33897E-15 + (- 1.194629E-18 +2.556E-22*tiso5)*tiso5)
           *tiso5)*tiso5)*tiso5)*tiso5)*tiso5)*tiso5)*tiso5;
 
        /*..... convert from uV to mV */
        tiemf3 = 1e-3*tiemf3;
        tiemf4 = 1e-3*tiemf5;
        tiemf5 = 1e-3*tiemf5;
 
        /*...... (d): */
        tem3 = t3emf + tiemf3;
        tem4 = t4emf + tiemf4;
        tem5 = t5emf + tiemf5;
 
        /*....... (e): */
        xthmp3[i] = (1.70225E1 + (-2.2097E-1 + (5.48093E-3
                     -5.76699E-5*tem3)*tem3)*tem3)*tem3;
        xthmp4[i] = (1.70225E1 + (-2.2097E-1 + (5.48093E-3
                     -5.76699E-5*tem4)*tem4)*tem4)*tem4;
        xthmp5[i] = (1.70225E1 + (-2.2097E-1 + (5.48093E-3
                     -5.76699E-5*tem5)*tem5)*tem5)*tem5;


        /*...... (f): Correct for dynamic heating; r=0.7 */
        xthmp3[i] = (xthmp3[i] + 273.15) / (1.0 + 0.7 * factor) - 273.15;
        xthmp4[i] = (xthmp4[i] + 273.15) / (1.0 + 0.7 * factor) - 273.15;
        xthmp5[i] = (xthmp5[i]+ 273.15) / (1.0 + 0.7 * factor) - 273.15;

	if ((xthmp3[i] < -100.)||(xthmp3[i] > 100.))
	   xthmp3[i] = 99.;
	if ((xthmp4[i] < -100.)||(xthmp4[i] > 100.))
	   xthmp4[i] = 99.;
	if ((xthmp5[i] < -100.)||(xthmp5[i] > 100.))
	   xthmp5[i] = 99.;
   }

}
/* -------------------------------------------------------------------- */
void seggTemp1(varp)
VARTBL  *varp;          /* Information about current variable   */
{
    long    start[3], count[3];


/*	Call the routine that calculates all the temperatures */
	seggTemps();
        /* Write out new variable.*/
        count[0] = 1; count[1] = 25; count[2] = 1;
        start[0] = CurrentOutputRecordNumber;
        start[1] = start[2] = 0;
        ncvarput(OutputFile, varp->outVarID, start, count, (void *)xthmp3);
 
}
/* -------------------------------------------------------------------- */
/* -------------------------------------------------------------------- */
void seggTemp2(varp)
VARTBL  *varp;          /* Information about current variable   */
{
    long    start[3], count[3];

        /* Write out new variable.*/

        count[0] = 1; count[1] = 25; count[2] = 1;
        start[0] = CurrentOutputRecordNumber;
        start[1] = start[2] = 0;
        ncvarput(OutputFile, varp->outVarID, start, count, (void *)xthmp4);
 
}
/* -------------------------------------------------------------------- */
/* -------------------------------------------------------------------- */
void seggTemp3(varp)
VARTBL  *varp;          /* Information about current variable   */
{
    long    start[3], count[3];
	int i;

        /* Write out new variable.*/

	for (i=0 ; i< 25; i++)
	   xthmp5[i] = xthmp5[i]/1.03 + 0.907354;
        count[0] = 1; count[1] = 25; count[2] = 1;
        start[0] = CurrentOutputRecordNumber;
        start[1] = start[2] = 0;
        ncvarput(OutputFile, varp->outVarID, start, count, (void *)xthmp5);
 
}
/* -------------------------------------------------------------------- */
/* -------------------------------------------------------------------- */
/* Leave this function alone.
 */
void SetComputeFunctions()
{

	register int	i, indx;

	for (i = 0; derivftns[i].Compute; ++i)
		if ((indx = SearchTable(Variable,nVariables,derivftns[i].Name)) != ERR)
			Variable[indx]->Compute = derivftns[i].Compute;

}	/* END SETCOMPUTEFUNCTIONS */

/* END AMLIB.C */
