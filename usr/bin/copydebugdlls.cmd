@echo off
set arch=x64
set mydir=%~dp0
if /i "%1" == "x64" set arch=x64
if /i "%1" == "x86" set arch=x86

call %mydir%varsall_%arch%.cmd
set projdir=%mydir:\usr\bin\=%

:: guaranteed random number by mashing on keyboard
qtpaths --binaries-dir>tmp3642987.out
set /p QtBinDir= < tmp3642987.out
set QtBinDir=%QtBinDir:/=\%
del tmp3642987.out

@echo on

copy %QtBinDir%\icudt53.dll %projdir%\build-%arch%-debug\src
copy %QtBinDir%\icuin53.dll %projdir%\build-%arch%-debug\src
copy %QtBinDir%\icuuc53.dll %projdir%\build-%arch%-debug\src
copy %QtBinDir%\Qt5Cored.dll %projdir%\build-%arch%-debug\src
copy %QtBinDir%\Qt5Cored.pdb %projdir%\build-%arch%-debug\src
copy %QtBinDir%\Qt5Guid.dll %projdir%\build-%arch%-debug\src
copy %QtBinDir%\Qt5Guid.pdb %projdir%\build-%arch%-debug\src
copy %QtBinDir%\Qt5Widgetsd.dll %projdir%\build-%arch%-debug\src
copy %QtBinDir%\Qt5Widgetsd.pdb %projdir%\build-%arch%-debug\src
copy %QtBinDir%\Qt5Networkd.dll %projdir%\build-%arch%-debug\src
copy %QtBinDir%\Qt5Networkd.pdb %projdir%\build-%arch%-debug\src

@echo off
