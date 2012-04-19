#ifndef QMESYDAQ_STDAFX
#define QMESYDAQ_STDAFX

////Definition for Windows Console
//#if defined(_MSC_VER)
//	#include <io.h>
//	#include <FCNTL.H>
//	//#include <Windows.h>	//This Include was moved to the main.cpp.
//#endif
//
////QT Includes
//#include <QtCore>
//#include <QtGui>
//#include <QtNetwork>
//#include <QtSvg>
//
////QWT Includes
//#include <qwt_data.h>
//#include <qwt_raster_data.h>
//#include <qwt_plot_curve.h>
//#include <qwt_plot_zoomer.h>
//#include <qwt_plot_layout.h>
//#include <qwt_plot_spectrogram.h>
//#include <qwt_scale_map.h>
//#include <qwt_scale_widget.h>
//#include <qwt_scale_engine.h>
//#include <qwt_color_map.h>
//
////C++ Includes
//#include <iostream>
//#include <vector>
//
////C Includes
//#include <cmath>
//#include <cstdlib>
//
////LibQMesyDAQ Includes
//#include <structures.h>
//#include <measurement.h>

//Functions
#if defined(_MSC_VER)
//Functions
	int round(double value);
//POSIX Functions
	void sleep(unsigned int s);
	void usleep(unsigned int us);
#endif

#if defined(_MSC_VER)
#	define VERSION "0.22"
#endif

#endif //QMESYDAQ_STDAFX
