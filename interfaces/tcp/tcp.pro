VERSION 	= 0.0.2

include( ../../qmesydaqconfig.pri )

TEMPLATE 	= lib
TARGET 		= tcpInterface
DEPENDPATH 	+= . .. ../.. ../../qmesydaq ../../lib
INCLUDEPATH 	+= . .. ../.. ../../qmesydaq ../../lib

LIBS		+= -L../../lib $${TACOLIBS}

QT		+= network

target.path	= $${TARGETLIBPATH}

# Input
HEADERS 	+= remoteserver.h \
		TCPLoop.h 

SOURCES 	+= remoteserver.cpp \
		TCPLoop.cpp
		

