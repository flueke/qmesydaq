VERSION		= 0.2.0
SRCBASE		= ../..

include($${SRCBASE}/mesydaqconfig.pri)

TEMPLATE	= app
TARGET		= simulator

QT		+= core network

CONFIG		-= release warn_off
CONFIG		+= debug warn_on console

DEPENDPATH	+= . $${SRCBASE}/lib
INCLUDEPATH	+= . $${SRCBASE}/lib

LIBS		+= -L$${SRCBASE}/lib

SOURCES		+= main.cpp simmcpd8.cpp
HEADERS		+= main.h simmcpd8.h

INSTALLS	= target

target.path	= $${TARGETPATH}/bin
