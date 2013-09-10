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
#include "logging.h"
#include "stdafx.h"

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
    \fn bool QtInterface::waitForEvent()

    handles a sleep to wait for an event as a response to a sent action
  */
bool QtInterface::waitForEvent()
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
            {
#ifdef DEBUGBUILD
                Q_ASSERT(false);
#endif
                return false;
            }
            usleep(1000);
        }
    }
    return true;
}

/*!
    \fn void QtInterface::postRequestCommand(CommandEvent::Command cmd, QList<QVariant> args)

    \param cmd
    \param args
 */
void QtInterface::postRequestCommand(CommandEvent::Command cmd, QList<QVariant> args)
{
    CommandEvent *newEvent;
    m_eventReceived = false;
    if(args.isEmpty())
        newEvent = new CommandEvent(cmd);
    else
        newEvent = new CommandEvent(cmd, args);
    postEvent(newEvent);
    if (!waitForEvent())
    {
        const char* szCmd(NULL);
        switch (cmd)
        {
#define CMD(x) case CommandEvent::x: szCmd=#x; break;
            CMD(C_START) CMD(C_STOP) CMD(C_CLEAR) CMD(C_RESUME) CMD(C_SET_PRESELECTION) CMD(C_PRESELECTION)
            CMD(C_READ_DIFFRACTOGRAM) CMD(C_STATUS) CMD(C_READ_HISTOGRAM_SIZE) CMD(C_READ_HISTOGRAM)
            CMD(C_READ_SPECTROGRAM) CMD(C_READ_COUNTER) CMD(C_SELECT_COUNTER) CMD(C_SET_LISTMODE)
            CMD(C_SET_LISTHEADER) CMD(C_MAPCORRECTION) CMD(C_MAPPEDHISTOGRAM) CMD(C_UPDATEMAINWIDGET)
            CMD(C_QUIT) CMD(C_SET_RUNID) CMD(C_GET_RUNID) CMD(C_GET_LISTMODE) CMD(C_COUNTER_SELECTED)
#undef CMD
            default: szCmd="???"; break;
        }
        MSG_FATAL << "timeout for command " << cmd << "(" << szCmd << ")" << args;
#ifdef DEBUGBUILD
        Q_ASSERT(false);
#endif
    }
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
