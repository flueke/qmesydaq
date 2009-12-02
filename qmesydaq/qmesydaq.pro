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

include(../qmesydaq.pri)

TEMPLATE 	= app
TARGET 		= qmesydaq
DEPENDPATH 	+= . ../lib
INCLUDEPATH 	+= . ../lib ..

INSTALLS	= target

target.path	= /usr/local/bin

QT 		+= network svg

# Input
HEADERS 	+= mainwidget.h \
		mainwindow.h \
		mesydaqdata.h \
		controlinterface.h \
		ModuleSpinBox.h \
		MCPDSpinBox.h

FORMS 		+= mesydaq2mainwidget.ui \
		mesydaq2mainwindow.ui

SOURCES 	+= main.cpp \
		mainwidget.cpp \
		mainwindow.cpp \
		mesydaqdata.cpp \
		controlinterface.cpp \
		ModuleSpinBox.cpp \
		MCPDSpinBox.cpp

RESOURCES 	+= images.qrc

DISTFILES	+= images/mesytec.jpg

