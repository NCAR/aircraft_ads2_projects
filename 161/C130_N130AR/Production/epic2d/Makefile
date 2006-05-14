#
#    Makefile for dnload
#
DESTDIR=	/usr/local
BINDIR=		${DESTDIR}/bin
MANDIR=		${DESTDIR}/man/manl
INCDIR=		${DESTDIR}/include
INCDIR=		/usr/local/dev/libraf++

LIBS=		-lraf++
INCLUDES=	-I$(INCDIR)

CC=		g++
CFLAGS=		-g ${INCLUDES}

SRCS=	epic2d.cc
PROG=	epic2d

.c.o:
	${CC} ${CFLAGS} -c $*.c

${PROG}: epic2d.cc ${LOCAL}/lib/libraf++.a
	${CC} ${CFLAGS} epic2d.cc ${LIBS} -o $@

install: ${PROG}
	cp ${PROG} ${BINDIR}

clean:
	rm -f core ${PROG}

print:
	enscript -2Gr -bepic2d ${HDRS} ${SRCS}
