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
#include "controlinterface.h"
#include "mesydaq2.h"
#include "mdefines.h"

/**
 * constructor
 *
 * \param parent parant object
 */

ControlInterface	*globalControlInterface = NULL;

ControlInterface::ControlInterface(QObject *parent)
	: MesydaqObject(parent)
	, m_status(false)
	, m_monitor(0)
	, m_time(0.0)
{
	protocol(tr("Initialize remote control interface"));
	m_theApp = reinterpret_cast<Mesydaq2*>(parent);
}

ControlInterface::~ControlInterface()
{
}

void ControlInterface::finish()
{
}

void ControlInterface::start()
{
	if (!m_status)
		emit sigStartStop();
}

void ControlInterface::stop()
{
	if (m_status)
		emit sigStartStop();
}

void ControlInterface::cont()
{
	if (!m_status)
		emit sigStartStop();
}

void ControlInterface::clear()
{
	emit sigClear();
}

void ControlInterface::setTimePreselection(const double time)
{
	emit sigEnableTimer(true);
	emit sigSetTimer(time);
}
	
void ControlInterface::setMonitorPreselection(const quint32 counts)
{
	emit sigEnableMonitor(true);
	emit sigSetMonitor(counts);
}

void ControlInterface::statusChanged(bool status)
{
	m_status = status;
}

void ControlInterface::timePreselectionChanged(double time)
{
	m_time = time;
}

void ControlInterface::monitorPreselectionChanged(int monitor)
{
	m_monitor = monitor;
}
