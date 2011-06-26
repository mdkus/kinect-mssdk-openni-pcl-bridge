@echo Note: this script must be run as administrator.
"%OPEN_NI_BIN%\niReg" -u %0\..\bin\Release\kinect-mssdk-openni-bridge.dll && @echo OK
"%OPEN_NI_BIN%\niReg" -u %0\..\bin\Debug\kinect-mssdk-openni-bridge.dll && @echo OK
pause
