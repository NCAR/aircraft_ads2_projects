DESTDIR=
LIBDIR=		${NIMBUS}/lib
LINTDIR=	/usr/lib/lint
MANDIR=		/usr/man/man3
INCLUDEDIR=	/usr/include

LIBNAME=	user

INCLUDES=	-I${NIMBUS}/include -I${LOCAL}/include
DEFINES=
CFLAGS=		${COPS} ${INCLUDES} ${DEFINES}

OBJS=	chemcal.o co2c.o conccn.o cvi.o efield.o gegg.o licor.o ndvi.o bits.o\
	ophir3.o prcltrk1.o prcltrk2.o rlwp.o zero.o spefm.o raz.o sail.o

SRCS=	chemcal.c co2c.c conccn.c cvi.c efield.c gegg.c licor.c ndvi.c bits.c\
	ophir3.c prcltrk1.c prcltrk2.c rlwp.c zero.c spefm.c raz.c sail.c

HDRS=	${NIMBUS}/include/nimbus.h ${NIMBUS}/include/amlib.h \
	${LOCAL}/include/constants.h ${LOCAL}/include/header.h

.SUFFIXES: .o .h .c

.c.o:
#	${CC} -pg ${CFLAGS} -c $*.c
#	mv $*.o profiled/$*.o
	${CC} ${CFLAGS} -c $*.c

all: ${LIBDIR}/lib${LIBNAME}.a ${LIBDIR}/lib${LIBNAME}_p.a

${OBJS}: ${HDRS}

${LIBDIR}/lib${LIBNAME}.a: ${OBJS}
	rm -f ${LIBDIR}/lib${LIBNAME}.a
	ar cq ${LIBDIR}/lib${LIBNAME}.a ${OBJS}

${LIBDIR}/lib${LIBNAME}_p.a: ${OBJS}
#	rm -f ${LIBDIR}/lib${LIBNAME}_p.a
#	cd profiled; ar cq ${LIBDIR}/lib${LIBNAME}_p.a ${OBJS}

lint:
	lint -C${LIBNAME} ${INCLUDES} *.c

install: all
#	cp lib${LIBNAME}.a ${DESTDIR}/${LIBDIR}
#	cp lib${LIBNAME}_p.a ${DESTDIR}/${LIBDIR}

clean:
	rm -f *.o *.a *.ln profiled/*.o
