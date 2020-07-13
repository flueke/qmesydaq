greaterThan(QT_MAJOR_VERSION, 4) {
	QT += designer
}
else {
	CONFIG += designer
}
CONFIG +=	debug_and_release link_prl
## _and_release
TARGET =	qledplugin # $$qtLibraryTarget($$TARGET)
TEMPLATE =	lib
QTDIR_build:	DESTDIR = $$QT_BUILD_TREE/plugins/designer

QT +=		svg
INCLUDEPATH +=	../src/

DEFINES +=	QDESIGNER_EXPORT_WIDGETS

HEADERS =	qledplugin.h
SOURCES =	qledplugin.cpp

unix: LIBS += 	-L../src -lqled
win32 {
CONFIG(debug, debug|release):	LIBS +=	-L../src/debug -lqledd
CONFIG(release, debug|release):	LIBS += -L../src/release -lqled
}

build_all:!build_pass {
	CONFIG -= build_all
	CONFIG += release
}

# install
target.path =	$$[QT_INSTALL_PLUGINS]/designer
sources.files = $$SOURCES $$HEADERS *.pro
sources.path =	$$[QT_INSTALL_EXAMPLES]/designer/qledplugin
INSTALLS +=	target sources
