######################################################################
# Automatically generated by qmake (2.01a) Mo. Okt 5 11:04:42 2009
######################################################################

VERSION		= 1.0.0
SRCBASE		= ../..

include($${SRCBASE}/mesydaqconfig.pri)

TEMPLATE 	= app
TARGET 		= qmmaster

QT		+= core network

CONFIG		+= debug

DEPENDPATH 	+= . $${SRCBASE}/lib
INCLUDEPATH 	+= . $${SRCBASE}/lib

LIBS		+= -L$${SRCBASE}/lib -lmesydaq

# Input
SOURCES 	+= main.cpp

INSTALLS        = target

target.path     = $${TARGETPATH}/bin
