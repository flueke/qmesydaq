############################################################################
#   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    
#   Copyright (C) 2009 by Jens Kr�ger <jens.krueger@frm2.tum.de>          
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

include( ../qmesydaq.pri )

TEMPLATE 	= subdirs


TARGET = 	
DEPENDPATH 	+= . lib qmesydaq
INCLUDEPATH 	+= . qmesydaq lib

DISTFILES	+= AUTHOR COPYING qmesydaq.desktop

contains(INTERFACE, TACO) {
SUBDIRS		+= taco 
}

contains(INTERFACE, CARESS) {
SUBDIRS		+= caress 
}