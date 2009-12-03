include( ../../qmesydaqconfig.pri )

TEMPLATE 	= lib
TARGET 		= tacoInterface
DEPENDPATH 	+= . .. ../.. ../../qmesydaq ../../lib
INCLUDEPATH 	+= . .. ../.. ../../qmesydaq ../../lib

LIBS		+= -L../../lib $${TACOLIBS}

# Input
HEADERS 	+= MesyDAQDetectorDetector.h \
		MesyDAQServer.h \
		tacocontrol.h \
		tacothread.h

SOURCES 	+= MesyDAQDetectorDetector.cpp \
           	MesyDAQDetectorDetectorImpl.cpp \
           	tacocontrol.cpp \
           	tacothread.cpp \
		startup.cpp

