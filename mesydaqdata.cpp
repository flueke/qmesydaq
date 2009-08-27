/***************************************************************************
 *   Copyright (C) 2009 by Jens Krüger <jens.krueger@frm2.tum.de>          *
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

#include "mesydaqdata.h"

MesydaqData::MesydaqData()
	: QwtData()
	, d_size(1)
{
	m_data.clear();
	for (quint32 i = 0; i <= d_size; ++i)
		m_data.append(i * i);
}


QwtData *MesydaqData::copy() const
{
	MesydaqData *tmp = new MesydaqData();
	ulong *t = (ulong *)m_data.data();
	tmp->setData(t, d_size);
	return tmp;
}

double MesydaqData::x(size_t i) const
{
	return i;
}

double MesydaqData::y(size_t i) const
{
	return m_data[i];
}

void MesydaqData::setData(ulong *data, quint32 len)
{
	m_data.clear();
	for (quint32 i = 0; i < len; ++i)
		m_data.append(data[i]);
	d_size = len;
}

quint32 MesydaqData::max(void)
{
	quint32 m = 0;
	for(int i = 0; i < m_data.size(); ++i)
		if (m_data.value(i, 0) > m)
			m = m_data.value(i, 0);
	return m;
}
