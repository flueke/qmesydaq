############################################################################
 #   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 #   Copyright (C) 2009-2012 by Jens Kr�ger <jens.krueger@frm2.tum.de>     *
 #                                                                         *
 #   This program is free software; you can redistribute it and/or modify  *
 #   it under the terms of the GNU General Public License as published by  *
 #   the Free Software Foundation; either version 2 of the License, or     *
 #   (at your option) any later version.                                   *
 #                                                                         *
 #   This program is distributed in the hope that it will be useful,       *
 #   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 #   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 #   GNU General Public License for more details.                          *
 #                                                                         *
 #   You should have received a copy of the GNU General Public License     *
 #   along with this program; if not, write to the                         *
 #   Free Software Foundation, Inc.,                                       *
 #   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
############################################################################

include (../../qmesydaqconfig.pri)

TEMPLATE 	= app
TARGET 		= plot
DEPENDPATH 	+= . ../../qmesydaq
INCLUDEPATH 	+= . ../../qmesydaq

SOURCES 	+= main.cpp \
		plot.cpp \
		mainwindow.cpp \
		zoomer.cpp \
		data.cpp \
		plotwidget.cpp \
		colormaps.cpp

HEADERS 	+= data.h \
		plot.h \
		mainwindow.h \
		zoomer.h \
		plotwidget.h \
		colormaps.h

CONFIG		+= debug

FORMS		+= plotwidget.ui
