$BAM_FREAD1 = 0x40;
$BAM_FREAD2 = 0x80;
$BAM_UNMAPPED = 0x4;

while(<>){
    ($id,$flag,$chr) = split /\t/;
    next unless($chr=~/^chr/);
    $read = ($flag & $BAM_FREAD1) ? 1 : ($flag & $BAM_FREAD2 ? 2 : 0);
    $f{$read}{$id}+= ($flag & $BAM_UNMAPPED ? 0 : 1);
}

foreach $x(keys(%f)) {
    foreach $y(keys(%{$f{$x}})) {
	$n = $f{$x}{$y};
	$n=9 if($n>9);
	$count{$n}{$x}++;
    }
}

for($i=0;$i<10;$i++) {
    print "$i\t$count{$i}{'1'}\t$count{$i}{'2'}\n";
}



