/*
-------------------------------------------------------------------------
OBJECT NAME:	spef.c

FULL NAME:	Sail Plane E-Fields

ENTRY POINTS:	sfunc()

STATIC FNS:	

DESCRIPTION:	Template for AMLIB std & user directories.

INPUT:		

OUTPUT:		

REFERENCES:	

REFERENCED BY:	compute.c

COPYRIGHT:	University Corporation for Atmospheric Research, 1995
-------------------------------------------------------------------------
*/

#include "nimbus.h"
#include "amlib.h"

#include <math.h>


static NR_TYPE	*vft0, *vft1, *vft2, *vfb0, *vfb1, *vfb2,
		*vfp0, *vfp1, *vfp2, *vfs0, *vfs1, *vfs2,
		*vnt0, *vnt1, *vnb0, *vnb1, *vnp0, *vnp1, *vns0, *vns1;

/* -------------------------------------------------------------------- */
void EFinit()
{
	vft0 = GetDefaultsValue("VFT0");
	vft1 = GetDefaultsValue("VFT1");
	vft2 = GetDefaultsValue("VFT2");

	vfb0 = GetDefaultsValue("VFB0");
	vfb1 = GetDefaultsValue("VFB1");
	vfb2 = GetDefaultsValue("VFB2");

	vfp0 = GetDefaultsValue("VFP0");
	vfp1 = GetDefaultsValue("VFP1");
	vfp2 = GetDefaultsValue("VFP2");

	vfs0 = GetDefaultsValue("VFS0");
	vfs1 = GetDefaultsValue("VFS1");
	vfs2 = GetDefaultsValue("VFS2");

	vnt0 = GetDefaultsValue("VNT0");
	vnt1 = GetDefaultsValue("VNT1");

	vnb0 = GetDefaultsValue("VNB0");
	vnb1 = GetDefaultsValue("VNB1");

	vnp0 = GetDefaultsValue("VNP0");
	vnp1 = GetDefaultsValue("VNP1");

	vns0 = GetDefaultsValue("VNS0");
	vns1 = GetDefaultsValue("VNS1");

}	/* END EFINIT */

/* -------------------------------------------------------------------- */
void sefvftc(varp)
DERTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	vft, sign, at, vftc;

	vft	= GetSample(varp, 0);
	sign	= GetSample(varp, 1);

	if (vft > 1.43)
		at = vft0[0] + vft0[1] * vft;
	else
	if (vft > 0.093)
		at = vft1[0] + vft1[1] * vft +
		vft1[2] * pow((double)vft, (double)2.0) +
		vft1[3] * pow((double)vft, (double)3.0) +
		vft1[4] * pow((double)vft, (double)4.0) +
		vft1[5] * pow((double)vft, (double)5.0);
	else
		at = vft2[0] + vft2[1] * vft +
		vft2[2] * pow((double)vft, (double)2.0) +
		vft2[3] * pow((double)vft, (double)3.0);

	vftc = pow((double)10.0, (double)at) / 1000.0 * sign;

	PutSample(varp, vftc);

}	/* END SVFTC */

/* -------------------------------------------------------------------- */
void sefvfbc(varp)
DERTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	vfb, sign, at, vfbc;

	vfb	= GetSample(varp, 0);
	sign	= GetSample(varp, 1);

	if (vfb > 1.43)
		at = vfb0[0] + vfb0[1] * vfb;
	else
	if (vfb > 0.093)
		at = vfb1[0] + vfb1[1] * vfb +
		vfb1[2] * pow((double)vfb, (double)2.0) +
		vfb1[3] * pow((double)vfb, (double)3.0) +
		vfb1[4] * pow((double)vfb, (double)4.0) +
		vfb1[5] * pow((double)vfb, (double)5.0);
	else
		at = vfb2[0] + vfb2[1] * vfb +
		vfb2[2] * pow((double)vfb, (double)2.0) +
		vfb2[3] * pow((double)vfb, (double)3.0);

	vfbc = pow((double)10.0, (double)at) / 1000.0 * sign;

	PutSample(varp, vfbc);

}	/* END SVFBC */

