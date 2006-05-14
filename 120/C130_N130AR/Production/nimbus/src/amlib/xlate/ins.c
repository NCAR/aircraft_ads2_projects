/*
-------------------------------------------------------------------------
OBJECT NAME:	ins.c

FULL NAME:	Convert Litton 51 Intertial block

ENTRY POINTS:	xlinssec(), xlalon(), xlalat(), xlgsi(), xltrki(), xlthi(),
		xlalpha(), xlxvi(), xlyvi(), xlpitch51(), xlcroll51(),
		xlroll51(), xlphdg51(), xlvzi51()

STATIC FNS:	angle()
		velocity()

DESCRIPTION:	

INPUT:		

OUTPUT:		

REFERENCES:	angcr.c

REFERENCED BY:	rec_decode.c

COPYRIGHT:	University Corporation for Atmospheric Research, 1992
-------------------------------------------------------------------------
*/

#include "nimbus.h"
#include "amlib.h"
#include "ctape.h"
#include "gust.h"

#define	INS_MASK	0x0003ffff
#define MASK		0x3ffff000
#define MSIGN		0x40000000


static double	angle(), velocity();
NR_TYPE	angcr();


/* -------------------------------------------------------------------- */
void xlinssec(varp, input, output)
RAWTBL		*varp;
struct Ins_blk	*input;
NR_TYPE		*output;
{
	*output = (NR_TYPE)input->sec + (NR_TYPE)input->t250 / 250.0;
}

/* -------------------------------------------------------------------- */
void xlalat(varp, input, output)
RAWTBL		*varp;
struct Ins_blk	*input;
NR_TYPE		*output;
{
	NR_TYPE	alat;

	if ((alat = (NR_TYPE)angle(input->lat)) > 180.0)
		alat -= 360.0;

	*output = (NR_TYPE)alat;

}	/* END XLALAT */

/* -------------------------------------------------------------------- */
void xlalon(varp, input, output)
RAWTBL		*varp;
struct Ins_blk	*input;
NR_TYPE		*output;
{
	NR_TYPE	alon;

	if ((alon = (NR_TYPE)angle(input->lon)) > 180.0)
		alon -= 360.0;

	*output = (NR_TYPE)alon;

}	/* END XLALON */

/* -------------------------------------------------------------------- */
void xlgsi(varp, input, output)
RAWTBL		*varp;
struct Ins_blk	*input;
NR_TYPE		*output;
{
	*output = (NR_TYPE)((input->gndspd >> 12) & INS_MASK) * CKTMS;
	
}	/* END XLGSI */

/* -------------------------------------------------------------------- */
void xltrki(varp, input, output)
RAWTBL		*varp;
struct Ins_blk	*input;
NR_TYPE		*output;
{
	*output = (NR_TYPE)fmod(angle(input->track)+360.0, (double)360.0);

}	/* END XLTRKI */

/* -------------------------------------------------------------------- */
void xlthi(varp, input, output)
RAWTBL		*varp;
struct Ins_blk	*input;
NR_TYPE		*output;
{
	output[0] = (NR_TYPE)fmod(angle(input->truehd1)+360.0, (double)360.0);
	output[1] = (NR_TYPE)fmod(angle(input->truehd2)+360.0, (double)360.0);
	output[2] = (NR_TYPE)fmod(angle(input->truehd3)+360.0, (double)360.0);
	output[3] = (NR_TYPE)fmod(angle(input->truehd4)+360.0, (double)360.0);
	output[4] = (NR_TYPE)fmod(angle(input->truehd5)+360.0, (double)360.0);

}	/* END XLTHI */

/* -------------------------------------------------------------------- */
void xlalpha(varp, input, output)
RAWTBL		*varp;
struct Ins_blk	*input;
NR_TYPE		*output;
{
	output[0] = (NR_TYPE)fmod(angle(input->alpha1)+360.0, (double)360.0);
	output[1] = (NR_TYPE)fmod(angle(input->alpha2)+360.0, (double)360.0);
	output[2] = (NR_TYPE)fmod(angle(input->alpha3)+360.0, (double)360.0);
	output[3] = (NR_TYPE)fmod(angle(input->alpha4)+360.0, (double)360.0);
	output[4] = (NR_TYPE)fmod(angle(input->alpha5)+360.0, (double)360.0);

}	/* END XLALPHA */

/* -------------------------------------------------------------------- */
void xlxvi(varp, input, output)
RAWTBL		*varp;
struct Ins_blk	*input;
NR_TYPE		*output;
{
	output[0] = (NR_TYPE)velocity(input->vx1) * 0.3048;
	output[1] = (NR_TYPE)velocity(input->vx2) * 0.3048;
	output[2] = (NR_TYPE)velocity(input->vx3) * 0.3048;
	output[3] = (NR_TYPE)velocity(input->vx4) * 0.3048;
	output[4] = (NR_TYPE)velocity(input->vx5) * 0.3048;
	output[5] = (NR_TYPE)velocity(input->vx6) * 0.3048;
	output[6] = (NR_TYPE)velocity(input->vx7) * 0.3048;
	output[7] = (NR_TYPE)velocity(input->vx8) * 0.3048;
	output[8] = (NR_TYPE)velocity(input->vx9) * 0.3048;
	output[9] = (NR_TYPE)velocity(input->vx10) * 0.3048;

}	/* END XLXVI */

