@echo ----------
@setlocal
@SET BUILD_TYPE=Release
@SET NIREG_PATH="%OPEN_NI_BIN%\niReg"
SET KINECTSDK_VERSION=V10
%NIREG_PATH%  %0\..\bin\kinect-mssdk-openni-pcl-bridge-%KINECTSDK_VERSION%%BUILD_TYPE%.dll
@IF NOT "%ERRORLEVEL%" == "0" GOTO ERR
@echo OK!
@GOTO END
:ERR
@echo ----------
@echo [en] Error occured. If you see "Failed to write to the file!" message, try again "as Administrator" by right-clicking the batch file.
:END
@echo ----------
@pause
