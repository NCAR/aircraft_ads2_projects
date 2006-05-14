C  This file is just a reformatted version of the file "nfix_ins.f".
C  It consistently indents all the do loops and if-endif constructs.
C      RLR
C
      subroutine fix_ins( icount, alat, alon, vew, vns, roll
     +      , ui, vi, glat, glon, gstat
     +      , gvew, gvns, alatc, alonc
     +      , vewc, vnsc, wdc, wsc
     +      , dvx, dvy, dtime
     +  )
c
c   Al Cooper's algorithm for correcting aircraft positions and velocities
c  using GPS info.
C
C   Revised by RLR on 931006 to eliminate a latitude/longitude threshold
C    comprising a 60-degree x 100-degree box around Hawaii, instead making
C    it a +-20 degree latitude and/or longitude difference between the GPS
C    and the INS.  Differences detected outside of the threshold would
C    turn off the GPS corrections.  Before these corrections, any flight
C    occurring outside of the box would have no GPS corrections done to
C    the data.  One failure mode of the present code is that, when the
C    GPS is not working, it outputs 0 degrees for lat/lon.  If one is
C    near the 0 meridian near the equator, the code could be fooled.
C
C   Revised by RLR on 931111 to narrow the threshold to +-1.0 degree.
C    The code can still be fooled in a small area in the Gulf of Guinea
C    off the west coast of Africa.
C
c
      logical first, trip, catch
      integer YES, TRIP_POINT
      parameter( LAST=-99, YES=1, NO=0, TRIP_POINT=111 )
      double precision dtime0, dtime, gp_dtime0
c....... arrays for filter values
      dimension zf(6,10)
      double precision h,det,hi
      double precision c,am,bm,cp
      dimension lwork(3),mwork(3) 
      dimension h(3,3),am(3,2),hi(3,3)
      dimension bm(3,2)
      dimension c(3,2),cp(3,2)
      integer flat
c
c....... initialize position
      data cdm/111120./
c
      data twopi/6.2831853/,upfctr/0.999444/,kountr/0/
      data tau/150./,taup/100./,fctrf/.997/
      data cktms,pi/.51444,3.14159265/
      data deg,rad/57.2957795128,.01745329252/ 
      data ncf/3/,kupd/0/
c
      data zf/60*0./
      data am/6*0./,bm/6*0./,h/9*0./
      data dlat, dlon / 2*0 /
      data ideltt, roll_max / 1000., 11. /
      data jj, kk, igps, flat, first, catch / 4*0, 2*.true. /
c
      save zf
      save h,hi,c,am,bm,cp
      save dlat,dlon, dtime0
      save old_alat, old_alon, old_glat, old_glon
      save twopi,upfctr,kountr,ncf,kupd
      save igps, fctrf, first, cdm, flat, jrej, irej, catch, keepc
      save taup, ideltt, factorp, roll_max
c
c
c
c
      if( first ) then
        first = .false.
        jrej = 0
        irej = 0
        call gp_fx_ini( dtime0, roll_max )    
        dtime0 = gp_dtime0()
        factorp=0.002*ideltt*pi/taup
        old_alat = alat
        old_alon = alon
        old_glat = glat
        old_glon = glon
c
        if( ifit_stuff() .eq. YES ) then
c   open the save file
          lu = lun_gp_rd()
          if( lu .lt. 1 ) then
            go to 11
          endif
          read(lu) zf, h, hi, c, cp, am, bm
          read(lu) igps, kupd, kountr
          read(lu) dvx, dvy, dlat, dlon, alatc, alonc, dtime0
c   close the save file
          lu = lun_gp_rd()
c
          call dfprint( 'h', h, 3*3 )
          call dfprint( 'hi', hi, 3*3 )
          call dfprint( 'c', c, 3*2 )
