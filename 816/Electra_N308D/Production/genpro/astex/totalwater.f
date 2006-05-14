	subroutine twater(tdry,twet,dpbc,atb,psfdc,qcxc,concf,twat,rtot,
     $                   xmrb)
c	This is the code for the CSIRO total water probe use during ASTEX
c	There are a number of extra parameters that must be included. These
c	parameters are passed through the subroutine statement. The
c	additional parameters are:
c	DPBC
c	ATB
c	PSFDC
c	QCXC
c	CONCF
c
c These are the thermodynamic constants needed in the calculations
      	data alhv0/2.49910E10/,dalhv/0.0234E10/
      	data cpd/1.005E7/
      	DATA eps/0.622/,CW/4.218E3/,RD/287.05/,ALHV/2.501E6/
c !!!!!!!!!!!!!!!!!!!!!!!ASTEX!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
c The dry and wet bulb temperatures in the probe are assumed to be recorded
c as voltages with no gain or offset and a cal coefficient of 0 -1 0.
c The following values are the calibration coefficents determined during
c post-cals after the project.
c     Dry Bulb c0= 16.0370   c1 = 7.5431   c2 = -0.4856
      	data c0d/16.0370/,c1d/7.5431/, c2d/ -0.4856/
c     wet bulb c0= 14.8766   c1 = +7.4765   c2 = -0.4077
      	data c0w/14.8766/,c1w/7.4765/, c2w/ -0.4077/
c Calculate the mixing ratio from the bottom dew pointer
      	edpb=vapor(dpbc)
      	XMRB=622*edpb/(psfdc-edpb)
c Calculate the saturation mixing ratio at this temperature
	esat=vapor(atb)
        satmr=622*esat/(psfdc-esat)
c If in cloud, assume that the mixing ratio is that at saturation for the given
c temperature
	if (concf.gt.5)then
	   xmr=satmr
	else
	   xmr=xmrb
	end if
222     T = c0d     +c1d*tdry +c2d*tdry*tdry
	tdry=t
        TW= c0w     +c1w*twet +c2w*twet*twet
	twet=tw
        P = PSFDC
        ATBK = ATB + 273.16
        TK = T + 273.16
        TWK = TW + 273.16
c	calculate the wet bulb mixing ratio
111	etw=vapor(tw)
   	rw=0.62197*etw/(p-etw+qcxc)
c  calculate the dry bulb mixing ratio
c..... ratio Cw/Cp = 4.16667
        cp=cpd*(1.+4.166667*rw)
c....... calculate latent heat for average temperature in range
        alhv=alhv0+dalhv*tw
c....... get mixing ratio inside, hence also outside
        r=cp/alhv*(tw-t)+rw
c..
	if (r.le.0)r=rw
	r=r*1000.
c!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ASTEX ONLY!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
c	This is a correction to account for things we don't understand
c	This makes the out-of-cloud mixing ratios between the bottom
c	dew pointer and wet bulb values agree fairly well.
	r=0.419268 +0.397572*r + 0.0649092*r*r
c!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
C  solve for TOTAL WATER g/m**3
c
        rhod= 0.34838*psfdc/atbk
	if (r.lt.xmr)r=xmrb
        twat = (r-xmr)*rhod
	if ((twat.lt.0).or.(concf.lt.1))twat=0
	rtot=r
	return
        end 
        FUNCTION VAPOR(TFP)
C INPUT IS IN DEGREES C.
C ROUTINE CODES GOFF-GRATCH FORMULA
        T=273.15+TFP
C THIS IS WATER SATURATION VAPOR PRESSURE
        E=-7.90298*(373.16/T-1.)+5.02808*ALOG10(373.16/T)
     $  -1.3816E-7*(10.**(11.344*(1.-T/373.16))-1.)
     $  +8.1328E-3*(10.**(3.49149*(1.-373.16/T))-1.)
        VAPOR=1013.246*10.**E
        RETURN
        END
