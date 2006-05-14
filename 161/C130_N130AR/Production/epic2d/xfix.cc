/*
-------------------------------------------------------------------------
OBJECT NAME:	xfix.cc

FULL NAME:	

DESCRIPTION:	See description in epic2d.cc

INPUT:		

OUTPUT:		

NOTES:		

COPYRIGHT:	University Corporation for Atmospheric Research, 2001
-------------------------------------------------------------------------
*/

#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>

#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/TextF.h>
#include <Xm/PushB.h>

#include "adsIO.h"

#define OFFSET	5

char	buffer[65000];
char	sourceFile[128], destFile[128];
int	outFile, nRecsC, nRecsP, currentStartC = OFFSET, currentStartP = OFFSET;

Widget AppShell, text[15];
XtAppContext context;


struct _twodTime
  {
  short	hour;
  short	minute;
  short	second;
  short	msec;
  } Cstamp[50000], Pstamp[50000];

void	CloseDiskFiles(), CreateWindow();
int	WriteDisk(char buff[], int nBytes);
void NextFuckUp(Widget w, XtPointer client, XtPointer call);
void WriteData(Widget w, XtPointer client, XtPointer call);


/* -------------------------------------------------------------------- */
main(int argc, char *argv[])
{
  int		i, rc, nBytes;
  ADS_DataFile	*source;
  P2d_rec	twoD;

  outFile = ERR;


  printf("Enter source file : ");
  gets(sourceFile);

  if (strlen(sourceFile) == 0)
    {
    fprintf(stderr, "Ppppphhhhhhtttttt!\n");
    return(1);
    }

  strcpy(destFile, sourceFile);
  strcat(destFile, ".2d");

  if (strlen(sourceFile))
    strcat(sourceFile, ".2dnew");

  source = new ADS_DataFile(sourceFile);


printf("%s --> %s\n", sourceFile, destFile);

  if ((outFile = open(destFile, O_CREAT | O_WRONLY | O_TRUNC, 0666)) == ERR)
    {
    fprintf(stderr, "Can not open destination file %s\n", destFile);
    return(1);
    }



  /* Read file and acquire all time stamps.
   */
  nRecsC = nRecsP = 0;
  source->FirstPMS2dRecord(&twoD);

  do
    {
    if (((char *)&twoD.id)[0] == 'C')
      {
      Cstamp[nRecsC].hour = ntohs(twoD.hour);
      Cstamp[nRecsC].minute = ntohs(twoD.minute);
      Cstamp[nRecsC].second = ntohs(twoD.second);
      Cstamp[nRecsC].msec = ntohs(twoD.msec);
      ++nRecsC;
      }
    else
      {
      Pstamp[nRecsP].hour = ntohs(twoD.hour);
      Pstamp[nRecsP].minute = ntohs(twoD.minute);
      Pstamp[nRecsP].second = ntohs(twoD.second);
      Pstamp[nRecsP].msec = ntohs(twoD.msec);
      ++nRecsP;
      }
    }
  while (source->NextPMS2dRecord(&twoD) > 0);

  delete source;

  printf("%d C, %d P 2d records found.\n", nRecsC, nRecsP);


NextFuckUp(NULL, NULL, NULL);
WriteData(NULL, NULL, NULL);
return(0);

  AppShell = XtAppInitialize(&context, "XmShit", NULL, 0, &argc, argv,
              NULL, NULL, 0);

  CreateWindow();
  XtAppMainLoop(context);
exit(0);

}

/* -------------------------------------------------------------------- */
void WriteData(Widget w, XtPointer c0, XtPointer c1)
{
  ADS_DataFile	*source;
  int		nBytes, i;

  /* Write data back out.
   */
  source = new ADS_DataFile(sourceFile);

  for (i = 0; i < 3; ++i)
    {
    nBytes = source->NextPhysicalRecord(buffer);
    write(outFile, buffer, nBytes);
    }


  while ((nBytes = source->NextPhysicalRecord(buffer)) > 0)
    {
    if (WriteDisk(buffer, nBytes) == ERR)
      break;
    }

  if (nBytes == ERR)
    {
    fprintf(stderr, "READ ERROR, errno = %d.\n", errno);
    }

  CloseDiskFiles();

}	/* END MAIN */

