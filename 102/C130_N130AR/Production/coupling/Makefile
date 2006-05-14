#
#    Makefile for Skeleton program
#
DESTDIR=
BINDIR=		/home/local/bin
MANDIR=		/home/man/manl
LIBS=		-lraf -lm -lnetcdf -lXm -lXt -lX11
LINTLIBS=	${LIBS}
INCLUDES=	-I${LOCAL}/include
CFLAGS=		-g ${INCLUDES}
CC=		gcc

SRCS=	skel.c globals.c init.c timeseg.c sort.c amlib.c search.c lrloop.c\
		compute.c netcdf.c cb_main.c cb_time.c cb_pause.c Xwin.c error.c

HDRS=	define.h

OBJS=	skel.o globals.o init.o timeseg.o sort.o amlib.o search.o lrloop.o\
		compute.o netcdf.o cb_main.o cb_time.o cb_pause.o Xwin.o error.o

PROG=	skel

.c.o:
	${CC} ${CFLAGS} -c $*.c

${PROG}: ${OBJS}
	${CC} ${CFLAGS} ${OBJS} ${LIBS} -o $@

install: ${PROG}
	install -m 755 -s ${PROG} ${DESTDIR}${BINDIR}
	install -m 444 ${PROG}.l ${DESTDIR}${MANDIR}
	install -m 444 ${RESOURCE} ~

clean:
	rm -f ${OBJS} ${PROG}

lint:
	lint ${INCLUDES} ${SRCS} ${LINTLIBS}

print:
	enscript -2Gr -b${PROG} ${HDRS} ${SRCS}

${OBJS}:	define.h
