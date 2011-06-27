&generate("install", "", ["Release"]);
&generate("install-debug", "", ["Debug"]);
&generate("uninstall", "-u", ["Release", "Debug"]);

sub generate {
	my($file_name, $opt, $dirs) = @_;
	$file_name .= ".bat";
	
	open(FILE, ">../$file_name") || die;
	
	for $dir (@$dirs) {
		print FILE <<EOT;
\@echo ----------
"%OPEN_NI_BIN%\\niReg" ${opt} %0\\..\\bin\\${dir}\\kinect-mssdk-openni-bridge.dll
\@IF NOT "%ERRORLEVEL%" == "0" GOTO ERR
\@echo OK!
EOT
	}
	
	print FILE <<EOT;
\@GOTO END
:ERR
\@echo ----------
\@echo [en] Error occured. If you see "Failed to write to the file!" message, try again "as Administrator" by right-clicking the batch file.
\@echo [ja] エラーが発生しました。"Failed to write to the file!" というメッセージが出ている場合、バッチファイルを右クリックして「管理者として実行」してください。
:END
\@echo ----------
\@pause
EOT
	
	close(FILE);
}

