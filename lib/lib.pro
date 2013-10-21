############################################################################
#   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>
#   Copyright (C) 2009-2013 by Jens Krüger <jens.krueger@frm2.tum.de>
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


VERSION		= 1.22.5
SRCBASE		= ..

include($${SRCBASE}/mesydaqconfig.pri)

TEMPLATE 	= lib
TARGET 		= mesydaq
DEPENDPATH 	+= .
INCLUDEPATH 	+= .
LIBS		-= -lmesydaq

INSTALLS	= target

target.path	= $${TARGETLIBPATH}

QT 		+= core network

# Input
HEADERS 	+= stdafx.h \
		   libqmesydaq_global.h \
		   counter.h \
		   datarepeater.h \
		   histogram.h \
		   logging.h \
		   mapcorrect.h \
		   usermapcorrect.h \
		   mdllcorrect.h \
		   mappedhistogram.h \
		   mcpd.h \
		   mcpd8.h \
		   mdefines.h \
		   measurement.h \
		   mesydaq2.h \
		   mpsd8.h \
		   mstd16.h \
		   mcpd2.h \
		   mdll.h \
		   networkdevice.h \
        	   structures.h \
		   calibration.h

SOURCES 	+= stdafx.cpp \
		   counter.cpp \
		   datarepeater.cpp \
		   histogram.cpp \
		   logging.cpp \
		   mapcorrect.cpp \
		   usermapcorrect.cpp \
		   mdllcorrect.cpp \
		   mappedhistogram.cpp \
		   mcpd.cpp \
		   mcpd8.cpp \
		   mcpd2.cpp \
		   measurement.cpp \
		   mesydaq2.cpp \
		   mpsd8.cpp \
		   mpsd8p.cpp \
		   mpsd8old.cpp \
		   mpsd8sadc.cpp \
		   mstd16.cpp \
		   mdll.cpp \
		   mpsdfactory.cpp \
		   networkdevice.cpp
