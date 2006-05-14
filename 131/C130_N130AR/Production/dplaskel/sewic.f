      SUBROUTINE SEWIC(T,P,E)                                                   
C ** CORRECTION ** CORRECTION **                                                
C ** MEMO FROM RON RUTH ON 29 MAY 1990                                          
C    THIS SUBROUTINE IS TO CALCULATE VAPOR PRESSURE ONLY WITH RESPECT           
C    TO A PLANE WATER SURFACE. SINCE THE WATER-TO-ICE CORRECTION HAS            
C    ALREADY BEEN MADE.                                                         
C **                                                                            
C ** MEMO FROM ERIK M. AND RICHARD F. SEPT.10,1985                              
C    EW - WATER VAPOR PRESSURE IN MB (GOFF-GRATCH)                              
C    TK - TEMPERATURE IN DEG K (DPC/TLCL/ATX)                                   
C    FW - ENHANCEMENT FACTORS                                                   
C    P - PRESSURE IN MB                                                         
C    E= EWC - WATER VAPOR PRESSURE FOR 'MOIST' AIR                              
C                                                                               
      TK = T + 273.16                                                           
C  Check added by RLR on 19Mar1992
      IF (TK.LT.1.0) TK = 1.0
      EW =10.**(23.832241-5.02808*ALOG10(TK)-1.3816E-7                         
     1     *(10.**(11.334-.0303998*TK))+8.1328E-3                               
     2     *(10.**(3.49149-1302.8844/TK))-2949.076/TK)                          
      FW = 1.0007+(3.46E-6 * P)                                                
      E  = FW*EW                                                               
      RETURN                                                                    
      END                                                                       
