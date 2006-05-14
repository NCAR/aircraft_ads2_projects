/* gcc -I/home/local/include nctestattr.c -o nctestattr -lnetcdf

	Modification of /home/local/proj/130/Production/ncfixattr/ncfixattr.c
	to inquire about a variable's attribute
	by Ronald L. Ruth                                       27 Feb 1996

    Usage:  nctestattr <filename>

 */

#include <netcdf.h>

main(argc, argv)
int	argc;
char	*argv[];
{
	int			fd, var_id;
	int			len;
	nc_type		datatype;
	char		vbuffer[12];
	char		abuffer[80];
	char		buffer[128];

	printf("Inquire about a variable's attribute in a netCDF file\n\n");

	printf("Enter Variable's Name (e.g., ATWH): ");
	gets(vbuffer);

	printf("Enter Attribute's Name (e.g., units or long_name): ");
	gets(abuffer);

	fd = ncopen(argv[1], NC_WRITE);

	var_id = ncvarid(fd,vbuffer);

	ncattinq(fd, var_id, abuffer, &datatype, &len);

	ncclose(fd);

	printf(" Attribute type = %i\n", datatype);
	printf(" Number of attributes = %i\n", len);

}
