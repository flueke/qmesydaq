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
#	warning HAVE_CONFIG_H
#endif

#include "QMesydaqDetectorInterface.h"

#include "CommandEvent.h"
#include "LoopObject.h"

#include <iostream>
#include <QThread>

QMesyDAQDetectorInterface::QMesyDAQDetectorInterface()
{
    //
}

void QMesyDAQDetectorInterface::postCommand(CommandEvent::Command cmd, QList<QVariant> args)
{
	CommandEvent *newEvent;
	if(args.isEmpty())
        	newEvent = new CommandEvent(cmd);
	else
        	newEvent = new CommandEvent(cmd, args);
	postEvent(newEvent);
}

void QMesyDAQDetectorInterface::postRequestCommand(CommandEvent::Command cmd, QList<QVariant> args)
{
	m_eventReceived = false;
	postCommand(cmd, args);
	waitForEvent();
}

void QMesyDAQDetectorInterface::start()
{
	postCommand(CommandEvent::START);
}

void QMesyDAQDetectorInterface::stop()
{
	postCommand(CommandEvent::STOP);
}

void QMesyDAQDetectorInterface::clear()
{
	postCommand(CommandEvent::CLR);
}

void QMesyDAQDetectorInterface::resume()
{
	postCommand(CommandEvent::RESUME);
}

void QMesyDAQDetectorInterface::setPreSelection(double value)
{
	postCommand(CommandEvent::SET_PRESELECTION,QList<QVariant>() << value);
}

double QMesyDAQDetectorInterface::preSelection()
{
	postRequestCommand(CommandEvent::PRESELECTION);
	return m_preSelection;
}

std::vector<DevULong> QMesyDAQDetectorInterface::read()
{
	postRequestCommand(CommandEvent::READ);
	std::vector<DevULong> rtn(3,1);
	rtn.push_back(m_value);
	return rtn;
}

int QMesyDAQDetectorInterface::status()
{
	postRequestCommand(CommandEvent::STATUS);
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
				case CommandEvent::PRESELECTION:
					m_preSelection = args[0].toDouble();
					m_eventReceived = true;
                			break;
				case CommandEvent::READ:
					m_value = args[0].toDouble();
					m_eventReceived = true;
					break;
				case CommandEvent::STATUS:
					m_status = args[0].toInt();
					m_eventReceived = true;
					break;
				default:
					break;
			}
		}
	}
}

void QMesyDAQDetectorInterface::waitForEvent()
{
	m_eventReceived = false;

	while(true)
	{
		if (m_eventReceived)
			break;

		LoopObject *loop = dynamic_cast<LoopObject*>(QThread::currentThread());
		if (loop)
			loop->pSleep(1);
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


