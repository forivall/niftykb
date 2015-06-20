set CMAKE_PREFIX_PATH=%SYSTEMDRIVE%\Qt\5.4\msvc2013;%CMAKE_PREFIX_PATH%
set OWD=%CD%
call "%SYSTEMDRIVE%\Qt\5.4\msvc2013\bin\qtenv2.bat"
cd /D %OWD%
set OWD=
echo on
set PATH=%PATH%;%SYSTEMDRIVE%\Qt\Tools\QtCreator\bin
call "%PROGRAMFILES(X86)%\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86
