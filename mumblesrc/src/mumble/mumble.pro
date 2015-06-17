include(../mumble.pri)

DEFINES		*= MUMBLE
TEMPLATE	= app
TARGET		= mumble

!CONFIG(qt4-legacy-compat) {
  CONFIG += no-qt4-legacy-compat
}

CONFIG(no-qt4-legacy-compat):isEqual(QT_MAJOR_VERSION, 4) {
  error("$$escape_expand(\\n)$$escape_expand(\\n)"\
        "Mumble client support for Qt 4 is deprecated and will be dropped$$escape_expand(\\n)"\
        "completely in the future. We highly recommend switching to$$escape_expand(\\n)"\
        "building Mumble with Qt 5. For now CONFIG+=qt4-legacy-compat$$escape_expand(\\n)"\
        "can be used to build with Qt 4. Note that if built this way,$$escape_expand(\\n)"\
        "Mumble might lack certain bug-fixes and capabilities available$$escape_expand(\\n)"\
        "when built with Qt 5.$$escape_expand(\\n)"\
        "$$escape_expand(\\n)")
}

isEqual(QT_MAJOR_VERSION, 4) {
  warning("$$escape_expand(\\n)$$escape_expand(\\n)"\
          "Mumble client support for Qt 4 is deprecated and will be dropped$$escape_expand(\\n)"\
          "completely in the future. We highly recommend switching to$$escape_expand(\\n)"\
          "building Mumble with Qt 5. When built with Qt 4, Mumble might$$escape_expand(\\n)"\
          "lack certain bug-fixes and capabilities available when built$$escape_expand(\\n)"\
          "with Qt 5 already.$$escape_expand(\\n)"\
          "$$escape_expand(\\n)")
}

CONFIG(static) {
  # On Windows, building a static client
  # means building the main app into a DLL.
  win32 {
    TEMPLATE = lib
    TARGET = mumble_app
    VERSION =

    CONFIG -= static
    CONFIG += shared qt_static mumble_dll
    DEFINES += USE_MUMBLE_DLL QT_SHARED
    isEqual(QT_MAJOR_VERSION, 5) {
      # Qt 5 uses an auto-generated .cpp file for importing plugins.
      # However, it is only automatically enabled for TEMPLATE = app.
      # Since we're building mumble_app.dll, we're not an app, but a library.
      # This means we'll have to explicitly ask Qt to generate and build the
      # plugin importer.
      CONFIG += force_import_plugins

      # Pretend we're inside a Qt build to get the Qt headers to dllexport correctly.
      # This is achievable in Qt 4 by defining QT_SHARED, but in Qt 5 we have to
      # hack our way around it. Even QT_SHARED will give us dllimport unless Qt thinks
      # it's doing a Qt build.
      DEFINES += QT_BUILD_CORE_LIB QT_BUILD_GUI_LIB QT_BUILD_WIDGETS_LIB QT_BUILD_NETWORK_LIB QT_BUILD_XML_LIB QT_BUILD_SQL_LIB QT_BUILD_SVG_LIB
    }

    DEF_FILE = $${DESTDIR}/$${TARGET}.def

    QMAKE_LFLAGS += /DEF:$${DEF_FILE}
    QMAKE_LFLAGS += /ignore:4102 # export of deleting destructor
    QMAKE_LFLAGS += /ignore:4197 # specified multiple times

    CONFIG(debug, debug|release) {
      DEF_KIND = debug
    }

    CONFIG(release, debug|release) {
      DEF_KIND = release
    }

    gendef.commands = python ../../scripts/gen-mumble_app-qt-def.py $${DEF_KIND} $$[QT_INSTALL_LIBS] $${DEF_FILE}
    QMAKE_EXTRA_TARGETS *= gendef
    PRE_TARGETDEPS *= gendef
    QMAKE_DISTCLEAN *= $${DEF_FILE}
  }

  DEFINES *= USE_STATIC
  CONFIG += static_qt_plugins
}

QT		*= network sql xml svg
isEqual(QT_MAJOR_VERSION, 5) {
  QT *= widgets
  # Allow native widget access.
  win32:QT *= gui-private
  macx:QT *= gui-private
}

HEADERS *=
    GlobalShortcut.h

SOURCES *=
    GlobalShortcut.cpp

DIST		*= ../../icons/mumble.ico licenses.h smallft.h ../../icons/mumble.xpm murmur_pch.h mumble.plist
# RESOURCES	*=
FORMS *= GlobalShortcut.ui


CONFIG(no-xinput2) {
  DEFINES	*= NO_XINPUT2
}

win32 {
  # CONFIG(mumble_dll) {
  #   RC_FILE = mumble_dll.rc
  # } else {
  #   RC_FILE = mumble.rc
  # }
  HEADERS	*= GlobalShortcut_win.h
  SOURCES	*= GlobalShortcut_win.cpp
  LIBS		*= -ldxguid -ldinput8
  # -lsapi -lole32 -lws2_32 -ladvapi32 -lwintrust -ldbghelp -llibsndfile-1 -lshell32 -lshlwapi -luser32 -lgdi32 -lpsapi
  # LIBS		*= -ldelayimp -delayload:shell32.dll



  DEFINES	*= WIN32

  !CONFIG(mumble_dll) {
    !CONFIG(no-elevation) {
      CONFIG(release, debug|release) {
        QMAKE_LFLAGS *= /MANIFESTUAC:\"level=\'asInvoker\' uiAccess=\'true\'\"
      }
    }
    QMAKE_POST_LINK = $$QMAKE_POST_LINK$$escape_expand(\\n\\t)$$quote(mt.exe -nologo -updateresource:$(DESTDIR_TARGET);1 -manifest mumble.appcompat.manifest)
  }
}

CONFIG(static_qt_plugins) {
  DEFINES += USE_STATIC_QT_PLUGINS
  QTPLUGIN += qtaccessiblewidgets qico qsvg qsvgicon

  win32 {
    isEqual(QT_MAJOR_VERSION, 5) {
      QTPLUGIN += qwindows
    }
  }

  # Icon engines are special; they don't get their lib directory
  # included automatically by mkspecs/features/qt.prf
  LIBS *= -L$$[QT_INSTALL_PLUGINS]/iconengines
}

lrel.output = ${QMAKE_FILE_BASE}.qm
lrel.commands = $${QMAKE_LRELEASE} ${QMAKE_FILE_NAME}
lrel.input = TRANSLATIONS
lrel.CONFIG *= no_link
lrel.variable_out = rcc.depends

QMAKE_EXTRA_COMPILERS *= lrel

include(../../symbols.pri)
