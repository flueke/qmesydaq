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

VERSION		= 1.0.0
SRCBASE		= ../..

include($${SRCBASE}/mesydaqconfig.pri)

TEMPLATE 	= app
TARGET 		= mstd16

QT		+= core network

CONFIG		+= debug console link_prl

DEPENDPATH 	+= . $${SRCBASE}/lib
INCLUDEPATH 	+= . $${SRCBASE}/lib

LIBS		+= -L$${SRCBASE}/lib -lmesydaq

# Input
SOURCES 	+= main.cpp \
		   readfile.cpp

INSTALLS        = target

target.path     = $${TARGETPATH}/bin

LIBS		+= $${MESYDAQ_LIBS}
