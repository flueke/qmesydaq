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
	, m_bDoLoop(true)
	, m_width(0)
	, m_height(0)
	, m_status(0)
{
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

double QMesyDAQDetectorInterface::readCounter(int id)
{
	double r(0.0);
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_READ_COUNTER, QList<QVariant>() << id);
	r = m_counter;
	m_mutex.unlock();
	return r;
}

void QMesyDAQDetectorInterface::selectCounter(int id)
{
	postCommand(CommandEvent::C_SELECT_COUNTER,QList<QVariant>() << id);
}

void QMesyDAQDetectorInterface::setPreSelection(double value)
{
        postCommand(CommandEvent::C_SET_PRESELECTION,QList<QVariant>() << value);
}

double QMesyDAQDetectorInterface::preSelection()
{
	double r(0.0);
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_PRESELECTION);
	r=m_preSelection;
	m_mutex.unlock();
	return r;
}

QList<quint32> QMesyDAQDetectorInterface::read()
{
	QList<quint32> rtn;
	for (int i = 0; i < 3; ++i)
		rtn.push_back(1);
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_READ_DIFFRACTOGRAM);
        rtn[0] = m_values.count();
	for (QList<quint64>::const_iterator it = m_values.begin(); it != m_values.end(); ++it)
		rtn.push_back(quint32(*it));
	m_mutex.unlock();
	return rtn;
}

void QMesyDAQDetectorInterface::readHistogramSize(quint16& width, quint16& height)
{
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_READ_HISTOGRAM_SIZE);
	width=m_width;
	height=m_height;
	m_mutex.unlock();
}

QList<quint64> QMesyDAQDetectorInterface::readHistogram()
{
	QList<quint64> r;
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_READ_HISTOGRAM);
	r=m_values;
	m_mutex.unlock();
	return r;
}

QList<quint64> QMesyDAQDetectorInterface::readDiffractogram()
{
	QList<quint64> r;
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_READ_DIFFRACTOGRAM);
	r=m_values;
	m_mutex.unlock();
	return r;
}

QList<quint64> QMesyDAQDetectorInterface::readSpectrogram(int iSpectrogram/*=-1*/)
{
	QList<quint64> r;
	m_mutex.lock();
	if (iSpectrogram>=0)
		postRequestCommand(CommandEvent::C_READ_SPECTROGRAM, QList<QVariant>() << iSpectrogram);
	else
		postRequestCommand(CommandEvent::C_READ_SPECTROGRAM);
	r=m_values;
	m_mutex.unlock();
	return r;
}

int QMesyDAQDetectorInterface::status()
{
	int r(0);
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_STATUS);
	r=m_status;
	m_mutex.unlock();
	return r;
}

void QMesyDAQDetectorInterface::setHistogramFileName(const QString name)
{
	m_histFileName = name;
}

void QMesyDAQDetectorInterface::setListFileName(const QString name)
{
	m_listFileName = name;
}

void QMesyDAQDetectorInterface::setListMode(bool bEnable)
{
	postCommand(CommandEvent::C_SET_LISTMODE,QList<QVariant>() << bEnable);
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
				case CommandEvent::C_READ_DIFFRACTOGRAM:
				case CommandEvent::C_READ_SPECTROGRAM:
					m_values.clear();
					for (QList<QVariant>::const_iterator it = args.begin(); it != args.end(); ++it)
						m_values.push_back(it->toULongLong());
					m_eventReceived = true;
					break;
				case CommandEvent::C_READ_HISTOGRAM:
				{
//! \todo hack to transfer a QList<quint64> to QtInterface without to copy it
#warning TODO hack to transfer a QList<quint64> to QtInterface without to copy it
					QList<quint64>* tmpData=(QList<quint64>*)args[0].toULongLong();
					if (tmpData!=NULL)
					{
						m_values=*tmpData;
						delete tmpData;
					}
					else
						m_values.clear();
					m_eventReceived = true;
					break;
				}
				case CommandEvent::C_READ_HISTOGRAM_SIZE:
				{
					int i=0;
					for (QList<QVariant>::const_iterator it = args.begin(); it != args.end(); ++it, ++i)
					{
						switch (i)
						{
							case 0: m_width=it->toUInt(); m_height=0; break;
							case 1: m_height=it->toUInt(); break;
							default: break;
						}
					}
					m_eventReceived = true;
					break;
				}
				case CommandEvent::C_STATUS:
					m_status = args[0].toInt();
					m_eventReceived = true;
					break;
				case CommandEvent::C_READ_COUNTER:
					m_counter = args[0].toDouble();
					m_eventReceived = true;
					break;
				default:
					break;
			}
		}
		else
			if (cmd == CommandEvent::C_QUIT)
				m_bDoLoop = false;
	}
}
