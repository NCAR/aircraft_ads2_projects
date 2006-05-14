/*
-------------------------------------------------------------------------
OBJECT NAME:	epic2d.cc

FULL NAME:	

DESCRIPTION:	Fix EPIC bad time stamps.  Goto line 131 to choose which
		flight to fix.  Run this program first the xfix.c to clean
		up.

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

#include "adsIO.h"


char	buffer[65000];
char	sourceFile[128], destFile[128];
int	outFile;

void	CloseDiskFiles();
int	WriteDisk(char buff[], int nBytes);


/* -------------------------------------------------------------------- */
main(int argc, char *argv[])
{
  int		i, rc, nBytes;
  ADS_DataFile	*source;

  outFile = ERR;


//  printf("Enter source file : ");
//  gets(sourceFile);

strcpy(sourceFile, argv[1]);
  if (strlen(sourceFile) == 0)
    {
    fprintf(stderr, "Ppppphhhhhhtttttt!\n");
    return(1);
    }


  if (strlen(sourceFile))
    {
    strcpy(destFile, sourceFile);
    strcat(destFile, ".2dnew");

    strcat(sourceFile, ".2d.orig");
    }

  source = new ADS_DataFile(sourceFile);

printf("%s --> %s\n", sourceFile, destFile);

  if ((outFile = open(destFile, O_CREAT | O_WRONLY | O_TRUNC, 0666)) == ERR)
    {
    fprintf(stderr, "Can not open destination file %s\n", destFile);
    return(1);
    }



  rc = 0;	/* Return Code	*/

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
    rc = 1;
    }

  CloseDiskFiles();

  return(rc);

}	/* END MAIN */

/* -------------------------------------------------------------------- */
void CloseDiskFiles()
{
  if (outFile != ERR)
    {
    close(outFile);
    }

  outFile = ERR;

}	/* END CLOSEDISKFILES */

/* -------------------------------------------------------------------- */
int WriteDisk(char buff[], int nBytes)
{
  int		i, rc, thisTime, cutOff;
  P2d_rec	*p = (P2d_rec *)buff;

  static int	prevSecC = -1, prevMSecC = -1;
  static int	prevSecP = -1, prevMSecP = -1;
  static int x = 0;


  for (i = 0; i < 7; ++i)
    {
    thisTime = ntohs(p[i].hour) * 3600 + ntohs(p[i].minute) * 60;
//    cutOff = (int)(-145 + (thisTime - 53640) * 0.0124890); // RF05
//    cutOff = (int)(400 + (thisTime - 48060) * 0.0133378); // RF06
//    cutOff = (int)(588 + (thisTime - 54900) * 0.0104909); // RF07
//    cutOff = (int)(84 + (thisTime - 52920) * 0.0124531); // RF09
//    cutOff = (int)(-72 + (thisTime - 49080) * 0.0133998); // RF10
//    cutOff = (int)(341 + (thisTime - 50940) * 0.0123863); // RF12
//    cutOff = (int)(536 + (thisTime - 53820) * 0.0108014); // RF13
//    cutOff = (int)(502 + (thisTime - 51660) * 0.0126294); // RF14
//    cutOff = (int)(127 + (thisTime - 54720) * 0.0149176); // RF17
//    cutOff = (int)(209 + (thisTime - 51360) * 0.0133625); // RF18
    cutOff = (int)(540 + (thisTime - 52200) * 0.0125416); // RF19

    if (cutOff < 0)
      cutOff+= 1000;

    if (ntohs(p[i].msec) > cutOff)
      {
      if (ntohs(p[i].second) == 0)
        {
        p[i].second = htons(59);
        if (ntohs(p[i].minute) == 0)
          {
          p[i].minute = htons(59);
          p[i].hour = htons(ntohs(p[i].hour) - 1);
          }
        else
          p[i].minute = htons(ntohs(p[i].minute) - 1);
        }
      else
        p[i].second = htons(ntohs(p[i].second) - 1);
      }

    if (ntohs(p[i].overld) > 900)
      p[i].overld = htons(0);
    }

  rc = write(outFile, buff, nBytes);


  if (x++ % 500 == 0)
    printf("%02d:%02d:%02d - cutOff = %d\n", ntohs(p[0].hour),
		ntohs(p[0].minute), ntohs(p[0].second), cutOff);

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

/* END EPIC2D.CC */
