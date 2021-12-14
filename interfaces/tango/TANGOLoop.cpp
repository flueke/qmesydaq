// Interface to the QMesyDAQ software
// Copyright (C) 2002 by Gregor Montermann <g.montermann@mesytec.com>
// Copyright (C) 2008 by Lutz Rossa <rossa@hmi.de>
// Copyright (C) 2009-2014 by Jens Krüger <jens.krueger@frm2.tum.de>
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

#include "TANGOLoop.h"
#include "qmlogging.h"

#include <QCoreApplication>
#include <QDebug>
#include <QSettings>
#include <QStringList>

#include <tango.h>

#include "CommandEvent.h"


TANGOLoop::TANGOLoop(QtInterface * /* interface */)
	: m_server("qmesydaq")
{
	setObjectName("TANGOLoop");

	m_db = new Tango::Database();
}

QString TANGOLoop::version()
{
	return "TANGO " VERSION;
}

void TANGOLoop::register_server(const QString &process, const QString &personal)
{
	std::string 	_process(process.toStdString()),
			_personal(personal.toStdString());

	std::string	fullname(_process + "/" + _personal);
	try
	{
		m_db->delete_server(fullname);
	}
	catch (Tango::DevFailed &)
	{
	}
	Tango::DbDatum data = m_db->get_services(_process, _personal);
	std::vector<std::string> serverList;

	data >> serverList;

	if (serverList.empty())
	{
		MSG_ERROR << "Have to register: " << process;
		Tango::DbDevInfo dinfo;
		register_device("dserver/" + process + "/" + personal, process + "/" + personal, "DServer");
	}
}

void TANGOLoop::register_device(const QString &device, const QString &fullservername, const QString &classname)
{
	Tango::DbDevInfo dinfo;
	dinfo.name = device.toStdString();
	dinfo._class = classname.toStdString();
	dinfo.server = fullservername.toStdString();
	m_db->add_device(dinfo);
}


void TANGOLoop::runLoop()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
	settings.beginGroup("TANGO");
	m_personal = settings.value("personal", "qm").toString();
	m_timerDevice = settings.value("timer", "qm/qmesydaq/timer").toString();
	m_eventDevice = settings.value("events", "qm/qmesydaq/events").toString();
	m_imageDevice = settings.value("image", "qm/qmesydaq/image").toString();
	for (int i = 0; i < 6; ++i)
		m_counterDevice[i] = settings.value(QString("counter%1").arg(i), QString("qm/qmesydaq/counter%1").arg(i)).toString();
	settings.endGroup();

	QStringList	deviceList;
	deviceList << m_timerDevice << m_eventDevice << m_imageDevice;
	for (int i = 0; i < 6; ++i)
		deviceList << m_counterDevice[i];

	QString devices = deviceList.join(" ");
	QString fullname("qmesydaq/" + m_personal);
	MSG_ERROR << tr("device_server to start %1 with device(s) %2").arg(fullname).arg(devices);

	register_server("qmesydaq", m_personal);

	// register_device(m_detDevice, fullname, "DetectorChannel");
	register_device(m_timerDevice, fullname, "TimerChannel");
	{
		register_device(m_eventDevice, fullname, "CounterChannel");
		Tango::DbDatum channel("channel");
		Tango::DbData data;
		channel << 100;
		data.clear();
		data.push_back(channel);
		m_db->put_device_property(m_eventDevice.toStdString(), data);
	}
	for (int i = 0; i < 6; ++i)
	{
		register_device(m_counterDevice[i], fullname, "CounterChannel");
		Tango::DbDatum channel("channel");
		Tango::DbData data;
		channel << i;
		data.clear();
		data.push_back(channel);
		m_db->put_device_property(m_counterDevice[i].toStdString(), data);
	}
	register_device(m_imageDevice, fullname, "ImageChannel");

	try
	{
		int argc(2);
		char **argv = new char *[2];
		argv[0] = new char[QCoreApplication::applicationFilePath().toStdString().size() + 1];
		strcpy(argv[0], const_cast<char *>(QCoreApplication::applicationFilePath().toStdString().c_str()));
		argv[1] = new char[m_personal.toStdString().size() + 1];
		strcpy(argv[1], const_cast<char *>(m_personal.toStdString().c_str()));

		MSG_ERROR << argv[0] << " " << argv[1];
		// Initialise the device server
		//----------------------------------------
		Tango::Util *tg = Tango::Util::init(argc, argv);

		// Create the device server singleton
		//	which will create everything
		//----------------------------------------
		tg->server_init(false);

		// Run the endless loop
		//----------------------------------------
		MSG_ERROR << "Ready to accept request";
		tg->server_run();

		delete [] argv;
	}
	catch (std::bad_alloc &)
	{
		MSG_ERROR << "Can't allocate memory to store device object !!!";
		MSG_ERROR << "Exiting";
	}
	catch (CORBA::Exception &e)
	{
		Tango::Except::print_exception(e);

		MSG_ERROR << "Received a CORBA_Exception";
		MSG_ERROR << "Exiting";
	}
	Tango::Util::instance()->server_cleanup();
	return;
}

