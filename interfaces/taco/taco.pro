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

VERSION 	= 1.13.0
SRCBASE		= ../..

include($${SRCBASE}/mesydaqconfig.pri)

TEMPLATE 	= lib
TARGET 		= tacoInterface
DEPENDPATH 	+= . .. $${SRCBASE} $${SRCBASE}/qmesydaq
INCLUDEPATH 	+= . .. $${SRCBASE} $${SRCBASE}/qmesydaq

LIBS		+= $${TACOLIBS}

CONFIG		+= debug_and_release build_all create_prl

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
}

# Input
HEADERS 	+= MesyDAQDetectorDetector.h \
		MesyDAQIOCounter.h \
		MesyDAQIOTimer.h \
		MesyDAQServer.h \
		MesyDAQBase.h \
		TACOLoop.h \
		TACOQMesydaqInterface.h \
		taco_utils.h

SOURCES 	+= MesyDAQDetectorDetector.cpp \
           	MesyDAQDetectorDetectorImpl.cpp \
		MesyDAQIOCounter.cpp \
		MesyDAQIOCounterImpl.cpp \
		MesyDAQIOTimer.cpp \
		MesyDAQIOTimerImpl.cpp \
		MesyDAQBase.cpp \
		MesyDAQBaseImpl.cpp \
		startup.cpp \
		TACOLoop.cpp \
		device_server.c

target.path	= $${TARGETLIBPATH}
headers.path	= $$[QT_INSTALL_HEADERS]

INSTALLS	+= target headers
