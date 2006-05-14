/*
-------------------------------------------------------------------------
OBJECT NAME:    psql.c (PostgreS)

FULL NAME:      PostgreSQL database routines.

ENTRY POINTS:   InitSQL()
		WriteSQL()
		Write2dSQL()

STATIC FNS:     none

DESCRIPTION:

REFERENCES:     none

REFERENCED BY:  rtplot.c

NOTES:		

COPYRIGHT:      University Corporation for Atmospheric Research, 2003
-------------------------------------------------------------------------
*/

#include "nimbus.h"
#include "decode.h"
#include "gui.h"
#include "vardb.h"
#include "UdpSocket.h"
#include <ctype.h>
#include <libpq-fe.h>

#include <errno.h>
#include <sys/time.h>
#include <unistd.h>

static char	sql_str[30000];		/* SQL string		*/
static char	sql2d_str[50000];	/* 2D SQL string	*/
static char	brd_str[30000];		/* Broadcast string	*/

static PGconn	*conn;

static UdpSocket	brdcst(2101, "128.117.84.255");

extern NR_TYPE	*SampledData, *AveragedData;

void GetPMS1DAttrsForSQL(RAWTBL *rp, char sql_buff[]);
void SQLcommand(char command[]);

/* -------------------------------------------------------------------- */
bool InitSQL()
{
  int		i, nVars = 0;
  PGresult	*res;
  char		temp[5000], name[32];
printf("InitSQL\n");
  conn = PQconnectdb("");

  /* check to see that the backend connection was successfully made
   */
  if (PQstatus(conn) == CONNECTION_BAD)
  {
    fprintf(stderr, "Connection to database failed, check environment variables.\n");
    fprintf(stderr, "%s", PQerrorMessage(conn));
    PQfinish(conn);
    exit(1);
  }

  PQsetnonblocking(conn, true);

  PQclear(PQexec(conn, "DROP TABLE VariableList"));
  PQclear(PQexec(conn, "DROP TABLE PMS1Dlist"));
  PQclear(PQexec(conn, "DROP TABLE PMS2Dlist"));
  PQclear(PQexec(conn, "DROP TABLE RAF_1hz"));
  PQclear(PQexec(conn, "DROP TABLE current"));

  PQclear(PQexec(conn, "CREATE TABLE VariableList (Name char(20) PRIMARY KEY, Units char(16), Title char(80), SampleRateTable char(16))"));
  PQclear(PQexec(conn, "CREATE TABLE PMS1Dlist (Name char(20), SerialNumber char(16), SampleRateTable char(16), FirstBin INT, LastBin INT, CellSizes FLOAT[])"));
  PQclear(PQexec(conn, "CREATE TABLE PMS2Dlist (Name char(20), SerialNumber char(16))"));

  strcpy(sql_str, "CREATE TABLE RAF_1hz (datetime time PRIMARY KEY, ");

  for (i = 0; i < nsdi; ++i)
    {
    strcat(sql_str, sdi[i]->name);
    strcat(sql_str, " FLOAT, ");
    ++nVars;

    sprintf(temp, "INSERT INTO VariableList VALUES ('%s', '%s', '%s', 'RAF_%dhz')",
	sdi[i]->name, VarDB_GetUnits(sdi[i]->name), VarDB_GetTitle(sdi[i]->name),
	sdi[i]->SampleRate);
    PQclear(PQexec(conn, temp));
    fprintf(stderr, "%s", PQerrorMessage(conn));
    }

  for (i = 0; i < nraw; ++i)
    {
    if (isdigit(raw[i]->name[0]))	// Can't support vars starting with number.
      {
      name[0] = 'A';
      strcpy(&name[1], raw[i]->name);
      }
    else
      strcpy(name, raw[i]->name);

    if (strncmp(raw[i]->name, "A1D", 3) == 0)
      {
      char	name[16];

      strcpy(name, raw[i]->name);
      name[strlen(name)-1] = '\0';

      sprintf(temp, "INSERT INTO PMS2Dlist VALUES ('%s', '%s')",
	name, raw[i]->SerialNumber);
      PQclear(PQexec(conn, temp));
      fprintf(stderr, "%s", PQerrorMessage(conn));

      sprintf(temp, "DROP TABLE %s", name);
      PQclear(PQexec(conn, temp));
      sprintf(temp, "CREATE TABLE %s (datetime time, msec int, nSlices int, particle int[])", name);
      PQclear(PQexec(conn, temp));
      fprintf(stderr, "%s", PQerrorMessage(conn));

//      sprintf(temp, "TRUNCATE TABLE %s", name);
//      PQclear(PQexec(conn, temp));
//      fprintf(stderr, "%s", PQerrorMessage(conn));
      }

    if (raw[i]->Length > 1)
      {
      sprintf(temp, "INSERT INTO PMS1Dlist VALUES ('%s', '%s', 'RAF_%dhz'",
	&raw[i]->name[1], raw[i]->SerialNumber, raw[i]->SampleRate);

      GetPMS1DAttrsForSQL(raw[i], buffer);
      strcat(temp, buffer);
      strcat(temp, " )");
      PQclear(PQexec(conn, temp));
      fprintf(stderr, "%s", PQerrorMessage(conn));
//printf("%s - %d\n", raw[i]->name, raw[i]->Length);
      }

    strcat(sql_str, name);

    if (raw[i]->Length > 1)
      strcat(sql_str, " FLOAT[], ");
    else
      strcat(sql_str, " FLOAT, ");

    ++nVars;

    sprintf(temp, "INSERT INTO VariableList VALUES ('%s', '%s', '%s', 'RAF_%dhz')",
	name, VarDB_GetUnits(raw[i]->name), VarDB_GetTitle(raw[i]->name),
	raw[i]->SampleRate);
    PQclear(PQexec(conn, temp));
    fprintf(stderr, "%s", PQerrorMessage(conn));
    }

  for (i = 0; i < nderive; ++i)
    {
    strcat(sql_str, derived[i]->name);

    if (derived[i]->Length > 1)
      strcat(sql_str, " FLOAT[]");
    else
      strcat(sql_str, " FLOAT");

    if (i != nderive-1)
      strcat(sql_str, ", ");
    ++nVars;

    sprintf(temp, "INSERT INTO VariableList VALUES ('%s', '%s', '%s', 'RAF_%dhz')",
	derived[i]->name, VarDB_GetUnits(derived[i]->name),
	VarDB_GetTitle(derived[i]->name), derived[i]->OutputRate);
    PQclear(PQexec(conn, temp));
    fprintf(stderr, "%s", PQerrorMessage(conn));
    }

  strcat(sql_str, ")");
  PQclear(PQexec(conn, sql_str));
  fprintf(stderr, "%s", PQerrorMessage(conn));

  PQclear(PQexec(conn, "CREATE TABLE current (lastTime TIME, cntr INT)"));
  fprintf(stderr, "%s", PQerrorMessage(conn));
  PQclear(PQexec(conn, "INSERT INTO current VALUES ('00:00:00', 0)"));
  fprintf(stderr, "%s", PQerrorMessage(conn));

  printf("nVars = %d\n", nVars);

  PQclear(PQexec(conn, "CREATE RULE update AS ON UPDATE TO current DO NOTIFY current"));
  fprintf(stderr, "%s", PQerrorMessage(conn));

  PQclear(PQexec(conn, "VACUUM FULL"));
  fprintf(stderr, "%s", PQerrorMessage(conn));

  if (Mode == REALTIME)
    brdcst.openSock(UDP_BROADCAST);

  return(true);

}	/* END INITSQL */

