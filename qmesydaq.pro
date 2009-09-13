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

TEMPLATE 	= app
TARGET 		= qmesydaq
DEPENDPATH 	+= .
INCLUDEPATH 	+= .
VERSION 	= 0.0.0
DEFINES		+= VERSION=\\\"$${VERSION}\\\" HAVE_CONFIG_H

QT 		+= qt3support network

QWT_ROOT 	= /usr/local/qwt5

QWTLIB 		= qwt

INTERFACES	= 

INCLUDEPATH 	+= $${QWT_ROOT}/include 
DEPENDPATH  	+= $${QWT_ROOT}/include 
LIBS        	+= -L$${QWT_ROOT}/lib -l$${QWTLIB} 

# Input
HEADERS 	+= mesydaqobject.h \
		histogram.h \
		mainwidget.h \
		mainwindow.h \
		mcpd8.h \
		mdefines.h \
		measurement.h \
		mesydaq2.h \
		mpsd8.h \
		networkdevice.h \
		structures.h \
		mesydaqdata.h \
		counter.h \
		controlinterface.h 

FORMS 		+= mesydaq2mainwidget.ui \
		mesydaq2mainwindow.ui

SOURCES 	+= mesydaqobject.cpp \
		histogram.cpp \
		main.cpp \
		mainwidget.cpp \
		mainwindow.cpp \
		mcpd8.cpp \
		measurement.cpp \
		mesydaq2.cpp \
		mpsd8.cpp \
		mpsd8p.cpp \
		networkdevice.cpp \
		mesydaqdata.cpp \
		counter.cpp \
		controlinterface.cpp 

RESOURCES 	+= images.qrc

DISTFILES	+= qmesydaq.desktop

contains(INTERFACES, TACO) {
DEFINES		+= TACO=1
TACO_ROOT	= /opt/taco

INCLUDEPATH 	+= $${TACO_ROOT}/include
DEPENDPATH  	+= $${TACO_ROOT}/include
LIBS		+= -L$${TACO_ROOT}/lib -ltaco++ -llog4taco -llog4cpp -lTACOExtensions

TACO_ROOT	= /opt/taco

HEADERS		+= tacocontrol.h \
		tacothread.h

SOURCES		+= tacocontrol.cpp \
		tacothread.cpp \
		startup.cpp \
		MesyDAQDetectorDetector.cpp \
		MesyDAQDetectorDetectorImpl.cpp 
}

contains(INTERFACES, CARESS) {
DEFINES		+= CARESS=1

HEADERS		+= corbathread.h \
		caresscontrol.h 

SOURCES		+= corbathread.cpp \
		caresscontrol.cpp \
		caressmeasurement.cpp 
}

