include( ../../qmesydaqconfig.pri )

TEMPLATE 	= lib
TARGET 		= tacoInterface
DEPENDPATH 	+= . .. ../.. ../../qmesydaq ../../lib
INCLUDEPATH 	+= . .. ../.. ../../qmesydaq ../../lib

LIBS		+= -L../../lib $${TACOLIBS}

contains(CONFIG, bit64) {
target.path	= /usr/local/lib64
}
else {
target.path	= /usr/local/lib
}

# Input
HEADERS 	+= MesyDAQDetectorDetector.h \
		MesyDAQServer.h \
		TACOLoop.h 

#		tacocontrol.h \
#		tacothread.h 

SOURCES 	+= MesyDAQDetectorDetector.cpp \
           	MesyDAQDetectorDetectorImpl.cpp \
		startup.cpp \
		TACOLoop.cpp
		
#           	tacocontrol.cpp \
#           	tacothread.cpp \