/* -------------------------------------------------------------------- */
void Start2dSQL(RAWTBL *rp)
{
  strcpy(sql2d_str, "BEGIN;");

}	/* END START2DSQL */

/* -------------------------------------------------------------------- */
void Write2dSQL(RAWTBL *rp, long time, long msec, ulong *p, int nSlices)
{
  int	i;
  char	name[16], temp[128];

  if (nSlices < 2)
    return;

  strcpy(name, rp->name);
  name[strlen(name)-1] = '\0';

  sprintf(temp, "INSERT INTO %s VALUES ('%02d:%02d:%02d', %u, %d, '{%lu", name, time/3600, (time%3600)/60, time%60, msec, nSlices, p[0]);
  strcat(sql2d_str, temp);

  for (i = 1; i < nSlices; ++i)
    {
    sprintf(temp, ", %d", p[i]);
    strcat(sql2d_str, temp);
    }

  strcat(sql2d_str, "}');");

}	/* END WRITE2DSQL */

/* -------------------------------------------------------------------- */
void Submit2dSQL()
{
  // Overwrite trailing ',' with ending paren.
//  sql2d_str[strlen(sql2d_str)-1] = ')';
//printf("%s\n", sql2d_str);
  strcat(sql2d_str, "COMMIT;");
  SQLcommand(sql2d_str);

}	/* END SUBMIT2DSQL */

