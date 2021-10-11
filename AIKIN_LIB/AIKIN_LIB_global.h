#pragma once

#include <QtCore/qglobal.h>

#if defined(AIKIN_LIB_LIBRARY)
#  define AIKIN_LIB_EXPORT Q_DECL_EXPORT
#else
#  define AIKIN_LIB_EXPORT Q_DECL_IMPORT
#endif