/* -------------------------------------------------------------------- */
void sefvfpc(varp)
DERTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	vfp, sign, at, vfpc;

	vfp	= GetSample(varp, 0);
	sign	= GetSample(varp, 1);

	if (vfp > 1.43)
		at = vfp0[0] + vfp0[1] * vfp;
	else
	if (vfp > 0.093)
		at = vfp1[0] + vfp1[1] * vfp +
		vfp1[2] * pow((double)vfp, (double)2.0) +
		vfp1[3] * pow((double)vfp, (double)3.0) +
		vfp1[4] * pow((double)vfp, (double)4.0) +
		vfp1[5] * pow((double)vfp, (double)5.0);
	else
		at = vfp2[0] + vfp2[1] * vfp +
		vfp2[2] * pow((double)vfp, (double)2.0);

	vfpc = pow((double)10.0, (double)at) / 1000.0 * sign;

	PutSample(varp, vfpc);

}	/* END SVFPC */

/* -------------------------------------------------------------------- */
void sefvfsc(varp)
DERTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	vfs, sign, at, vfsc;

	vfs	= GetSample(varp, 0);
	sign	= GetSample(varp, 1);

	if (vfs > 1.43)
		at = vfs0[0] + vfs0[1] * vfs;
	else
	if (vfs > 0.093)
		at = vfs1[0] + vfs1[1] * vfs +
		vfs1[2] * pow((double)vfs, (double)2.0) +
		vfs1[3] * pow((double)vfs, (double)3.0) +
		vfs1[4] * pow((double)vfs, (double)4.0) +
		vfs1[5] * pow((double)vfs, (double)5.0);
	else
		at = vfs2[0] + vfs2[1] * vfs +
		vfs2[2] * pow((double)vfs, (double)2.0) +
		vfs2[3] * pow((double)vfs, (double)3.0);

	vfsc = pow((double)10.0, (double)at) / 1000.0 * sign;

	PutSample(varp, vfsc);

}	/* END SVFSC */

/* -------------------------------------------------------------------- */
void sefvntc(varp)
DERTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	vnt, sign, vntc;

	vnt	= GetSample(varp, 0);
	sign	= GetSample(varp, 1);

	if (sign > 0.1)
		vntc = (vnt - vnt0[0]) * vnt0[1];
	else
		vntc = (vnt - vnt1[0]) * vnt1[1];

	PutSample(varp, vntc);

}	/* END SVNTC */

/* -------------------------------------------------------------------- */
void sefvnbc(varp)
DERTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	vnb, sign, vnbc;

	vnb	= GetSample(varp, 0);
	sign	= GetSample(varp, 1);

	if (sign > 0.1)
		vnbc = (vnb - vnb0[0]) * vnb0[1];
	else
		vnbc = (vnb - vnb1[0]) * vnb1[1];

	PutSample(varp, vnbc);

}	/* END SVNBC */

/* -------------------------------------------------------------------- */
void sefvnpc(varp)
DERTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	vnp, sign, at, vnpc;

	vnp	= GetSample(varp, 0);
	sign	= GetSample(varp, 1);

	if (sign > 0.1)
		vnpc = (vnp - vnp0[0]) * vnp0[1];
	else
		vnpc = (vnp - vnp1[0]) * vnp1[1];

	PutSample(varp, vnpc);

}	/* END SVNPC */

/* -------------------------------------------------------------------- */
void sefvnsc(varp)
DERTBL	*varp;		/* Information about current variable	*/
{
	NR_TYPE	vns, sign, vnsc;

	vns	= GetSample(varp, 0);
	sign	= GetSample(varp, 1);

	if (sign > 0.1)
		vnsc = (vns - vns0[0]) * vns0[1];
	else
		vnsc = (vns - vns1[0]) * vns1[1];

	PutSample(varp, vnsc);

}	/* END SVNSC */

/* END SPEF.C */
