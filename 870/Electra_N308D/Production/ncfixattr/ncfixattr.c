/* gcc -I/home/local/include ncfixattr.c -o ncfixattr -lnetcdf

	Modification of /home/local/proj/130/Production/ncfixhdr/ncfixhdr2.c
	to change only one variable's (text only, at this time) attribute
	by Ronald L. Ruth                                       26 Feb 1996

    Usage:  ncfixattr <filename>
            (respond to the prompts)

 */

#include <netcdf.h>

main(argc, argv)
int	argc;
char	*argv[];
{
	int		fd, var_id, len;
	nc_type	datatype;
	char	vbuffer[12];
	char	abuffer[80];
	char	buffer[128];

	printf("Change a character-based variable attribute in a netCDF file\n\n");

	printf("Enter Variable's Name (e.g., ATWH): ");
	gets(vbuffer);

	printf("Enter Attribute's Name (e.g., units or long_name): ");
	gets(abuffer);

	printf("Enter Attribute's new value\n   (e.g., C or Ambient Temperature, Deiced Wing): ");
	gets(buffer);

	fd = ncopen(argv[1], NC_WRITE);

	ncattinq(fd, var_id, abuffer, &datatype, &len);
	switch (datatype)
	{
		case (NC_CHAR):
		{
			ncredef(fd);
			var_id = ncvarid(fd,vbuffer);
			ncattdel(fd, var_id, abuffer);
			ncattput(fd, var_id, abuffer, NC_CHAR, strlen(buffer)+1,
															(void *)buffer);
			ncendef(fd);
			printf(" Changed variable %s attribute %s to %s\n",vbuffer,
															abuffer, buffer);
			break;
		}
		default:
		{
			printf(" Cannot yet change %s.  Sorry.\n", abuffer);
		}
	}
	ncclose(fd);
}
