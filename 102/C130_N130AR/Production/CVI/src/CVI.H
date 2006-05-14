//////////////////////////////////////////////////////////////////////////
// cvi.h
//
// This header file contains the structure definitions for Cindy Twohy's
// CVI instrument flown on NASA's DC-8 during SUCCESS
//
// Written by:
//      Keith S. Barr
//      In support of SUCCESS
//
// Copyright 1996, National Center for Atmospheric Research
//////////////////////////////////////////////////////////////////////////

#ifndef CVI_H
#define CVI_H

#define CN_LAG	1
#define RH_LAG	0

#define CVI 	0
#define INT 	0
#define WHOLE	0

// figure out the maximum lag for use in arrays
#define MAX_LAG	(CN_LAG > RH_LAG ? CN_LAG+1 : RH_LAG+1)

// Structure of input data.
struct CVIQ
	{
	float setValue;
	float inflag;
	float time;
	float cvrad;
	float cvf3;

	float cvl;
	float cvf2c;
	float fcn1c;
	float fcn2c;
	float fhc;

	float fx2c;
	float cvtas;
	float cnc1;
	float cnc2;
	float cfact;

	float cnco1;
	float cnco2;
	float rhouv;
	float rhola;
	float rhoola;

	float rhoouv;
	float cvf1;
	float mxuv;
	float cvpcn;
	float fx1c;
	
	float fx0c;
	float cvf1R;
	float cvf2R;
	float fcn1R;
	float fcn2R;
	
	float fhR;
	float fx2R;
	float cvttR;
	float cvpcnR;
	float cvqcR;
	
	float cnt1R;
	float cnt2R;
	float cvtpR;
	float rholaR;
	float cnhtR;
	
	float fx0R;
	float fx1R;
	float cnct1;
	};

struct CVI_EXCH
	{
	float	gmts;		// GMT Seconds (sec)
	long	inflag;		// which inlet? (dim)
	float	cvrad;		// min radius...CVI only (um)
	float	iwc;		// Lyman-alpha Ice Water Content (g/m3)
	float	num;		// Number conc of non-volatile particles (#/cm3)
	float	rhola;
	float	rhouv;
	float	rholaR;
	float	cfact;
	float	cnc1;
	float	fcn1c;
	float	fcn1R;
	float	cvpcn;
	float	cvpcnR;
	float	cvtas;
	float	fx0c;
	float	fx1c;
	float	fx2c;
	float	fcn2c;
	float	cnhtR;
	};

# endif
