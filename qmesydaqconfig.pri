############################################################################
#   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    
#   Copyright (C) 2009 by Jens Krüger <jens.krueger@frm2.tum.de>          
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
VERSION 	= 0.0.3 
}
SVNVERSION	= $$system(svnversion .)
DEFINES		+= VERSION=\\\"$${VERSION}\\(r$${SVNVERSION}\\)\\\" HAVE_CONFIG_H

INSTALLS	= target

#
# emtpy
# TACO		work as TACO server
# CARESS	work as CARESS server
# TCP		work as TCP server
#
INTERFACE	= TACO

target.path	= /usr/local

#
# for 64 bit machines add bit64
#
CONFIG		+= debug

# QMAKE_CXXFLAGS	+= -fstack-check
# QMAKE_LFLAGS	+= --stack=0x1000000

QWT_ROOT 	= /usr/local/qwt5
QWTLIB		= qwt

# additional debug messages for QMesyDAQDetectorInterface and CARESS interface
#DEFINES         += DEBUGBUILD

QMESYDAQCONFIG = qmesydaqconfig_$$system(hostname -s).pri
exists($${QMESYDAQCONFIG}) {
  include($${QMESYDAQCONFIG})
}

contains(CONFIG, bit64) {
	DEFINES	+= HAVE_BIT64
	QWTLIBS	= -L$${QWT_ROOT}/lib64
}
else {
	QWTLIBS	= -L$${QWT_ROOT}/lib
}

QWTLIBS		+= -l$${QWTLIB}

INCLUDEPATH 	+= $${QWT_ROOT}/include
DEPENDPATH  	+= $${QWT_ROOT}/include
LIBS        	+= $${QWTLIBS} -L../lib -lmesydaq

contains(INTERFACE, TACO) {
	DEFINES		+= HAVE_CONFIG_H 
	DEFINES		+= USE_TACO=1
	TACO_ROOT	= /opt/taco

	contains(CONFIG, bit64) {
		TACOLIBS	= -L$${TACO_ROOT}/lib64
	}
	else {
		TACOLIBS	= -L$${TACO_ROOT}/lib
	}
	TACOLIBS	+= -ltaco++ -llog4taco -llog4cpp -lTACOExtensions
	INCLUDEPATH 	+= $${TACO_ROOT}/include
	DEPENDPATH  	+= $${TACO_ROOT}/include
	message(build the TACO remote interface)
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
	message(build the CARESS remote interface)
}

interfaces = $$find(INTERFACE, "TACO") $$find(INTERFACE, "CARESS") $$find(INTERFACE, "TCP")
count(interfaces, 2) {
	error(you may either use TACO, TCP, _or_ CARESS or nothing as remote interface)
}
