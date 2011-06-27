@echo ----------
"%OPEN_NI_BIN%\niReg"  %0\..\bin\Debug\kinect-mssdk-openni-bridge.dll
@IF NOT "%ERRORLEVEL%" == "0" GOTO ERR
@echo OK!
@GOTO END
:ERR
@echo ----------
@echo [en] Error occured. If you see "Failed to write to the file!" message, try again "as Administrator" by right-clicking the batch file.
@echo [ja] エラーが発生しました。"Failed to write to the file!" というメッセージが出ている場合、バッチファイルを右クリックして「管理者として実行」してください。
:END
@echo ----------
@pause
