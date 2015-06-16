
win32 {
	# Import dependency paths for windows
	include(winpaths_default.pri)


	# Sanity check that DXSDK_DIR, LIB and INCLUDE are properly set up.
	#
	# On Windows/x86, we build using the VS2013 v120_xp toolchain, which targets
	# a slightly modified Win7 SDK that still allows building for Windows XP. In that
	# environment, we must use the "external" DirectX SDK (June 2010). This SDK is
	# specified via the SXSDK_DIR.
	#
	# On Windows/amd64, we build using the VS2013 v120 platform, and we target the
	# Windows 8.1 SDK. In this setup, we use the DirectX SDK included with the Windows
	# 8.1 SDK, but only to a point. The bundled SDK does not include all things that
	# we depend on for the overlay, such as d3dx{9,10,11}. To overcome this, we use
	# both SDKs: the one bundled with the Windows 8.1 SDK for most libraries, and the
	# external June 2010 variant for the things that are not in the Windows 8.1 SDK
	# variant of the DirectX SDK. This is the approach recommended by Microsoft:
	# http://msdn.microsoft.com/en-us/library/windows/desktop/ee663275(v=vs.85).aspx
	# (see step 5).
	#
	# Because of these things, the Windows build currently expects the build environment
	# to properly set up the LIB and INCLUDE environment variables, with correct ordering
	# of the Windows SDK and DirectX depending on the platform we're targetting.
	# It's tough to check these things with qmake, we'll have to do with a simple sanity
	# check for the presence of the variables.
	DXSDK_DIR_VAR=$$(DXSDK_DIR)
	INCLUDE_VAR=$$(INCLUDE)
	LIB_VAR=$$(LIB)
	isEmpty(DXSDK_DIR_VAR) {
		error("Missing DXSDK_DIR environment variable. Are you missing the DirectX SDK (June 2010)?")
	}
	isEmpty(LIB_VAR) {
		error("The LIB environment variable is not set. Are you not in a build environment?")
	}
	isEmpty(INCLUDE_VAR) {
		error("The INCLUDE environment variable is not set. Are you not in a build environment?")
	}

	QMAKE_CFLAGS_RELEASE *= -Ox /fp:fast
	QMAKE_CXXFLAGS_RELEASE *= -Ox /fp:fast

	equals(QMAKE_TARGET.arch, x86) {
		QMAKE_LFLAGS_RELEASE -= /SafeSEH
	}

	# MSVS 2012 and 2013's cl.exe will generate SSE2 code by default,
	# unless an explict arch is set.
	# For our non-64 x86 builds, our binaries should not contain any
	# SSE2 code, so override the default by using -arch:SSE.
	equals(QMAKE_TARGET.arch, x86) {
		QMAKE_CFLAGS_RELEASE *= -arch:SSE
		QMAKE_CXXFLAGS_RELEASE *= -arch:SSE
	}

	# Qt 5.4 uses -Zc:strictStrings by default on MSVS 2013.
	# TextToSpeech_win.cpp uses sapi.h, which isn't compatible
	# with the strictStrings option due to bad conversions
	# in some of its functions's default parameters.
	QMAKE_CFLAGS_RELEASE -= -Zc:strictStrings
	QMAKE_CXXFLAGS_RELEASE -= -Zc:strictStrings
	QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO -= -Zc:strictStrings
	QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO -= -Zc:strictStrings

	# Explicitly set the subsystem versions to
	# 5.01 (XP) for x86 and 6.00 (Vista) for x64.
	#
	# Qt expands the @QMAKE_SUBSYSTEM_SUFFIX@ via
	# qt_config.prf, which doesn't seem to trigger
	# for us. So we'll just try our luck.
	QMAKE_LFLAGS_CONSOLE -= /SUBSYSTEM:CONSOLE
	QMAKE_LFLAGS_CONSOLE -= /SUBSYSTEM:CONSOLE@QMAKE_SUBSYSTEM_SUFFIX@
	QMAKE_LFLAGS_WINDOWS -= /SUBSYSTEM:WINDOWS
	QMAKE_LFLAGS_WINDOWS -= /SUBSYSTEM:WINDOWS@QMAKE_SUBSYSTEM_SUFFIX@
	!isEmpty(QMAKE_LFLAGS_WINDOWS) {
		error("QMAKE_LFLAGS_WINDOWS is not empty. Please adjust the pri file.")
	}
	!isEmpty(QMAKE_LFLAGS_CONSOLE) {
		error("QMAKE_LFLAGS_CONSOLE is not empty. Please adjust the pri file.")
	}
	equals(QMAKE_TARGET.arch, x86) {
		QMAKE_LFLAGS_CONSOLE += /SUBSYSTEM:CONSOLE,5.01
		QMAKE_LFLAGS_WINDOWS += /SUBSYSTEM:WINDOWS,5.01
	}
	equals(QMAKE_TARGET.arch, x86_64) {
		QMAKE_LFLAGS_CONSOLE += /SUBSYSTEM:CONSOLE,6.00
		QMAKE_LFLAGS_WINDOWS += /SUBSYSTEM:WINDOWS,6.00
	}

	CONFIG(analyze) {
		QMAKE_CFLAGS_DEBUG *= /analyze
		QMAKE_CXXFLAGS_DEBUG *= /analyze
		QMAKE_CFLAGS_RELEASE *= /analyze
		QMAKE_CXXFLAGS_RELEASE *= /analyze
	}
	DEFINES *= RESTRICT=
	CONFIG(sse2) {
	      QMAKE_CFLAGS_RELEASE -= -arch:SSE
	      QMAKE_CFLAGS_DEBUG -= -arch:SSE
	      QMAKE_CFLAGS += -arch:SSE2
	}

	# Define the CONFIG options 'force-x86-toolchain' and
	# 'force-x86_64-toolchain'. These can be used to force
	# the target of a .pro file to be built for a specific
	# architecture, regardless of the actual architecture
	# used by the current build environment.
	FULL_MKSPEC_PATH = $$QMAKESPEC
	CURRENT_MKSPEC = $$basename(QMAKESPEC)

	CONFIG(force-x86-toolchain) {
		include(toolchain/$${CURRENT_MKSPEC}/x86-xp.toolchain)
	}

	CONFIG(force-x86_64-toolchain) {
		include(toolchain/$${CURRENT_MKSPEC}/x64.toolchain)
	}

	CONFIG(symbols) {
		# Configure build to be able to properly debug release builds
		# (https://msdn.microsoft.com/en-us/library/fsk896zz.aspx).
		# This includes explicitely disabling /Oy to help debugging
		# (https://msdn.microsoft.com/en-us/library/2kxx5t2c.aspx).
		# Also set /Zo to enhance optimized debugging
		# (https://msdn.microsoft.com/en-us/library/dn785163.aspx?f=255&MSPPError=-2147217396).
		QMAKE_CFLAGS_RELEASE -= -Oy
		QMAKE_CXXFLAGS_RELEASE -= -Oy

		QMAKE_CFLAGS_RELEASE *= -GR -Zi -Zo -Oy-
		QMAKE_CXXFLAGS_RELEASE *= -GR -Zi -Zo -Oy-

		QMAKE_LFLAGS *= /DEBUG /OPT:REF /OPT:ICF /INCREMENTAL:NO
	}

	CONFIG(vld) {
		CONFIG(debug, debug|release) {
			DEFINES *= USE_VLD
			INCLUDEPATH *= "$$VLD_PATH/include"
			QMAKE_LIBDIR *= "$$VLD_PATH/lib"
		}
	}
}

