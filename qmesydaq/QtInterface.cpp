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

#include "QtInterface.h"

#include "QApplication"
#include"LoopObject.h"

QtInterface::QtInterface(QObject *receiver, QObject *parent)
    : QObject(parent)
    , m_receiver(receiver)
{
}

void QtInterface::setReceiver(QObject *receiver)
{
    this->m_receiver = receiver;
}

QObject *QtInterface::getReceiver()
{
    return this->m_receiver;
}

void QtInterface::postEvent(QEvent *event)
{
    if (this->m_receiver)
        QApplication::postEvent(this->m_receiver, event);
}

void QtInterface::postCommand(CommandEvent::Command cmd, QList<QVariant> args)
{
    CommandEvent *newEvent;
    if(args.isEmpty())
        newEvent = new CommandEvent(cmd);
    else
        newEvent = new CommandEvent(cmd, args);
    postEvent(newEvent);
}

void QtInterface::waitForEvent()
{
        m_eventReceived = false;

        while(true)
        {
                if (m_eventReceived)
                        break;

                LoopObject *loop = dynamic_cast<LoopObject*>(QThread::currentThread());
                if (loop)
                        loop->pSleep(1);
        	QApplication::processEvents();
        }
}

void QtInterface::postRequestCommand(CommandEvent::Command cmd, QList<QVariant> args)
{
        m_eventReceived = false;
        postCommand(cmd, args);
        waitForEvent();
}

void QtInterface::postCommandToInterface(CommandEvent::Command cmd, QList<QVariant> args)
{
    CommandEvent *newEvent;
    if(args.isEmpty())
        newEvent = new CommandEvent(cmd);
    else
        newEvent = new CommandEvent(cmd, args);
    QApplication::postEvent(this, newEvent);
}
