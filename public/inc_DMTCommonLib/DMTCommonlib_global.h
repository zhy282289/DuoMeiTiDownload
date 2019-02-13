#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(DMTCOMMONLIB_LIB)
#  define DMTCOMMONLIB_EXPORT Q_DECL_EXPORT
# else
#  define DMTCOMMONLIB_EXPORT Q_DECL_IMPORT
# endif
#else
# define DMTCOMMONLIB_EXPORT
#endif

#include <QObject>
#include <QString>
