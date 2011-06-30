@files = glob("*.bin");
open(OUT, ">../PrimeSenseBinaryPropertyDef.h");
select(OUT);

for $file (@files) {
	($name = $file) =~ s/\.bin//;
	open(DATA, $file) || die;
	binmode DATA;
	read DATA, $bytes, -s DATA;
	
	$length = length($bytes);
	$values = join ", ", map { ord($_) } split(//, $bytes);
	
	print "DEFINE_BINARY_PROPERTY($name, $length, $values);\n";
	
	close(DATA);
}

