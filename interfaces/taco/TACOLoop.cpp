// Interface to the QMesyDAQ software
// Copyright (C) 2002 by Gregor Montermann <g.montermann@mesytec.com>
// Copyright (C) 2008 by Lutz Rossa <rossa@hmi.de>
// Copyright (C) 2009-2020 by Jens Krüger <jens.krueger@frm2.tum.de>
// Copyright (C) 2010 by Alexander Lenz <alexander.lenz@frm2.tum.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include "TACOLoop.h"
#include "qmlogging.h"

#include <iostream>
#include <QApplication>
#include <QDebug>
#include <QSettings>
#include <QStringList>

#include <API.h>
#include <private/ApiP.h>

#include "CommandEvent.h"

TACOLoop::TACOLoop(QtInterface * /* interface */)
	: m_server("qmesydaq")
{
	setObjectName("TACOLoop");
}

QString TACOLoop::version()
{
	return "TACO " VERSION;
}

void TACOLoop::runLoop()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
	settings.beginGroup("TACO");
	m_personal = settings.value("personal", "srv0").toString();
	m_detDevice[0] = settings.value("detector", "test/qmesydaq/det").toString();
	m_detDevice[1] = settings.value("raw", "test/qmesydaq/detraw").toString();
	m_detDevice[2] = settings.value("amplitude", "test/qmesydaq/detamp").toString();
	m_timerDevice = settings.value("timer", "test/qmesydaq/timer").toString();
	for (int i = 0; i < 6; ++i)
		m_counterDevice[i] = settings.value(QString("counter%1").arg(i), QString("test/qmesydaq/counter%1").arg(i)).toString();
	m_counterDevice[6] = settings.value("events", "test/qmesydaq/events").toString();
	settings.endGroup();

	QStringList	deviceList;
	deviceList << m_detDevice[0] << m_detDevice[1] << m_detDevice[2] << m_timerDevice;
	for (int i = 0; i < 7; ++i)
		deviceList << m_counterDevice[i];

	QString devices = deviceList.join(" ");
	MSG_DEBUG << tr("device_server to start %1/%2 with device(s) ").arg(m_server).arg(m_personal) << devices;

	char **devList = new char*[deviceList.size()];
	for (int i = 0; i < deviceList.size(); ++i)
	{
		devList[i] = new char[deviceList.at(i).toStdString().size() + 1];
		strcpy(devList[i], deviceList.at(i).toStdString().c_str());
	}
	DevLong error(0);

	DevLong status = nethost_alloc(&error);
	if (status == DS_OK)
		status = device_server(const_cast<char *>(m_server.toStdString().c_str()), const_cast<char *>(m_personal.toStdString().c_str()), 0, 1, 0, 0, deviceList.size(), devList);
	MSG_FATAL << tr("device_server does not run : %1").arg(status);
}
