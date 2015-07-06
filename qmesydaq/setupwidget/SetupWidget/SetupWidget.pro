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


TEMPLATE = app
TARGET = 
DEPENDPATH += . src res
INCLUDEPATH += . ../GraphicsSceneWidget
DESTDIR = bin
LIBS = -lGraphicsSceneWidget -L../GraphicsSceneWidget/bin
OBJECTS_DIR = .obj
MOC_DIR = .moc

# Input
SOURCES += \
    mcpd_8.cpp \
    mpsd_8.cpp \
    maindialog.cpp \
    mcpd_8inputdialog.cpp \
    mpsd_8inputdialog.cpp \
    channel.cpp \
    channelinputdialog.cpp \
    canvas.cpp \
    main.cpp \
    pc.cpp \
    pcinputdialog.cpp \
    setupview.cpp \
    addtreedialog.cpp

HEADERS += \
    mcpd_8.h \
    mpsd_8.h \
    maindialog.h \
    mcpd_8inputdialog.h \
    mpsd_8inputdialog.h \
    channel.h \
    channelinputdialog.h \
    canvas.h \
    pc.h \
    pcinputdialog.h \
    setupview.h \
    addtreedialog.h

RESOURCES += \
    res/Icons.qrc

CONFIG += debug
