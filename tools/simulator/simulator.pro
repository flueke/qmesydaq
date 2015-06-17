############################################################################
#   Copyright (C) 2013-2014 by Lutz Rossa <rossa@helmholtz-berlin.de>
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

VERSION		= 0.14.0
SRCBASE		= ../..

include($${SRCBASE}/mesydaqconfig.pri)

TEMPLATE	= app
TARGET		= simulator

QT		+= core network

CONFIG		-= release warn_off
CONFIG		+= debug warn_on console
CONFIG		+= link_prl

DEPENDPATH	+= . $${SRCBASE}/lib
INCLUDEPATH	+= . $${SRCBASE}/lib

LIBS		+= -L$${SRCBASE}/lib

unix!macx:	LIBS	+= -lrt

SOURCES		+= main.cpp simmcpd8.cpp simapp.cpp
HEADERS		+= utils.h simmcpd8.h simapp.h

INSTALLS	= target

target.path	= $${TARGETPATH}/bin

LIBS		+= $${MESYDAQ_LIBS}
