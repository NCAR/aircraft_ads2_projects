/*
-------------------------------------------------------------------------
OBJECT NAME:	cb_time.c

FULL NAME:		Callbacks for TimeSlice Window

ENTRY POINTS:	EditTimeSlices()
				ValidateTime()
				DismissTimeSliceWindow()

STATIC FNS:		

DESCRIPTION:	

INPUT:		

OUTPUT:		

REFERENCES:	

REFERENCED BY:	

COPYRIGHT:	University Corporation for Atmospheric Research, 1993
-------------------------------------------------------------------------
*/

#include <Xm/TextF.h>

#include "nimbus.h"
#include "gui.h"


/* -------------------------------------------------------------------- */
/* ARGSUSED */
void
EditTimeSlices(Widget w, XtPointer client, XtPointer call)
{
	XtManageChild(TimeSliceWindow);
	XtPopup(XtParent(TimeSliceWindow), XtGrabNone);

}	/* END EDITTIMESLICES */

/* -------------------------------------------------------------------- */
/* ARGSUSED */
void
ValidateTime(Widget w, XtPointer client, XtPointer call)
{
	int		hour, min, sec;
	char	*p = XmTextFieldGetString(w);

	if (strlen(p) == 0)
		return;

	hour = min = sec = 0;

	if (strchr(p, ':') != NULL)
		sscanf(p, "%u:%u:%u", &hour, &min, &sec);
	else
		sscanf(p, "%02u%02u%02u", &hour, &min, &sec);

	if (hour < 0)	hour = 0;
	if (hour > 48)	hour = 48;
	if (min < 0)	min = 0;
	if (min > 59)	min = 59;
	if (sec < 0)	sec = 0;
	if (sec > 59)	sec = 59;

	sprintf(buffer, "%02d:%02d:%02d", hour, min, sec);
	XmTextFieldSetString(w, buffer);
	XtFree(p);

}	/* END VALIDATETIME */

/* -------------------------------------------------------------------- */
/* ARGSUSED */
void
DismissTimeSliceWindow(Widget w, XtPointer client, XtPointer call)
{
	XtPopdown(XtParent(TimeSliceWindow));
	XtUnmanageChild(TimeSliceWindow);
}

/* END CB_TIME.C */