/* -------------------------------------------------------------------- */
void CloseDiskFiles()
{
  if (outFile != ERR)
    close(outFile);

  outFile = ERR;

}	/* END CLOSEDISKFILES */

/* -------------------------------------------------------------------- */
int WriteDisk(char buff[], int nBytes)
{
  int		i, rc;
  P2d_rec	*p = (P2d_rec *)buff;

  static int	outCntrC = 0, outCntrP = 0;

  for (i = 0; i < 7; ++i)
    {
    if (((char *)&p[i].id)[0] == 'C')
      {
      p[i].hour = htons(Cstamp[outCntrC].hour);
      p[i].minute = htons(Cstamp[outCntrC].minute);
      p[i].second = htons(Cstamp[outCntrC].second);
      p[i].msec = htons(Cstamp[outCntrC].msec);
      ++outCntrC;
      }
    else
      {
      p[i].hour = htons(Pstamp[outCntrP].hour);
      p[i].minute = htons(Pstamp[outCntrP].minute);
      p[i].second = htons(Pstamp[outCntrP].second);
      p[i].msec = htons(Pstamp[outCntrP].msec);
      ++outCntrP;
      }
    }

  rc = write(outFile, buff, nBytes);

  if (rc == ERR)
    {
    if (errno == ENOSPC)
      {
      fprintf(stderr, ">>> DISK FULL, closing disk file(s) and continuing.\n");
      CloseDiskFiles();
      }

    fprintf(stderr, "Write failure to dest disk, errno = %d.\n", errno);
    return(ERR);
    }

  return(OK);

}	/* END WRITEDISK */

/* -------------------------------------------------------------------- */
void ApplyTimeStamps(Widget w, XtPointer client, XtPointer call)
{
  int		i;
  unsigned      hour, min, sec, msec;
  char          *p;

  for (i = 0; i < 15; ++i)
    {
    hour = min = sec = 0;

    p = XmTextFieldGetString(text[i]);

    if (strchr(p, ':') != NULL)
      sscanf(p, "%u:%u:%u.%u", &hour, &min, &sec, &msec);
    else
      sscanf(p, "%02u%02u%02u.%u", &hour, &min, &sec, &msec);

    if (currentStartC >= nRecsC)
      {
printf("Apply to P, currCnt = %d\n", currentStartP);
      Pstamp[currentStartP-OFFSET+i].hour = hour;
      Pstamp[currentStartP-OFFSET+i].minute = min;
      Pstamp[currentStartP-OFFSET+i].second = sec;
      Pstamp[currentStartP-OFFSET+i].msec = msec;
      }
    else
      {
printf("Apply to C, currCnt = %d\n", currentStartC);
      Cstamp[currentStartC-OFFSET+i].hour = hour;
      Cstamp[currentStartC-OFFSET+i].minute = min;
      Cstamp[currentStartC-OFFSET+i].second = sec;
      Cstamp[currentStartC-OFFSET+i].msec = msec;
      }

    XtFree(p);
    }

  if (currentStartC >= nRecsC)
    currentStartP += OFFSET;
  else
    currentStartC += OFFSET;

}

