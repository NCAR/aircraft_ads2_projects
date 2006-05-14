/*
-------------------------------------------------------------------------
OBJECT NAME:	amlib.h

DESCRIPTION:	Header File declaring amlib functions.
-------------------------------------------------------------------------
*/

#ifndef AMLIB_H
#define AMLIB_H

#include "header.h"

/* Values for 'FeedBack' variable	*/
#define	nFeedBackTypes		2

#define LOW_RATE_FEEDBACK	0
#define HIGH_RATE_FEEDBACK	1

#define GetSample(dp, di)	\
	(FeedBack == LOW_RATE_FEEDBACK \
		? AveragedData[dp->depend_LRindex[di]] \
		: HighRateData[dp->depend_HRindex[di] + SampleOffset])

#define GetVector(dp, di, vlen)	\
	(FeedBack == LOW_RATE_FEEDBACK \
		? &AveragedData[dp->depend_LRindex[di]] \
		: &HighRateData[dp->depend_HRindex[di] + (SampleOffset * vlen)])


#define PutSample(dp, y)	\
	if (FeedBack == LOW_RATE_FEEDBACK) \
		AveragedData[dp->LRstart] = y; \
	else \
		HighRateData[dp->HRstart + SampleOffset] = y

#define PutStaticSample(dp, y)	\
	if (FeedBack == LOW_RATE_FEEDBACK) \
		AveragedData[dp->LRstart] = y[HIGH_RATE]; \
	else \
		HighRateData[dp->HRstart + SampleOffset] = y[SampleOffset]

struct _dnfn
	{
	char		*name;
	void		(*constructor)();
	void		(*xlate)();
	void		(*compute)();
	} ;

#define	FirstPoly(x, c)		(c[0] + (x * c[1]))
#define	SecondPoly(x, c)	(c[0] + (x * c[1]) + (x * c[2]))


int	SearchDERIVFTNS();
void	RunAMLIBinitializers(), SetLookupSuffix();

extern NR_TYPE		*AveragedData, *HighRateData;
extern int		FeedBack, SampleOffset;
extern struct _dnfn	deriveftns[];

NR_TYPE *GetDefaultsValue(char target[]);

/*      AMLIB Compute functions
 */
void	sc200x(), sconcx(), sdispx(), splwcx(), sdbarx(), sc200y(), sconcy(),
	sdispy(), splwcy(), sdbary(), sc260x(), sconc6(), sdisp6(), splwc6(),
	sdbar6(), sakdf(), sakrd(), scasas(), sconca(), sdispa(), sxco2c(),
	sdbara(), satb(), satf(), satfh(), satkp(), satrf(), sattack(), satx(),
	scon2c(), scon2p(), sdei(), sdni(), sdpc(), sdpxc(), sdvalue(),
	scf300(), sconc3(), sdbar3(), scfssp(), sfact(), sfbmfr(), sconcf(),
	sdispf(), splwcf(), sdbarf(), sgsf(), sirc(), swp3(), shi3(), svewL51(),
	svnsL51(), swp3L51(), shi3L51(), sacinsL51(), sui(), svi(), sux(),
	svy(), swi(), swd(), sws(), slwcc(), smr(), sedpc(), smrla(), spalt(),
	splwcc(), spsbc(), spsfc(), spsfd(), spsfd2(), spsfdc(), spsrxc(),
	spsurf(), spswc(), spsx(), spsxc(), sqcbc(), sqcfc(), sqcrc(), sqcrxc(),
	sqcwc(), sqcx(), sqcxc(), srhodt(), srhola(), srhola1(), srhum(),
	ssphum(), sssdf(), ssslip(), sssrd(), stasf(), stasr(),
	stasw(), stasx(), stheta(), sthetae(), sthetav(), sthf(), spaltf(),
	sttx(), stvir(), sfunc(), sxmach2(), sxrlwcc(), sxlh2oc(),
	sconcn(), scFlow(), scntsm(), sconccn(), scvl(), scvfcc(), sCItas(),
	scvf2c(), scvfhc(), scvfxc(), scvcnc(), scvcno(), scfact(), sxno2(),
	sdpcrc(), sfpcrc(), scmrcr(), srhocr(), sefqz(), sefqyr(), sefqzr(),
	sefx1(), sefx2(), sefyr(), sefzr(), sefz(), sefqyrs(), sefqzrs(),
	sefqzs(), sefx1s(), sefx2s(), sefyrs(), sefzrs(), sefzs(), sndvi(),
	soat(), sotdetc(), sodsigc(), sobbcr(), sogainf(), sodetof(), steo3c(),
	so3fc(), sxnom(), sxnoym(), slatc(), slonc(), svewc(), svnsc(), stash(),
	srlwc(), sxthmp3(), sxthmp4(), sxthmp5(), sxtas(), sxattck(), sxsslip(),
	scvs4(), scvs7(), srhcv(), sxeco2c(), szero(), sone(), sxno2c(),
	sxmrli(), sflux_wx(), sflux_ww(), sflux_xx(), sxo3fdg(), sefeft(),
	sefefb(), sefefp(), sefefs(), sefent(), sefenb(), sefens(), srange(),
	sefenp(), sefefqz(), sefefqy(), sefenqz(), sefenqy(), sefexy(),
	sefexz(), sefeyo(), sefezo(), sefefz(), sefefy(), sefenz(), sefeny(),
	sazimuth(), sptdrdst(), sptdrrad(), sptdrtim(), sptlgdst(), sptlgrad(),
	sptlgalt(), sspias(), sspalt(), sbit0(), sbit1(), sbit2(), sbit3(),
	sbit4(), sbit5(), sbit6(), sbit7();


