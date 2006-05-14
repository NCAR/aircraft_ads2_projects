C
C** DPLA - DEW POINT CALCULATION  (C)
       IF ((LDLA.AND.LDPLA) .OR. (LDLA1.AND.LDPLA1))
C    Following Cooper's code line caused problems, hence deleted.
C       IF (RHOLA.GT.20.) RHOLA = 0.000001
        ELA = (ATXK* RHOLA)/1322.3
        IF (ELA.GT.0.0)
         Z = ALOG(ELA)
        ELSE
         Z = 0.0
        END IF
        DPLA = (237.3*Z)/(17.27-Z)
        IF (ATX.LT.0.0) DPLA = (273.0*Z)/(22.51-Z)
C  Blow up prevention -- unreasonably large dew point
        DPLA = AMIN(DPLA,ATX+10.)
