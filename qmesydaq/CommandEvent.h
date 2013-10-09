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

#ifndef __COMMANDEVENT_H__
#define __COMMANDEVENT_H__

#include <QEvent>
#include <QVariant>
#include <QList>

/*!
    \class CommandEvent

    \short This class handles commands sent via the remote interfaces

    \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class CommandEvent : public QEvent
{
public:
	//! definitions of the commands
	enum Command{
		C_START,               // start acq; no parameters
		C_STOP,                // stop/pause acq; no parameters
		C_CLEAR,               // clear all data; no parameters
		C_RESUME,              // resume acq; no parameters
		C_SET_PRESELECTION,    // select preset value (and counter); args: preset value, counter id
		C_PRESELECTION,        // get preset value; optional args: counter id; result: preset value
		C_READ_DIFFRACTOGRAM,  // read diffractogram; no args; result: diffractogram as array of numbers
		C_STATUS,              // read acq status; no args; result: acquisition active software, hardware
		C_READ_HISTOGRAM_SIZE, // read histogram size; no args; result: width, height
		C_READ_HISTOGRAM,      // read histogram; no args; result: histogram as array of numbers
		C_READ_SPECTROGRAM,    // read spectrogram; no args; result: spectrogram as array of numbers
		C_READ_COUNTER,        // read single counter value; args: counter id; result: counter value
		C_SELECT_COUNTER,      // select preset counter; args: counter id (and preset value)
		C_SET_LISTMODE,        // select list mode; args: list mode enabled (and write protect data files)
		C_SET_LISTHEADER,      // set header for list mode; args: pointer to data and length
		C_MAPCORRECTION,       // read/set map correction; optional arg: new correction; result: current correction
		C_MAPPEDHISTOGRAM,     // read corrected histogram; result: pointer to histogram class
		C_UPDATEMAINWIDGET,    // set data for CARESS tab in main widget; args: width, height, output file
		C_QUIT,                // quit interface; no parameters
		C_SET_RUNID,           // set new run id; args: run id (and auto increment enabled)
		C_GET_RUNID,           // read run id and autoincrement; no args; result: run id (and auto increment enabled)
		C_GET_LISTMODE,        // read list mode; no args; result: list mode enabled and write protect data files
		C_COUNTER_SELECTED,    // read, if single counter is selected as preset counter; args: counter id; result: counter enabled
        C_VERSIONTEXT,         // read QMesyDAQ and library version; no args; result: string
	};

	CommandEvent(Command command, QList<QVariant> args = QList<QVariant>());

	//! \return command number
	Command getCommand() const { return m_command; }

	//! \return command argument list
	QList<QVariant> getArgs() const { return m_args; }

private:
	//! current command
	Command 	m_command;

	//! list of command arguments
	QList<QVariant> m_args;
};

#endif // __COMMANDEVENT_H__
