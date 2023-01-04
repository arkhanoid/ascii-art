#!/usr/bin/perl 
#open L, "<lista.txt" or die "frames?\n";
#@list = <L>; close L;
@list = ();
#foreach $l (`ls -1 *.png`) { push @list, $l if $k++ % 3 == 0 }
foreach $l (`DIR /B *.png`) { push @list, $l if $k++ % 3 == 0 }
$max_pics = @list;
$bgcolor  = '000000';
$fgcolor  = '88FF44';
#$delay    = 125; # fps = 24
$delay    = 100; # fps = 30
$top      = 10;
$left     = 10;
#$tmp      = "";

# use memory filesystem, for speed and disk saving...
#print `sudo  mount -t tmpfs -o size=100m tmpfs $tmp`;

open O, ">o.htm" or die "saida!\n";
open JS, "|jsmin > engine.js" or die "saida JS!\n";


#original HTML and JS code wirtten by Mic Barendsz

print O <<EOF;
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML><HEAD><TITLE>ascii-movie</TITLE>
<META http-equiv=Content-Type content="text/html; charset=windows-1252">

<STYLE type=text/css>PRE {
	FONT-SIZE: 10px; COLOR: #$fgcolor; FONT-FAMILY: "Courier New", Courier, mono; FONT-WEIGHT: Bold
}
</STYLE>
<SCRIPT language="JavaScript" type="text/javascript"> <!--
EOF
print JS <<EOF;
/***** initialize global film variables ******/

var     i=0;      // <---- actual picture position (frame counter)
var     p=1;      // <---- previous picture number
var     r=true;    // <---- boolean : true (play) or !true (pause)


/***** initialize style sheets ****/

document.write("<"+"style type='text/css'><"+"!-- ");
for (j=0;j<=$max_pics;j++)
 document.write("#frm"+j+"{ position:absolute; left:${left}px; top:${top}px; z-index:"+j+"; visibility:hidden}");
document.write("--"+"><"+"/style>");


/******* browser detect ******/

with(navigator) {
	Browser=appName;
	Version=parseInt(appVersion.charAt(0));
} //document.write(Browser+" "+Version+" : ");


/******* browser config ******/

if (document.layers) {
        var d = 'document.';
        var v = '.visibility';
//        var bsr = "Ns4";
}
else
if (document.all) {
        var d = 'document.all.';
        var v = '.style.visibility';
//        var bsr = "Ie4";
}
else
if (document.getElementById) {
        var d = 'document.getElementById("';
        var v = '").style.visibility';
//        var bsr = "Ns6";
}
else {
	document.write("<"+"h2><"+"font color='#888888'>This animation requires a Mozilla 4 Browser minimum!<"+"/font><"+"/h2>");
}

/******* animation module ******/

function s() {
        if(r) setTimeout("s()", $delay);
        if (++i>$max_pics) i=1;
        eval(d + 'frm' + p + v + ' = "hidden"');
        eval(d + 'frm' + i + v + ' = "visible"');
        p=i;
}
/*function MM_reloadPage(init) {  //reloads the window if Nav4 resized
  if (init==true) with (navigator) {if ((appName=="Netscape")&&(parseInt(appVersion)==4)) {
    document.MM_pgW=innerWidth; document.MM_pgH=innerHeight; onresize=MM_reloadPage; }}
  else if (innerWidth!=document.MM_pgW || innerHeight!=document.MM_pgH) location.reload();
}
MM_reloadPage(true);
*/
EOF

close JS;

open JS, "<engine.js" or die "reopen JS";
print O <JS>;
close JS;

print O <<EOF;
//-->
</script>
</HEAD>
<BODY bgColor=#$bgcolor onload=s();>
<EMBED SRC="audiodump.wav" LOOP="true" autostart="true" hidden="true"> </EMBED>
<!-- frames -->
</font>
EOF

#print `cp *.bmp $tmp`;
#print `cp  0*.png $tmp`;
$i = 1;

$status = 0;

foreach $j (@list) {
    print O "<DIV id=frm$i onClick=\"r=!r;s()\"><PRE>\n";
    $i++;
    chop($j);
    $b = $j;
    $b =~ s/png/bmp/;
    `convert $j -normalize -compress none $b`;

#    print "$j -> $b\n";
    printf "[%s]\r", "#" x ( $i * 76 / $max_pics ) .
    qw(| / - \\)[$i & 3]  .
    "." x ( 76 - $i * 76 / $max_pics );

    $rnd = int ( 10000 * rand );    

#    printf "Frame = %03d\n", $i;
    print O `asciiart.exe $b - $rnd 2>null`;

    # print O `cat test.txt`;
    unlink "$b";
    print O "</PRE></DIV>\n";
}
#print `sudo umount $tmp`;
print O "</body></html>";
close O;
