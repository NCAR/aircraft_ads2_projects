#!/usr/bin/perl -TI../
#
# C130_status.pl
#
# $Id$
#
# Generates NCAR C-130Q Instrument Status Report
#
# April 2002 -Steve Roberts
#   Revision  1 by Ron Ruth (Redo for C-130Q)                      22 Oct 2003
#     - customize directories
#     - customize instrument list
#     - create two copies of output
#       (one generic, one with time-stamped file name)
#
#  First insertion point  -- lines 394-398 (before modification)
#  Second insertion point -- lines 588-592 (before modification)
#
use CGI;

#require "config.pl";
#require "util.pl";

#------------------------
# Configurable parameters
#------------------------
# Notes for Ron Ruth:
#  Search for the text "BEGIN FORM INPUT" for where the input html starts.
#   You will need to customize this to your needs.
#  Search for the text "BEGIN FORM OUTPUT" for where the output html starts.
#   Make sure this matches the input html section.
#
# These were defined in "config.pl" as follows.
$DOCROOT = "/jnet/data/migs/ruth";
$REAL_DOCDIR = "/net/www/docs/raf/Field/RICO";
$DOCDIR = "/net/www/docs/raf/Projects/RICO";
#  Password must be <= 8 characters
$PASSWORD = "RICOupd";
$PLATFORM = "NCAR_C-130Q";
$FORM_NAME = "status";

# JOSS footer had contact info etc. Put whatever you need here.
$FOOTER = " </body>\n</html>";
$FILE_TEMPLATE  = "$FORM_NAME.$PLATFORM.\@TIME\@.html";
$FILE_TEMPLATE2 = "$FORM_NAME.$PLATFORM.html";
# Uncomment this line if you want the form to remember the last user entries
$STATE_FILE = "$REAL_DOCDIR/state/$PLATFORM/$FORM_NAME.state";

$GREEN = "#00ff00";
$YELLOW = "#ffff00";
$RED = "#ff0000";
#$BLUE = "#0000ff";
$BLUE = "#99CCFF";
#----------------------------
# End Configurable parameters
#----------------------------
$|=1;
$ENV{'PATH'} = '';
umask 002;
($PROG = $0) =~ s%.*/%%;

$FORM = new CGI();

