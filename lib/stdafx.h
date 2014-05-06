#ifndef LIB_QMESYDAQ_STDAFX
#define LIB_QMESYDAQ_STDAFX

//QT Includes
#include <QCoreApplication>
#include <QtCore>
//#include <QtNetwork>

//C++ Includes
//#include <list>
//#include <iostream>

//C Includes
//#include <cstdio>
//#include <cmath>

#include "libqmesydaq_global.h"

#if defined(_MSC_VER)
#define VERSION "1.2"

//Functions
LIBQMESYDAQ_EXPORT int round(double value);
//POSIX Functions
LIBQMESYDAQ_EXPORT void sleep(unsigned int s);
LIBQMESYDAQ_EXPORT void usleep(unsigned int us);
#else
#include <unistd.h>
#endif

#endif //LIB_QMESYDAQ_STDAFX
