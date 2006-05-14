#  Make file for ncfixattr -- fix netCDF file attribute    RLR  <990113.1430>
#
#  Note:  To create an executable version of this program, library files are
#          required in addition to the files in this directory.  These include
#          two netCDF libraries (-lnetcdf -lnsl) as well as the COS-block
#          utility library (-lncaru).
#
#    Compiler options:
#     -g -- generate symbol table for use by debuggers
#     -c -- do not invoke the linker
#
#CC=  cc
CC=  gcc
DESTDIR=  /usr/local/
BINDIR=   bin
MANDIR=   man/man1
LIBS=     -lnetcdf -lnsl
LINTLIBS= ${LIBS}
INCLUDES= -I/home/local/include
CFLAGS=   -g ${INCLUDES}
#CFLAGS=   -O2 ${INCLUDES}

SRCS= ncfixattr.c

HDRS=

OBJS=  ncfixattr.o

PROG=  ncfixattr

.c.o:
	umask 2
	${CC} ${CFLAGS} -c $*.c

${PROG}: ${OBJS}
	umask 2
	${CC} ${CFLAGS} ${OBJS} ${LIBS} -o $@

install: ${PROG}
	install -m 755 -s ${PROG} ${DESTDIR}${BINDIR}
	install -m 444 ${PROG}.1 ${DESTDIR}${MANDIR}

clean:
	rm -f ${OBJS} ${PROG}

lint:
	lint ${INCLUDES} ${SRCS} ${LINTLIBS}

print:
	enscript -2Gr -b${PROG} ${HDRS} ${SRCS}

${OBJS}:
