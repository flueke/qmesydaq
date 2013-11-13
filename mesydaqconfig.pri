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

isEmpty(VERSION) {
	VERSION	= 0.0.4 
}
isEmpty(SRCBASE) {
	SRCBASE = .
}

GITVERSION	= $$system(git describe --always)
DEFINES		+= VERSION=\\\"$${VERSION}\\($${GITVERSION}\\)\\\" HAVE_CONFIG_H

INSTALLS	= target

#
# emtpy
# TACO		work as TACO server
# CARESS	work as CARESS server
# TCP		work as TCP server
#
# INTERFACE	= 

#
# for 64 bit machines add bit64
#
CONFIG		+= debug 

# QMAKE_CXXFLAGS	+= -fstack-check
# QMAKE_LFLAGS	+= --stack=0x1000000

# additional debug messages for QMesyDAQDetectorInterface and CARESS interface
# DEFINES         += DEBUGBUILD

QMESYDAQCONFIG = mesydaqconfig_$$system(hostname -s).pri
exists($${QMESYDAQCONFIG}) {
	include($${QMESYDAQCONFIG})
}

isEmpty(TARGETPATH) {
	TARGETPATH	= /usr/local
}

isEmpty(QWT_ROOT) {
	QWT_ROOT 	= /usr/local/qwt-5.2.1
}

isEmpty(QWTINCLUDE) {
	QWTINCLUDE	= /usr/include/qwt-qt4
}

isEmpty(QWTLIB) {
	QWTLIB		= qwt-qt4
}

target.path	= $${TARGETPATH}

contains(CONFIG, bit64) {
	DEFINES	+= HAVE_BIT64
	isEmpty(QWTLIBS) {
		QWTLIBS	= $${QWT_ROOT}/lib64
	}
	isEmpty(TARGETLIBPATH) {
		TARGETLIBPATH = $${TARGETPATH}/lib64
	}
}
else {
	isEmpty(QWTLIBS) {
		QWTLIBS	= $${QWT_ROOT}/lib
	}
	isEmpty(TARGETLIBPATH) {
		TARGETLIBPATH = $${TARGETPATH}/lib
	}
}

QWTLIBS		= -L$${QWTLIBS} -l$${QWTLIB}
QWTINCLUDES 	= $${QWTINCLUDE}

INCLUDEPATH 	+= $${QWTINCLUDES} $${SRCBASE}/lib
DEPENDPATH  	+= $${QWTINCLUDES}
LIBS        	+= $${QWTLIBS} -L$${SRCBASE}/lib -lmesydaq

contains(INTERFACE, TACO) {
	DEFINES		+= HAVE_CONFIG_H 
        DEFINES		+= USE_TACO=1
	isEmpty(TACO_ROOT) {
		TACO_ROOT	= /opt/taco
	}

	isEmpty(TACOLIBS) {
		contains(CONFIG, bit64) {
			TACOLIBS	= -L$${TACO_ROOT}/lib64
		}
		else {
			TACOLIBS	= -L$${TACO_ROOT}/lib
		}
	}
	TACOLIBS	+= -lTACOExtensions -ltaco++ -llog4taco -llog4cpp
	INCLUDEPATH 	+= $${TACO_ROOT}/include
	DEPENDPATH  	+= $${TACO_ROOT}/include
	message("build the TACO remote interface")
}

contains(INTERFACE, CARESS) {
# the CARESS interface needs the omniORB CORBA implementation
#   please look at  http://omniorb.sourceforge.net/
#
# if you have a omniORB 4.x installation on non-standard directories,
# point the variables INCLUDEPATH and CARESSLIBS to the correct directories
# note: Debian and Ubuntu have prebuild packages for omniORB and SuSE can
#       build a RPM package with command rpmbuild and the omniORB sources

	DEFINES		+= USE_CARESS=1
#	INCLUDEPATH	+= $${OMNIORB_ROOT}/include
#	CARESSLIBS	+= -L$${OMNIORB_ROOT}/lib
	CARESSLIBS	= -lomniDynamic4 -lomniORB4 -lomnithread -lz
	message("build the CARESS remote interface")
}

interfaces = $$find(INTERFACE, "TACO") $$find(INTERFACE, "CARESS") $$find(INTERFACE, "TCP")
!count(interfaces, 0) {
        !count(interfaces, 1) {
		error("you may either use TACO, TCP, _or_ CARESS or nothing as remote interface")
	}
}

UI_DIR		= .ui
MOC_DIR		= .moc
OBJECTS_DIR	= .obj
