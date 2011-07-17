use strict;
my(@files) = glob("*.bin");
my($file);
my($name);
for $file (@files) {
	($name = $file) =~ s/\.bin//;
	open(DATA, $file) || die;
	binmode DATA;
	read DATA, my($bytes), -s DATA;
	
	my($length) = length($bytes);
	my(@data) = split(//, $bytes);
	my(@chunk);
	while (@data) {
		push @chunk, "\"" . (join "", map { sprintf("\\x%02x", ord($_)) } splice(@data, 0, 4000)) . "\"";
	}
	close(DATA);
	
	open(OUT, ">$name.h") || die;
	print OUT "DEFINE_BINARY_PROPERTY($name, $length, @chunk);\n";
	close(OUT);
}

