######################################################################
# Automatically generated by qmake (2.01a) Mo. Okt 5 11:04:42 2009
######################################################################

VERSION		= 1.0.0

include(../../mesydaqconfig.pri)

TEMPLATE 	= app
TARGET 		= qmmaster

QT		+= core network

CONFIG		+= debug

DEPENDPATH 	+= ../../lib .
INCLUDEPATH 	+= ../../lib .

# Input
SOURCES 	+= main.cpp

LIBS        	+= -L../../lib -lmesydaq

INSTALLS        = target

target.path     = $${TARGETPATH}/bin
