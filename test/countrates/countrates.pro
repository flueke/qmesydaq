# -*- mode: sh -*- ################################################
# Qwt Widget Library
# Copyright (C) 1997   Josef Wilgen
# Copyright (C) 2002   Uwe Rathmann
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the Qwt License, Version 1.0
###################################################################

include( ../../mesydaqconfig.pri )

TARGET       	= countrates

INSTALLS	+=

HEADERS 	= \
		rate_plot.h

SOURCES		= \
		rate_plot.cpp \
		main.cpp