/* Xlate functions
 */
void	xldmedi(), xldmech(), xldmefl(), xlevent(), xlgstat(), xlglat(),
	xlglon(), xlgalt(), xlgtimp(), xlgvew(), xlgvns(), xlgvzi(),
	xlgtimv(), xlgmode(), xlgmtime(), xlgmlat(), xlgmlon(), xlgmalt(),
	xlgmgeoh(), xlgmvew(), xlgmvns(), xlgmmode(), xlinssec(), xlalat(),
	xlalon(), xlgsi(), xltrki(), xlthi(), xlalpha(), xlxvi(), xlyvi(),
	xlilat(), xlilon(), xlipitch(), xliroll(), xlithdg(), xlivspd(),
	xlvacc(), xlivew(), xlivns(), xligspd(), xllag50(), xllag25(),
	xllag10(), xllag5(), xlcstat(), xlctmlag(), xlcnavtm(), xlclat(),
	xlclon(), xlccep(), xlccgs(), xlcctk(), xlcmagvr(), xlclortm(),
	xlcwarn(), xlatmns(), xluvhsmp(), xluvhref(), xluvkrsp(), xluvkrrf(),
	xluvchsp(), xluvchrf(), xluvpvol(), xluvtvol(), xlmruv(), xlhour(),
	xlmin(), xlsec(), xlfreset(), xlfstrob(), xlfrange(), xlP16bin(),
	xlP16v2bin(), xlP260bin(), xlP32v2bin(), xlP300bin(), xlP64v2bin(),
	xlpitch51(), xlcroll51(), xlroll51(), xlphdg51(), xlvzi51(), xletorq(),
	xlhgm(), xlhgme(), xlchgme(), xlpsfd(), xlpms_hskp0(), xlpms_hskp1(),
	xlpms_hskp2(), xlpms_hskp3(), xlpms_hskp4(), xlpms_hskp5(), xlaact(),
	xlpms_hskp6(), xlpms_hskp7(), xlogain(), xlobbcod(), xlodetdc(),
	xlotsnt(), xlotbnch(), xlotdet(), xlotbbc(), xlotbbf(), xlosmotr(),
	xlodetsg(), xlotbox(), xlfrange2(), xlpartspace(), xlserial(), xlialt(),
	xlpsfd_ADS2(), xlbpitchr(), xlbrollr(), xlbyawr(), xlblona(), xlblata(),
	xlbnorma();

/* AMLIB Constructor functions
 */
void	irsInit(), atfhInit(), c200xInit(), c200yInit(), c260xInit(), razInit(),
	plwccInit(), lymanInit(), cnctrInit(), kokInit(), ophir3Init(),
	cf300Init(), casasInit(), conc2cInit(), conc2pInit(), xrlwccInit(),
	cfsspInit(), deiInit(), cviInit(), LICORinit(), initFlux(), EFinit(),
	parcelInit(), deadReckInit();

#endif

/* END AMLIB.H */
