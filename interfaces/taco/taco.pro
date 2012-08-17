VERSION 	= 1.0.2

include( ../../qmesydaqconfig.pri )

TEMPLATE 	= lib
TARGET 		= tacoInterface
DEPENDPATH 	+= . .. ../.. ../../qmesydaq ../../lib
INCLUDEPATH 	+= . .. ../.. ../../qmesydaq ../../lib

LIBS		+= -L../../lib $${TACOLIBS}

target.path	= $${TARGETLIBPATH}

# Input
HEADERS 	+= MesyDAQDetectorDetector.h \
		MesyDAQIOCounter.h \
		MesyDAQIOTimer.h \
		MesyDAQServer.h \
		TACOLoop.h 

SOURCES 	+= MesyDAQDetectorDetector.cpp \
           	MesyDAQDetectorDetectorImpl.cpp \
		MesyDAQIOCounter.cpp \
		MesyDAQIOCounterImpl.cpp \
		MesyDAQIOTimer.cpp \
		MesyDAQIOTimerImpl.cpp \
		startup.cpp \
		TACOLoop.cpp
		

