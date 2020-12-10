############################################################################
#   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>
#   Copyright (C) 2009-2014 by Jens Krüger <jens.krueger@frm2.tum.de>
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

exists(.git) {
	system(git describe > version)
}
unix:VERSION		= $$system(cat version)
win32:VERSION		= $$system(type version)

include (mesydaqconfig.pri)

TEMPLATE 	= subdirs
CONFIG		+= ordered edist debug_and_release

SUBDIRS		+= lib interfaces tools

isEmpty(QLEDLIBS) {
	SUBDIRS		+= qled
}

SUBDIRS		+= qmesydaq test

TARGET		= mesydaq

DEPENDPATH 	+= . lib qmesydaq
INCLUDEPATH 	+= . qmesydaq lib

DISTFILES	+= AUTHORS \
		COPYING \
		NEWS \
		README \
		TODO \
		qmesydaq.desktop \
		mesydaq.pro \
		mesydaqconfig.pri \
		QMesyDAQ.sln \
		config.h \
		Doxyfile \
		edist.prf

dox.target = doc
dox.commands = doxygen Doxyfile; \
    test -d doxydoc/html/images || mkdir doxydoc/html/images; \
    cp documentation/images/* doxydoc/html/images
dox.depends =

QMAKE_EXTRA_TARGETS += dox

package.target	= dist
unix: package.commands = @git archive --prefix=qmesydaq-$${VERSION}/ --format=tar HEAD $${SUBDIRS} debian doc misc $${DISTFILES} > qmesydaq-$${VERSION}.tar && \
         mkdir qmesydaq-$${VERSION} && cp -a version qmesydaq-$${VERSION} && tar rf qmesydaq-$${VERSION}.tar qmesydaq-$${VERSION}/version && gzip -f qmesydaq-$${VERSION}.tar && \
	 rm -rf qmesydaq-$${VERSION} || \
	 echo \"dist is currently only enable for git checkouts\"
#	 for i in $${SUBDIRS} ; do echo $${LITERAL_DOLLAR}$${LITERAL_DOLLAR}i; done && touch qmesydaq-$${VERSION}.tar.gz
package.depends = $${DISTFILES}

QMAKE_EXTRA_TARGETS += package

! isEmpty(LIBS) {
	error("Please do not set the LIBS variable : " $${LIBS})
}

