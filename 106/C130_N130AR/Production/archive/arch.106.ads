#! /bin/csh -f
#
#  Archive stripped ADS SDI data files for 1998-106 WiFE  (Radke, Clark)
#
#  File adapted from the file /home/local/proj/101/Production/arch.101.ads
#    by Ron Ruth  30 September 1998
#
#   To find executable (non-comment) lines (from within `vi'), type:
#       /^[^#]
#
#  `vi' :map commands for annotating job completion:
#  :map g eea 
#  :map v AOK @ 
#
#
set YEAR = 1998
set PROJ = 106
#set SDIR = $TEMP
set SDIR = $DATA_DIR
#set SDIR = /home/staticdata
set TURBRT = ADS
#
# 1st wave
#set SFILES = ( \
#106ff01.ads \
#106rf01.ads \
#106rf03.ads \
#106rf04.ads \
#106rf05.ads \
#)
#set DFILES = ( \
#FF01 \
#RF01 \
#RF03 \
#RF04 \
#RF05 \
#)
#
# 2nd wave
#set SFILES = ( \
#106rf06.ads \
#106rf07.ads \
#)
#set DFILES = ( \
#RF06 \
#RF07 \
#)
#
# 3rd wave
#set SFILES = ( \
#106ff01.ads \
#106rf02.ads \
#106ff06.ads \
#)
#set DFILES = ( \
#FF02 \
#RF02 \
#FF06 \
#)
#
# 4th wave (using msput_job instead of MIGS)
#set SFILES = ( \
#106tf01.ads \
#106tf02.ads \
#)
#set DFILES = ( \
#TAPE/TF01 \
#TAPE/TF02 \
#)
#
#
#  Get started:
if ($#SFILES != $#DFILES) then
  echo " Number of source and destination file names don't match.  Quitting"
  exit 1
endif
#
set todo = $#SFILES
set num = 1
#        begin new code for 4th wave
set RPWD = ""
set msrcpMachine = "mistral"
echo "#  $#SFILES Job(s) submitted on `date`"
#         end of new code
while ($num <= $todo)
  set SFILE = $SFILES[$num]
  set DFILE = $DFILES[$num]
  if (-r $SDIR/$SFILE) then
#        begin original code for 1st through 3rd waves
#    echo \
#    nrnet msput $SDIR/$SFILE l df=tr r flnm=/RAF/$YEAR/$PROJ/$TURBRT/$DFILE \
#      rtpd=4095 name=RAF proj=41113009 w=RAFDMG
#        end code for 1st through 3rd waves
#
#        begin new code for 4th wave
    echo \
    rsh $msrcpMachine msput_job -pe 4095 -pr 41113009 -wpwd RAFDMG $RPWD \
     $SDIR/$SFILE mss:/RAF/$YEAR/$PROJ/$TURBRT/$DFILE
    if ($status == 0) then
      echo "#  msrcp job for $TURBRT/$DFILE -- OK -- `date`"
    else
      echo "  msrcp job FAILED.  See email message for details."
    endif
#         end of new code
  else
    echo " Cannot access file $SDIR/$SFILE"
  endif
  @ num ++
end
#
# Done.
echo "#   Completed on `date`"
#
exit
#
#
#  MSS path for all bitfiles listed below:  /RAF/1998/106/
#
# 1st wave
#  Job(s) submitted on Wed Sep 30 15:23:15 MDT 1998
# Sequence number = MI2557 -- ADS/FF01 -- OK @ 30 Sep 1998 15:30:53
# Note:  FF01 has been renumbered to FF02, since it accompanies RF02.
# (MSS bitfile removed from the MSS on 14 Oct 1998 11:34:08)
# Sequence number = MI2565 -- ADS/RF01 -- OK @ 30 Sep 1998 15:27:16
# Sequence number = MI2578 -- ADS/RF03 -- OK @ 30 Sep 1998 15:33:47
# Sequence number = MI2588 -- ADS/RF04 -- OK @ 30 Sep 1998 15:30:37
# Sequence number = MI2597 -- ADS/RF05 -- OK @ 30 Sep 1998 15:33:18
#
# 2nd wave
#  Job(s) submitted on Fri Oct  9 16:47:12 MDT 1998
# Sequence number = MI3289 -- ADS/RF06 -- OK @ 9 Oct 1998 16:52:11
# Sequence number = MI3296 -- ADS/RF07 -- OK @ 9 Oct 1998 16:52:48
#
# 3rd wave
#  Job(s) submitted on Mon Oct 12 10:29:06 MDT 1998
# Sequence number = MI4813 -- ADS/FF02 -- OK @ 12 Oct 1998 10:33:52
# Sequence number = MI4820 -- ADS/RF02 -- OK @ 12 Oct 1998 10:36:18
# Sequence number = MI4829 -- ADS/FF06 -- OK @ 12 Oct 1998 10:33:52
#
# 4th wave (using msput_job instead of MIGS)
#  2 Job(s) submitted on Thu Jun 26 13:59:36 MDT 2003
#  msrcp job for ADS/TAPE/TF01 -- OK -- Thu Jun 26 14:02:34 MDT 2003
#  msrcp job for ADS/TAPE/TF02 -- OK -- Thu Jun 26 14:05:38 MDT 2003
#   Completed on Thu Jun 26 14:05:38 MDT 2003
#
