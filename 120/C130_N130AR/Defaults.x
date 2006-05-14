# Greg Kok's cal coe's

o3fsm_cal	40.7
xnoym_cal[2]	-20.0, 5.2
xnom_cal[2]	-10.0, 43.0
xno2c_cal	958.0

# LI-COR 6262 CO2

c2t	309.45
co2_cal	0.15207, 1.032e-5, 7.2522e-9, -9.8728e-13, 6.5812e-17

h2t	314.65
h2o_cal	7.1352e-3, 3.6484e-6, -3.8355e-11


FLUX_RUN	120

# Ophir radiometer 

MAXCNT		62500
THERM_BIAS	5.0
THERM_RZ	36.4
THERM_COEFF	-2700.0
DFLT_GAIN	-4.4913167e-5
DFLT_OFFSET	31515.0
# Digital signal coefficients
A1              -5.6558e-5
A2              2.0565e-6


# King Probe wire temperatures - PMS version

TWIRE_PMS	115.0

# King Probe wire temperatures - RAF version

TWIRE_RAF	115.0


# IRS - Primary unit pitch and heading biases (-0.4)

PITCH_BIAS_1	0.10
HEADING_BIAS_1	-0.4

# IRS - Secondary unit pitch and heading biases (IRS right on 308)

PITCH_BIAS_2	0.0
HEADING_BIAS_2	0.0


# PMS2D Effective Area Width

C1_EAW	0.8
C2_EAW	0.8

P1_EAW	3.2
P2_EAW	3.2


# Lyman-Alpha constants - unit 1

XC_1	0.546
THRSH_1	900.0
CX_1[4]	7.08, 0.9823, 0.0, 0.0

# Lyman-Alpha constants - unit 2

XC_2	0.50
THRSH_2	900.0
CX_2[4]	4.2, 0.8712, -0.0045, -0.0056

#
# CVI
#
CVTBL	7.62
CVTBR	0.3
CVOFF	0.0
CVDIV	1.0


# FSSP constants

DOF	3.2		# Depth of field for FSSP
bdia	0.19		# Beam diameter for FSSP
tau1	0.0000058	# Multiplier for STROBES for FSSP
tau2	0.0000025	# Multiplier for RESETS for FSSP
dens	1.0		# particle density default

FSSP_CSIZ[64]	0.0,3.5,6.5,9.5,12.5,15.5,18.5,21.5,24.5,
		 27.5,30.5,33.5,36.5,39.5,42.5,45.5,		# range 0
        0.0,3.0,5.0,7.0,9.0,11.0,13.0,15.0,17.0,19.0,
                 21.0,23.0,25.0,27.0,29.0,31.0,			# range 1
        0.0,1.,2.,3.,4.,5.,6.,7.,8.,9.,10.,11.,12.,13.,14.,15.,	# range 2
        0.0,.5,1.,1.5,2.,2.5,3.,3.5,4.,4.5,5.,5.5,6.,6.5,7.,7.5	# range 3

ASAS_CSIZ[16]   0.0, 0.11, 0.13, 0.155, 0.185, 0.225, 0.275, 0.35,
		0.45, 0.6, 0.8, 1.05, 1.35, 1.75, 2.25, 2.75

F300_CSIZ[33]	0.0, 0.370,0.420,0.468,0.510,0.550,0.594,0.641,0.692,0.806,
		0.850,0.891,0.926,1.033,1.105,1.406,2.178,2.271,2.360,2.493,
		2.654,3.986,4.149,5.296,7.043,8.365,9.872,11.307,13.249,13.540,
		15.380,15.506,18.544

# CN Counter

DIV	1		# counter card prescale factor

# DNI/DEI start lat and lon

DENI_START_LAT	0.0
DENI_START_LON	0.0
