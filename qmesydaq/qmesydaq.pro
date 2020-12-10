############################################################################
#   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>
#   Copyright (C) 2009-2016 by Jens Krüger <jens.krueger@frm2.tum.de>
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the
#   Free Software Foundation, Inc.,
#   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
############################################################################

VERSION		= 0.79.0
SRCBASE		= ..

CONFIG		+= debug_and_release link_prl # build_all

# DEFINES		+= QWT_DLL

include($${SRCBASE}/mesydaqconfig.pri)

isEmpty(QLEDLIBS) {

win32{
CONFIG(debug, release|debug): QLEDLIBS = -L$${SRCBASE}/qled/src/debug -lqledd
CONFIG(release, release|debug): QLEDLIBS = -L$${SRCBASE}/qled/src/release -lqled
}
else:	QLEDLIBS	= -L$${SRCBASE}/qled/src -lqled
	QLEDINCLUDE	= $${SRCBASE}/qled/src
}

unix {
	COMPILER_VERSION = V$$system($$QMAKE_CXX " -dumpversion")
	ver = $$find(COMPILER_VERSION, "V7") $$find(COMPILER_VERSION, "V8") $$find(COMPILER_VERSION, "V9")
	!count(ver, 0) {
		QMAKE_CXXFLAGS += -Wimplicit-fallthrough=1
	}
}

TEMPLATE 	= app
TARGET 		= qmesydaq

DEPENDPATH 	+= . $${SRCBASE}/test/plot $${SRCBASE}/lib
INCLUDEPATH 	+= . $${SRCBASE} $${QLEDINCLUDE}

SUBDIRS		+= diskspace

QT 		+= network svg # webkit

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += printsupport widgets
}

RESOURCES       += qmesydaq.qrc

# Input
HEADERS 	+= mainwidget.h \
		mainwindow.h \
		mesydaqdata.h \
		ModuleSpinBox.h \
		MCPDSpinBox.h \
		ChannelSpinBox.h \
		StatusBarEntry.h \
		LoopObject.h \
		QMesydaqDetectorInterface.h \
		CommandEvent.h \
		QtInterface.h \
		MultipleLoopApplication.h \
		ipaddresswidget.h \
		presetwidget.h \
		doublepresetwidget.h \
		monitorpresetwidget.h \
		generalsetup.h \
		mcpdsetup.h \
		modulestatus.h \
		modulewizard.h \
		moduleidentificationpage.h \
		modulemasterpage.h \
                modulesetup.h \
                mdllsetup.h \
		passworddialog.h \
		plot.h \
		zoomer.h \
		colormaps.h \
		colorwidget.h \
		mpsdpulser.h \
		mdllpulser.h \
		histogrammappingeditor.h \
		histogramedittablewidget.h \
		tacosetup.h \
		tangosetup.h \
		caresssetup.h \
		tcpsetup.h \
		diskspace/diskspace.h \
		data.h \
		ConfigFileDialog.h \
		datetimelabel.h
#		website.h \

FORMS 		+= mainwidget.ui \
		mainwindow.ui \
		generalsetup.ui \
		mcpdsetup.ui \
		monitorpresetwidget.ui \
		doublepresetwidget.ui \
		modulestatus.ui \
		modulewizard.ui \
		moduleidentificationpage.ui \
		modulemasterpage.ui \
		channelhistogramsetup.ui \
                modulesetup.ui \
                mdllsetup.ui \
		statusbarentry.ui \
		passworddialog.ui \
		mpsdpulser.ui \
		mdllpulser.ui \
		histogrammappingeditor.ui \
		tacosetup.ui \
		tangosetup.ui \
		caresssetup.ui \
		tcpsetup.ui \
		ipaddresswidget.ui \
		website.ui

SOURCES 	+= main.cpp \
		mainwindow.cpp \
		mesydaqdata.cpp \
		ModuleSpinBox.cpp \
		MCPDSpinBox.cpp \
		ChannelSpinBox.cpp \
		LoopObject.cpp \
		QMesydaqDetectorInterface.cpp \
		CommandEvent.cpp \
		QtInterface.cpp \
		MultipleLoopApplication.cpp \
		ipaddresswidget.cpp \
		presetwidget.cpp \
		monitorpresetwidget.cpp \
		doublepresetwidget.cpp \
		generalsetup.cpp \
		mcpdsetup.cpp \
		modulesetup.cpp \
                mdllsetup.cpp \
		modulestatus.cpp \
		modulewizard.cpp \
		moduleidentificationpage.cpp \
		modulemasterpage.cpp \
		mainwidget.cpp \
		plot.cpp \
		zoomer.cpp \
		colormaps.cpp \
		colorwidget.cpp \
		mpsdpulser.cpp \
		mdllpulser.cpp \
		histogrammappingeditor.cpp \
		histogramedittablewidget.cpp \
		tacosetup.cpp \
		tangosetup.cpp \
		caresssetup.cpp \
		tcpsetup.cpp \
		diskspace/diskspace.cpp \
		data.cpp \
		ConfigFileDialog.cpp \
		datetimelabel.cpp
#		website.cpp \

DISTFILES	+= images/mesytec.jpg \
		images/mesylogo_200x95_yellow.png

! isEmpty(LIBS) {
	error("Please do not set the LIBS variable : " $${LIBS})
}

contains(INTERFACE, TACO) {
	DEPENDPATH	+= $${SRCBASE}/interfaces/taco
	INCLUDEPATH	+= $${SRCBASE}/interfaces/taco
	CONFIG		+= link_pkgconfig
	PKGCONFIG	= taco
	LIBS		+= -L$${SRCBASE}/interfaces/taco -ltacoInterface $${TACOLIBS} $${LIBS}
}

contains(INTERFACE, TANGO) {
	DEPENDPATH	+= $${SRCBASE}/interfaces/tango
	INCLUDEPATH	+= $${SRCBASE}/interfaces/tango
	INCLUDEPATH	+= $${SRCBASE}/interfaces/tango
	LIBS		+= -L$${SRCBASE}/interfaces/tango -ltangoInterface $${LIBS}
	CONFIG		+= link_pkgconfig
	PKGCONFIG	= tango
	QMAKE_CXXFLAGS	+= -Wno-class-memaccess -Wno-deprecated
}

contains(INTERFACE, CARESS) {
	DEPENDPATH	+= $${SRCBASE}/interfaces/caress
	INCLUDEPATH	+= $${SRCBASE}/interfaces/caress
	LIBS		+= -L$${SRCBASE}/interfaces/caress -lcaressInterface $${LIBS}
}

contains(INTERFACE, TCP) {
	DEPENDPATH	+= $${SRCBASE}/interfaces/tcp
	INCLUDEPATH	+= $${SRCBASE}/interfaces/tcp
	LIBS		+= -L$${SRCBASE}/interfaces/tcp -ltcpInterface $${LIBS}
}

INCLUDEPATH	+= diskspace
DEPENDPATH	+= diskspace

unix {
isEmpty(BOOST_LIBS) {
	BOOST_LIBS	=  -lboost_filesystem -lboost_system
}
}

LIBS		+= $${QLEDLIBS}
LIBS		+= $${MESYDAQ_LIBS}
LIBS		+= $${QWTLIBS}

unix:LIBS	+= $${BOOST_LIBS}

unix {
	target.path	= $${TARGETPATH}/bin
}

INSTALLS	+= target
