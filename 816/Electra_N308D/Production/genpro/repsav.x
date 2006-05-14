C*********************************************************************          
C  GLOBAL SAVE BLOCK
C  DIMENSION SIZE INCREASED SEPT 1990 FOR UNICOS VERSION GENPRO (-CC)
C  X1 version setup for HaRP Project (ISPACE=900000)
C  THIS IS SETUP TO PROCESS - 600 VARIABLES
C                           - TOTAL OF UP TO 30 OPERATIONS
C                           - UP TO 5 INPUT OPERATIONS
C  
.OPTION OC=1 UC=0
.REPL /$VERS/X1/                                                                
.REPL /$INOP/5/                                                                 
.REPL /$IOP/30/                                                                 
.REPL /$NVAR/600/                                                               
.REPL /$NFILE/500/                                                              
.REPL /$ISPACE/900000/                                                          
.REPL /$LENSP/875000/                                                           
.REPL /$LENTR/25000/  
.REPL /$MAXCA/30000/                                                            
.REPL /$BITCHR/8/                                                               
.REPL /$NCPNSU/8/                                                               
.REPL /$IVDIM/3/                                                                
.AC   A                                                                         
.DE   G                                                                         
.DE   U                                                                         
.DE   V                                                                         
.DE   M                                                                         
.AC   H                                                                         
.AC   T                                                                         
.DE   R                                                                         
.DE   K                                                                         
.DE   Y                                                                         
.SAVE TYPVAR                                                                    
C  ALL WRONG TYPE VARIABLES IN  BLANK,CYCVAR,OP,INPUT, AND IOUNIT               
      INTEGER  FLUSHP, BITCHR, BITNSU, OVRLAP, THISOP                           
      INTEGER  EXDATE, EXTIME, PRDATE, PRTIME                                   
.END                                                                            
.SAVE TBLANK                                                                    
C  BLANK                                                                        
      INTEGER IBUF, ICON, IDATA, ISAV, ISCR, LENBUF, LENCA, LENDAT,             
     1 LENSAV, LENSCR, LENSP, LOCBUF, LOCCA, LOCDAT, LOCSAV, LOCSCR             
      REAL SPACE, BUF, CON, DATA, SAV, SCR, OVRFLO                              
.END                                                                            
.SAVE TCYCVAR                                                                   
C  CYCVAR                                                                       
      INTEGER IVDIM, LABIV, LENPER, FLUSHP, BITCHR, BITNSU, MXCHR               
      INTEGER  EXDATE, EXTIME, PRDATE, PRTIME, MACHIN, JOBIDS, NSUIDS           
      REAL BEGIV, ENDIV, DELIV, SCALIV, BEGPER, ENDPER, BITIV                   
.END                                                                            
.SAVE TERROR                                                                    
      INTEGER IABORT, NSTAGE, IERR, NUMERR                                      
.END                                                                            
.SAVE TFLOW                                                                     
      INTEGER INOP1,ITYPOP,JOBS,JOBSTP,KORD,LENFIX,LENTOT,LINDCY,MINCY,         
      INTEGER NCASE,NCYEND,NCYOLD,NUMOPT,NUMREM,NUMOPS,NXTTOP,                  
      INTEGER SNPBRK,TOTOPS                                                     
.END                                                                            
.SAVE TINPUT                                                                    
C  INPUT                                                                        
      INTEGER INNUM, INTOT, ISTATE, NAMFIL, NAMREC, NAMVOL, NUNIT               
      REAL BEGTAP, ENDTAP                                                       
.END                                                                            
.SAVE TIOUNIT                                                                   
C  IOUNIT                                                                       
      INTEGER KCHECK, KDUMP, KERR, KFICHE, KFILM, KPROC, KPRINT                 
.END                                                                            
.SAVE TOP                                                                       
C  OP                                                                           
      INTEGER INDCYC, THISOP, LASTOP, LENCYC, NAMEOP, NCDO, NCYNOW,             
     1 NUMCAL, NUMCYC, OVRLAP                                                   
      REAL GAPFLG, GAPVAL, VALIV                                                
.END                                                                            
.SAVE TSNAP                                                                     
      INTEGER LENSNP,LOCBEG,LOCEND,NUMGRP                                       
      REAL BEGSNP,ENDSNP                                                        
