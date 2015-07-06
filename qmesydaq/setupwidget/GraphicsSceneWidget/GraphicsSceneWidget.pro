############################################################################
#   Copyright (C) 2013-2015 by Stefan Rainow <stefan.rainow@frm2.tum.de>
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

TEMPLATE = lib
TARGET =
DEPENDPATH += . res 
INCLUDEPATH += .
DESTDIR = bin
MOC_DIR = .moc
OBJECTS_DIR = .obj

CONFIG += debug

# Input
HEADERS += button.h \
           dialog.h \
           dialogbase.h \
           graphicsscene.h \
           inputdialog.h \
           item.h \
           line.h \
           templine.h \
           itemaction.h
SOURCES += button.cpp \
           dialog.cpp \
           dialogbase.cpp \
           graphicsscene.cpp \
           inputdialog.cpp \
           item.cpp \
           line.cpp \
           main.cpp \
           templine.cpp \
           itemaction.cpp
RESOURCES += res/icons.qrc


# Installation
# Libraries
target.path = /usr/local/lib/graphicsscenewidget
INSTALLS += target

# Headers
headers.files = $$HEADERS
headers.path = /usr/local/include/graphicsscenewidget
INSTALLS += headers