($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = gmtime(time);
#$gtime = sprintf("%04d%02.2d%02.2d",$year+1900,$mon+1,$mday);
$gtime = sprintf("%04d%02.2d%02.2d%02.2d%02.2d",$year+1900,$mon+1,$mday,$hour,$min);
$gyear =sprintf("%02d",$year+1900);
$gmon =sprintf("%02.2d",$mon+1);
$gday =sprintf("%02.2d",$mday);
$ghour =sprintf("%02.2d",$hour);
$gmin =sprintf("%02.2d",$min);
$gdate = substr $gtime,0,8;

if($FORM->param("back")) {
  #print $FORM->header();
  #print $FORM->start_html(-title=>"Cancel: $PROJECT OPERATIONS REPORT");
  #print $FORM->h3("Report Canceled");
  #print $FORM->end_html();
#  print_form();
} elsif($FORM->param("preview")) {
  print_preview();
  $FORM->delete("password");
  #Edit Previous report
  if($FORM->param("edit_file")) {
    #do nothing
  #New report
  } elsif($STATE_FILE) {
    if(-e $STATE_FILE) {
      $oldfile = "$STATE_FILE.".$time;
      system("/bin/cp $STATE_FILE $oldfile");
    }
    $FORM->delete("edit_file");
    $FORM->delete("submit_time");
    open(TMP,"> $STATE_FILE") || die "<!-- died 1 - $! -->\n";
    $FORM->save(TMP);
    close TMP;
    chmod 0666,$STATE_FILE;
#    print "<!-- test point 4 state_file created: $STATE_FILE -->\n";
  }
} elsif($FORM->param("submit")) {
  submit_form();
} elsif($FORM->param("clear")) {
  $FORM->delete_all();
  print_form();
} else {
  $edit_msg = "";
  if($edit_file = $FORM->param("edit_file")) {
    $edit_msg = "<font color=red size=+1>Editing document $edit_file</font><p>";
    $FORM = &read_meta($FORM,"$DOCROOT$edit_file");
    if(!$FORM->param("edit_script")) {
      print $FORM->header();
      print $FORM->start_html(-title=>"ERROR: File uneditable",
                          -BGCOLOR=>'#ffffff',
                          -LINK=>'#0000ff',
                          -VLINK=>'#ff0000',
                          -ALINK=>'#ff0000',
                          -TEXT=>'#000000'
                         );
      print "ERROR: File uneditable\n";
      print $FORM->end_html();
      exit;
    }

    $eyear = $FORM->param('yyyy');
    $emonth = $FORM->param('mm');
    $eday = $FORM->param('dd');
    $ehour = $FORM->param('hh');
    $emin = $FORM->param('nn');
    @YEARS = ($eyear ne "") ? ("$eyear"): ("missing");
    @MONTHS = ($emonth ne "") ? ("$emonth"): ("missing");
    @DAYS = ($eday ne "") ? ("$eday"): ("missing");
    @HOURS = ($ehour ne "") ? ("$ehour"): ("none");
    @MINS = ($emin ne "") ? ("$emin"): ("none");

  } elsif(-e "$STATE_FILE") {
    open(TMP,"$STATE_FILE") || die "<-- died 2 - $! -->\n";
    $FORM = new CGI(TMP);
    close TMP;
  }
  print_form();
}
exit;

#---------------------------------------------------------------------
#
#
#---------------------------------------------------------------------
sub submit_form {
  local($time,$date,$msg,$title);
  local($category,$platform,$name);

  $doc = &generate_html($FORM);

  #Write report to directory based on $title
  ($category,$platform,$time,$name) = split /\./,$title;
  $date = substr $time,0,8;
  $file_dir = "$REAL_DOCDIR/$category/$platform";
  #$url_dir = "$DOCDIR/$category/$platform/$date";
  $file = "$file_dir/$title";
#-- system("/bin/mkdir -p $file_dir");
#-- chmod 0775, "$file_dir";
  print $FORM->header();

  if(!-d "$file_dir") {
    print $FORM->start_html(-title=>"ERROR: Directory not found",
                          -BGCOLOR=>'#ffffff',
                          -LINK=>'#0000ff',
                          -VLINK=>'#ff0000',
                          -ALINK=>'#ff0000',
                          -TEXT=>'#000000'
                         );
    print "Directory not found: $file_dir\n";
    print $FORM->end_html();
    exit;
  }
  if(-e "$file" && $FORM->param("submit") eq "Submit Report") {
    print $FORM->start_html(-title=>"File $file already exists",
                          -BGCOLOR=>'#ffffff',
                          -LINK=>'#0000ff',
                          -VLINK=>'#ff0000',
                          -ALINK=>'#ff0000',
                          -TEXT=>'#000000'
                         );
    print "<center>\n";
    print $FORM->h3("File $file already exists.");
    print $FORM->startform();
    print "<b>Overwrite? </b>";
    print $FORM->submit(-name=>"submit",-value=>"Yes");
    print " ";
    print '<input type="button" name="back" value="Back to Preview" OnClick="history.go(-1);return true;">';

    foreach $param ($FORM->param()) {
      print $FORM->hidden(-name=>"$param"),"\n";
    }

    print $FORM->endform();
    print "</center>\n";
    print $FOOTER;
  } else {
    if(-e "$file") {
     #($oldfile = $file) =~ s/.*\///;
     #$oldfile = "$OLDDIR/$oldfile." . time;
     $oldfile = "$file." . time;
     system("/bin/cp -fp $file $oldfile");
    }
    if(open (OUTFILE,"> $file")) {
      $FORM->delete("password");
      $FORM->delete("submit");
      $FORM->delete("edit_file");
      $FORM->delete("rev_time");
      $FORM->param(-name=>"edit_script",-value=>"$PROG");
      foreach $param ($FORM->param()) {
        $meta{$param} = $FORM->escapeHTML($FORM->param($param));
      }
      print OUTFILE $FORM->start_html(-title=>"$title",
                          -meta=>{%meta},
                          -BGCOLOR=>'#ffffff',
                          -LINK=>'#0000ff',
                          -VLINK=>'#ff0000',
                          -ALINK=>'#ff0000',
                          -TEXT=>'#000000'
                         );
      print OUTFILE $doc;
      #$edit_link = "$CGIDIR/forms/$PROG?edit_file=$url_dir/$title";
      #$EDIT_FOOTER =~ s/\@EDIT_LINK\@/$edit_link/;
      #print OUTFILE $EDIT_FOOTER;
      print OUTFILE $FORM->end_html();
      close OUTFILE;
      chmod 0664,$file;
      $title2 = $FILE_TEMPLATE2;
      $file2 = "$file_dir/$title2"; 
      system("/bin/cp $file $file2");

      print $FORM->start_html(-title=>"Submit Successful:$title",
                          -BGCOLOR=>'#ffffff',
                          -LINK=>'#0000ff',
                          -VLINK=>'#ff0000',
                          -ALINK=>'#ff0000',
                          -TEXT=>'#000000'
                         );
      print "<center>\n";
      print $FORM->h3("Submit Successful");
      print "</center>\n";
      print $FOOTER;
    } else {
      print $FORM->start_html(-title=>"Submit failed",
                          -BGCOLOR=>'#ffffff',
                          -LINK=>'#0000ff',
                          -VLINK=>'#ff0000',
                          -ALINK=>'#ff0000',
                          -TEXT=>'#000000'
                         );
      print "<center>\n";
      print $FORM->h3("Submit Failed! Unable to write to $file: $!");
      print "</center>\n";
      print $FOOTER;
    }
  }
}

#---------------------------------------------------------------------
#
#
#---------------------------------------------------------------------
sub print_preview {
  local($password,$time,$date,$title);

  if(! $FORM->param("submit_time") ) {
    $FORM->param(-name=>"submit_time",-value=>"$gyear/$gmon/$gday $ghour:$gmin");
  }
  if($FORM->param('edit_file')) {
    $FORM->delete("rev_time");
    $FORM->param(-name=>"rev_time",-value=>"$gyear/$gmon/$gday $ghour:$gmin");
  }
  #
  # Begin Report Form
  #
  $doc = &generate_html($FORM);


  #
  # Print Review Page
  #
  print $FORM->header();
  print $FORM->start_html(-title=>"$title",
                          -BGCOLOR=>'#ffffff',
                          -LINK=>'#0000ff',
                          -VLINK=>'#ff0000',
                          -ALINK=>'#ff0000',
                          -TEXT=>'#000000'
                         );

#    print "<html>\n";
#    print "<title>Review: $PROJECT INSTRUMENT STATUS REPORT</title>\n";
#    print "<style type=\"text/css\">\n";
#    print "<!-- \n";
#    print "BODY {font-size: xx-large; font-style: italic;color: red}\n";
#    print "TABLE,TR,TD,B,P {font-size: 8pt; font-style: italic; color: red}\n";
#    print "--> \n";
#    print "</style>\n";
#    print "<body>\n";



  # Print Review label and submit/cancel buttons
  print "\n";
  print "<table bgcolor=\"#99CCFF\" width=100% border=1 cellpadding=5><tr><td>\n";

  print "<h1>Review:</h1>\n";
  print "<b><p>\n";
  print "Please review this report carefully before submitting it.\n";
  print $FORM->startform();
  print "\n";
  print $FORM->submit(-name=>"submit",-value=>"Submit Report");
  print "&nbsp;";
  print '<input type="button" name="back" value="Back to Form" OnClick="history.go(-1);return true;">';
  #print '<input type="submit" name="back" value="Back to Form">';
  print "\n";

  $FORM->delete("preview");
  foreach $param ($FORM->param()) {
    print $FORM->hidden(-name=>"$param"),"\n";
  }
  print $FORM->endform();
  print "</b></td></tr></table><br>\n";


  # Print The report
  print "$doc\n";

  print $FOOTER;

}

sub reset_count {
 $COUNT = 0;
 return '';
}

#vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
#vvvvvvvvvvvvvvvvvvvvvvvvvvv BEGIN FORM INPUT vvvvvvvvvvvvvvvvvvvvvvvv
#vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
#---------------------------------------------------------------------
#
# Print the Input Report form
#
#---------------------------------------------------------------------
sub print_form {
  print "Content-type: text/html\n\n";
  print <<EOF;

<HTML>
<HEAD>
<TITLE>$PROJECT NCAR C-130Q Instrument Status Report Form</TITLE>
</HEAD>
<BODY BGCOLOR="#99CCFF">
<H1>$PROJECT NCAR C-130Q  Instrument Status Report Form</H1>
<H2>For use by coordinators only please.</H2>
$edit_msg
<hr>
<form method="POST">
<nobr>
<b>Date of report(UTC):</b>
&nbsp;
  year: @{[$FORM->popup_menu(-name=>'yyyy',-default=>"$gyear",-value=>['2004','2005','2006'])]}
  month: @{[$FORM->popup_menu(-name=>'mm',-default=>"$gmon",-override=>1,-value=>['01','02','03','04','05','06','07','08','09','10','11','12'])]}
  day: @{[$FORM->popup_menu(-name=>'dd',-default=>"$gday",-override=>1,-value=>['01','02','03','04','05','06','07','08','09','10','11','12','13','14','15','16','17','18','19','20','21','22','23','24','25','26','27','28','29','30','31'])]}
  hour: @{[$FORM->popup_menu(-name=>'hh',-default=>"$ghour",-override=>1,-value=>['00','01','02','03','04','05','06','07','08','09','10','11','12','13','14','15','16','17','18','19','20','21','22','23'])]}
  min: @{[$FORM->popup_menu(-name=>'nn',-default=>"$gmin",-override=>1,-value=>['00','01','02','03','04','05','06','07','08','09','10','11','12','13','14','15','16','17','18','19','20','21','22','23','24','25','26','27','28','29','30','31','32','33','34','35','36','37','38','39','40','41','42','43','44','45','46','47','48','49','50','51','52','53','54','55','56','57','58','59'])]}
</nobr>

<br>
<br>
<b>Author of report:</b>
 @{[$FORM->textfield(-name=>'author',value=>'',-size=>'20',-maxlength=>'256')]}
&nbsp;
Password:
 @{[$FORM->password_field(-name=>'password',-default=>'',-override=>1,-size=>'8',-maxlength=>'8')]}
<hr>
<b>Remaining flight hours:</b>&nbsp;
 @{[$FORM->textfield(-name=>'hours',-value=>'',-size=>'20',-maxlength=>'80')]}
<hr>
<b>General Comments:</b>
<ul>
 @{[$FORM->textarea(-name=>'general',-rows=>'4', -cols=>'80')]}
</ul>
<hr>
@{[$FORM->hidden(-name=>'submit_time')]}
@{[$FORM->hidden(-name=>'edit_file',-value=>"$edit_file")]}

<h3>INSTRUMENTS/SYSTEMS STATUS</h3>

<table>
@{[status_input('20',' Airborne Data System (ADS)')]}
@{[status_input('21',' Aft User Display (Clyde)')]}
@{[status_input('22',' Aft User Display (Grendel)')]}
@{[status_input('23',' Aft User Display (Agave)')]}
@{[status_input('24',' Aft User Display (Nuvola)')]}
@{[status_input('25',' Flight Deck Display (Bonnie)')]}
@{[status_input('26',' Honeywell HG1095-AC03 (IRS)')]}
@{[status_input('27',' Garmin GPS16 (GPS) - Research')]}
@{[status_input('28',' CMIGETS (INS)')]}
@{[status_input('29',' Differential GPS')]}
@{[status_input('30',' Rosemount 1501 Static pressure (PSFDC)')]}
@{[status_input('31',' Ruska 7885 Static pressure (PSFC)')]}
@{[status_input('32',' Rosemount 1201F1 Cabin pressure (PCAB)')]}
@{[status_input('33',' Rosemount 1221F1VL Dynamic pressure (QCRC)')]}
@{[status_input('34',' Rosemount 1221F1VL Dynamic pressure (QCFC)')]}
@{[status_input('35',' Rosemount 1221F1VL Dynamic pressure (QCFRC)')]}
@{[status_input('36',' Rosemount temperature (ATRR)')]}
@{[status_input('37',' Rosemount temperature (ATRL)')]}
@{[status_input('38',' Rosemount deiced temperature (ATWH)')]}
@{[status_input('39',' Ophir temperature (OAT)')]}
@{[status_input('40',' Rosemount 1221F1VL Attack (ADIFR)')]}
@{[status_input('41',' Rosemount 1221F1VL Sideslip (BDIFR)')]}
@{[status_input('42',' General Eastern 1011B Dewpoint temperature (DPT)')]}
@{[status_input('43',' General Eastern 1011B Dewpoint temperature (DPB)')]}
@{[status_input('44',' SpectraSensors TDL (MRLH)')]}
@{[status_input('45',' CrossFlow Lyman-alpha (MRLA)')]}
@{[status_input('46',' Crossflow Lyman-alpha (MRLA1)')]}
@{[status_input('47',' Eppley PSP Pyranometer top (SWT)')]}
@{[status_input('48',' Eppley PSP Pyranometer bottom (SWB)')]}
@{[status_input('49',' Eppley TUVR Ultraviolet top (UVT)')]}
@{[status_input('50',' Eppley TUVR Ultraviolet bottom (UVB)')]}
@{[status_input('51',' Eppley PIR Pyrgeometer top (IRTC)')]}
@{[status_input('52',' Eppley PIR Pyrgeometer bottom (IRBC)')]}
@{[status_input('53',' Heiman KT19.85 Remote surface temperature (RSTB)')]}
@{[status_input('54',' Heiman KT19.85 Remote surface temperature (RSTB1)')]}
@{[status_input('55',' Heiman KT19.85 Remote sky/cloud temperature (RSTT)')]}
@{[status_input('56',' Rosemount 871FA Icing rate (RICE)')]}
@{[status_input('57',' PMS/King LWC (PLWCC)')]}
@{[status_input('58',' PMS/King LWC (PLWCC1)')]}
@{[status_input('61',' TSI 3760 CN Counter (CONCN)')]}
@{[status_input('62',' PMS SPP-200')]}
@{[status_input('63',' PMS SPP-100')]}
@{[status_input('64',' PMS 2D-C')]}
@{[status_input('65',' PMS 2D-P')]}
@{[status_input('66',' PMS 260X')]}
@{[status_input('67',' Gerber PVM-100 LWP (XGLWC)')]}
@{[status_input('68',' Radar altimeter (HGM232)')]}
@{[status_input('69',' VHS Video cameras (forward, down)')]}
@{[status_input('70',' Digital Video camera (forward)')]}
@{[status_input('71',' TECO Ozone Analyzer (TEO3C)')]}
@{[status_input('72',' RAF Fast-Response Ozone Analyzer (O3FSC)')]}
@{[status_input('73',' Scanning Aerosol Backscatter Lidar (SABL)')]}
@{[status_input('74',' TD GPS Dropsonde System')]}
@{[status_input('75',' Radial Differential Mobility Analyzer (RDMA)')]}
@{[status_input('76',' Wet/Dry Nephelometers')]}
@{[status_input('77',' RAF Counter-flow Virtual Impactor (CVI)')]}
@{[status_input('78',' CSIRO/RAF Giant Aerosol Impactor')]}
@{[status_input('79',' PH Meter')]}
@{[status_input('80',' CSIRO Wet/Dry Bulb Temperature')]}
@{[status_input('82',' Anderson Filter Pack')]}
@{[status_input('83',' Bandy SO2')]}
@{[status_input('84',' Bandy DMS')]}
@{[status_input('85',' Brenguier Fast FSSP')]}
@{[status_input('86',' CIRPAS Phased Doppler')]}
@{[status_input('87',' Gerber Cloud-Integrating Nephelometer')]}
@{[status_input('88',' U Hawaii Total Air Sampler')]}
@{[status_input('89',' Heikes In-Situ Peroxides')]}
@{[status_input('90',' Hudson CCN')]}
@{[status_input('91',' SPEC 2D-S Dual Particle Imager')]}
@{[status_input('92',' SPEC Cloud Particle Imager (CPI)')]}
</table>
<hr>


<!-- ********************************************************** -->
<p>
<table bgcolor="#99CCFF" width=100% border=1>
 <tr>
  <td>
   <br>&nbsp;
   <input type=submit name="preview" value="Preview form">
   <!-- <input type=reset value="clear and reset form"> -->
   <input type=submit name="clear" value="clear and reset form">
   <br>&nbsp;
 </td>
</tr>
</table>
<br>
</form>
$FOOTER
EOF
}

#---------------------------------------------------------------------
#
#
#---------------------------------------------------------------------
sub status_input {
 local($num,$system) = @_;
 local($menu_name) = "s$num";
 local($text_name) = "t$num";
 $COUNT++;
# $SELECT_NAME = "STATUS_OK$COUNT";
# $INPUT_NAME = "STATUS_TEXT$COUNT";
 return <<EOSTATUS;
 <tr>
  <td>
   $COUNT.
  </td>
  <td>
   @{[$FORM->popup_menu(-name=>"$menu_name",-values=>['up','provisional', 'down','no report'],-default=>'no report')]}
  </td>
  <td>
   <b>$system: </b>
  </td>
  <td>
   @{[$FORM->textfield(-name=>"$text_name",-size=>'55',-maxlength=>'512')]}
  </td>
 </tr>
EOSTATUS

}
sub category {
 local($text) = @_;
 return <<EOFCATEGORY;
 <tr bgcolor=black>
  <td colspan=4>
   <font color=white size=+1><b>&nbsp;
    $text
   </b></font>
  </td></tr>
EOFCATEGORY
}

#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#^^^^^^^^^^^^^^^^^^^^^^^^^ END FORM INPUT ^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


#vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
#vvvvvvvvvvvvvvvvvvvvvvvvvvv BEGIN FORM OUTPUT vvvvvvvvvvvvvvvvvvvvvvv
#vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
#---------------------------------------------------------------------
#
# Generate the final HTML report document using the users answers from
# 'print_form'
#
#---------------------------------------------------------------------
sub generate_html {
  local($FORM) = @_;
  #local($yyyy,$mm,$dd,$hh,$nn);
  if ($FORM->param('password') ne $PASSWORD) {
    print $FORM->header();
    print $FORM->start_html(-title=>"Invalid Password");
    print $FORM->h1("Sorry - Invalid Password");
    print $FORM->end_html();
    exit;
  }
  if($FORM->param('author') !~ /\w+/) {
    print $FORM->header();
    print $FORM->start_html(-title=>"No author provided");
    print $FORM->h1("Sorry - No Author provided. Please fill in Author field");
    print $FORM->end_html();
    exit;
  }
  if($FORM->param("yyyy") =~ /^(20[0-9][0-9])/) {
    $yyyy = $1;
  } else {
    print $FORM->header();
    print $FORM->start_html(-title=>"Invalid Year value");
    print $FORM->h1("Sorry - Invalid Year value");
    print $FORM->end_html();
    exit;
  }
  if($FORM->param("mm") =~ /^(\d\d)/) {
    $mm = $1;
  } else {
    print $FORM->header();
    print $FORM->start_html(-title=>"Invalid month value");
    print $FORM->h1("Sorry - Invalid month value");
    print $FORM->end_html();
    exit;
  }
  if($FORM->param("dd") =~ /^(\d\d)/) {
    $dd = $1;
  } else {
    print $FORM->header();
    print $FORM->start_html(-title=>"Invalid day value");
    print $FORM->h1("Sorry - Invalid day value");
    print $FORM->end_html();
    exit;
  }
  if($FORM->param("hh") =~ /^(\d\d)/) {
    $hh = $1;
  } else {
    $hh = '';
  }
  if($FORM->param("nn") =~ /^(\d\d)/) {
    $nn = $1;
  } else {
    $nn = '';
  }
  $time = "${yyyy}${mm}${dd}";
  if($hh =~ /\d\d/) {
    $time = "${time}${hh}${nn}";
  }
  $stime = "$yyyy/$mm/$dd $hh:$nn";
  $stime =~ s/\s:\s*$//;

  $submit_time = $FORM->param("submit_time");
  $rev_time = $FORM->param("rev_time");
  $srev_time = "<br><b>Revised at(UTC):</b> $rev_time" if($rev_time);

  ($title = $FILE_TEMPLATE) =~ s/\@TIME\@/$time/;

  local($pre,$pre_end) = ('','');
  if ($FORM->param('pref') eq "yes") {
    ($pre,$pre_end) = ('<pre>','</pre>');
  }

#
# Begin Report Form
#
    return <<EOOUTPUT

<h1>$PROJECT NCAR C-130Q Instrument Status Report</h1>
<hr>
<b>Date of report(UTC):</b>&nbsp;&nbsp;
 $stime
<br>
<b>Author of report:</b>&nbsp;&nbsp;
 @{[$FORM->param(-name=>'author')]}
<br>
<b>Submitted at(UTC):</b>&nbsp;&nbsp;
 $submit_time
 $srev_time
<hr>
<b>Remaining flight hours:</b>&nbsp;&nbsp;
 @{[$FORM->param(-name=>'hours')]}
<hr>
<b>General Comments:</b>
<ul>
 @{[$FORM->param(-name=>'general')]}
</ul>
<hr>
<h3>INSTRUMENTS/SYSTEMS STATUS</h3>
<dl>
 <dd>
 <table border=0>
  <tr>
   <td bgcolor="$GREEN">&nbsp;&nbsp;&nbsp;<td> = up;&nbsp;&nbsp;</td>
   <td bgcolor="$YELLOW">&nbsp;&nbsp;&nbsp;<td> = provisional; &nbsp;&nbsp;</td>
   <td bgcolor="$RED">&nbsp;&nbsp;&nbsp;<td> = down ;&nbsp;&nbsp;</td>
   <td bgcolor="$BLUE">&nbsp;&nbsp;&nbsp;<td> = no report</td>
  </tr>
 </table>
 </dd>
 <br>
 <table>
@{[status_output('20',' Airborne Data System (ADS)')]}
@{[status_output('21',' Aft User Display (Clyde)')]}
@{[status_output('22',' Aft User Display (Grendel)')]}
@{[status_output('23',' Aft User Display (Agave)')]}
@{[status_output('24',' Aft User Display (Nuvola)')]}
@{[status_output('25',' Flight Deck Display (Bonnie)')]}
@{[status_output('26',' Honeywell HG1095-AC03 (IRS)')]}
@{[status_output('27',' Garmin GPS16 (GPS) - Research')]}
@{[status_output('28',' CMIGETS (INS)')]}
@{[status_output('29',' Differential GPS')]}
@{[status_output('30',' Rosemount 1501 Static pressure (PSFDC)')]}
@{[status_output('31',' Ruska 7885 Static pressure (PSFC)')]}
@{[status_output('32',' Rosemount 1201F1 Cabin pressure (PCAB)')]}
@{[status_output('33',' Rosemount 1221F1VL Dynamic pressure (QCRC)')]}
@{[status_output('34',' Rosemount 1221F1VL Dynamic pressure (QCFC)')]}
@{[status_output('35',' Rosemount 1221F1VL Dynamic pressure (QCFRC)')]}
@{[status_output('36',' Rosemount temperature (ATRR)')]}
@{[status_output('37',' Rosemount temperature (ATRL)')]}
@{[status_output('38',' Rosemount deiced temperature (ATWH)')]}
@{[status_output('39',' Ophir temperature (OAT)')]}
@{[status_output('40',' Rosemount 1221F1VL Attack (ADIFR)')]}
@{[status_output('41',' Rosemount 1221F1VL Sideslip (BDIFR)')]}
@{[status_output('42',' General Eastern 1011B Dewpoint temperature (DPT)')]}
@{[status_output('43',' General Eastern 1011B Dewpoint temperature (DPB)')]}
@{[status_output('44',' SpectraSensors TDL (MRLH)')]}
@{[status_output('45',' CrossFlow Lyman-alpha (MRLA)')]}
@{[status_output('46',' Crossflow Lyman-alpha (MRLA1)')]}
@{[status_output('47',' Eppley PSP Pyranometer top (SWT)')]}
@{[status_output('48',' Eppley PSP Pyranometer bottom (SWB)')]}
@{[status_output('49',' Eppley TUVR Ultraviolet top (UVT)')]}
@{[status_output('50',' Eppley TUVR Ultraviolet bottom (UVB)')]}
@{[status_output('51',' Eppley PIR Pyrgeometer top (IRTC)')]}
@{[status_output('52',' Eppley PIR Pyrgeometer bottom (IRBC)')]}
@{[status_output('53',' Heiman KT19.85 Remote surface temperature (RSTB)')]}
@{[status_output('54',' Heiman KT19.85 Remote surface temperature (RSTB1)')]}
@{[status_output('55',' Heiman KT19.85 Remote sky/cloud temperature (RSTT)')]}
@{[status_output('56',' Rosemount 871FA Icing rate (RICE)')]}
@{[status_output('57',' PMS/King LWC (PLWCC)')]}
@{[status_output('58',' PMS/King LWC (PLWCC1)')]}
@{[status_output('61',' TSI 3760 CN Counter (CONCN)')]}
@{[status_output('62',' PMS SPP-200')]}
@{[status_output('63',' PMS SPP-100')]}
@{[status_output('64',' PMS 2D-C')]}
@{[status_output('65',' PMS 2D-P')]}
@{[status_output('66',' PMS 260X')]}
@{[status_output('67',' Gerber PVM-100 LWP (XGLWC)')]}
@{[status_output('68',' Radar altimeter (HGM232)')]}
@{[status_output('69',' VHS Video cameras (forward, down)')]}
@{[status_output('70',' Digital Video camera (forward)')]}
@{[status_output('71',' TECO Ozone Analyzer (TEO3C)')]}
@{[status_output('72',' RAF Fast-Response Ozone Analyzer (O3FSC)')]}
@{[status_output('73',' Scanning Aerosol Backscatter Lidar (SABL)')]}
@{[status_output('74',' TD GPS Dropsonde System')]}
@{[status_output('75',' Radial Differential Mobility Analyzer (RDMA)')]}
@{[status_output('76',' Wet/Dry Nephelometers')]}
@{[status_output('77',' RAF Counter-flow Virtual Impactor (CVI)')]}
@{[status_output('78',' CSIRO/RAF Giant Aerosol Impactor')]}
@{[status_output('79',' PH Meter')]}
@{[status_output('80',' CSIRO Wet/Dry Bulb Temperature')]}
@{[status_output('82',' Anderson Filter Pack')]}
@{[status_output('83',' Bandy SO2')]}
@{[status_output('84',' Bandy DMS')]}
@{[status_output('85',' Brenguier Fast FSSP')]}
@{[status_output('86',' CIRPAS Phased Doppler')]}
@{[status_output('87',' Gerber Cloud-Integrating Nephelometer')]}
@{[status_output('88',' U Hawaii Total Air Sampler')]}
@{[status_output('89',' Heikes In-Situ Peroxides')]}
@{[status_output('90',' Hudson CCN')]}
@{[status_output('91',' SPEC 2D-S Dual Particle Imager')]}
@{[status_output('92',' SPEC Cloud Particle Imager (CPI)')]}
 </table>
</dl>

EOOUTPUT
#
# End Report Form
#
}

#---------------------------------------------------------------------
#
#
#---------------------------------------------------------------------
sub status_output {
 local($num,$system) = @_;
 local($menu_name) = "s$num";
 local($text_name) = "t$num";
 local($img,$bgcolor,$bgcolor_stat);
 $COUNT++;
# $SELECT_NAME = "STATUS_OK$COUNT";
# $INPUT_NAME = "STATUS_TEXT$COUNT";

 $bgcolor = "white";
 $bgcolor = "lightgrey" if($COUNT%2 == 1);

 if(($FORM->param(-name=>"$menu_name")) eq "up") {
   $bgcolor_stat = "$GREEN";
 } elsif(($FORM->param(-name=>"$menu_name")) eq "provisional") {
   $bgcolor_stat = "$YELLOW";
 } elsif(($FORM->param(-name=>"$menu_name")) eq "down") {
   $bgcolor_stat = "$RED";
 } else {
   $bgcolor_stat = "$BLUE";
 }
 return <<EOSTATUS;
  <tr bgcolor=$bgcolor>
   <td bgcolor=$bgcolor_stat>&nbsp;
    $COUNT.
   <td>
    <b>$system </b></td>
   <td>
    <b>Comment:</b> </td>
   <td>
    @{[$FORM->param(-name=>"$text_name")]}
    &nbsp;</td>
  </tr>
EOSTATUS

}
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#^^^^^^^^^^^^^^^^^^^^^^^^^ END FORM OUTPUT ^^^^^^^^^^^^^^^^^^^^^^^^^^^
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
