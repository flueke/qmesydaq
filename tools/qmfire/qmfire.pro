######################################################################
# Automatically generated by qmake (2.01a) Mo. Okt 5 11:04:42 2009
######################################################################

VERSION		= 1.0.2

include(../../qmesydaqconfig.pri)

TEMPLATE 	= app
TARGET 		= qmfire

QT		+= core network

CONFIG		+= debug

DEPENDPATH 	+= ../../lib .
INCLUDEPATH 	+= ../../lib .

# Input
SOURCES 	+= main.cpp \
		readfile.cpp

LIBS        	+= -L../../lib -lmesydaq

INSTALLS        = target

target.path     = /usr/local/bin
