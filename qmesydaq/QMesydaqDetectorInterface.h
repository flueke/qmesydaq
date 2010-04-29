/***************************************************************************
 *   Copyright (C) 2002 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2008 by Lutz Rossa <rossa@hmi.de>                       *
 *   Copyright (C) 2009-2010 by Jens Krüger <jens.krueger@frm2.tum.de>     *
 *   Copyright (C) 2010 by Alexander Lenz <alexander.lenz@frm2.tum.de>     *
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

#ifndef MESYDAQ_DETECTOR_INTERFACE_H
#define MESYDAQ_DETECTOR_INTERFACE_H

#include "QtInterface.h"

#include "CommandEvent.h"

#if SIZEOF_UNSIGNED_LONG == 4
typedef unsigned long DevULong;
#else
typedef unsigned int DevULong;
#endif

#include <vector>

class QMesyDAQDetectorInterface : public QtInterface
{
	Q_OBJECT
	Q_PROPERTY(QString listFileName READ getListFileName WRITE setListFileName)
	Q_PROPERTY(QString histFileName READ getHistogramFileName WRITE setHistogramFileName)
public:
	QMesyDAQDetectorInterface();

	void start();
	void stop();
	void clear();
	void resume();
	void setPreSelection(double);
	double preSelection();
	std::vector<DevULong> read();
	int status();

        QString getListFileName(void) const {return m_listFileName;} 

        void setListFileName(const QString name);

        QString getHistogramFileName(void) const {return m_histFileName;}

        void setHistogramFileName(const QString name);

protected:
	void customEvent(QEvent *);

	void postCommand(CommandEvent::Command,QList<QVariant> = QList<QVariant>());

	void postRequestCommand(CommandEvent::Command,QList<QVariant> = QList<QVariant>());

	void waitForEvent();

protected:
	double 	m_preSelection;

	double 	m_value;

	double 	m_status;

	bool 	m_eventReceived;

	QString	m_listFileName;

	QString m_histFileName;
};

#endif // MESYDAQDETECTORQTINTERFACE_H
