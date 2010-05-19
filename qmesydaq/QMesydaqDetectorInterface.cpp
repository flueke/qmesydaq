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

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include "QMesydaqDetectorInterface.h"

#include "CommandEvent.h"
#include "LoopObject.h"

#include <iostream>
#include <QThread>

QMesyDAQDetectorInterface::QMesyDAQDetectorInterface(QObject *receiver, QObject *parent)
    	: QtInterface(receiver, parent)
	, m_status(0)
{
    //
}

void QMesyDAQDetectorInterface::start()
{
        postCommand(CommandEvent::C_START);
}

void QMesyDAQDetectorInterface::stop()
{
        postCommand(CommandEvent::C_STOP);
}

void QMesyDAQDetectorInterface::clear()
{
        postCommand(CommandEvent::C_CLEAR);
}

void QMesyDAQDetectorInterface::resume()
{
        postCommand(CommandEvent::C_RESUME);
}

void QMesyDAQDetectorInterface::setPreSelection(double value)
{
        postCommand(CommandEvent::C_SET_PRESELECTION,QList<QVariant>() << value);
}

double QMesyDAQDetectorInterface::preSelection()
{
        postRequestCommand(CommandEvent::C_PRESELECTION);
	return m_preSelection;
}

std::vector<DevULong> QMesyDAQDetectorInterface::read()
{
        postRequestCommand(CommandEvent::C_READ);
	std::vector<DevULong> rtn(3,1);
	rtn[0] = m_values.length();
	for (QList<unsigned long>::const_iterator it = m_values.begin(); it != m_values.end(); ++it)
		rtn.push_back(*it);
	return rtn;
}

int QMesyDAQDetectorInterface::status()
{
        postRequestCommand(CommandEvent::C_STATUS);
	return m_status;
}

void QMesyDAQDetectorInterface::customEvent(QEvent *e)
{
	CommandEvent *event = dynamic_cast<CommandEvent*>(e);
	if (!event)
	{
		QtInterface::customEvent(e);
		return;
	}
	else
	{
		CommandEvent::Command cmd = event->getCommand();
		QList<QVariant> args = event->getArgs();

		if (!args.isEmpty())
		{
			switch(cmd)
			{
                                case CommandEvent::C_PRESELECTION:
					m_preSelection = args[0].toDouble();
					m_eventReceived = true;
                			break;
                                case CommandEvent::C_READ:
					m_values.clear();
					for (QList<QVariant>::const_iterator it = args.begin(); it != args.end(); ++it)
						m_values.push_back((*it).toUInt());
					m_eventReceived = true;
					break;
                                case CommandEvent::C_STATUS:
					m_status = args[0].toInt();
					m_eventReceived = true;
					break;
				default:
					break;
			}
		}
	}
}

void QMesyDAQDetectorInterface::setHistogramFileName(const QString name)
{
        m_histFileName = name;
}

void QMesyDAQDetectorInterface::setListFileName(const QString name)
{
        m_listFileName = name;
}


