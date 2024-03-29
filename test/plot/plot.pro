############################################################################
#   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>
#   Copyright (C) 2009-2020 by Jens Krüger <jens.krueger@frm2.tum.de>
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

VERSION		= 1.0.1
SRCBASE		= ../..

include ($${SRCBASE}/mesydaqconfig.pri)

TEMPLATE 	= app
TARGET 		= plot
DEPENDPATH 	+= . $${SRCBASE}/qmesydaq $${SRCBASE}/lib
INCLUDEPATH 	+= . $${SRCBASE}/qmesydaq $${SRCBASE}/lib
CONFIG		+= link_prl

QT		+= widgets

SOURCES 	+= main.cpp \
		mainwindow.cpp \
		testdata.cpp \
		plotwidget.cpp \
		$${SRCBASE}/qmesydaq/colormaps.cpp \
		$${SRCBASE}/qmesydaq/plot.cpp \
		$${SRCBASE}/qmesydaq/zoomer.cpp \
		$${SRCBASE}/qmesydaq/data.cpp \
		$${SRCBASE}/qmesydaq/mesydaqdata.cpp

HEADERS 	+= testdata.h \
		mainwindow.h \
		plotwidget.h \
		colormaps.h \
		$${SRCBASE}/qmesydaq/plot.h

CONFIG		+= debug

FORMS		+= plotwidget.ui

INSTALLS	=

LIBS		+= $${QWTLIBS} -L$${SRCBASE}/lib $${MESYDAQ_LIBS}

unix {
	COMPILER_VERSION = V$$system($$QMAKE_CXX " -dumpversion")
	ver =  $$find(COMPILER_VERSION, "V7")$$find(COMPILER_VERSION, "V8") $$find(COMPILER_VERSION, "V9")
	!count(ver, 0) {
		QMAKE_CXXFLAGS += -Wimplicit-fallthrough=1
	}
}
