#ifndef NIFTYKB_COMMON_H_
#define NIFTYKB_COMMON_H_

#include <QtCore/QStandardPaths>
#include <QtCore>
#include <QtCore/QSet>
#include <QStringList>
#include <QtCore/QDir>
#include <QtGui>
#include <QtWidgets>
#include <QFocusEvent>

#include <math.h>
#include <boost/typeof/typeof.hpp>

#define STACKVAR(type, varname, count) type *varname=reinterpret_cast<type *>(_alloca(sizeof(type) * (count)))
#define __cdecl
#include <Dbt.h>

#endif
