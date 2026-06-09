#pragma once

#include <QtCore/qglobal.h>

#if defined(ASTERIXQT_STATIC)
#    define ASTERIXQT_EXPORT
#elif defined(ASTERIXQT_LIBRARY)
#    define ASTERIXQT_EXPORT Q_DECL_EXPORT
#else
#    define ASTERIXQT_EXPORT Q_DECL_IMPORT
#endif
