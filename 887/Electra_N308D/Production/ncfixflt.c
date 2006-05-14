/* gcc -I/home/local/include ncfixflt.c -o ncfixflt -lnetcdf
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


	printf("Enter new flight number: ");
	gets(buffer);


	fd = ncopen(argv[1], NC_WRITE);
	ncredef(fd);
	ncattput(fd, NC_GLOBAL, "FlightNumber", NC_CHAR, strlen(buffer)+1,
							(void *)buffer);

	ncendef(fd);

	ncclose(fd);

}
