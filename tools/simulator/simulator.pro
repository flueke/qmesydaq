VERSION		= 0.1.0
SRCBASE		= ../..

include($${SRCBASE}/mesydaqconfig.pri)

TEMPLATE        = app
TARGET          = simulator

QT             += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG         -= release warn_off
CONFIG         += debug warn_on

DEPENDPATH 	+= . $${SRCBASE}/lib
INCLUDEPATH 	+= . $${SRCBASE}/lib

SOURCES        += main.cpp\
                  mainwindow.cpp

HEADERS        += mainwindow.h

FORMS          += mainwindow.ui

OTHER_FILES    += moc_mainwindow.cpp \
                  ui_mainwindow.h

INSTALLS        = target

target.path     = $${TARGETPATH}/bin
