/***************************************************************************
 *   Copyright (C) 2002 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2008 by Lutz Rossa <rossa@hmi.de>                       *
 *   Copyright (C) 2009-2010 by Jens Kr�ger <jens.krueger@frm2.tum.de>     *
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

#ifndef TACOEVENT_H
#define TACOEVENT_H

#include <QEvent>
#include <QVariant>
#include <QList>

class CommandEvent : public QEvent
{
public:
	enum Command{
                C_START,
                C_STOP,
                C_CLEAR,
                C_RESUME,
                C_SET_PRESELECTION,
                C_PRESELECTION,
                C_READ_DIFFRACTOGRAM,
                C_STATUS,
		C_READ_HISTOGRAM_SIZE,
		C_READ_HISTOGRAM,
		C_READ_SPECTROGRAM,
		C_READ_COUNTER,
		C_SELECT_COUNTER,
		C_SET_LISTMODE,
		C_QUIT
        };

	CommandEvent(Command command, QList<QVariant> args = QList<QVariant>());

	Command getCommand() const { return m_command; }
	QList<QVariant> getArgs() const { return m_args; }

private:
	Command 	m_command;
	QList<QVariant> m_args;
};

#endif // TACOEVENT_H
