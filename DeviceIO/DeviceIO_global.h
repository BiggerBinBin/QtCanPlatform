#ifndef DEVICEIO_GLOBAL_H
#define DEVICEIO_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(DEVICEIO_LIBRARY)
#  define DEVICEIO_EXPORT Q_DECL_EXPORT
#else
#  define DEVICEIO_EXPORT Q_DECL_IMPORT
#endif

#endif // DEVICEIO_GLOBAL_H
