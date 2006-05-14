DESTDIR=
LIBDIR=		$(NIMBUS)/lib
LINTDIR=	$(NIMBUS)/lib/lint

LIBNAME=	xlate

INCLUDES=	-I${NIMBUS}/include -I${LOCAL}/include
DEFINES=
CFLAGS=		${COPS} ${INCLUDES} ${DEFINES}

OBJS=	asas.o dme.o etorq.o event.o fssp.o gps.o gpsm.o hgm.o ins.o irs.o\
	loranc.o ophir3.o pms1_data.o pms1_hskp.o pms1_ps.o psfd.o serial.o\
	time.o uvhyg.o

SRCS=	asas.c dme.c etorq.c event.c fssp.c gps.c gpsm.c hgm.c ins.c irs.c\
	loranc.c ophir3.c pms1_data.c pms1_hskp.c pms1_ps.c psfd.c serial.c\
	time.c uvhyg.c

HDRS=	${LOCAL}/include/header.h ${NIMBUS}/include/amlib.h \
	${NIMBUS}/include/nimbus.h ${LOCAL}/include/constants.h

.SUFFIXES: .o .h .c

.c.o:
#	${CC} -pg ${CFLAGS} -c $*.c
#	mv $*.o profiled/$*.o
	${CC} ${CFLAGS} -c $*.c

all: ${LIBDIR}/lib${LIBNAME}.a ${LIBDIR}/lib${LIBNAME}_p.a

${OBJS}: ${HDRS}
hgm.o ins.o:	${NIMBUS}/include/gust.h

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
