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
#ifndef CONTROLINTERFACE_H
#define CONTROLINTERFACE_H

#include "mesydaqobject.h"

class Mesydaq2;

/**
 * \short Interface class for external control (CARESS, TACO, ...)
 *
 * \author Gregor Montermann <g.montermann@mesytec.com>
*/
class ControlInterface : public MesydaqObject
{
	Q_OBJECT
public:
	ControlInterface(QObject *parent = NULL);

	~ControlInterface();

	//! starts the data aquisition
	void start();

	//! stops the data aquisition
	void stop();

	//! continues a stopped data aquisition
	void cont();

	//! clears the histogram
	void clear();

	//! sets the timer preselection
	void setTimePreselection(const double);

	double timePreselection(void) {return m_time;}

	//! sets the monitor preselection
	void setMonitorPreselection(const quint32);

	quint32 monitorPreselection(void) {return m_monitor;}

	//! returns the current state
	int status() {return m_status;}

	virtual void finish();

	virtual QString getListFileName(void)
	{
		return QString(NULL);
	}


	virtual QString getHistogramFileName(void)
	{
		return QString(NULL);
	}

public slots:
	void statusChanged(bool);

	void timePreselectionChanged(double);

	void monitorPreselectionChanged(int);

signals:
	void sigStartStop();

	void sigClear();

	void sigEnableTimer(bool);

	void sigEnableMonitor(bool);

	void sigSetTimer(double);

	void sigSetMonitor(int);

protected:
	//! the object to control the measurement
	Mesydaq2	*m_theApp;

	int		m_status;

	int		m_monitor;

	double		m_time;
};

#endif

