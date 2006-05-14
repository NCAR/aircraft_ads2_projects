////////////////////////////////////////////////////////////////////////
// CVI.CC -- This program will create a NASA exchange format file from
//           raw CVI data.
//
// Original Author:  Keith Barr
//                   
// Copyright 1996
// National Center for Atmospheric Research
// Written in support of SUCCESS
//
// Revision History:
//     960411, ksb, created
//     960531, ct, revised
//     960607, ct, revised to put amb. CN conc in standard cc units
//     960610, ct, revised to allow for constant offset in hyg. baseline
//     960617, ct, revised to allow for time-variable offset in hyg. baseline
//     960709, ct, revised to add better cvrad calc, inc. slip correction
//     960709, ct, revised to change L-a cal for 960512 and 960515
//		(need to check comments in 3 places, "rhola")
//		Note: this changed later so only need to change in two places
//     960912, ksb, despiked cfact and cvtas
//     961020, ct, revised due to FCN2 on other inlet 960510-960515
//		(need to check comments "other inlet" in 2 places)
//     961104, ct, revised to allow BL=rhola itself
//     971217, ct, corrected UVH routine and added new comments
//     980220, cjw, overhauled for Lake-Ice.
//     980310, ct, fine-tuned.
//     980319, cjw and ct, rhola smoothed over n sec and CFACT > 75 spiked
//     990621, ct, Mods for INDOEX: Change all scale factors to 1.0
//		Change inputs, add some (passthru) outputs, cvrad min 3.0,
//		fix "bad data" double time output
//     990713, cjw, Input time offset and adding 1 s every N records
/////////////////////////////////////////////////////////////////////////

#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cvi.h"

#define BUFFSIZE	4096
// no. of sec. to smooth over
#define N_RHOLA_AV	1

char	buffer[BUFFSIZE];

const int	MISSING_VALUE = -32767;


// Dilution factor vars.
int	df_count;
float	dfStart[100], dfEnd[100], df[100];

// Bad value time segments.
int	bv_count;
float	bvStart[100], bvEnd[100];

// Time alteration stuff
int	timeBias, recordOffset, recCnt;

// Baseline time segment stuff
int	iwc, calType[100];
int	bl_count = 0, blTime[100];
float	laC0[100], laC1[100], laC2[100];
float	uvC0[100], uvC1[100], uvC2[100];

float	tasFactor;

FILE	*inFile, *out;


void QueryUser(), WriteNASAheader(FILE *out, int iwc);
void compute(struct CVIQ *cviq);


/* -------------------------------------------------------------------- */
int main()
{
  struct CVIQ cviq;


  QueryUser();
  WriteNASAheader(out, iwc);


  // Main loop. cviq values are from input file.
  while (fgets(buffer, BUFFSIZE, inFile))
    {
    sscanf(buffer,	"%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f"
			"%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f"
			"%f %f %f %f %f %f %f %f %f %f %f",
	&cviq.setValue, &cviq.inflag, &cviq.time, &cviq.cvrad, &cviq.cvf3,
	&cviq.cvl, &cviq.cvf2c, &cviq.fcn1c, &cviq.fcn2c, &cviq.fhc,
	&cviq.fx2c, &cviq.cvtas, &cviq.cnc1, &cviq.cnc2, &cviq.cfact,
	&cviq.cnco1, &cviq.cnco2, &cviq.rhouv, &cviq.rhola, &cviq.rhoola,
	&cviq.rhoouv, &cviq.cvf1, &cviq.mxuv, &cviq.cvpcn, &cviq.fx1c,
	&cviq.fx0c, &cviq.cvf1R, &cviq.cvf2R, &cviq.fcn1R,&cviq.fcn2R,&cviq.fhR,
	&cviq.fx2R, &cviq.cvttR, &cviq.cvpcnR, &cviq.cvqcR, &cviq.cnt1R,
	&cviq.cnt2R, &cviq.cvtpR, &cviq.rholaR, &cviq.cnhtR, &cviq.fx0R, 
	&cviq.fx1R, &cviq.cnct1);

    cviq.time = (int)(cviq.time + 0.5);	// Round up.

    compute(&cviq);

    if ((recCnt % recordOffset) == 0)
      {
      ++timeBias;
      compute(&cviq);
      }
    }

}	/* END MAIN */

