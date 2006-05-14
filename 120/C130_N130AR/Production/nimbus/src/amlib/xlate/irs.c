/*
-------------------------------------------------------------------------
OBJECT NAME:	irs.c

FULL NAME:	Convert Honeywell Inertial block data

ENTRY POINTS:	xlilat(), xlilon(), xlipitch(), xliroll(), xlithdg(),
		xligspd(), xlvacc(), xlivspd(), xlivns(), xlivew(),
		xllag50(), xllag25(), xllag10(), xllag5(), xlialt()

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

#define FTMTR	0.3048
#define MPS2	9.7959
#define KTS2MS	0.514791
#define FTMIN	0.00508

static float thdglcorr;		/* irs box alignment correction	*/
static float thdgrcorr;		/* irs box alignment correction	*/
static float pitchrcorr;	/* irs box alignment correction	*/
static float pitchlcorr;	/* irs box alignment correction	*/


/* -------------------------------------------------------------------- */
void irsInit()
{
	thdglcorr = (GetDefaultsValue("HEADING_BIAS_1"))[0];
	thdgrcorr = (GetDefaultsValue("HEADING_BIAS_2"))[0];
	pitchlcorr = (GetDefaultsValue("PITCH_BIAS_1"))[0];
	pitchrcorr = (GetDefaultsValue("PITCH_BIAS_2"))[0];

}	/* END IRSINIT */

/* -------------------------------------------------------------------- */
void xlilat(varp, p, np)
RAWTBL		*varp;
struct Irs_blk	*p;	/* Source	*/
NR_TYPE		*np;	/* Destination	*/
{
	int	i, x;

	/* (degs) 21 bits, +- 1.0 pirad, label 310	*/

	for (i = 0; i < varp->SampleRate; ++i)
		{
		x = p->present_lat[i] & 0xfffff800;
		np[i] = x / 2147483648.0 * 180.0;
		}

	varp->DynamicLag = p->lag_5hz_frame;

}	/* END XLILAT */

/* -------------------------------------------------------------------- */
void xlilon(varp, p, np)
RAWTBL		*varp;
struct Irs_blk	*p;	/* Source	*/
NR_TYPE		*np;	/* Destination	*/
{
	int	i, x;

	/* (degs) 21 bits, +- 1.0 pirad, label 311	*/

	for (i = 0; i < varp->SampleRate; ++i)
		{
		x = p->present_lon[i] & 0xfffff800;
		np[i] = x / 2147483648.0 * 180.0;
		}

	varp->DynamicLag = p->lag_5hz_frame;

}	/* END XLILON */

/* -------------------------------------------------------------------- */
void xlipitch(varp, p, np)
RAWTBL		*varp;
struct Irs_blk	*p;	/* Source	*/
NR_TYPE		*np;	/* Destination	*/
{
	int	i, x;
	float	correction;

	if (varp->name[5] != '\0')
		correction = pitchrcorr;
	else
		correction = pitchlcorr;

	/* (degs) 20 bits, +- 1.0 pirad, label 324	*/

	for (i = 0; i < varp->SampleRate; ++i)
		{
		x = p->pitch_angle[i] & 0xfffff000;
		np[i] = x / 2147483648.0 * 180.0 + correction;
		}

	varp->DynamicLag = p->lag_50hz_frame;

}	/* END XLIPITCH */

/* -------------------------------------------------------------------- */
void xliroll(varp, p, np)
RAWTBL		*varp;
struct Irs_blk	*p;	/* Source	*/
NR_TYPE		*np;	/* Destination	*/
{
	int	i, x;

	/* (degs) 20 bits, +- 1.0 pirad, label 325	*/

	for (i = 0; i < varp->SampleRate; ++i)
		{
		x = p->roll_angle[i] & 0xfffff000;
		np[i] = x / 2147483648.0 * 180.0;
		}

	varp->DynamicLag = p->lag_50hz_frame;

}	/* END XLIROLL */

