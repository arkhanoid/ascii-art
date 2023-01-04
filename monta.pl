open F, "lista.txt" or die "$!";
unlink "o.txt";
foreach $j (<F>)
{
 chop($j);
 $g = $j; $b = $j;
 $g =~ s/jpg/gif/;
 $b =~ s/jpg/bmp/;
 `convert $j $g -gamma 1.5`;
 print "$j -> $g\n";
 `convert $g -resize 60x60 -colors 16 -flip -compress none $b`;
 unlink $g;
 print "$g -> $b\n";
 `bmterm $b 60 60 0 10 >> o.txt 2>>errors.log`;
 unlink $b;
}
close F;