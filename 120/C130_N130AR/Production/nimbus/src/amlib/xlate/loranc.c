/*
-------------------------------------------------------------------------
OBJECT NAME:	loranc.c

FULL NAME:	Translate LORANC block

ENTRY POINTS:	xlcstat(), xlctmlag(), xlcnavtm(), xlclat(), xlclon(),
		xlccep(), xlcgs(), xlctk(), xlcmagvr(), xlclortm(),
		xlcwarn(), xlatmns()

DESCRIPTION:	

INPUT:		

OUTPUT:		

REFERENCES:	none

REFERENCED BY:	rec_decode.c

COPYRIGHT:	University Corporation for Atmospheric Research, 1992
-------------------------------------------------------------------------
*/

#include "nimbus.h"
#include "amlib.h"

double	atof();

/* -------------------------------------------------------------------- */
void xlcstat(varp, p, output)
RAWTBL		*varp;
struct Lrn_blk	*p;
NR_TYPE		*output;
{
	*output = (NR_TYPE)p->stat;
}

/* -------------------------------------------------------------------- */
void xlctmlag(varp, p, output)
RAWTBL		*varp;
struct Lrn_blk	*p;
NR_TYPE		*output;
{
	*output = (NR_TYPE)p->sec + (NR_TYPE)p->msec / 1000.0;
}

/* -------------------------------------------------------------------- */
void xlcnavtm(varp, p, output)
RAWTBL		*varp;
struct Lrn_blk	*p;
NR_TYPE		*output;
{
	char	*dp = (char *)&p->data1[5];
	char	c;
	NR_TYPE	cnavtm;

	c = *dp; *dp = '\0';
	cnavtm = (NR_TYPE)atof((char *)p->data1) / 60.0;
	*dp = c;

	*output = (NR_TYPE)cnavtm;

}	/* END XLCNAVTM */

/* -------------------------------------------------------------------- */
void xlclat(varp, p, output)
RAWTBL		*varp;
struct Lrn_blk	*p;
NR_TYPE		*output;
{
	char	*dp = &((char *)p->data1)[8];
	char	c;
	NR_TYPE	clat, arg1;

	c = *dp; *dp = '\0';
	clat = (NR_TYPE)atof(&((char *)p->data1)[5]);
	*dp = c;

	dp = (char *)&p->data1[12];
	c = *dp; *dp = '\0';
	arg1 = (NR_TYPE)atof(&((char *)p->data1)[8]) * 0.00016667;
	*dp = c;

	if (clat < 0)
		arg1 = -arg1;

	*output = (NR_TYPE)clat + arg1;

}	/* END XLCLAT */

/* -------------------------------------------------------------------- */
void xlclon(varp, p, output)
RAWTBL		*varp;
struct Lrn_blk	*p;
NR_TYPE		*output;
{
	char	*dp = &((char *)p->data1)[16];
	char	c;
	NR_TYPE	clon, arg1;

	c = *dp; *dp = '\0';
	clon = (NR_TYPE)atof(&((char *)p->data1)[12]);
	*dp = c;

	dp = (char *)&p->data1[20];
	c = *dp; *dp = '\0';
	arg1 = (NR_TYPE)atof(&((char *)p->data1)[16]) * 0.00016667;
	*dp = c;

	if (clon < 0)
		arg1 = -arg1;

	*output = (NR_TYPE)clon + arg1;

}	/* END XLCLON */


/* -------------------------------------------------------------------- */
void xlccep(varp, p, output)
RAWTBL		*varp;
struct Lrn_blk	*p;
NR_TYPE		*output;
{
	char	*dp = &((char *)p->data1)[41];
	char	c;
	NR_TYPE	ccep;

	c = *dp; *dp = '\0';
	ccep = (NR_TYPE)atof(&((char *)p->data1)[38]) / 10.0;
	*dp = c;

	*output = (NR_TYPE)ccep;

}	/* END XLCCEP */

/* -------------------------------------------------------------------- */
void xlccgs(varp, p, output)
RAWTBL		*varp;
struct Lrn_blk	*p;
NR_TYPE		*output;
{
	char	*dp = &((char *)p->data2)[4];
	char	c;
	NR_TYPE	ccgs;

	c = *dp; *dp = '\0';
	ccgs = (NR_TYPE)atof((char *)p->data2) * 0.051479; /* Meters/Second */
	*dp = c;

	*output = (NR_TYPE)ccgs;

}	/* END XLCCGS */

/* -------------------------------------------------------------------- */
void xlcctk(varp, p, output)
RAWTBL		*varp;
struct Lrn_blk	*p;
NR_TYPE		*output;
{
	char	*dp = &((char *)p->data2)[18];
	char	c;
	NR_TYPE	cctk;

	c = *dp; *dp = '\0';
	cctk = (NR_TYPE)atof(&((char *)p->data2)[14]) / 10.0;
	*dp = c;

	*output = (NR_TYPE)cctk;

}	/* END XLCCTK */


/* -------------------------------------------------------------------- */
void xlcmagvr(varp, p, output)
RAWTBL		*varp;
struct Lrn_blk	*p;
NR_TYPE		*output;
{
	char	*dp = &((char *)p->data2)[39];
	char	c;
	NR_TYPE	cmagvr;

	c = *dp; *dp = '\0';
	cmagvr = (NR_TYPE)atof(&((char *)p->data2)[34]) / 10.0;
	*dp = c;

	*output = (NR_TYPE)cmagvr;

}	/* END XLCMAGVR */

/* -------------------------------------------------------------------- */
void xlclortm(varp, p, output)
RAWTBL		*varp;
struct Lrn_blk	*p;
NR_TYPE		*output;
{
	char	*dp = &((char *)p->data4)[5];
	char	c;
	NR_TYPE	clortm;

	c = *dp; *dp = '\0';
	clortm = (NR_TYPE)atof((char *)p->data4) / 60.0;
	*dp = c;

	*output = (NR_TYPE)clortm;

}	/* END XLCLORTM */

/* -------------------------------------------------------------------- */
void xlcwarn(varp, p, output)
RAWTBL		*varp;
struct Lrn_blk	*p;
NR_TYPE		*output;
{
	char	*dp = &((char *)p->data4)[11];
	char	c;
	NR_TYPE	cwarn;

	c = *dp; *dp = '\0';
	cwarn = (NR_TYPE)atof(&((char *)p->data4)[8]);
	*dp = c;

	*output = (NR_TYPE)cwarn;

}	/* END XLCWARN */

/* -------------------------------------------------------------------- */
void xlatmns(varp, p, output)
RAWTBL		*varp;
struct Lrn_blk	*p;
NR_TYPE		*output;
{
	char	*dp = &((char *)p->data4)[20];
	char	c;
	NR_TYPE	catmns;

	c = *dp; *dp = '\0';
	catmns = (NR_TYPE)atof(&((char *)p->data4)[17]);
	*dp = c;

	*output = (NR_TYPE)catmns;

}	/* END XLCATMNS */

/* END LORANC.C */