/* -------------------------------------------------------------------- */
void WriteSQL(char timeStamp[])
{
  int	i, j;
  char	*sql_p, *brd_p;
("WriteSQL\n");
  static int	cntr = 0;


  sprintf(brd_str, "RAF-TS %s ", timeStamp);
  brd_p = &brd_str[strlen(brd_str)];

  sprintf(sql_str, "INSERT INTO RAF_1hz VALUES ('%s', ", timeStamp);
  sql_p = &sql_str[strlen(sql_str)];

  for (i = 0; i < nsdi; ++i)
    {
    sprintf(buffer, "%e,", AveragedData[sdi[i]->LRstart]);
    if (buffer[0] == 'n' || buffer[0] == 'i')
      strcpy(buffer, "-32767.0,");

    strcpy(sql_p, buffer);
    sql_p += strlen(buffer);

    if (sdi[i]->Broadcast)
      {
      strcpy(brd_p, buffer);
      brd_p += strlen(buffer);
      }
    }

  for (i = 0; i < nraw; ++i)
    {
    if (raw[i]->Length > 1)
      {
      strcpy(sql_p, "'{");
      sql_p += 2;

      for (j = 0; j < raw[i]->Length; ++j)
        {
        sprintf(buffer, "%e", AveragedData[raw[i]->LRstart+j]);
        if (j != raw[i]->Length-1)
          strcat(buffer, ",");
        strcpy(sql_p, buffer);
        sql_p += strlen(buffer);
        }

      strcpy(sql_p, "}',");
      sql_p += 3;
      continue;
      }

    sprintf(buffer, "%e,", AveragedData[raw[i]->LRstart]);
    if (buffer[0] == 'n' || buffer[0] == 'i')
      strcpy(buffer, "-32767.0,");

    strcpy(sql_p, buffer);
    sql_p += strlen(buffer);

    if (raw[i]->Broadcast)
      {
      strcpy(brd_p, buffer);
      brd_p += strlen(buffer);
      }
    }

  for (i = 0; i < nderive; ++i)
    {
    if (derived[i]->Length > 1)
      {
      strcpy(sql_p, "'{");
      sql_p += 2;

      for (j = 0; j < derived[i]->Length; ++j)
        {
        sprintf(buffer, "%e", AveragedData[derived[i]->LRstart+j]);
        if (j != derived[i]->Length-1)
          strcat(buffer, ",");
        strcpy(sql_p, buffer);
        sql_p += strlen(buffer);
        }

      strcpy(sql_p, "}',");
      sql_p += 3;
      continue;
      }

    sprintf(buffer, "%e", AveragedData[derived[i]->LRstart]);
    if (buffer[0] == 'n' || buffer[0] == 'i')
      strcpy(buffer, "-32767.0");

    if (i != nderive-1)
      strcat(buffer, ",");

    strcpy(sql_p, buffer);
    sql_p += strlen(buffer);

    if (derived[i]->Broadcast)
      {
      strcpy(brd_p, buffer);
      brd_p += strlen(buffer);
      }
    }

  if (Mode == REALTIME)
    {
    strcpy(brd_p, "\n");
    brdcst.writeSock(brd_str, strlen(brd_str));
    sprintf(buffer, "); UPDATE current SET lastTime='%s';", timeStamp);
    }
  else
    strcpy(buffer, ");");

  strcpy(sql_p, buffer);
  SQLcommand(sql_str);


  if (++cntr < 10900 && cntr % 3600 == 0)     /* every hour, 1st 3 hours */
    {
fprintf(stderr, "Performing ANALYZE @ %s\n", timeStamp);
    SQLcommand("ANALYZE raf_1hz (datetime)");
    }

}	/* END WRITESQL */

/* -------------------------------------------------------------------- */
void SQLcommand(char command[])
{
  PGresult	*res;
printf("SQLcommand\n");
  while ( (res = PQgetResult(conn)) )
    PQclear(res);

  PQsendQuery(conn, command);
  fprintf(stderr, "%s", PQerrorMessage(conn));

  while ( (res = PQgetResult(conn)) )
    PQclear(res);

//  PQclear(PQexec(conn, command));

}	/* END CLOSESQL */

/* -------------------------------------------------------------------- */
void CloseSQL()
{
  PQfinish(conn);

}	/* END CLOSESQL */

/* END PSQL.C */
