/* gcc -I/home/local/include ncfixhdr2.c -o ncfixhdr2 -lnetcdf

	Modification of /home/local/proj/127/Production/ncfixhdr.c
     to change only the netCDF header's "ProjectName" field
      by Ronald L. Ruth                                            16 Feb 1996

 */

#include <netcdf.h>
#include <time.h>

main(argc, argv)
int	argc;
char	*argv[];
{
	int		fd, id, BaseTime;
	char		buffer[128];
	struct tm	StartFlight;


/*	printf("Enter new date [09/02/1994] : ");
	gets(buffer);
 */

	printf("Enter new ProjectName [ACE - 1]: ");
	gets(buffer);


	fd = ncopen(argv[1], NC_WRITE);
	ncredef(fd);
	ncattput(fd, NC_GLOBAL, "ProjectName", NC_CHAR, strlen(buffer)+1,
							(void *)buffer);

	ncendef(fd);

/*   Modification ends here.  The code below is commented out,
      because it is not needed.

	sscanf(buffer, "%d/%d/%d",	&StartFlight.tm_mon,
					&StartFlight.tm_mday,
					&StartFlight.tm_year);

	ncattget(fd, NC_GLOBAL, "TimeInterval", (void *)buffer);

	sscanf(buffer, "%02d:%02d:%02d",&StartFlight.tm_hour,
					&StartFlight.tm_min,
					&StartFlight.tm_sec);

	StartFlight.tm_mon--;
	StartFlight.tm_hour -= 6;
	StartFlight.tm_year -= 1900;
	BaseTime = mktime(&StartFlight);
	ncvarput1(fd, 0, 0, (void *)&BaseTime);

 */

	ncclose(fd);

}