VERSION		= 1.3.0
DIST		= mumble.pri Message.h PacketDataStream.h CryptState.h Timer.h Version.h OSInfo.h SSL.h Mumble.proto
CONFIG		+= qt thread debug_and_release warn_on
DEFINES		*= MUMBLE_VERSION_STRING=$$VERSION
INCLUDEPATH	+= $$PWD .
VPATH		+= $$PWD
HEADERS		*= OSInfo.h Timer.h Version.h
SOURCES 	*= Timer.cpp OSInfo.cpp Version.cpp

win32 {
	INCLUDEPATH *= "$$PROTOBUF_PATH/vsprojects/include" "$$PROTOBUF_PATH/src"
	CONFIG(debug, debug|release) {
		QMAKE_LIBDIR *= "$$PROTOBUF_PATH/vsprojects/Debug"
	} else {
		QMAKE_LIBDIR *= "$$PROTOBUF_PATH/vsprojects/Release"
	}
	INCLUDEPATH *= "$$OPENSSL_PATH/include"
	QMAKE_LIBDIR *= "$$OPENSSL_PATH/lib"

	LIBS *= -llibprotobuf -lcrypt32 -lws2_32 -llibeay32
	LIBS *= -ldelayimp -lQwave -delayload:Qwave.DLL
}

CONFIG(debug, debug|release) {
  CONFIG += console
  QMAKE_LIBDIR += ../../debug
  DESTDIR	= ../../debug
}

CONFIG(release, debug|release) {
  QMAKE_LIBDIR += ../../release
  DESTDIR	= ../../release
}
