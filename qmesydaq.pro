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

include (qmesydaqconfig.pri)

TEMPLATE 	= subdirs
CONFIG		+= ordered

SUBDIRS		+= lib interfaces qmesydaq tools test


TARGET = 	
DEPENDPATH 	+= . lib qmesydaq
INCLUDEPATH 	+= . qmesydaq lib

DISTFILES	+= AUTHORS \
		COPYING \
		qmesydaq.desktop \
		qmesydaqconfig.pri

dox.target = doc
dox.commands = doxygen Doxyfile; \
    test -d doxydoc/html/images || mkdir doxydoc/html/images; \
    cp documentation/images/* doxydoc/html/images
dox.depends = 

QMAKE_EXTRA_TARGETS += dox

package.target	= dist
package.commands = for i in $${SUBDIRS} ; do echo \$i; done && touch qmesydaq-$${VERSION}.tar.gz
package.depends = $${DISTFILES}

QMAKE_EXTRA_TARGETS += package

