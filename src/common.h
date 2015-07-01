#ifndef NIFTYKB_COMMON_H_
#define NIFTYKB_COMMON_H_

#include <QtCore>
#include <QtWidgets>

#include <math.h>
#include <boost/typeof/typeof.hpp>

#define STACKVAR(type, varname, count) type *varname=reinterpret_cast<type *>(_alloca(sizeof(type) * (count)))
#define __cdecl
#include <Dbt.h>

#endif