.END                                                                            
.SAVE BLANK                                                                     
C                                                                               
C  THIS COMMON BLOCK CONTAINS ALL LARGE STORAGE PARTITIONS USED BY ALL          
C  OPERATIONS, AS WELL AS POINTERS AND LENGTHS FOR THOSE PARTITIONS             
C                                                                               
      COMMON SPACE($ISPACE), OVRFLO, LENSP,                                     
     1 LENBUF($IOP),LENCA($IOP),LENDAT($IOP),LENSAV($IOP),LENSCR,               
     2 LOCBUF($IOP),LOCCA($IOP),LOCDAT($IOP),LOCSAV($IOP),LOCSCR                
      DIMENSION  CON($ISPACE), SAV($ISPACE), BUF($ISPACE)                       
     -, DATA($ISPACE), SCR($ISPACE)                                             
      DIMENSION ICON($ISPACE),ISAV($ISPACE),IBUF($ISPACE)                       
     -,IDATA($ISPACE),ISCR($ISPACE)                                             
      EQUIVALENCE (SPACE(1), CON(1), SAV(1), BUF(1), DATA(1), SCR(1))           
      EQUIVALENCE (SPACE(1),ICON(1),ISAV(1),IBUF(1),IDATA(1),ISCR(1))           
.END                                                                            
.SAVE CYCVAR                                                                    
C  THIS COMMON BLOCK CONTAINS THE INDEPENDENT VARIABLE RUN PERIOD,              
C  THE INDEPENDENT VARIABLE CYCLE INCREMENT, UNITS OF THE INDEPENDENT           
C  VARIABLE, THE BITS PER CHARACTER AND BITS PER NSU OF THE                     
C  COMPUTER BEING USED, THE SNAPSHOT DISPLAY PERIODS, AND THE PROJECT           
C  AND EXECUTION DATES, TIMES, AND JOB IDS                                      
C                                                                               
      COMMON /CYCVAR/ BEGIV, ENDIV, DELIV, IVDIM, LABIV(6), SCALIV(6),          
     1 BEGPER(7), ENDPER(7), LENPER, FLUSHP, BITCHR, BITNSU,BITIV,MXCHR         
     2,EXDATE(3),EXTIME(3),PRDATE(3),PRTIME(3),MACHIN,JOBIDS(10),NSUIDS         
.END                                                                            
.SAVE OP                                                                        
C  THIS COMMON BLOCK CONTAINS ITEMS RELATED TO THE DATA CYCLE NUMBER            
C  AND SIZE, THE OPERATION CALL, AND THE NEEDED HOOKS INTO THE DATA             
C                                                                               
      COMMON /OP/ GAPFLG, GAPVAL, INDCYC($IOP), THISOP, LASTOP,                 
     1 LENCYC($IOP),NAMEOP($IOP), NCDO, NCYNOW($IOP), NUMCAL($IOP),             
     2 NUMCYC($IOP),OVRLAP($IOP), VALIV($IOP)                                   
.END                                                                            
.SAVE ERROR                                                                     
C                                                                               
C  THIS COMMON BLOCK PROVIDES ERROR COMMUNICATIONS BETWEEN THE DRIVER           
C  AND THE OPS AT VARIOUS STAGES OF JOB EXECUTION.  IERR IS 0 WHEN AN           
C  OP IS CALLLED.  THE OP SUMS ALL FATAL ERRORS AT THE STAGE OF EXECU-          
C  TION NSTAGE AND RETURNS THIS VALUE IN IERR.                                  
C                                                                               
      COMMON /ERROR/ IABORT, NSTAGE, IERR, NUMERR($IOP)                         
.END                                                                            
.SAVE INPUT                                                                     
      COMMON /INPUT1/ BEGTAP($INOP), ENDTAP($INOP), INNUM, INTOT,               
     1 ISTATE($INOP), NAMFIL($INOP),NAMREC($INOP),NAMVOL($INOP),                
     2 NUNIT($INOP)                                                             
.END                                                                            
.SAVE IOUNIT                                                                    
C THIS COMMON BLOCK CONTAINS GENERIC OUTPUT UNIT NUMBERS                        
      COMMON /IOUNIT/ KCHECK, KDUMP, KERR, KFICHE, KFILM, KPROC, KPRINT         
.END                                                                            
.SAVE CASINT                                                                    
      COMMON /CASINT/ IVINTS,IVINT                                              