/* -------------------------------------------------------------------- */
void compute(struct CVIQ *cviq)
{
  static int	firstTime = true;
  static struct CVI_EXCH cviOut[MAX_LAG], save;
  static int	count, cur_bl;
  static int	prevTime, rholaAvP = 0;
  static float	cvtasOld;
  static float	rav[N_RHOLA_AV];

  int	i, rhBin, cnBin, lastcount, thisTime;
  float	cvlrad, rad, rholaAv;
  float	rholac, offsetla;
  float	rhouvc, offsetuv;
  float	rhouvdiff, cfactdiff, cvtasdiff;
  float	cvtascc, cvpmb, cvttc, rhoa, nu, lambda, re, cc, fcor, cvstp;
  float	cnct1coin, fcn1slpm; 
  unsigned short cfact;
  int	LAST;


  if (firstTime)
    {
    firstTime = false;

    recCnt = 0;
    count = 0;
    cur_bl = 0;
    memset(cviOut, 0, sizeof(cviOut));
    }

  // Increment baseline time segment.
  if (cviq->time > blTime[cur_bl+1])
    ++cur_bl;

  // calculate calibrated rhola from RholaRaw if necessary
  // for Lake Ice R6-R12 and Indoex R16 only; comment out otherwise
  //cviq->rhola = 5.2749 + 1.891408 * cviq->rholaR + 
//	0.0989252*(cviq->rholaR * cviq->rholaR);

  // Rhola averaging (due to noisy signal in Lake-Ice).
  rav[rholaAvP++] = cviq->rhola;
  if (rholaAvP == N_RHOLA_AV)
    rholaAvP = 0;

  for (i = 0, rholaAv = 0.0; i < N_RHOLA_AV; ++i)
    rholaAv += rav[i];

  rholaAv /= N_RHOLA_AV;


  // zero out our saving aray
//  memset((char *)&cviOut[count], 0, sizeof(CVI_EXCH));

  // correct cfact for faster airspeed at CVI location
  cviq->cfact *= tasFactor;

  // correct cfact for FCN2 on other inlet
//  cviq->cfact *= 6.1 / 7.7;

  // put what we can into the array
  cviOut[count].gmts = cviq->time + timeBias;


  // if cfact is 1.0, use the last one
  cfact = (unsigned short)cviq->cfact;
  LAST = (count + MAX_LAG - 1) % MAX_LAG;
  if (cfact == 1)
    cviq->cfact = cviOut[LAST].cfact;

  // save these values until we are done
  rhBin = (count + (MAX_LAG - RH_LAG)) % MAX_LAG;
  cnBin = (count + (MAX_LAG - CN_LAG)) % MAX_LAG;
  cviOut[rhBin].rholaR = cviq->rholaR;
  cviOut[rhBin].rhola = rholaAv; // average over n sec instead of cviq->rhola;
  cviOut[rhBin].rhouv = cviq->rhouv;
  cviOut[cnBin].inflag = (long)cviq->inflag;
  cviOut[cnBin].cnc1 = cviq->cnc1;
  cviOut[cnBin].fcn1c = cviq->fcn1c;
  cviOut[cnBin].fcn1R = cviq->fcn1R;
  cviOut[count].cfact = cviq->cfact;
  cviOut[count].cvpcnR = cviq->cvpcnR;
  cviOut[count].fx0c = cviq->fx0c;
  cviOut[count].fx1c = cviq->fx1c;
  cviOut[count].fx2c = cviq->fx2c;
  cviOut[count].fcn2c = cviq->fcn2c;
  cviOut[count].cnhtR = cviq->cnhtR;

//  printf("%f %f %f \n", cviq->time, cviq->cfact, cviq->cvtas);

  if (recCnt > 10)	// Don't start despiking for 10 seconds.
    {
    // do we need to despike cfact?
    cfactdiff = cviOut[count].cfact - cviOut[LAST].cfact;
    if (fabs(cfactdiff) > 8.0)
         //&& cviOut[count].cfact < 75 && cviOut[LAST].cfact < 75)
      {
      printf("CFACT spiked @ %d\n", (int)cviOut[count].gmts);
      printf("  %f %f\n", cviOut[LAST].cfact, cviOut[count].cfact);
      cviOut[count].cfact = cviOut[LAST].cfact;
      }
		
    // do we need to despike the rhouv value?
    lastcount = (rhBin + MAX_LAG - 1) % MAX_LAG;
    rhouvdiff = cviOut[rhBin].rhouv - cviOut[lastcount].rhouv;
    if (rhouvdiff > 50.0 || rhouvdiff < -50.0)
      cviOut[rhBin].rhouv = cviOut[lastcount].rhouv;

    // do we need to despike cvtas?
    cvtasdiff = cviq->cvtas - cvtasOld;
    if (cvtasdiff > 19.0 || cvtasdiff < -19.0)
      {
      cviq->cvtas = cvtasOld;
      printf("TAS SPIKED @ %d\n", (int)cviOut[count].gmts);
      }
    }

  cvtasOld = cviq->cvtas;

  // we can now process the data from MAX_LAG seconds ago
  count++;
  count %= MAX_LAG;
  thisTime = cviOut[count].gmts;

  // calculate offsets
  if (calType[cur_bl] == 2)
    {	// constant baseline
    offsetla = laC0[cur_bl];
    offsetuv = uvC0[cur_bl];
    }
  else
  if (calType[cur_bl] == 3)
    {	// use coefficients
    offsetla = laC0[cur_bl]
		+ laC1[cur_bl] * cviOut[count].cvpcn
		+ laC2[cur_bl] * cviOut[count].cvpcn * cviOut[count].cvpcn;

    offsetuv = uvC0[cur_bl]
		+ uvC1[cur_bl] * cviOut[count].cvpcn
		+ uvC2[cur_bl] * cviOut[count].cvpcn * cviOut[count].cvpcn;
    }
  else
  if (calType[cur_bl] == 4)
    {	// use coefficients
    offsetla = laC0[cur_bl]
		+ laC1[cur_bl] * cviOut[count].gmts
		+ laC2[cur_bl] * cviOut[count].gmts * cviOut[count].gmts;

    offsetuv = uvC0[cur_bl]
		+ uvC1[cur_bl] * cviOut[count].gmts
		+ uvC2[cur_bl] * cviOut[count].gmts * cviOut[count].gmts;
    }
  else
  if (calType[cur_bl] == 5)
    {//	 calc rhola if cal from raw voltage otherwise use rhola
	//below is cal for Indoex R12 on
   //offsetla = 5.2749 + 1.891408 * cviOut[count].rholaR + 
    // 0.0989252 * (cviOut[count].rholaR * cviOut[count].rholaR);

    offsetla = cviOut[count].rhola;
    offsetuv = cviOut[count].rhouv;
    }
  
  rholac = rholaAv - offsetla;
  rhouvc = cviOut[count].rhouv - offsetuv;

  if (cviOut[count].inflag == CVI)
    {
    // convert cvpcn and cvtt from volts
    cvpmb = 99.003 + 498.94 * cviq->cvpcnR + 0.00 * cviq->cvpcnR 
	* cviq->cvpcnR;
    cvttc = -50.0 + 40.0 * cviq->cvttR;

    // calculate air density, viscosity, lambda
    rhoa = cvpmb * 0.1 / (287.0 * (cvttc + 273.15));
    nu = (1.718 + 0.0049 * cvttc) * 0.0001;
    lambda = 2.398E-5 * (cvttc + 273.15) / cvpmb;
    cvtascc = cviq->cvtas * tasFactor * 100;
    rad = 3.0;
    cvlrad = 0.1;

    // correction for fcn2 on other inlet
//    cviq->cvl = cviq->cvl + 1.6;

    while (cvlrad <= cviq->cvl)
      {
      rad += 0.1;
      re = 0.0002 * rad * cvtascc * rhoa / nu;
      cc = 1.0 + lambda * (1.257 + 0.4 *
		double (pow(2.71728, -0.00011 * rad / lambda))) / (1E-4 * rad);	
      fcor = 0.408248 * pow(re, 0.3333333) +
		atan(2.44949 * pow(re, -0.3333333)) - 1.570796327;

      // calc. stop distance in cm
      cvstp = rad * 0.0001 * 14.6969 * cc * fcor / (3.0 * rhoa);
      cvlrad = cvstp - 0.53;
//printf("%10.3f %10.7f %10.6f %10.6f %10.3f %10.5f %10.5f %10.5f\n", rad, lambda, nu, rhoa, cviq->time, re, cc, fcor);
      }

    if (rad > 0.0 && rad < 19.99)
      cviOut[cnBin].cvrad = (rad+0.05);
    else
      cviOut[cnBin].cvrad = MISSING_VALUE;
								
    if (cviOut[count].cfact > 0)
      {
      if (iwc == 0)
        cviOut[count].iwc = (rholac / cviOut[count].cfact);
      else
        cviOut[count].iwc = (rhouvc / cviOut[count].cfact);

      cviOut[count].num =
			(cviOut[count].cnc1 / cviOut[count].cfact);
      }
    else
      {
      cviOut[count].iwc = MISSING_VALUE;
      cviOut[count].num = MISSING_VALUE;
      }
    }
  else
    {
    cviOut[count].iwc = 0;

// If desired, comment 6 calc. lines below, and uncomment calc. section 
// below that to convert to CN conc. in stand cm-3
    
//      cviOut[count].num = 
//			(cviOut[count].cnc1);
    
    // do we need to correct the dilution factor?
    
//      for (i = 0; i < df_count; i++)
//      if (cviOut[count].gmts >= dfStart[i] && cviOut[count].gmts <= dfEnd[i])
//        cviOut[count].num =
//		(df[i] * cviOut[count].cnc1);

// Uncomment 9 lines below to convert to CN conc. in stand cm-3
    
// change cn conc. back to cncts/s
   cnct1coin = 
		(cviOut[count].cnc1 * (cviOut[count].fcn1c * 1000.0 / 60.0));

   // change fcn1 from volts to slpm
   fcn1slpm = (0.03 + cviOut[count].fcn1R * 0.39036 + cviOut[count].fcn1R 
	* cviOut[count].fcn1R * 0.00);

   // recalc. cn conc in numb per std. cc
   cviOut[count].num = (cnct1coin / (fcn1slpm * 1000.0 / 60.0));

   // do we need to correct the dilution factor?
     for (i = 0; i < df_count; i++)
     if (cviOut[count].gmts >= dfStart[i] && cviOut[count].gmts <= dfEnd[i])
       cviOut[count].num = 
		(df[i] * cnct1coin / (fcn1slpm * 1000.0 / 60.0));

    cviOut[count].cvrad = MISSING_VALUE;
    }


// Lines below were used for SUCCESS and LakeIce when had scaling factors
//  if (cviOut[count].num < 100)
//    cviOut[count].num = 0;

// Lines below were used for SUCCESS and LakeIce when had scaling factors
// if (cviOut[count].iwc < 1)
//    cviOut[count].iwc = 0;


  // Don't write anything out for the first few seconds, let lag buffer fill up.
  if (recCnt++ < MAX_LAG + 2)
    {
    prevTime = thisTime;
    return;
    }


  // Print out missing values if "bad data".
  for (i = 0; i < bv_count; ++i)
    if (cviOut[count].gmts > bvStart[i] && cviOut[count].gmts < bvEnd[i])
      {
      fprintf(out, "%10.3f %1d %4d %7d %6d %8.2f %8.2f %8.2f %8.2f %8.2f %8.2f\n",
		cviOut[count].gmts, 
		cviOut[count].inflag, 
		MISSING_VALUE, 
		MISSING_VALUE,
		MISSING_VALUE, 
		cviOut[count].cfact,
		cviOut[count].fx0c,
		cviOut[count].fx1c,
		cviOut[count].fx2c,
		cviOut[count].fcn2c,
		cviOut[count].cnhtR);
      prevTime = thisTime; 
      return;
      }

  // Check for time gaps.
  if (thisTime != prevTime + 1)
    {
    if (prevTime < thisTime)
      {
      struct CVI_EXCH	temp, *pb;
      int     timeDiff = thisTime - prevTime;
      float   fac;
 
      printf("Time gap - %d %d\n", prevTime, thisTime);
      pb = &save;
 
      for (i = 1; i < timeDiff; ++i)
        {
        temp.gmts = prevTime + i;
        fac = (float)i / timeDiff;

        // Set up buffer with interp'ed values.
        temp.inflag = pb->inflag + ((cviOut[count].inflag - pb->inflag) * fac);
        temp.cvrad = pb->cvrad + ((cviOut[count].cvrad - pb->cvrad) * fac);
        temp.iwc = pb->iwc + ((cviOut[count].iwc - pb->iwc) * fac);
        temp.num = pb->num + ((cviOut[count].num - pb->num) * fac);
//printf("pb=%d, (cur=%d, ), fac=%f\n", pb->num, cviOut[count].num, fac);
        temp.cfact = pb->cfact + ((cviOut[count].cfact - pb->cfact) * fac);
        temp.fx0c = pb->fx0c + ((cviOut[count].fx0c - pb->fx0c) * fac);
        temp.fx1c = pb->fx1c + ((cviOut[count].fx1c - pb->fx1c) * fac);
        temp.fx2c = pb->fx2c + ((cviOut[count].fx2c - pb->fx2c) * fac);
        temp.fcn2c = pb->fcn2c + ((cviOut[count].fcn2c - pb->fcn2c) * fac);
        temp.cnhtR = pb->cnhtR + ((cviOut[count].cnhtR - pb->cnhtR) * fac);
 
        printf("%10.3f %1d %4.1f %7.3f %7.1f %8.2f %8.2f %8.2f %8.2f %8.2f %8.2f\n",
		temp.gmts, temp.inflag, temp.cvrad, temp.iwc, temp.num,
		temp.cfact, temp.fx0c, temp.fx1c, temp.fx2c, temp.fcn2c,
		temp.cnhtR);
        fprintf(out, "%10.3f %1d %4.1f %7.3f %7.1f %8.2f %8.2f %8.2f %8.2f %8.2f %8.2f		\n",
		temp.gmts, temp.inflag, temp.cvrad, temp.iwc, temp.num,
		temp.cfact, temp.fx0c, temp.fx1c, temp.fx2c, temp.fcn2c,
		temp.cnhtR);
        }
      }
 
    if (prevTime == thisTime)
      {
      fprintf(stderr, "Duplicate time stamp @ %d, ignoring.\n", thisTime);
      return;
      }
    }

  // print out the data, if not "bad value"
  fprintf(out, "%10.3f  %1d  %4.1f %7.3f %7.1f %8.2f %8.2f %8.2f %8.2f %8.2f %8.2f\n",
		cviOut[count].gmts,
		cviOut[count].inflag,
		cviOut[count].cvrad,
		cviOut[count].iwc,
		cviOut[count].num,
		cviOut[count].cfact,
		cviOut[count].fx0c,
		cviOut[count].fx1c,
		cviOut[count].fx2c,
		cviOut[count].fcn2c,
		cviOut[count].cnhtR);

  prevTime = thisTime;
  save = cviOut[count];

}	/* END COMPUTE */

