/*
-------------------------------------------------------------------------
OBJECT NAME:	gust.h

FULL NAME:	Include file for ins.c & irs.c in std & xlate directories.
-------------------------------------------------------------------------
*/

#ifndef GUST_H
#define GUST_H

#include <math.h>

#define PI	M_PI		/* defined in math.h    */
#define DEG	(180.0 / PI)
#define RAD	(PI / 180.0)
#define RESOLV	(180.0/8192.0)	/* Resolver/synchro conversion equation */

#define EARTH_RADIUS	6.371229e06	/* Mean Earth radius (m)	*/
#define OMEGAE		7.292116e-5	/* Angular vel of the Earth (rad/sec) */

#define CKTMS		0.51479

#define THDG_TEST	(180.0 * RAD)
#define PITCH_TEST	(22.5 * RAD)

float	GetBoomLength();

#endif
