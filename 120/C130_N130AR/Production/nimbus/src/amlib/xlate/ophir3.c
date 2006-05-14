/*
-------------------------------------------------------------------------
OBJECT NAME:	ophir3.c

FULL NAME:	Translate Ophir III data block

ENTRY POINTS:	xlogain(), xlobbcod(), xlodetdc(), xlotsnt(), xlotbnch(),
		xlotdet(), xlotbbc(), xlotbbf(), xlosmotr(), xlodetsg(),
		xlotbox()

STATIC FNS:	none

DESCRIPTION:	
	Valid black body codes are as follows:

        "b" = 98        normal operation
        "z" = 122       detector zero
        "c" = 99        black body full cool cycle
        "h" = 104       black body ful heat cycle

  OGAIN          electronic gain, 1, 2, or 3
  OBBCOD         blackbody code, see above
  ODETDC         detector dc bias
  OTSNT          optical snout temperature
  OTBNCH         optical bench temperature
  OTDET          HgCdTe detector temperature
  OTBBC          cooled blackbody temperature
  OTBBF          floating blackbody temperature
  OSMOTR         motor speed
  OTBOX          detector signal
  ODETSG         electronics box temperature


INPUT:		

OUTPUT:		

REFERENCES:	none

REFERENCED BY:	rec_decode.c

COPYRIGHT:	University Corporation for Atmospheric Research, 1992
-------------------------------------------------------------------------
*/

#include "nimbus.h"
#include "amlib.h"


/* -------------------------------------------------------------------- */
void xlogain(varp, p, np)
RAWTBL		*varp;
struct Oph3_blk	*p;
NR_TYPE		*np;
{
	*np = (NR_TYPE)FirstPoly((float)p->gain, varp->cof);

}	/* END XLOGAIN */

/* -------------------------------------------------------------------- */
void xlobbcod(varp, p, np)
RAWTBL		*varp;
struct Oph3_blk	*p;
NR_TYPE		*np;
{
	*np = (NR_TYPE)p->bb_pos[0];

}	/* END XLOBBCOD */

/* -------------------------------------------------------------------- */
void xlodetdc(varp, p, np)
RAWTBL		*varp;
struct Oph3_blk	*p;
NR_TYPE		*np;
{
	*np = (NR_TYPE)FirstPoly((float)p->det_bias, varp->cof);

}	/* END XLODETDC */

/* -------------------------------------------------------------------- */
void xlotsnt(varp, p, np)
RAWTBL		*varp;
struct Oph3_blk	*p;
NR_TYPE		*np;
{
	*np = (NR_TYPE)FirstPoly((float)p->snout_tmp, varp->cof);

}	/* END XLOTSNT */

/* -------------------------------------------------------------------- */
void xlotbnch(varp, p, np)
RAWTBL		*varp;
struct Oph3_blk	*p;
NR_TYPE		*np;
{
	*np = (NR_TYPE)FirstPoly((float)p->bench_tmp, varp->cof);

}	/* END XLOTBNCH */

/* -------------------------------------------------------------------- */
void xlotdet(varp, p, np)
RAWTBL		*varp;
struct Oph3_blk	*p;
NR_TYPE		*np;
{
	*np = (NR_TYPE)FirstPoly((float)p->det_tmp, varp->cof);

}	/* END XLOTDET */

/* -------------------------------------------------------------------- */
void xlotbbc(varp, p, np)
RAWTBL		*varp;
struct Oph3_blk	*p;
NR_TYPE		*np;
{
	*np = (NR_TYPE)FirstPoly((float)p->cool_tmp, varp->cof);

}	/* END XLOTBBC */

/* -------------------------------------------------------------------- */
void xlotbbf(varp, p, np)
RAWTBL		*varp;
struct Oph3_blk	*p;
NR_TYPE		*np;
{
	*np = (NR_TYPE)FirstPoly((float)p->float_tmp, varp->cof);

}	/* END XLOTBBF */

/* -------------------------------------------------------------------- */
void xlosmotr(varp, p, np)
RAWTBL		*varp;
struct Oph3_blk	*p;
NR_TYPE		*np;
{
	*np = (NR_TYPE)FirstPoly((float)p->motor_spd, varp->cof);

}	/* END XLOSMOTR */

/* -------------------------------------------------------------------- */
void xlodetsg(varp, p, np)
RAWTBL		*varp;
struct Oph3_blk	*p;
NR_TYPE		*np;
{
	*np = (NR_TYPE)FirstPoly((float)p->det_sig, varp->cof);

}	/* END XLDETSG */

/* -------------------------------------------------------------------- */
void xlotbox(varp, p, np)
RAWTBL		*varp;
struct Oph3_blk	*p;
NR_TYPE		*np;
{
	*np = (NR_TYPE)FirstPoly((float)p->box_tmp, varp->cof);

}	/* END XLOTBOX */

/* END OPHIR3.C */
