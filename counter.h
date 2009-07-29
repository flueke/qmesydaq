/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
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
#ifndef COUNTER_H
#define COUNTER_H

#include "mesydaqobject.h"

#include "mdefines.h"

class MesydaqCounter : public MesydaqObject
{
Q_OBJECT

public:
	MesydaqCounter() : MesydaqObject()
		, m_value(0)
		, m_limit(0)
	{
	}

	void inc(void)
	{
		if (!m_limit || m_value < m_limit)
			++m_value;
	}

	bool isStopped(void) 
	{
		if (!m_limit)
			return false;
		return (m_value >= m_limit);
	}

	void set(quint64 val) {m_value = val;}
	
	void reset(void) {set(0);}

	void setLimit(quint64 val) {m_limit = val;}

	quint64 limit(void) {return m_limit;}

	quint64 value(void) {return m_value;}

signals:
	void stop();

private:
	quint64 m_value;

	quint64 m_limit;
};


#endif // COUNTER_H
