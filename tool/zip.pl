use strict;
use Archive::Zip;
use File::Basename;

my($path) = shift @ARGV;
my($dir) = (fileparse($path))[0];

my($zip) = Archive::Zip->new();
$zip->addTree($path, $dir);
$zip->writeToFileNamed($path . ".zip");
