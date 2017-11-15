greaterThan(QT_MAJOR_VERSION, 4) {
	QT += designer
}
else {
	CONFIG += designer
}
CONFIG +=	plugin debug
## _and_release
TARGET =	$$qtLibraryTarget($$TARGET)
TEMPLATE =	lib
QT +=		svg

QTDIR_build:	DESTDIR = $$QT_BUILD_TREE/plugins/designer

INCLUDEPATH +=	../src/
HEADERS =	qledplugin.h
SOURCES =	qledplugin.cpp
unix: LIBS += 	-L../src -lqled
win32: LIBS +=  -L../src/debug -lqledd

# install
target.path =	$$[QT_INSTALL_PLUGINS]/designer
sources.files = $$SOURCES $$HEADERS *.pro
sources.path =	$$[QT_INSTALL_EXAMPLES]/designer/qledplugin
INSTALLS +=	target
