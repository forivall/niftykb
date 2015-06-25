set CMAKE_PREFIX_PATH=%SYSTEMDRIVE%\Qt\5.4\msvc2013;%CMAKE_PREFIX_PATH%
if not defined DXSDK_DIR (
  set DXSDK_DIR="%PROGRAMFILES(X86)%\Microsoft DirectX SDK (June 2010)\"
)
set OWD=%CD%
call "%SYSTEMDRIVE%\Qt\5.4\msvc2013\bin\qtenv2.bat"
cd /D %OWD%
set OWD=
echo on
set PATH=%PATH%;%SYSTEMDRIVE%\Qt\Tools\QtCreator\bin
call "%PROGRAMFILES(X86)%\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86
