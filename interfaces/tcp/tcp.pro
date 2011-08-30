VERSION 	= 0.0.1

include( ../../qmesydaqconfig.pri )

TEMPLATE 	= lib
TARGET 		= tcpInterface
DEPENDPATH 	+= . .. ../.. ../../qmesydaq ../../lib
INCLUDEPATH 	+= . .. ../.. ../../qmesydaq ../../lib

LIBS		+= -L../../lib $${TACOLIBS}

QT		+= network

contains(CONFIG, bit64) {
target.path	= /usr/local/lib64
}
else {
target.path	= /usr/local/lib
}

# Input
HEADERS 	+= remoteserver.h \
		TCPLoop.h 

SOURCES 	+= remoteserver.cpp \
		TCPLoop.cpp
		

