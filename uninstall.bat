@setlocal
@SET NIREG_PATH="%OPEN_NI_BIN%\niReg"
%NIREG_PATH% -u %0\..\bin\kinect-mssdk-openni-pcl-bridge-V10Release.dll
@IF NOT "%ERRORLEVEL%" == "0" GOTO ERR
%NIREG_PATH% -u %0\..\bin\kinect-mssdk-openni-pcl-bridge-V10Debug.dll
@IF NOT "%ERRORLEVEL%" == "0" GOTO ERR
@GOTO END
:ERR
@echo ----------
@echo [en] Error occured. If you see "Failed to write to the file!" message, try again "as Administrator" by right-clicking the batch file.
:END
@echo OK!
@echo ----------
@pause
