&generate(0, 0);
&generate(0, 1);
&generate(1, 0);
&generate(1, 1);

sub generate {
	my($debug, $uninst) = @_;
	my($file_name) = ($uninst ? "uninstall" : "install") . ($debug ? "-debug" : "") . ".bat";
	my($dir) = $debug ? "Debug" : "Release";
	my($opt) = $uninst ? "-u" : "";
	
	open(FILE, ">../$file_name") || die;
	
	print FILE <<EOT;
\@echo Note: this script must be run as administrator.
"%OPEN_NI_BIN%\\niReg" ${opt} %0\\..\\bin\\${dir}\\kinect-mssdk-openni-bridge.dll && \@echo OK
pause
EOT

	close(FILE);
}

