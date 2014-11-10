#-------------------------------------------------
#
# Project created by QtCreator 2010-01-22T08:00:44
#
#-------------------------------------------------

TARGET =	example
TEMPLATE =	app
CONFIG +=	debug


SOURCES +=	main.cpp \
		dialog.cpp

HEADERS +=	dialog.h

FORMS +=	dialog.ui

INCLUDEPATH += 	../src

LIBS +=		-L../src -lqled

sources.files =	$$SOURCES $$HEADERS *.pro
sources.path =	$$[QT_INSTALL_EXAMPLES]/qled

INSTALLS +=	sources