/* -------------------------------------------------------------------- */
void xlithdg(varp, p, np)
RAWTBL		*varp;
struct Irs_blk	*p;	/* Source	*/
NR_TYPE		*np;	/* Destination	*/
{
	int	i, x;
	float	correction;

	if (varp->name[4] != '\0')
		correction = thdgrcorr;
	else
		correction = thdglcorr;


	/* (degs) 20 bits, +- 1.0 pirad, label 314	*/

	for (i = 0; i < varp->SampleRate; ++i)
		{
		x = p->true_heading[i] & 0xfffff000;
		np[i] = x / 2147483648.0 * 180.0 + correction;

		if (np[i] < 0.0)
			np[i] += 360.0;
		}

	varp->DynamicLag = p->lag_25hz_frame;

}	/* END XLITHDG */

/* -------------------------------------------------------------------- */
void xlialt(varp, p, np)
RAWTBL		*varp;
struct Irs_blk	*p;	/* Source	*/
NR_TYPE		*np;	/* Destination	*/
{
	int	i, x;

	/* (M) 20 bits, +- 131072.0 Ft, label 361	*/

	for (i = 0; i < varp->SampleRate; ++i)
		{
		x = p->inertial_alt[i] & 0xfffff000;
		np[i] = x / 2147483648.0 * 131072.0 * FTMTR;
		}

	varp->DynamicLag = p->lag_25hz_frame;

}	/* END XLIALT */

/* -------------------------------------------------------------------- */
void xlivspd(varp, p, np)
RAWTBL		*varp;
struct Irs_blk	*p;	/* Source	*/
NR_TYPE		*np;	/* Destination	*/
{
	int	i, x;

	/* (M/s) 20 bits, +- 32768.0 Ft/min, label 365	*/

	for (i = 0; i < varp->SampleRate; ++i)
		{
		x = p->inrt_vert_speed[i] & 0xfffff000;
		np[i] = x / 2147483648.0 * 32768.0 * FTMIN;
		}

	varp->DynamicLag = p->lag_25hz_frame;

}	/* END XLIVSPD */

/* -------------------------------------------------------------------- */
void xlvacc(varp, p, np)
RAWTBL		*varp;
struct Irs_blk	*p;	/* Source	*/
NR_TYPE		*np;	/* Destination	*/
{
	int	i, x;

	/* (M/s**2) 15 bits, +- 4.0 G's, label 364	*/

	for (i = 0; i < varp->SampleRate; ++i)
		{
		x = p->vertical_accel[i] & 0xfffe0000;
		np[i] = x / 2147483648.0 * 4.0 * MPS2;
		}

	varp->DynamicLag = p->lag_50hz_frame;

}	/* END XLVACC */

/* -------------------------------------------------------------------- */
void xlivew(varp, p, np)
RAWTBL		*varp;
struct Irs_blk	*p;	/* Source	*/
NR_TYPE		*np;	/* Destination	*/
{
	int	i, x;

	/* (M/s) 20 bits, +-4096 kts, label 367	*/

	for (i = 0; i < varp->SampleRate; ++i)
		{
		x = p->velocity_ew[i] & 0xfffff000;
		np[i] = x / 2147483648.0 * 4096.0 * KTS2MS;
		}

	varp->DynamicLag = p->lag_10hz_frame;

}	/* END XLIVEW */

/* -------------------------------------------------------------------- */
void xlivns(varp, p, np)
RAWTBL		*varp;
struct Irs_blk	*p;	/* Source	*/
NR_TYPE		*np;	/* Destination	*/
{
	int	i, x;

	/* (M/s) 20 bits, +-4096 kts, label 366	*/

	for (i = 0; i < varp->SampleRate; ++i)
		{
		x = p->velocity_ns[i] & 0xfffff000;
		np[i] = x / 2147483648.0 * 4096.0 * KTS2MS;
		}

	varp->DynamicLag = p->lag_10hz_frame;

}	/* END XLIVNS */

/* -------------------------------------------------------------------- */
void xligspd(varp, p, np)
RAWTBL		*varp;
struct Irs_blk	*p;	/* Source	*/
NR_TYPE		*np;	/* Destination	*/
{
	int	i, x;

	/* (M/s) 20 bits, 0 to 4095 kts, label 312	*/

	for (i = 0; i < varp->SampleRate; ++i)
		{
		x = p->ground_speed[i] & 0xfffff000;
		np[i] = x / 2147483648.0 * 4096.0 * KTS2MS;
		}

	varp->DynamicLag = p->lag_10hz_frame;

}	/* END XLIGSPD */

