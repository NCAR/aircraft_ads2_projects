#!/usr/bin/perl -T

# oxelson
# 04.05.02  
# status.cgi

$| = 1;
push(@INC, "/jnet/http/cgi-bin");    # push dir onto @INC

## secure path and get rid of risky environmental variables

$ENV{PATH} = "/bin:/usr/bin";
delete @ENV{'IFS', 'CDPATH', 'ENV', 'BASH_ENV'};

use strict;

#print "Content-type: text/plain\n\n";


my $date = `date`;
my ($file, @file);
$file = "/jnet/shared/proj/111/status_message";

open (FH, $file)||die "Can't open $file: $!\n";
@file = <FH>;
close(FH);


print <<"PFH";
Content-type: text/html

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<title>NCAR/RAF IDEAS Project 2002 Current Mission Status</title>
</head>
<body text="#000000" bgcolor="#FFFFFF" link="#0000FF" vlink="#800080" alink="#FF0000">
<center>
<h2>
<img src="/pics/med_c130fixa.jpg" ALT="C-130 photo (22,397 byte jpg)"  align=CENTER></h2></center>

<center>
<h2>
<font color="#0000BB">I</font>nstrument
<font color="#0000BB">D</font>evelopment
                      and
<font color="#0000BB">E</font>ducation in
<font color="#0000BB">A</font>irborne
<font color="#0000BB">S</font>cience</h2></center>
<center><h2>(<font color="#0000BB">IDEAS</font>)</h2></center>
<center><h2><font color="#BB0000">NCAR/RAF IDEAS Project 2002</font></h2></center>

<p>
<center><h2>
Current Mission Status
</h2></center>

</p>
<hr noshade>

<pre>
@file
</pre>

<hr noshade>
<a href="/Projects/IDEAS/oldstatus.html"><b><center>
Archived Mission Status Announcements</center></b></a>
<hr noshade>

<!--img alt="New!, 386 byte gif" src="/pics/new.png"-->

<center><b><i>
<a href="http://www.atd.ucar.edu/dir_off/projects/2002/IDEAS.html">IDEAS&nbsp;Announcement&nbsp;Page</a> |
<a href="/">RAF&nbsp;Home&nbsp;Page</a> |
<a href="http://www.atd.ucar.edu/">ATD&nbsp;Home&nbsp;Page</a> |
<a href="http://www.ncar.ucar.edu/">NCAR&nbsp;Home&nbsp;Page</a>
</i></b></center>

<hr noshade>

<font size="2">
Last update: $date
</font>

</body>
</html>
PFH
exit;
