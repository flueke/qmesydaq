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
#include <QApplication>
#include <QDateTime>
#include "QtInterface.h"
#include "LoopObject.h"
#if defined(_MSC_VER)
    #include "stdafx.h"
#else
    #include <unistd.h>
#endif

/*!
    constructor

    \param receiver
    \param parent
 */
QtInterface::QtInterface(QObject *receiver, QObject *parent)
    : QObject(parent)
    , m_receiver(receiver)
    , m_eventReceived(false)
{
}

/*!
    \fn void QtInterface::setReceiver(QObject *receiver)

    \param receiver
 */
void QtInterface::setReceiver(QObject *receiver)
{
    this->m_receiver = receiver;
}

/*!
    \fn QObject *QtInterface::getReceiver()

    \return the receiver object
 */
QObject *QtInterface::getReceiver()
{
    return this->m_receiver;
}

/*!
    \fn void QtInterface::postEvent(QEvent *event)

    \param event
 */
void QtInterface::postEvent(QEvent *event)
{
    if (this->m_receiver)
        QApplication::postEvent(this->m_receiver, event);
}

/*!
    \fn void QtInterface::postCommand(CommandEvent::Command cmd, QList<QVariant> args)

    \param cmd
    \param args
 */
void QtInterface::postCommand(CommandEvent::Command cmd, QList<QVariant> args)
{
    CommandEvent *newEvent;
    if(args.isEmpty())
        newEvent = new CommandEvent(cmd);
    else
        newEvent = new CommandEvent(cmd, args);
    postEvent(newEvent);
}

/*!
    \fn void QtInterface::waitForEvent()

    handles a sleep to wait for an event as a response to a sent action
  */
void QtInterface::waitForEvent()
{
    LoopObject *loop = dynamic_cast<LoopObject*>(QThread::currentThread());
    QTime tStart=QTime::currentTime();
    for (;;)
    {
        if (m_eventReceived)
        {
            m_eventReceived = false;
            break;
        }
        if (loop)
            loop->pSleep(1);
        else
        {
            int tDiff = tStart.msecsTo(QTime::currentTime());
            if (tDiff < 0)
                tDiff += 86400000;
            if (tDiff > 5000)
                break;
            usleep(1000);
        }
    }
}

/*!
    \fn void QtInterface::postRequestCommand(CommandEvent::Command cmd, QList<QVariant> args)

    \param cmd
    \param args
 */
void QtInterface::postRequestCommand(CommandEvent::Command cmd, QList<QVariant> args)
{
        m_eventReceived = false;
    postCommand(cmd, args);
    waitForEvent();
}

/*!
    \fn void QtInterface::postCommandToInterface(CommandEvent::Command cmd, QList<QVariant> args)

    \param cmd
    \param args
 */
void QtInterface::postCommandToInterface(CommandEvent::Command cmd, QList<QVariant> args)
{
    CommandEvent *newEvent;
    if(args.isEmpty())
        newEvent = new CommandEvent(cmd);
    else
        newEvent = new CommandEvent(cmd, args);
    QApplication::postEvent(this, newEvent);
}
