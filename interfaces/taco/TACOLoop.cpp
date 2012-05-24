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

#include "TACOLoop.h"
#include "logging.h"

#include <iostream>
#include <QApplication>
#include <QDebug>
#include <QSettings>

#include <API.h>
#include <private/ApiP.h>

#include "CommandEvent.h"

TACOLoop::TACOLoop(QtInterface *interface)
	: m_server("qmesydaq")
{
	setObjectName("TACOLoop");
}

void TACOLoop::runLoop()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
	settings.beginGroup("TACO");
	m_personal = settings.value("personal", "srv0").toString();
	m_device = settings.value("device", "puma/qmesydaq/det").toString();
	settings.endGroup(); 

	MSG_DEBUG << "device_server to start " << m_server << "/" << m_personal << " with device " << m_device;
	char *p = const_cast<char *>(m_device.toStdString().c_str());
	char *devList = new char[m_device.toStdString().size() + 1];
	strcpy(devList, p);
	DevLong error(0);

	DevLong status = nethost_alloc(&error);
	status = device_server(const_cast<char *>(m_server.toStdString().c_str()), const_cast<char *>(m_personal.toStdString().c_str()), 0, 1, 0, 0, 1, &devList);
	MSG_DEBUG << "device_server does not run";
}
