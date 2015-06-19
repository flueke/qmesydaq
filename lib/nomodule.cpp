/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2015 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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

#include "mpsd8.h"

NoModule::NoModule(quint8 id, QObject *parent)
	: MPSD8(id, parent)
{
	m_mpsdId = 0;
	setHistogram(false);
}

void NoModule::setActive(bool)
{
	MPSD8::setActive(false);
}

void NoModule::setActive(quint16 chan, bool)
{
	MPSD8::setActive(chan, false);
}

QString NoModule::getType(void) const
{
	return tr("-");
}

int NoModule::type(void) const
{
	return TYPE_NOMODULE;
}

bool NoModule::online(void) const
{
	return false;
}
