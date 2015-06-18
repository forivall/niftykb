#ifndef DINPUTKEYDEBUG_COMMON_H_
#define DINPUTKEYDEBUG_COMMON_H_

#include <QtCore/QStandardPaths>
#include <QtCore>
#include <QtCore/QSet>
#include <QStringList>
#include <QtCore/QDir>
#include <QtGui>
#include <QtWidgets>
#include <QFocusEvent>

#include <math.h>

#define STACKVAR(type, varname, count) type *varname=reinterpret_cast<type *>(_alloca(sizeof(type) * (count)))
#define __cdecl
// typedef WId HWND;
// #include <arpa/inet.h>
// #include <windows.h>
// #include <shellapi.h>
// #include <winsock2.h>
// #include <qos2.h>
// #include <wintrust.h>
// #include <Softpub.h>
#include <Dbt.h>
// #include <delayimp.h>
// #include <shlobj.h>
// #include <tlhelp32.h>
// #include <psapi.h>
// #include <math.h>

#endif
