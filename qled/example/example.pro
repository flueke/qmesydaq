#-------------------------------------------------
#
# Project created by QtCreator 2010-01-22T08:00:44
#
#-------------------------------------------------

TARGET =	example
TEMPLATE =	app
CONFIG +=	debug_and_release build_all link_prl

greaterThan(QT_MAJOR_VERSION, 4) {
QT +=		widgets
}

SOURCES +=	main.cpp \
		dialog.cpp

HEADERS +=	dialog.h

FORMS +=	dialog.ui

INCLUDEPATH += 	../src

unix:LIBS += 	-L../src -lqled
win32 {
CONFIG +=	windows
CONFIG(debug, debug|release):	LIBS +=	-L../src/debug -lqledd
CONFIG(release, debug|release):	LIBS += -L../src/release -lqled
QMAKE_LFLAGS += /MT
}


sources.files =	$$SOURCES $$HEADERS *.pro
sources.path =	$$[QT_INSTALL_EXAMPLES]/qled

INSTALLS +=	sources
