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

#include "libqmesydaq_export.h"

#if defined(_MSC_VER)
#if !defined(VERSION)
	#define VERSION "1.2"
#endif
#include <math.h>
//Functions
#if _MSC_VER < 1700
LIBQMESYDAQ_EXPORT int round(double value);
#endif
//POSIX Functions
LIBQMESYDAQ_EXPORT void sleep(unsigned int s);
LIBQMESYDAQ_EXPORT void usleep(unsigned int us);
#else
#include <unistd.h>
#endif

#endif //LIB_QMESYDAQ_STDAFX
