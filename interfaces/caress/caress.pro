############################################################################
#   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    
#   Copyright (C) 2009-2012 by Jens Krüger <jens.krueger@frm2.tum.de>          
#   Copyright (C) 2011-2013 by Lutz Rossa <lrossa@helmholtz-berlin.de>
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

VERSION 	= 0.3.0
SRCBASE		= ../..

include($${SRCBASE}/mesydaqconfig.pri)

TEMPLATE 	= lib
TARGET 		= caressInterface
DEPENDPATH 	+= . .. $${SRCBASE} $${SRCBASE}/qmesydaq
INCLUDEPATH 	+= . .. $${SRCBASE} $${SRCBASE}/qmesydaq

LIBS		+= $${CARESSLIBS}

target.path	= $${TARGETLIBPATH}

DISTFILES       += corbadevice.idl
QMAKE_DISTCLEAN += corbadevice.h corbadeviceSK.cpp

# Input
HEADERS 	+= CARESSLoop.h corbadevice.h \
    mapcorrectparser.h
SOURCES 	+= CARESSLoop.cpp corbadeviceSK.cpp \
    mapcorrectparser.cpp
OTHER_FILES     += corbadevice.idl

# CORBA interface to C++ compiler
idl1intermediate.target = corbadeviceSK.cpp
idl1intermediate.depends = idl2intermediate
idl2intermediate.target = corbadevice.h
idl2intermediate.commands = omniidl -bcxx -Wbh=.h -Wbs=SK.cpp -Wbd=DynSK.cpp corbadevice.idl

# This variable contains the extra targets that have been added
QMAKE_EXTRA_TARGETS += idl1intermediate idl2intermediate
