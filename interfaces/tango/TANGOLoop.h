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

#ifndef TANGOLOOP_H
#define TANGOLOOP_H

#include "LoopObject.h"

class QtInterface;

#include <tango.h>

/**
  \short TANGO loop to manange incoming TANGO requests

   This class gets its information from the TANGO section of the QMesyDAQ settings
   file

   \code
   [TANGO]
   personal='personal name of the TANGO server'
   device='TANGO device name which has to be created'
   \endcode

   \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class TANGOLoop : public LoopObject
{
	Q_OBJECT
public:
	//! Constructor
	TANGOLoop(QtInterface *interface = 0);

	QString version(void);

protected:
	//! thread loop
	void runLoop();

private:
	void register_server(const QString &process, const QString &personal);

	void register_device(const QString &device, const QString &fullservername, const QString &classname);

private:
	//! server name = "qmesydaq"
	QString m_server;

	//! personal name, default = "qm"
	QString m_personal;

	//! TANGO detector device name, default = "qm/qmesydaq/det"
	QString m_detDevice;

	//! TANGO timer device name, default = "qm/qmesydaq/timer"
	QString m_timerDevice;

	//! TANGO counter device names, default = "qm/qmesydaq/counter[0..5]"
	QString m_counterDevice[6];

	//! TANGO event counter device name, default "qm/qmesydaq/events"
	QString m_eventDevice;

	//! TANGO image device name, default = "qm/qmesydaq/image"
	QString m_imageDevice;

	Tango::Database *m_db;
};

#endif // TANGOLOOP_H
