######################################################################
# Automatically generated by qmake (2.01a) Mo. Sep 14 10:30:13 2009
######################################################################

TEMPLATE 	= lib
TARGET 		= mesydaq
DEPENDPATH 	+= .
INCLUDEPATH 	+= .

INSTALLS	= target

contains(CONFIG, bit64) {
	target.path    = /usr/local/lib64
}
else {
	target.path    = /usr/local/lib
}

CONFIG		+= debug

QT 		+= core network

# Input
HEADERS 	+= counter.h \
        	   histogram.h \
        	   inifile.h \
		   mapcorrect.h \
        	   mcpd8.h \
        	   mdefines.h \
        	   measurement.h \
        	   mesydaq2.h \
        	   mesydaqobject.h \
        	   mpsd8.h \
		   mstd16.h \
		   mcpd2.h \
        	   networkdevice.h \
        	   structures.h

SOURCES 	+= counter.cpp \
        	   histogram.cpp \
        	   inifile.cpp \
		   mapcorrect.cpp \
        	   mcpd8.cpp \
        	   measurement.cpp \
        	   mesydaq2.cpp \
        	   mesydaqobject.cpp \
        	   mpsd8.cpp \
        	   mpsd8p.cpp \
		   mpsd8old.cpp \
		   mstd16.cpp \
		   mcpd2.cpp \
        	   networkdevice.cpp
