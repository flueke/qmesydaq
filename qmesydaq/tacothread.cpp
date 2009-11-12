/***************************************************************************
 *   Copyright (C) 2002 by Gregor Montermann <g.montermann@mesytec.com>    *
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

// application specific includes
#include "tacothread.h"
#include "tacocontrol.h"

#include <API.h>
#include <private/ApiP.h>

#include "MesyDAQServer.h"

#include "mesydaq2.h"
// #include "mdefines.h"
// #include "measurement.h"

extern std::vector< ::TACO::Server *> taco_devices;

TACOThread::TACOThread(TACOControl *pcInt)
	: QRunnable()
	, m_pInt(NULL)
	, m_terminate(false)
{
	initialize(pcInt);
}

TACOThread::~TACOThread()
{
	qDebug(QObject::tr("CorbaThread::~CorbaThread()"));
}

void TACOThread::run()
{
	qDebug(QObject::tr("TACOThread::run()"));
	QString server("qmesydaq");
	QString personal("srv0");
	QString device("test/qmesydaq/det");
	char *p = const_cast<char *>(device.toStdString().c_str());
	char *devlist = new char[device.toStdString().size() + 1];
	strcpy(devlist, p);
	DevLong error;

	nethost_alloc(&error);
	DevLong status = device_server(const_cast<char *>(server.toStdString().c_str()), const_cast<char *>(personal.toStdString().c_str()), 0, 1, 0, 0, 1, &devlist); 
	qDebug(QObject::tr("device_server runs"));
	if (status == DS_OK)	
	{
		for (std::vector< ::TACO::Server *>::iterator it = taco_devices.begin(); it != taco_devices.end(); ++it)
			reinterpret_cast<MesyDAQ::Detector::Detector *>(*it)->setControlInterface(m_pInt);
	}
}

bool TACOThread::initialize(TACOControl *pcInt)
{
	qDebug(QObject::tr("initialize TACO"));
	m_pInt = pcInt;
	return true;
}
