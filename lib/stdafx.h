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

#if defined(_MSC_VER)
//Functions
Q_EXTERN_C int round(double value);
//POSIX Functions
Q_EXTERN_C void sleep(unsigned int s);
Q_EXTERN_C void usleep(unsigned int us);
#else
#include <unistd.h>
#endif

#endif //LIB_QMESYDAQ_STDAFX
