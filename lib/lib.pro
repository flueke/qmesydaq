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

VERSION		= 1.102.4
SRCBASE		= ..

include($${SRCBASE}/mesydaqconfig.pri)

TEMPLATE 	= lib
TARGET 		= mesydaq
DEPENDPATH 	+= .
INCLUDEPATH 	+= .
LIBS		-= -lmesydaq

CONFIG		+= debug_and_release build_all create_prl

win32 {
	VERSION =
	if (win32-msvc*) {
		CONFIG += staticlib
	}
	DEFINES += LIBQMESYDAQ_LIB
}

unix {
	COMPILER_VERSION = V$$system($$QMAKE_CXX " -dumpversion")
	ver = $$find(COMPILER_VERSION, "V7") $$find(COMPILER_VERSION, "V8") $$find(COMPILER_VERSION, "V9")
	!count(ver, 0) {
		QMAKE_CXXFLAGS += -Wimplicit-fallthrough=1
	}
	QMAKE_CXXFLAGS  += -Wno-class-memaccess
}

QT 		+= core network

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
}

# Input
HEADERS 	+= stdafx.h \
		   libqmesydaq_global.h \
		   counter.h \
		   datarepeater.h \
		   spectrum.h \
		   histogram.h \
		   qmlogging.h \
		   mapcorrect.h \
		   usermapcorrect.h \
		   mdllcorrect.h \
		   mappedhistogram.h \
		   mcpd.h \
		   mcpd8.h \
		   mdefines.h \
		   measurement.h \
		   pulsertest.h \
		   detector.h \
		   mpsd8.h \
		   mstd16.h \
		   mcpd2.h \
		   mdll.h \
		   networkdevice.h \
		   structures.h \
		   calibration.h \
		   mappeddetector.h \
		   editormemory.h \
		   streamwriter.h

SOURCES 	+= stdafx.cpp \
		   counter.cpp \
		   datarepeater.cpp \
		   spectrum.cpp \
		   histogram.cpp \
		   qmlogging.cpp \
		   mapcorrect.cpp \
		   linearmapcorrect.cpp \
		   usermapcorrect.cpp \
		   mdllcorrect.cpp \
		   mappedhistogram.cpp \
		   mcpd.cpp \
		   mcpd8.cpp \
		   mcpd2.cpp \
		   measurement.cpp \
		   pulsertest.cpp \
		   detector.cpp \
		   mpsd8.cpp \
		   mpsd8p.cpp \
		   mpsd8old.cpp \
		   mpsd8sadc.cpp \
		   mstd16.cpp \
		   m2d.cpp \
		   mdll.cpp \
		   mwpchr.cpp \
		   nomodule.cpp \
		   mpsdfactory.cpp \
		   m2dfactory.cpp \
		   networkdevice.cpp \
		   mappeddetector.cpp \
		   editormemory.cpp \
		   calibration.cpp \
		   streamwriter.cpp

target.path	= $${TARGETLIBPATH}

INSTALLS	+= target
