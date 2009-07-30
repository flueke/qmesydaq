/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009 by Jens Kr�ger <jens.krueger@frm2.tum.de>          *
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
	//! constructor
	MesydaqCounter(); 

	/** 
	 * increments the counter if not limit reached and emits the signal
	 * stop() if the limit is reached
	 */
	void inc(void);

	//! checks whether the the limit is reached or not
	bool isStopped(void);

	/**
	 * sets the counter value
	 * 
	 * \param val new counter value
	 */
	void set(quint64 val) {m_value = val;}
	
	//! sets the counter value to zero
	void reset(void) {set(0);}

	/**
	 * Sets the counter limit. If this limit will be reached the signal
	 * stop() will be emitted. 
	 *
	 * \param val counter limit, if the value is zero no limit will be set
	 */
	void setLimit(quint64 val) {m_limit = val;}

	//! \return the current counter limit if limit is 0 no limit is set
	quint64 limit(void) {return m_limit;}

	//! \return current counter value
	quint64 value(void) {return m_value;}

signals:
	//! the counter has to be stopped
	void stop();

private:
	//! current counter value
	quint64 m_value;

	//! current counter limit
	quint64 m_limit;
};


#endif // COUNTER_H
