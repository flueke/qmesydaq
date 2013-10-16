######################################################################
# Automatically generated by qmake (2.01a) Mo. Sep 14 10:30:13 2009
######################################################################

VERSION		= 1.22.2
SRCBASE		= ..

include($${SRCBASE}/mesydaqconfig.pri)

TEMPLATE 	= lib
TARGET 		= mesydaq
DEPENDPATH 	+= .
INCLUDEPATH 	+= .
LIBS		-= -lmesydaq

INSTALLS	= target

target.path	= $${TARGETLIBPATH}

QT 		+= core network

# Input
HEADERS 	+= stdafx.h \
		   libqmesydaq_global.h \
		   counter.h \
		   datarepeater.h \
		   histogram.h \
		   logging.h \
		   mapcorrect.h \
		   usermapcorrect.h \
		   mdllcorrect.h \
		   mappedhistogram.h \
		   mcpd.h \
		   mcpd8.h \
		   mdefines.h \
		   measurement.h \
		   mesydaq2.h \
		   mpsd8.h \
		   mstd16.h \
		   mcpd2.h \
		   mdll.h \
		   networkdevice.h \
        	   structures.h \
		   calibration.h

SOURCES 	+= stdafx.cpp \
		   counter.cpp \
		   datarepeater.cpp \
		   histogram.cpp \
		   logging.cpp \
		   mapcorrect.cpp \
		   usermapcorrect.cpp \
		   mdllcorrect.cpp \
		   mappedhistogram.cpp \
		   mcpd.cpp \
		   mcpd8.cpp \
		   mcpd2.cpp \
		   measurement.cpp \
		   mesydaq2.cpp \
		   mpsd8.cpp \
		   mpsd8p.cpp \
		   mpsd8old.cpp \
		   mpsd8sadc.cpp \
		   mstd16.cpp \
		   mdll.cpp \
		   mpsdfactory.cpp \
		   networkdevice.cpp
