######################################################################
# Automatically generated by qmake (2.01a) Tue Nov 11 16:21:25 2014
######################################################################

TEMPLATE =	lib
TARGET =	qled
TARGET = 	$$qtLibraryTarget($$TARGET)
DEPENDPATH +=	.
INCLUDEPATH +=	.
QT +=		svg

DEFINES +=	QDESIGNER_EXPORT_WIDGETS

if (win32-msvc*) {
	CONFIG += static
}

CONFIG +=	create_prl debug_and_release build_all

# Input
HEADERS +=	qled.h
SOURCES +=	qled.cpp

RESOURCES +=	qled.qrc

target.path =	$$[QT_INSTALL_LIBS]
headers.path =	$$[QT_INSTALL_HEADERS]
sources.files = $$SOURCES $$HEADERS *.pro
sources.path =	$$[QT_INSTALL_EXAMPLES]/designer/qledplugin
INSTALLS +=	target headers