/* -------------------------------------------------------------------- */
void QueryUser()
{
  int	i;
  char	fileName[512];

  // Open input & output files.
  do
    {
    printf("Enter input filename: ");
    gets(fileName);

    if ((inFile = fopen(fileName, "r")) == NULL)
      printf("Can't open %s.\n", fileName);
    }
  while (inFile == NULL);


  do
    {
    printf("Enter output filename: ");
    gets(fileName);

    if ((out = fopen(fileName, "w+")) == NULL)
      printf("Can't open %s.\n", fileName);
    }
  while (out == NULL);


  printf("\nEnter TAS factor [1.0]: ");
  gets(buffer);
  if (strlen(buffer))
    tasFactor = atof(buffer);
  else
    tasFactor = 1.0;


  printf("\nDo you want to use the Lyman-Alpha or the UVH for IWC?\n");
  printf(" [0] Lyman-Alpha\n");
  printf(" [1] UV Hygrometer\n");
  printf("choice?> ");
  gets(buffer);
  iwc = atoi(buffer);

  if (iwc == 0)
    {	// Lyman-Alpha
    printf("\nEnter start time of first segment [return for start] : ");
    gets(buffer);
    blTime[0] = atoi(buffer);

    do
      {
      printf("Enter start time of next segment [return for end of file] : ");
      gets(buffer);

      blTime[bl_count+1] = strlen(buffer) ? atoi(buffer) : 99900;

      do
        {
        printf("Constant BL, a func. of CVPCN or time, or RHOLA?\n");
        printf(" [2] constant\n");
        printf(" [3] function of CVPCN (volts)\n");
        printf(" [4] function of time (utc sec)\n");
        printf(" [5] RHOLA\n");
        printf("choice?> ");
        gets(buffer);
        calType[bl_count] = atoi(buffer);
        }
      while (calType[bl_count] < 2 || calType[bl_count] > 5);

      switch (calType[bl_count])
        {
        case 2:
          printf("Please enter the Lyman-alpha constant baseline: ");
          gets(buffer);
          laC0[bl_count] = atof(buffer);
          break;

        case 3:
          printf("Enter the Lyman-alpha/P 0-power coefficient: ");
          gets(buffer);
          laC0[bl_count] = atof(buffer);

          printf("Enter the Lyman-alpha/P 1-power coefficient: ");
          gets(buffer);
          laC1[bl_count] = atof(buffer);

          printf("Enter the Lyman-alpha/P 2-power coefficient: ");
          gets(buffer);
          laC2[bl_count] = atof(buffer);
          break;

        case 4:
          printf("Enter the Lyman-alpha/time 0-power coefficient: ");
          gets(buffer);
          laC0[bl_count] = atof(buffer);

          printf("Enter the Lyman-alpha/time 1-power coefficient: ");
          gets(buffer);
          laC1[bl_count] = atof(buffer);

          printf("Enter the Lyman-alpha/time 2-power coefficient: ");
          gets(buffer);
          laC2[bl_count] = atof(buffer);
          break;
        }
      }
    while (blTime[++bl_count] < 99900);
    }


  if (iwc == 1)		// UV Hygrometer
    {
    printf("\nEnter start time of first segment [return for start] : ");
    gets(buffer);
    blTime[0] = atoi(buffer);

    do
      {
      printf("Enter time of next segment [return for end of file] : ");
      gets(buffer);

      blTime[bl_count+1] = strlen(buffer) ? atoi(buffer) : 99900;

      do
        {
        printf("Constant BL, a func. of CVPCN or time, or RHOUV?\n");
        printf(" [2] constant\n");
        printf(" [3] function of CVPCN (volts)\n");
        printf(" [4] function of time (utc sec)\n");
        printf(" [5] RHOUV\n");
        printf("choice?> ");
        gets(buffer);
        calType[bl_count] = atoi(buffer);
        }
      while (calType[bl_count] < 2 || calType[bl_count] > 5);

      switch (calType[bl_count])
        {
        case 2:
          printf("Please enter the UVH constant baseline: ");
          gets(buffer);
          uvC0[bl_count] = atof(buffer);
          break;

        case 3:
          printf("Enter the UV Hygrometer/P 0-power coefficient: ");
          gets(buffer);
          uvC0[bl_count] = atof(buffer);

          printf("Enter the UV Hygrometer/P 1-power coefficient: ");
          gets(buffer);
          uvC1[bl_count] = atof(buffer);

          printf("Enter the UV Hygrometer/P 2-power coefficient: ");
          gets(buffer);
          uvC2[bl_count] = atof(buffer);
          break;

        case 4:
          printf("Enter the UV Hygrometer/time 0-power coefficient: ");
          gets(buffer);
          uvC0[bl_count] = atof(buffer);

          printf("Enter the UV Hygrometer/time 1-power coefficient: ");
          gets(buffer);
          uvC1[bl_count] = atof(buffer);

          printf("Enter the UV Hygrometer/time 2-power coefficient: ");
          gets(buffer);
          uvC2[bl_count] = atof(buffer);
          break;
        }	
      }
    while (blTime[++bl_count] < 99900);
    }



  printf("How many dilution factor times (100 max): ");
  gets(buffer);
  df_count = atoi(buffer);

  for (i = 0; i < df_count; i++)
    {
    printf("Please enter Dilution Factor block %d start time: ", i);
    gets(buffer);
    dfStart[i] = atof(buffer);

    printf("Please enter Dilution Factor block %d end time: ", i);
    gets(buffer);
    dfEnd[i] = atof(buffer);

    printf("Please enter the dilution factor for block %d: ", i);
    gets(buffer);
    df[i] = atof(buffer);
    }


  printf("\nHow many bad data time segments (100 max): ");
  gets(buffer);
  bv_count = atoi(buffer);

  for (i = 0; i < bv_count; i++)
    {
    printf("Enter bad data %d start time: ", i);
    gets(buffer);
    bvStart[i] = atof(buffer);

    printf("Enter bad data %d end time: ", i);
    gets(buffer);
    bvEnd[i] = atof(buffer);
    }

  printf("\nTime offset : ");
  gets(buffer);
  timeBias = atoi(buffer);

  printf("\nAdd 1 second every n records (enter n) : ");
  gets(buffer);
  recordOffset = atoi(buffer);

}	/* END QUERYUSER */

