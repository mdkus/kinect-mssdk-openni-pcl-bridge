@setlocal
@SET NIREG_PATH="%OPEN_NI_BIN%\niReg"
@rem %NIREG_PATH% -u %0\..\bin\kinect-mssdk-openni-bridge-Beta1Release.dll
@rem @IF NOT "%ERRORLEVEL%" == "0" GOTO ERR
@rem %NIREG_PATH% -u %0\..\bin\kinect-mssdk-openni-bridge-Beta1Debug.dll
@rem @IF NOT "%ERRORLEVEL%" == "0" GOTO ERR
@rem %NIREG_PATH% -u %0\..\bin\kinect-mssdk-openni-bridge-Beta2Release.dll
@rem @IF NOT "%ERRORLEVEL%" == "0" GOTO ERR
@rem %NIREG_PATH% -u %0\..\bin\kinect-mssdk-openni-bridge-Beta2Debug.dll
@rem @IF NOT "%ERRORLEVEL%" == "0" GOTO ERR
%NIREG_PATH% -u %0\..\bin\kinect-mssdk-openni-bridge-V10Release.dll
@IF NOT "%ERRORLEVEL%" == "0" GOTO ERR
%NIREG_PATH% -u %0\..\bin\kinect-mssdk-openni-bridge-V10Debug.dll
@IF NOT "%ERRORLEVEL%" == "0" GOTO ERR
@GOTO END
:ERR
@echo ----------
@echo [en] Error occured. If you see "Failed to write to the file!" message, try again "as Administrator" by right-clicking the batch file.
@echo [ja] エラーが発生しました。"Failed to write to the file!" というメッセージが出ている場合、バッチファイルを右クリックして「管理者として実行」してください。
:END
@echo OK!
@echo ----------
@pause