/* -------------------------------------------------------------------- */
void NextFuckUp(Widget w, XtPointer client, XtPointer call)
{
  int		i, cnt, thisTime, markSec;
  static int    prevSecC = -1, prevMSecC = -1;
  static int    prevSecP = -1, prevMSecP = -1;

  if (currentStartP >= nRecsP)
    {
//    WriteData();
    exit(0);
    }

  for (i = currentStartC; i < nRecsC; ++i)
    {
    thisTime = Cstamp[i].hour*3600+Cstamp[i].minute*60+Cstamp[i].second;

    if (prevSecC != -1 && prevSecC > thisTime)
      {
      markSec = Cstamp[i-1].second;

      for (cnt = 1; markSec == Cstamp[i-cnt].second; ++cnt)
        {
        if (Cstamp[i-cnt].second == 0)
          {
          Cstamp[i-cnt].second = 59;
          if (Cstamp[i-cnt].minute == 59)
            {
            Cstamp[i-cnt].minute = 59;
            Cstamp[i-cnt].hour--;
            }
          else
            Cstamp[i-cnt].minute--;
          }
        else
          Cstamp[i-cnt].second--;
        }
      
/*
      currentStartC = i;

      for (j = 0; j < 15; ++j)
        {
        sprintf(buffer, "%02d:%02d:%02d.%03d",
		Cstamp[currentStartC-OFFSET+j].hour,
		Cstamp[currentStartC-OFFSET+j].minute,
		Cstamp[currentStartC-OFFSET+j].second,
		Cstamp[currentStartC-OFFSET+j].msec);

        XmTextFieldSetString(text[j], buffer);
        }

      return;
*/
      }

    prevSecC = thisTime;
    }

  for (i = currentStartP; i < nRecsP; ++i)
    {
    thisTime = Pstamp[i].hour*3600+Pstamp[i].minute*60+Pstamp[i].second;

    if (prevSecP != -1 && prevSecP > thisTime)
      {
      markSec = Pstamp[i-1].second;

      for (cnt = 1; markSec == Pstamp[i-cnt].second; ++cnt)
        {
        if (Pstamp[i-cnt].second == 0)
          {
          Pstamp[i-cnt].second = 59;
          if (Pstamp[i-cnt].minute == 59)
            {
            Pstamp[i-cnt].minute = 59;
            Pstamp[i-cnt].hour--;
            }
          else
            Pstamp[i-cnt].minute--;
          }
        else
          Pstamp[i-cnt].second--;
        }
      }
    }

}

/* -------------------------------------------------------------------- */
void ValidateTime(Widget w, XtPointer client, XtPointer call)
{
  unsigned      hour, min, sec, msec;
  char          *p = XmTextFieldGetString(w);

  if (strlen(p) == 0)
    return;

  hour = min = sec = 0;

  if (strchr(p, ':') != NULL)
    sscanf(p, "%u:%u:%u.%u", &hour, &min, &sec, &msec);
  else
    sscanf(p, "%02u%02u%02u.%u", &hour, &min, &sec, &msec);

  if (hour > 48) hour = 48;
  if (min > 59) min = 59;
  if (sec > 59) sec = 59;

  sprintf(buffer, "%02d:%02d:%02d.%03d", hour, min, sec, msec);
  XmTextFieldSetString(w, buffer);
  XtFree(p);

}       /* END VALIDATETIME */

/* -------------------------------------------------------------------- */
void CreateWindow()
{
  Widget	parent, edit, rc, cbRC, b[4];
  Arg		args[15];
  Cardinal	n;
  int		i;

  n = 0;
  parent = XtCreatePopupShell("topLevelShell",
              topLevelShellWidgetClass, AppShell, args, n);

  n = 0;
  edit = XmCreateRowColumn(parent, "edit", args, n);

  n = 0;
  rc = XmCreateRowColumn(edit, "buttonRC", args, n);
  XtManageChild(rc);


  for (i = 0; i < 15; ++i)
    {
    sprintf(buffer, "text%d", i);
    text[i] = XmCreateTextField(rc, buffer, args, n);

    XtAddCallback(text[i], XmNlosingFocusCallback, ValidateTime, NULL);
    }

  XtManageChildren(text, 15);

  /* Command buttons.
   */
  n = 0;
  cbRC = XmCreateRowColumn(edit, "buttonRC", args, n);
  XtManageChild(cbRC);

  n = 0;
  b[0] = XmCreatePushButton(cbRC, "Apply Times", args, n);
  b[1] = XmCreatePushButton(cbRC, "Reset Times", args, n);
  b[2] = XmCreatePushButton(cbRC, "Next Fuck Up", args, n);
  XtAddCallback(b[0], XmNactivateCallback, ApplyTimeStamps, NULL);
  XtAddCallback(b[1], XmNactivateCallback, WriteData,NULL);
  XtAddCallback(b[2], XmNactivateCallback, NextFuckUp, NULL);
  XtManageChildren(b, 3);

  XtManageChild(edit);
  XtPopup(XtParent(edit), XtGrabNone);

}

/* END XFIX.CC */