c    
          write(*,*) 'Initializing'
          write(*,*) '  kountr=', kountr
          write(*,*) '  kupd=', kupd
          write(*,*) '  igps=', igps
          write(*,*) '  dtime0=', dtime0
          write(*,*) '  dtime=', dtime
          write(*,*) '  dvx=', dvx
          write(*,*) '  dvy=', dvy
          write(*,*) '  dlat=', dlat
          write(*,*) '  dlon=', dlon
          write(*,*) '  alatc=', alatc
          write(*,*) '  alonc=', alonc
        endif
c
      elseif( icount .eq. LAST ) then
c   open the save file
        lu = lun_gp_wt()
        write(lu) zf, h, hi, c, cp, am, bm
        write(lu) igps, kupd, kountr
        write(lu) dvx, dvy, dlat, dlon, alatc, alonc, dtime0
        lu = lun_gp_wt()
c   close the save file
        call dfprint( 'h', h, 3*3 )
        call dfprint( 'hi', hi, 3*3 )
        call dfprint( 'c', c, 3*2 )
c
        write(*,*) 'Terminating'
        write(*,*) '  kountr=', kountr
        write(*,*) '  kupd=', kupd
        write(*,*) '  igps=', igps
        write(*,*) '  dtime0=', dtime0
        write(*,*) '  dtime=', dtime
        write(*,*) '  dvx=', dvx
        write(*,*) '  dvy=', dvy
        write(*,*) '  dlat=', dlat
        write(*,*) '  dlon=', dlon
        write(*,*) '  alatc=', alatc
        write(*,*) '  alonc=', alonc
        write(*,*) '  irej=', irej
        write(*,*) '  keepc=', keepc
        write(*,*) '  jrej=', jrej
        go to 90
      endif
c
c
  11  continue
      time = dtime -dtime0
      dx = glat - old_glat
      dy = glon - old_glon
      dgps = dx*dx + dy*dy
      diff = abs(dgps - dins)
      old_glat = glat
      old_glon = glon
c
      keepgps = igps
      igps=igps+1
c
c....... update corrections to gps if available
      if(gstat.gt.0.001) then
        igps=0
      endif
C      if(abs(glat-30.).gt.60.) igps=0
C      if(abs(glon+125.).gt.100.) igps=0
C
C....... Code for revision 931006
C      if (abs(glat-alat).gt.20.) igps=0
C      difflon = abs(glon-alon)
CC........  Handle cases which cross the International Date Line.
C      if (difflon.gt.20. .and. difflon.lt.340.) igps=0
C
C....... Code for revision 931111
      if (abs(glat-alat).gt.1.) igps=0
      difflon = abs(glon-alon)
C........  Handle cases which cross the International Date Line.
      if (difflon.gt.1. .and. difflon.lt.359.) igps=0
C
c....... data appear bad in turns, so skip:
      if( abs(roll) .gt. roll_max ) igps=0
c
      if( dgps .eq. 0 ) then
c   gps has gone flat
        flat = flat +1
        if( flat .gt. 2 ) then
          igps = 0
        endif
      else
        flat = 0
      endif
c
c   oye's diagnostic stuff
c      if( igps .eq. 0 .and. keepgps .ne. 0 ) then
c        igps = 0
c      endif
      if( mod( kk, TRIP_POINT ) .eq. 0  ) then
        trip = .true.
      else
        trip = .false.
      endif
c
      if( igps .eq. 0 ) then
        jrej = jrej +1
      else
        jrej = 0
      endif
      kk = kk +1
      if( catch  ) then
        if( igps .ne. 0 .and. irej .ge. 10 ) then
          catch = .false.
        elseif( igps .eq. 0 ) then
          irej = irej +1
          keepc = kk
        endif
      endif
c
c
      if (igps.lt.10) then
c
        if (kountr.gt.1800) then
c
          if (kupd.eq.1) then
            kupd=0
c....... invert information matrix to obtain error matrix 
            h(2,1)=h(1,2)
            h(3,1)=h(1,3)
            h(3,2)=h(2,3)
            do 545 i=1,3
              do 545 j=1,3
                hi(i,j)=h(i,j)
  545       continue
            call invrt(hi,ncf,det,lwork,mwork) 
