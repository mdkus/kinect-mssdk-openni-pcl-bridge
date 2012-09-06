@rem Note: (1) Set VER to release version. (2) Install ActivePerl.

setlocal
set VER=v1.5.0.3-for-1.5.2.23
set TARGET=release\kinect-mssdk-openni-bridge-%VER%
rmdir /s /q %TARGET%
mkdir %TARGET%
xcopy ..\bin\*V10*.dll %TARGET%\bin\ /e
xcopy ..\bin\*V10*.ini %TARGET%\bin\ /e
copy ..\LICENSE.TXT %TARGET%
copy ..\README_*.TXT %TARGET%
copy ..\HISTORY_*.TXT %TARGET%
copy ..\*.bat %TARGET%
perl zip.pl %TARGET%
copy %TARGET%.zip ..\release
