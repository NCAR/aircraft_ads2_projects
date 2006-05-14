From darrel Wed Jan 15 20:19 MST 1997
From: darrel (Darrel Baumgardner)
Message-Id: <199701160319.UAA29522@raf.atd.ucar.EDU>
Subject: Pressure Altitude corrections
To: ace1@PMEL.NOAA
Date: Wed, 15 Jan 1997 20:19:51 -0700 (MST)
Cc: herzegh (Paul Herzegh), dick (Dick Friesen), allen (Allen Schanot),
        krista (Krista Laursen)
X-Mailer: ELM [version 2.4 PL23]
MIME-Version: 1.0
Content-Transfer-Encoding: 7bit
Content-Type: text/plain; charset=US-ASCII
Content-Length: 3852
Status: RO

Date:     January 15, 1997

To:       ACE-1 C-130 Data Users

From:     Darrel Baumgardner and Richard Schillawski, NCAR/RAF

Subject:  Pressure Altitude Corrections

The pressure altitudes that have been archived in the general RAF C-130 data
set use a standard altitude reference pressure and temperature of 1013.246
mbar, 288.15K.  This often leads to pressure altitudes during flight that are
inaccurate, particularly during low level legs over the ocean.  The research
radar altimeter was not operatioal during ACE-1 so pressure altitude is the
only measure of altitude that we have.

We have now reprocessed Flights RF03 through RF30 and corrected the pressure
altitude.  The pressure altitude is being calculated by
     PALTC = (Tr/gamma)[1-(P/Pr)^X],
where Tr and Pr are sea level values of temperature and pressure, gamma is the
standard lapse rate, and X is R*gamma/g, where R is the universal gas constant
for dry air, and g is the acceleration of gravity.

Rather than use a single reference temperature and pressure, we use the
temperature and pressure at takeoff and landing as two reference points
(Anchorage, Honolulu, Xmas Island, Fiji, Christchurch and Hobarth were all
nearly at sea level +- 5 M).  We then found all flight legs that were flown at
100, 300 or 500 ft and determined the altimeter setting (surface pressure) at
these points using the transform
     PSURF = P*exp(g/R(HGM/Tbar),
where P is the pressure at the flight altitude, g and R are as defined above,
HGM is the altimeter reading from the pilots radar altimeter, and Tbar is
the average temperature between surface and flight altitude.  The surface
temperature was assumed to be the ocean temperature determined by the
Heinmann thermometer and flight level temperature was taken from the radome
right Rosemount probe.  The pilots had a working radar altimeter so we know
the low-level flight legs fairly precisely, at least within +/-5 M.

We interpolated linearly between all the surface pressures to get estimates
between the reference points.  These were then used in the corrected
calculation of pressure altitude given above.

A major caution:  Whereas we think that these new pressure altitudes are much
more accurate than the original values, there still will be some uncertainties
that result from the assumption of a standard lapse rate.  The values close
to the surface will be most accurate, and the uncertainty will increase with
increasing altitude. 

The lidar data are presently being processed to determine aircraft altitude
when the lidar was pointing downward.  These will be the most accurate height
data, once processed, and will be used to ascertain the accuracy of the
corrected pressure altitudes.  In the meantime, we are reluctant to place
an uncertainty estimate on the corrected altitude values.  We are probably
safe in saying that within the boundary layer, the corrected altitudes are
acccurate to within +/- 25M.

The archived data files are in compressed ascii format (gzipped), and for the
time, being can be found by ftp'ing to chinook.atd.ucar.edu (user = anonymous,
password = your email)

Change directory to pub/ace.

The files are listed as newaltRn.asc.gz, where n = flight #.

We are requesting that these files be put on codiac in the near future. 
------------------------------------------------------------------------------
-- 
                      ___________________________________
                     |Darrel Baumgardner                 |
                     |NCAR/RAF                           |
                     |Box 3000                           |
                     |Boulder, Co. 80307                 |
                     |(303) 497-1054   1092 (fax)        |
                     |darrel@ncar.ucar.edu               |
                     |http://chinook.atd.ucar.edu/~darrel|
                     \___________________________________/