c 
c....... beware of matrix that cannot be inverted 
            if(det.eq.0.) then
              write(*,*) 'det=0, count=', kk
              kountr=0
              go to 546
            endif
c
c....... now multiply times am to get matrix of coefficients 
            do 34 k=1,2
              do 35 i=1,ncf 
                c(i,k)=0.
                cp(i,k)=0.
                do 36 j=1,ncf 
                  c(i,k)=c(i,k)+hi(i,j)*am(j,k)
                  cp(i,k)=cp(i,k)+hi(i,j)*bm(j,k)
   36           continue
   35         continue
   34       continue
          endif
c
          omegat=twopi*time/5040.
          sinwt=sin(omegat)
          coswt=cos(omegat)
c
c....... want to avoid sharp transition to fit because that would
c..   produce spike affecting power spectra.   instead, slowly
c..   adjust from last value toward fit value, time constant of
c..   about 5 min chosen to be small compared to schuller period
c..   but large compared to wavelengths important in spectral
c..   analysis
c
          dvy=dvy*fctrf+(1-fctrf)*(c(1,1)+c(2,1)*sinwt+c(3,1)*coswt)
          dvx=dvx*fctrf+(1-fctrf)*(c(1,2)+c(2,2)*sinwt+c(3,2)*coswt)
          dlat=dlat*fctrf +(1-fctrf)*(cp(1,1)+cp(2,1)*sinwt+cp(3,1)*coswt)
          dlon=dlon*fctrf +(1-fctrf)*(cp(1,2)+cp(2,2)*sinwt+cp(3,2)*coswt)
c
c....... perhaps should apply slower decay factor to avoid extrapolation over
c..   very long times.  (order of hours)  not done yet.  same, position?
c
  546     continue
c
        else
c........... no good fit accumulated yet, so just let last correction
c..       factors slowly decay
          dvy=dvy*fctrf
          dvx=dvx*fctrf
          dlat=dlat*fctrf
          dlon=dlon*fctrf
        endif
c
c
c........... good gps comes here
c
      else
c
c................... low-pass and high-pass filters:
        gvnsf=filter(gvns,zf(1,1))
        gvewf=filter(gvew,zf(1,2))
        vnsf=filter(vns,zf(1,3))
        vewf=filter(vew,zf(1,4))
        dvy=gvnsf-vnsf
        dvx=gvewf-vewf
        dlat=glat-alat
        dlon=glon-alon
c.......... also update best-fit to coefficients to use to
c..      extrapolate when gps not valid:
c....... "time" is the number of seconds from start of the day
        omegat=twopi*time/5040.
        sinwt=sin(omegat)
        coswt=cos(omegat)
c....... update these
        am(1,1)=upfctr*am(1,1)+dvy
        am(2,1)=upfctr*am(2,1)+dvy*sinwt
        am(3,1)=upfctr*am(3,1)+dvy*coswt
        am(1,2)=upfctr*am(1,2)+dvx
        am(2,2)=upfctr*am(2,2)+dvx*sinwt
        am(3,2)=upfctr*am(3,2)+dvx*coswt
        bm(1,1)=upfctr*bm(1,1)+dlat
        bm(2,1)=upfctr*bm(2,1)+dlat*sinwt
        bm(3,1)=upfctr*bm(3,1)+dlat*coswt
        bm(1,2)=upfctr*bm(1,2)+dlon
        bm(2,2)=upfctr*bm(2,2)+dlon*sinwt
        bm(3,2)=upfctr*bm(3,2)+dlon*coswt
c....... note, only one information matrix needed: independent
c..   of dx,dy
        h(1,1)=upfctr*h(1,1)+1.
        h(1,2)=upfctr*h(1,2)+sinwt
        h(1,3)=upfctr*h(1,3)+coswt
        h(2,2)=upfctr*h(2,2)+sinwt*sinwt
        h(2,3)=upfctr*h(2,3)+sinwt*coswt
        h(3,3)=upfctr*h(3,3)+coswt*coswt
c....... flag that matrix updated:
        kupd=1
        kountr=kountr+1
      endif