/* -------------------------------------------------------------------- */
void WriteNASAheader(FILE *out, int iwc)
{
  // write the output file header
  fprintf(out, "43 1001\n");	// number of header lines
  fprintf(out, "Twohy, Cynthia\n");
  fprintf(out, "National Center for Atmospheric Research\n");
  fprintf(out, "Condensed Water Cont. and Number Conc. from NCAR C-130 CVI\n");
  fprintf(out, "Indoex\n");
  fprintf(out, "1 1\n");
  fprintf(out, "1999 03 25  1999 7 15\n"); // flight&proc dates
  fprintf(out, "1\n");	// non uniform time intervals
  fprintf(out, "Elapsed UT Seconds of flight date\n");
  fprintf(out, "10\n");
  fprintf(out, "1 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0\n");
  fprintf(out, "9 %d %d %d %d %d %d %d %d %d %d\n", 
	MISSING_VALUE, MISSING_VALUE, MISSING_VALUE, MISSING_VALUE, 
	MISSING_VALUE, MISSING_VALUE, MISSING_VALUE, MISSING_VALUE, 
	MISSING_VALUE, MISSING_VALUE);
  fprintf(out, "Inlet; 0=CVI, 1=Whole, 2=Interstitial\n");
  fprintf(out, "CVI minimum radius sampled (microns)\n");
  fprintf(out, "CVI condensed water content (g m-3)\n");
  fprintf(out, "CVI particle number concentration (cm-3)\n");
  fprintf(out, "CVI concentration Factor(none)\n");
  fprintf(out, "CVI LASAIR Flow (vlpm)\n");
  fprintf(out, "CVI EM Flow 1 (vlpm)\n");
  fprintf(out, "CCNC Flow (vlpm)\n");
  fprintf(out, "CVI EM FLow 2 (vlpm)\n");
  fprintf(out, "CVI EM Switch (volt)\n");
  fprintf(out, "3\n");
  fprintf(out, "FINAL DATA SET\n");

  if (iwc == 0)
    fprintf(out, "CWC from Lyman-alpha\n");
  if (iwc == 1)
    fprintf(out, "CWC from UV Hygrometer\n");

  fprintf(out, "Cvrad includes slip correction\n");
  fprintf(out, "16\n");
  fprintf(out, "Inlet 0: CVI (samples particles larger than cvrad)\n");
  fprintf(out, "Inlet 1: Whole air sample (CWC set to 0)\n");
  fprintf(out, "Inlet 2: Interstitial sample (CWC set to 0)\n");
  fprintf(out, "Particle number concentrations in ambient cm-3 for CVI (0)\n");
  fprintf(out, "(per cloud physics convention) but in standard cm-3 for\n"); 
  fprintf(out, "aerosol inlets 1 and 2\n");
  fprintf(out,
   "CVI number concentrations represent # of residual particles measured\n");
  fprintf(out,
   "after crystal impaction and sublimation--accurate for small ice crystal\n");
  fprintf(out,
   "measurements, but to be used with caution when very large (>100 micron)\n");
  fprintf(out,
   "crystals are present (i.e, aged cirrus). Under these circumstances, one\n");
  fprintf(out,
   "crystal may yield >1 residual.\n");
  fprintf(out,
   "2D or 260X data can be consulted for information on large crystal abundances.\n");
  fprintf(out, "CVI number concentrations not reported if <0.1/cc.\n");
  fprintf(out,
    "Email twohy@ucar.edu with questions or for more details.\n");

  fprintf(out, "\n  GMTS CVINFLAG CVRAD  CVCWC     CVN   CVFACT   CVFX0C   CVFX1C  CVFX2C  CVFCN2C  CVEMSW\n");

}	/* END WRITEHEADER */

/* END CVI.CC */
