############################################################################
#   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>
#   Copyright (C) 2009-2015 by Jens Krüger <jens.krueger@frm2.tum.de>
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

VERSION 	= 1.0.2
SRCBASE		= ../..

include($${SRCBASE}/mesydaqconfig.pri)

CONFIG		+= debug_and_release build_all create_prl
CONFIG		+= link_pkgconfig
PKGCONFIG	= tango

TEMPLATE	= lib
TARGET 		= tangoInterface

target.path	= $${TARGETLIBPATH}

DEPENDPATH 	+= . mlzdevice detectorchannel imagechannel counterchannel timerchannel $${SRCBASE} $${SRCBASE}/qmesydaq
INCLUDEPATH 	+= . mlzdevice detectorchannel imagechannel counterchannel timerchannel $${SRCBASE} $${SRCBASE}/qmesydaq

# Input
HEADERS		+= TANGOLoop.h

SOURCES		+= TANGOLoop.cpp \
		ClassFactory.cpp

HEADERS		+= mlzdevice/MLZDevice.h \
		mlzdevice/MLZDeviceClass.h

SOURCES		+= mlzdevice/MLZDevice.cpp \
		mlzdevice/MLZDeviceClass.cpp \
		mlzdevice/MLZDeviceStateMachine.cpp

HEADERS		+= detectorchannel/DetectorChannel.h \
		detectorchannel/DetectorChannelClass.h \
		counterchannel/CounterChannel.h \
		counterchannel/CounterChannelClass.h \
		imagechannel/ImageChannel.h \
		imagechannel/ImageChannelClass.h \
		timerchannel/TimerChannel.h \
		timerchannel/TimerChannelClass.h

SOURCES		+= \
		timerchannel/TimerChannel.cpp \
		timerchannel/TimerChannelClass.cpp \
		timerchannel/TimerChannelStateMachine.cpp \
		counterchannel/CounterChannel.cpp \
		counterchannel/CounterChannelClass.cpp \
		counterchannel/CounterChannelStateMachine.cpp \
		imagechannel/ImageChannel.cpp \
		imagechannel/ImageChannelClass.cpp \
		imagechannel/ImageChannelStateMachine.cpp \
		detectorchannel/DetectorChannel.cpp \
		detectorchannel/DetectorChannelClass.cpp \
		detectorchannel/DetectorChannelStateMachine.cpp

unix {
QMAKE_CXXFLAGS	+= -Wno-deprecated -Wno-unused-variable
}

target.path =	$$[QT_INSTALL_LIBS]
headers.path =	$$[QT_INSTALL_HEADERS]

INSTALLS	+= target headers