/* -------------------------------------------------------------------- */
void xlyvi(varp, input, output)
RAWTBL		*varp;
struct Ins_blk	*input;
NR_TYPE		*output;
{
	output[0] = (NR_TYPE)velocity(input->vy1) * 0.3048;
	output[1] = (NR_TYPE)velocity(input->vy2) * 0.3048;
	output[2] = (NR_TYPE)velocity(input->vy3) * 0.3048;
	output[3] = (NR_TYPE)velocity(input->vy4) * 0.3048;
	output[4] = (NR_TYPE)velocity(input->vy5) * 0.3048;
	output[5] = (NR_TYPE)velocity(input->vy6) * 0.3048;
	output[6] = (NR_TYPE)velocity(input->vy7) * 0.3048;
	output[7] = (NR_TYPE)velocity(input->vy8) * 0.3048;
	output[8] = (NR_TYPE)velocity(input->vy9) * 0.3048;
	output[9] = (NR_TYPE)velocity(input->vy10) * 0.3048;

}	/* END XLYVI */

/* -------------------------------------------------------------------- */
static double angle(value)
long	value;
{
	NR_TYPE	angle;
	long	issigned = value & MSIGN;

	value = (value & MASK) >> 12;

	if (issigned)
		value = ~value & INS_MASK;

	angle = (NR_TYPE)(value * 360) / 262144.0;

	if (issigned)
		angle = -angle;

	return(angle);

}	/* END ANGLE */

/* -------------------------------------------------------------------- */
static double velocity(value)
long	value;
{
	NR_TYPE	velocity;
	long	issigned = value & MSIGN;

	value = (value & MASK) >> 12;

	if (issigned)
		value = ~value & INS_MASK;

	velocity = (NR_TYPE)value / 8.0;

	if (issigned)
		velocity = -velocity;

	return(velocity);

}	/* END VELOCITY */

/* -------------------------------------------------------------------- */
void xlpitch51(varp, input, output)
RAWTBL	*varp;
short	*input;
NR_TYPE	*output;
{
	register int	i;
	NR_TYPE		pitch;

	for (i = 0; i < varp->SampleRate; ++i)
		{
		pitch = ~input[i * varp->ADSoffset] * RESOLV * 0.125;
		pitch = angcr(pitch, (NR_TYPE)0.0, (float)45.0, (float)-180.0);

		output[i] = (NR_TYPE)pitch;
		}

}	/* END XLPITCH51 */

/* -------------------------------------------------------------------- */
static NR_TYPE	croll[50];

void xlcroll51(varp, input, output)	/* Course Roll	*/
RAWTBL	*varp;
short	*input;
NR_TYPE	*output;
{
	register int	i;

	for (i = 0; i < varp->SampleRate; ++i)
		output[i] = 
		croll[i] = (NR_TYPE)~input[i * varp->ADSoffset] * RESOLV;

}	/* END XLCROLL51 */

/* -------------------------------------------------------------------- */
void xlroll51(varp, input, output)
RAWTBL	*varp;
short	*input;
NR_TYPE	*output;
{
	register int	i;
	NR_TYPE		roll;

	for (i = 0; i < varp->SampleRate; ++i)
		{
		roll = ~input[i * varp->ADSoffset] * RESOLV * 0.125;
		roll = angcr(roll, croll[i], (float)45.0, (float)-180.0);

		output[i] = (NR_TYPE)roll;
		}

}	/* END XLROLL51 */

/* -------------------------------------------------------------------- */
void xlphdg51(varp, input, output)
RAWTBL	*varp;
short	*input;
NR_TYPE	*output;
{
	register int	i;

	for (i = 0; i < varp->SampleRate; ++i)
		output[i] = (NR_TYPE)~input[i*varp->ADSoffset] * RESOLV * 0.125;

}	/* END XLPHDG51 */

/* -------------------------------------------------------------------- */
void xlvzi51(varp, input, output)
RAWTBL	*varp;
short	*input;
NR_TYPE	*output;
{
	register int	i;
	NR_TYPE		vzi, vzidif;
	static bool	firstTime = TRUE;
	static int	ncvzi;
	static NR_TYPE	vzi0;

	if (firstTime)
		{
		vzi0 = input[0] * 0.012192;
		ncvzi = 0;
		firstTime = FALSE;
		}

	for (i = 0; i < varp->SampleRate; ++i)
		{
		vzi	= input[i] * 0.012192;
		vzidif	= vzi - vzi0;
		vzi0	= vzi;

		if (fabs((double)vzidif) > 600.0)
			if (vzidif < 0.0)
				ncvzi += 800.0;
			else
				ncvzi -= 800.0;

		vzi += ncvzi;

		output[i] = (NR_TYPE)vzi;
		}

}	/* END XLVZI51 */

/* END INS.C */
