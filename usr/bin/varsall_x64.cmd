set CMAKE_PREFIX_PATH=%SYSTEMDRIVE%\Qt\5.4\msvc2013_64;%CMAKE_PREFIX_PATH%
set OWD=%CD%
%SYSTEMDRIVE%\Qt\5.4\msvc2013_64\bin\qtenv2.bat
cd %OWD%
set OWD=
echo on
set PATH=%PATH%;%SYSTEMDRIVE%\Qt\Tools\QtCreator\bin
%PROGRAMFILES(X86)%\Microsoft Visual Studio 12.0\VC\vcvarsall.bat x64