.END                                                                            
.SAVE INDEX                                                                     
C                                                                               
C  CONTROL ARRAY INDICES FOR OPERATION THISOP                                   
C  SEE SAVE BLOCK INDEXC FOR A DEFINITION OF ALL VARIABLES                      
C                                                                               
      INDADD = LOCCA(THISOP) - 1                                                
      IF(INDST.LT.1.OR.INDST.GT.3) INDST = 2                                    
      INDCON = ICON(INDST*2+INDADD) + INDADD                                    
      NUMVEC = ICON(INDCON + 2)                                                 
      NUMCON = ICON(INDCON + 4)                                                 
      LENCON = ICON(INDCON + 6)                                                 
      INDOFF = INDCON + 7                                                       
      INDGRP = INDOFF + 1*(NUMCON + 1)                                          
      INDELM = INDOFF + 2*(NUMCON + 1)                                          
      INDTYP = INDOFF + 3*(NUMCON + 1)                                          
      INDNAM = INDOFF + 4*(NUMCON + 1)                                          
      INDVAL = INDOFF + 5*(NUMCON + 1)                                          
.END                                                                            
.SAVE INDEXS                                                                    
C               A SHORT FORM OF SAVE BLOCK INDEX                                
C          CONTROL ARRAY INDICES FOR OPERATION THISOP                           
C    SEE SAVE BLOCK INDEXC FOR A DEFINITION OF ALL VARIABLES                    
C                                                                               
      INDADD = LOCCA(THISOP) - 1                                                
      IF(INDST.LT.1.OR.INDST.GT.3) INDST = 2                                    
      INDCON = ICON(INDST*2+INDADD) + INDADD                                    
      NUMVEC = ICON(INDCON + 2)                                                 
      NUMCON = ICON(INDCON + 4)                                                 
      LENCON = ICON(INDCON + 6)                                                 
      INDOFF = INDCON + 7                                                       
      INDELM = INDOFF + 2*(NUMCON + 1)                                          
      INDVAL = INDOFF + 5*(NUMCON + 1)                                          
.END                                                                            
.SAVE FLOW                                                                      
C  THIS COMMON BLOCK CONTAINS ITEMS NEEDED BY THE DRIVER TO REGULATE            
C  THE DATA FLOW                                                                
C                                                                               
      INTEGER SNPBRK, TOTOPS                                                    
      COMMON /FLOW/ INOP1,ITYPOP($IOP),JOBS,JOBSTP,KORD($IOP), LENFIX,          
     1 LENTOT($IOP), LINDCY($IOP), MINCY($IOP), NCASE,   NCYEND($IOP),          
     2 NCYOLD($IOP),NUMOPT, NUMREM($IOP), NUMOPS, NXTTOP($IOP),SNPBRK,          
     3 TOTOPS                                                                   
.END                                                                            
.SAVE GETPUT                                                                    
C  THE CONTROL NO. OF VARIABLE CONTROLS INDGET, INRATE, INDPUT, AND RATE        
C                                                                               
      COMMON /GETPUT/NDXGET($IOP),NDXINR($IOP),NDXPUT($IOP),NDXRTE($IOP)        
.END                                                                            
.SAVE SNAP                                                                      
C THE START AND END PERIODS FOR SNAPSHOT OUTPUT OR DISPLAY                      
      COMMON /SNAP/ BEGSNP($IOP), ENDSNP($IOP), LENSNP($IOP),                   
     1 LOCBEG($IOP), LOCEND($IOP), NUMGRP($IOP)                                 
.END                                                                            
.SAVE TIMER                                                                     
C                                                                               
C  THIS COMMON BLOCK IS USED TO TIME THE OPERATIONS                             
      COMMON/TIMER/ TIM($IOP),TIMTR,TIMDR                                       
.END                                                                            
.SAVE MSGPR                                                                     
C  THIS COMMON IS USED TO GENERATE ERROR MESSAGES                               
      COMMON /MSGPR/ LEVPRT,KINFO,KWARN,KFATAL,KCAT,KBLNK,MSGVAL(10)            
.END                                                                            
.SAVE OUTCIO                                                                    
C  THIS COMMON IS USED TO CONTROL DATA FLOW                                     
      COMMON /OUTCIO/ ISTOP,NCRM,NCFL,LSVOP,LSVLST,LOCLST($IOP),                
     1 MVFLAG($IOP)                                                             
