/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2012 by Jens Krüger <jens.krueger@frm2.tum.de>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "colormaps.h"

StdLinColorMap :: StdLinColorMap()
	: QwtLinearColorMap(Qt::darkBlue, Qt::darkRed)
{
	addColorStop(0.143, Qt::blue);
	addColorStop(0.286, Qt::darkCyan);
	addColorStop(0.429, Qt::cyan);
	addColorStop(0.572, Qt::green);
	addColorStop(0.715, Qt::yellow);
	addColorStop(0.858, Qt::red);
}

StdLogColorMap::StdLogColorMap()
	: QwtLinearColorMap(Qt::darkBlue, Qt::darkRed)
{
	addColorStop(0.139, Qt::blue);
	addColorStop(0.193, Qt::darkCyan);
	addColorStop(0.269, Qt::cyan);
	addColorStop(0.373, Qt::green);
	addColorStop(0.519, Qt::yellow);
	addColorStop(0.721, Qt::red);
}

