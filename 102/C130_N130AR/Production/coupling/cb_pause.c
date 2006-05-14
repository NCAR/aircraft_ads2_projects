/*
-------------------------------------------------------------------------
OBJECT NAME:	cb_pause.c

FULL NAME:		Command CallBacks

ENTRY POINTS:	PauseProcessing()

STATIC FNS:		none

DESCRIPTION:	Contains callbacks for the nimbus GUI pause related stuff.

INPUT:			

OUTPUT:		

REFERENCES:		none

REFERENCED BY:	XtAppMainLoop()

COPYRIGHT:		University Corporation for Atmospheric Research, 1993
-------------------------------------------------------------------------
*/

#include <Xm/Form.h>
#include <Xm/PushBG.h>
#include <Xm/Text.h>

#include "define.h"

static Widget	PauseShell;

/* -------------------------------------------------------------------- */
/* ARGSUSED */
void
PauseProcessing(w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	PauseFlag = TRUE;
	XtSetSensitive(goButton, FALSE);

	XtManageChild(PauseShell);
	XtAddGrab(PauseShell, TRUE, FALSE);

}	/* END PAUSEPROCESSING */

/* -------------------------------------------------------------------- */
/* ARGSUSED */
void
PauseStop(w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	PauseFlag = FALSE;
	PauseWhatToDo = P_QUIT;

	XtRemoveGrab(PauseShell);
	XtUnmanageChild(PauseShell);

}	/* END PAUSEPROCESSING */

/* -------------------------------------------------------------------- */
/* ARGSUSED */
void
PauseContinue(w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
	PauseFlag = FALSE;
	PauseWhatToDo = P_CONTINUE;
	XtSetSensitive(goButton, TRUE);

	XtRemoveGrab(PauseShell);
	XtUnmanageChild(PauseShell);

}	/* END PAUSECONTINUE */

/* -------------------------------------------------------------------- */
/* ARGSUSED */
void
PauseXTV(w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{
/*
	XtPopUp(PauseXTVBox);
*/
}	/* END PAUSEXTV */

/* -------------------------------------------------------------------- */
CreatePauseWindows(topLevel)
Widget	topLevel;
{
	Widget	b[3];
	Arg		args[5];
	int		n;

	n = 0;
	XtSetArg(args[n], XmNtitle, "Pause Choices"); n++;
	PauseShell = XmCreateFormDialog(topLevel, "pauseForm", args, n);

	n = 0;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	b[0] = XmCreatePushButtonGadget(PauseShell, "pauseStop", args, n);
	XtAddCallback(b[0], XmNactivateCallback, PauseStop, NULL);

	n = 0;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNleftWidget, b[0]); n++;
	b[1] = XmCreatePushButtonGadget(PauseShell, "pauseCont", args, n);
	XtAddCallback(b[1], XmNactivateCallback, PauseContinue, NULL);

	n = 0;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNleftWidget, b[1]); n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	b[2] = XmCreatePushButtonGadget(PauseShell, "pauseXTV", args, n);
	XtAddCallback(b[2], XmNactivateCallback, PauseXTV, NULL);

	XtManageChildren(b, 2);

}	/* END CREATEPAUSEWINDOWS */

/* END CB_PAUSE.C */
