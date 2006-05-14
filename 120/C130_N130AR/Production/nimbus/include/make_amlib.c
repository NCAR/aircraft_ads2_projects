/*
-------------------------------------------------------------------------
OBJECT NAME:	make_amlib.c

FULL NAME:	Make amlibfn.h

ENTRY POINTS:	main()

STATIC FNS:	SortFns

DESCRIPTION:	

INPUT:		

OUTPUT:		

REFERENCES:	none

REFERENCED BY:	Makefile
-------------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_FILE	"amlib.fns"
#define OUTPUT_FILE	"amlibfn.h"
#define XLATE_FILE	"xlatefns.h"

#define BUFFSIZE	1024

char	buffer[BUFFSIZE], *file[10000];

/* -------------------------------------------------------------------- */
main()
{
	int	i, cnt;
	FILE	*in, *out, *xlate;


	if ((in = fopen(INPUT_FILE, "r")) == NULL)
		{
		fprintf(stderr, "make_amlib: can't open %s\n", INPUT_FILE);
		exit(1);
		}

	for (cnt = 0; fgets(buffer, BUFFSIZE, in); )
		{
		if (buffer[0] == '#' || strlen(buffer) < 12)
			continue;

		if ((file[cnt] = malloc(strlen(buffer)+1)) == NULL)
			{
			fprintf(stderr, "make_amlib: out of memory\n");
			exit(1);
			}

		strcpy(file[cnt++], buffer);
		}

	fclose(in);


	SortFns(0, cnt - 1);


	if ((out = fopen(OUTPUT_FILE, "w+")) == NULL)
		{
		fprintf(stderr, "make_amlib: can't open %s\n", OUTPUT_FILE);
		exit(1);
		}

	if ((xlate = fopen(XLATE_FILE, "w+")) == NULL)
		{
		fprintf(stderr, "make_amlib: can't open %s\n", XLATE_FILE);
		exit(1);
		}

	fprintf(out, "/*\n-------------------------------------------------------------------------\n");
	fprintf(out, "OBJECT NAME:\tamlibfn.h\n\n");

	fprintf(out, "DESCRIPTION:\tHeader File defining derived Variables with associated\n");
	fprintf(out, "\t\tprocessing functions.\n\n");

	fprintf(out, "NOTE:\t\tDO NOT modify this file.  This is automatically generated.\n");
	fprintf(out, "\t\tMake changes in amlib.fns\n");
	fprintf(out, "-------------------------------------------------------------------------\n*/\n\n");

	fprintf(out, "#ifndef DERIVEDEF_H\n#define DERIVEDEF_H\n\n");
	fprintf(xlate, "#ifndef DERIVEDEF_H\n#define DERIVEDEF_H\n\n");

	fprintf(out, "struct _dnfn deriveftns[] =\n\t\t{\n");
	fprintf(xlate, "struct _dnfn deriveftns[] =\n\t\t{\n");

	for (i = 0; i < cnt; ++i)
		{
		fprintf(out, "%s", file[i]);

		if (strstr(file[i], "xl"))
			fprintf(xlate, "%s", file[i]);
		}

	fprintf(out, "\t\tNULL,\t\tNULL,\tNULL,\tNULL\n\t\t};\n\n#endif\n");
	fprintf(xlate, "\t\tNULL,\t\tNULL,\tNULL,\tNULL\n\t\t};\n\n#endif\n");

	fclose(out);
	fclose(xlate);

	return(0);

}	/* END MAIN */

/* -------------------------------------------------------------------- */
char	*temp, *mid;

SortFns(beg, end)
int	beg;		/* Beggining array index */
int	end;		/* Last array index */
{
	register	x = beg,
			y = end;

	mid = file[(x + y) / 2];

	while (x <= y)
		{
	 	while (strcmp(file[x], mid) < 0)
			++x;

		while (strcmp(file[y], mid) > 0)
			--y;

		if (x <= y)
			{
			temp = file[x];
			file[x] = file[y];
			file[y] = temp;
			++x; --y;
			}
		}

	if (beg < y)
		SortFns(beg, y);

	if (x < end)
		SortFns(x, end);

}	/* END SORTFNS */

/* END MAKE_AMLIB.C */
