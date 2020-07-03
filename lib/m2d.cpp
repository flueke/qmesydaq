/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2020 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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

#include "mdll.h"

M2D::M2D(quint8, QObject *parent)
	: QObject(parent)
	, m_active(true)
	, m_histogram(true)
{
}

M2D::~M2D()
{
}

bool M2D::active(void) const
{
	return m_active;
}

void M2D::setActive(bool act)
{
	m_active = act;
}

bool M2D::histogram(void) const
{
	return m_histogram;
}

void M2D::setHistogram(bool histo)
{
	m_histogram = histo;
	if (!histo)
		setActive(false);
}

quint16 M2D::bins() const
{
	return 960;
}

quint16 M2D::channels() const
{
	return 960;
}

quint8 M2D::busNumber(void) const
{
	return 0;
}

quint16 M2D::capabilities(void) const
{
	return TPA;
}

bool M2D::online(void) const
{
	return true;
}