/* -------------------------------------------------------------------- */
void xlbrollr(varp, p, np)
RAWTBL		*varp;
struct Irs_blk	*p;	/* Source	*/
NR_TYPE		*np;	/* Destination	*/
{
	int	i, x;

	for (i = 0; i < varp->SampleRate; ++i)
		{
		x = p->roll_rate[i] & 0xfffe0000;
		np[i] = x / 2147483648.0 * 128.0;
		}

	varp->DynamicLag = p->lag_50hz_frame;

}	/* END XLBROLLR */

/* -------------------------------------------------------------------- */
void xlbpitchr(varp, p, np)
RAWTBL		*varp;
struct Irs_blk	*p;	/* Source	*/
NR_TYPE		*np;	/* Destination	*/
{
	int	i, x;

	for (i = 0; i < varp->SampleRate; ++i)
		{
		x = p->pitch_rate[i] & 0xfffe0000;
		np[i] = x / 2147483648.0 * 128.0;
		}

	varp->DynamicLag = p->lag_50hz_frame;

}	/* END XLBPITCHR */

/* -------------------------------------------------------------------- */
void xlbyawr(varp, p, np)
RAWTBL		*varp;
struct Irs_blk	*p;	/* Source	*/
NR_TYPE		*np;	/* Destination	*/
{
	int	i, x;

	for (i = 0; i < varp->SampleRate; ++i)
		{
		x = p->yaw_rate[i] & 0xfffe0000;
		np[i] = x / 2147483648.0 * 128.0;
		}

	varp->DynamicLag = p->lag_50hz_frame;

}	/* END XLBYAWR */

/* -------------------------------------------------------------------- */
void xlblata(varp, p, np)
RAWTBL		*varp;
struct Irs_blk	*p;	/* Source	*/
NR_TYPE		*np;	/* Destination	*/
{
	int	i, x;

	for (i = 0; i < varp->SampleRate; ++i)
		{
		x = p->lat_accel[i] & 0xfffe0000;
		np[i] = x / 2147483648.0 * 4.0 * MPS2;
		}

	varp->DynamicLag = p->lag_50hz_frame;

}	/* END XLBLATA */

/* -------------------------------------------------------------------- */
void xlblona(varp, p, np)
RAWTBL		*varp;
struct Irs_blk	*p;	/* Source	*/
NR_TYPE		*np;	/* Destination	*/
{
	int	i, x;

	for (i = 0; i < varp->SampleRate; ++i)
		{
		x = p->long_accel[i] & 0xfffe0000;
		np[i] = x / 2147483648.0 * 4.0 * MPS2;
		}

	varp->DynamicLag = p->lag_50hz_frame;

}	/* END XLBLONA */

/* -------------------------------------------------------------------- */
void xlbnorma(varp, p, np)
RAWTBL		*varp;
struct Irs_blk	*p;	/* Source	*/
NR_TYPE		*np;	/* Destination	*/
{
	int	i, x;

	for (i = 0; i < varp->SampleRate; ++i)
		{
		x = p->normal_accel[i] & 0xfffe0000;
		np[i] = x / 2147483648.0 * 4.0 * MPS2;
		}

	varp->DynamicLag = p->lag_50hz_frame;

}	/* END XLBNORMA */

/* -------------------------------------------------------------------- */
void xllag50(varp, p, output)
RAWTBL		*varp;
struct Irs_blk	*p;
NR_TYPE		*output;
{
	*output = (NR_TYPE)p->lag_50hz_frame;

}	/* END XLLAG50 */

/* -------------------------------------------------------------------- */
void xllag25(varp, p, output)
RAWTBL		*varp;
struct Irs_blk	*p;
NR_TYPE		*output;
{
	*output = (NR_TYPE)p->lag_25hz_frame;

}	/* END XLLAG25 */

/* -------------------------------------------------------------------- */
void xllag10(varp, p, output)
RAWTBL		*varp;
struct Irs_blk	*p;
NR_TYPE		*output;
{
	*output = (NR_TYPE)p->lag_10hz_frame;

}	/* END XLLAG10 */

/* -------------------------------------------------------------------- */
void xllag5(varp, p, output)
RAWTBL		*varp;
struct Irs_blk	*p;
NR_TYPE		*output;
{
	*output = (NR_TYPE)p->lag_5hz_frame;

}	/* END XLLAG5 */

/* END IRS.C */