.END                                                                            
.SAVE DRPRNT                                                                    
      COMMON/DRPRNT/IPR(9),NUMDMP,LENDMP,IDUMP(80),IYES,INO                     
.END                                                                            
.SAVE INCOM                                                                     
      INTEGER SNPBRK, TOTOPS , OVRLAP, THISOP , BITCHR , BITNSU                 
      INTEGER EXDATE, EXTIME, PRDATE, PRTIME                                    
      COMMON SPACE($ISPACE), OVRFLO, LENSP,                                     
     1 LENBUF($IOP),LENCA($IOP),LENDAT($IOP),LENSAV($IOP),LENSCR,               
     2 LOCBUF($IOP),LOCCA($IOP),LOCDAT($IOP),LOCSAV($IOP),LOCSCR                
      DIMENSION  CON($ISPACE), SAV($ISPACE), BUF($ISPACE)                       
     -, DATA($ISPACE), SCR($ISPACE)                                             
      DIMENSION ICON($ISPACE),ISAV($ISPACE),IBUF($ISPACE)                       
     -,IDATA($ISPACE),ISCR($ISPACE)                                             
      EQUIVALENCE (SPACE(1), CON(1), SAV(1), BUF(1), DATA(1), SCR(1))           
      EQUIVALENCE (SPACE(1),ICON(1),ISAV(1),IBUF(1),IDATA(1),ISCR(1))           
      COMMON /OP/ GAPFLG, GAPVAL, INDCYC($IOP), THISOP, LASTOP,                 
     1 LENCYC($IOP),NAMEOP($IOP), NCDO, NCYNOW($IOP), NUMCAL($IOP),             
     2 NUMCYC($IOP),OVRLAP($IOP), VALIV($IOP)                                   
      COMMON /ERROR/ IABORT, NSTAGE, IERR, NUMERR($IOP)                         
      COMMON /IOUNIT/ KCHECK, KDUMP, KERR, KFICHE, KFILM, KPROC, KPRINT         
      COMMON /CYCVAR/ BEGIV, ENDIV, DELIV, IVDIM, LABIV(6), SCALIV(6),          
     1 BEGPER(7), ENDPER(7), LENPER, FLUSHP, BITCHR, BITNSU,BITIV,MXCHR         
     2,EXDATE(3),EXTIME(3),PRDATE(3),PRTIME(3),MACHIN,JOBIDS(10),NSUIDS         
      COMMON /MSGPR/ LEVPRT,KINFO,KWARN,KFATAL,KCAT,KBLNK,MSGVAL(10)            
      COMMON /GETPUT/NDXGET($IOP),NDXINR($IOP),NDXPUT($IOP),NDXRTE($IOP)        
      COMMON /FLOW/ INOP1,ITYPOP($IOP),JOBS,JOBSTP,KORD($IOP), LENFIX,          
     1 LENTOT($IOP), LINDCY($IOP), MINCY($IOP), NCASE,   NCYEND($IOP),          
     2 NCYOLD($IOP),NUMOPT, NUMREM($IOP), NUMOPS, NXTTOP($IOP),SNPBRK,          
     3 TOTOPS                                                                   
      COMMON /OUTCIO/ ISTOP,NCRM,NCFL,LSVOP,LSVLST,LOCLST($IOP),                
     1 MVFLAG($IOP)                                                             
      COMMON/INPUT2/INSV($IOP)                                                  
      COMMON/DRPRNT/IPR(9),NUMDMP,LENDMP,IDUMP(80),IYES,INO                     
      COMMON /VECLEN/ LENCA1,LENOF5,LENVEC($IOP),NSUSAV                         
.END                                                                            
.SAVE RATAB                                                                     
      COMMON /RATAB/ LRATE($IOP,$NVAR),LISV($NVAR),NTYPE($IOP),LSTLEN           
.END                                                                            
.SAVE RANDOM                                                                    
V     COMMON/RNDM/KFF,KFILE,IPOS($NFILE,3),LENREC                               
H     COMMON/RNDM/NAMEF,KFILE,IPOS($NFILE,2)                                    
M     COMMON/RNDM/KFF,KFILE,IPOS($NFILE,3),LENREC                               
G     COMMON/RNDM/NAMEF,KFILE,IPOS($NFILE,2)                                    
.END                                                                            