c
c....... all values come here.  apply correction factors, determined
c..   in various ways above
      vnsc=vns+dvy
      vewc=vew+dvx
c....... to smooth, use vns and vew to calculate change, and
c   update these to measured lat-long to get smooth track.
c....... (take care of gross and start-up errors:)
      if(abs(alonc-alon).gt..5 .or. abs(alatc-alat).gt..5) then
        alonc=alon
        alatc=alat
      endif
      alonc=alonc+vewc/(cdm*cos(alat*rad))
      alatc=alatc+vnsc/cdm
      alonc=alonc+factorp*(alon+dlon-alonc)
      alatc=alatc+factorp*(alat+dlat-alatc)
c
      if(ui.eq.0..and.vi.eq.0.) vi = -0.0001  
c....... correct wind measurements for gps update
      vxg=ui+vewc-vew
      vyg=vi+vnsc-vns
      wdc=atan2(vxg,vyg)*deg+180.
c     if(wdc.lt.0.) wdc=wdc+360.
      wsc=sqrt(vxg*vxg+vyg*vyg)
c
  90  continue
      return
      end

c-----------------------------------------------------------------------------
      subroutine new_winds( xuc, xvc, xwsc, xwdc, xuxc, xvyc, thf )
c   recalculate the winds
      parameter( HIGH_RATE=20 )
      real xuc(*), xvc(*), xwsc(*), xwdc(*), xuxc(*), xvyc(*)
      real thf(*)
        data deg /57.2957795128/
c
c
      do 11 i = 1, HIGH_RATE
          xwsc(i) = sqrt( xuc(i)*xuc(i) + xvc(i)*xvc(i))
  11  continue
c
      do 22 i = 1, HIGH_RATE
          if( xuc(i) .eq. 0 .and. xvc(i) .eq. 0 ) then
            xvc(i) = .0001
          endif
          xwdc(i) = atan2( xuc(i), xvc(i) )*deg + 180.
  22  continue
c
c
      do 33 i = 1, HIGH_RATE
          sinhd = sin(thf(i))
          coshd = cos(thf(i))
          xuxc(i) =  xuc(i)*sinhd + xvc(i)*coshd
          xvyc(i) = -xuc(i)*coshd + xvc(i)*sinhd
  33  continue
c
      return
      end
c-----------------------------------------------------------------------------
      subroutine ffprint( s, d, n )
      character s*(*)
      real d(*)
c
      write(*,*) ' ', s
      do 11 i = 1, n
          write(*, 789 ) i, d(i)
  11  continue
      return
 789  format( ' ', i2, ') ', e16.9 )
      end
c-----------------------------------------------------------------------------
      subroutine dfprint( s, d, n )
      character s*(*)
      double precision d(*)
c
      write(*,*) ' ', s
      do 11 i = 1, n
          write(*, 789 ) i, d(i)
  11  continue
      return
 789  format( ' ', i2, ') ', e16.9 )
      end
c-----------------------------------------------------------------------------
      integer function lun_gp_rd(ok)
      parameter( lunit=13 )
      parameter( YES=1, NO=0 )
      data isopen / NO /
      save isopen
c
      if( isopen .eq. YES ) then
          close( lunit )
          isopen = NO
      else
          ierr = YES
          open( unit=lunit, form='UNFORMATTED', err=22 )
          ierr = NO
  22      continue
          if( ierr .eq. YES ) then
            write(*,*) ' Unable to open logical unit ', lunit
            lun_gp_rd = -1
            return
          endif
          isopen = YES
      endif
      lun_gp_rd = lunit
c
      return
      end
c---------------------------------------------------------------------------
      integer function lun_gp_wt()
      parameter( lunit=14 )
      parameter( YES=1, NO=0 )
      data isopen / NO /
      save isopen
c
      if( isopen .eq. YES ) then
          close( lunit )
          isopen = NO
      else
          open( unit=lunit, form='UNFORMATTED' )
          isopen = YES
      endif
      lun_gp_wt = lunit
c
      return
      end
c-----------------------------------------------------------------------------
