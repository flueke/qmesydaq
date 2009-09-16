/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009 by Jens Krüger <jens.krueger@frm2.tum.de>          *
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
#ifndef CARESSCONTROL_H
#define CARESSCONTROL_H

#include "controlinterface.h"

class CorbaThread;

/**
 * Interface class for external control for CARESS
 *
 * \author Gregor Montermann <g.montermann@mesytec.com>
*/

class CARESSControl : public ControlInterface
{
	Q_OBJECT
public:
	CARESSControl(QObject *parent = NULL);

	~CARESSControl();

	void caressTask();
	bool isActive(void);
	void completeCar();

	void setCaressTaskPending(bool val) {m_caressTaskPending = val;}
	bool caressTaskPending(void) {return m_caressTaskPending;}
	bool asyncTaskPending(void) {return m_asyncTaskPending;}

protected:
	bool 		m_asyncTaskPending;
	bool 		m_caressTaskPending;
	quint32 	m_caressTaskNum;
	quint32 	m_caressSubTaskNum;
	quint8  	m_caressMaster;
	quint8  	m_caressDevice;
	ulong		*m_transferBuffer;
	ulong		m_caressStartChannel;
	ulong		m_caressEndChannel;
	ulong		m_caressHistoSize;
	quint32 	m_caressHeight;
	quint32 	m_caressWidth;
	ulong		m_caressPreset;

private:
	CorbaThread	*m_ct;
};

#endif
