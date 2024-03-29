// Interface to the QMesyDAQ software
// Copyright (C) 2002 by Gregor Montermann <g.montermann@mesytec.com>
// Copyright (C) 2008 by Lutz Rossa <rossa@hmi.de>
// Copyright (C) 2009-2014 by Jens Kr�ger <jens.krueger@frm2.tum.de>
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

#ifndef TACOLOOP_H
#define TACOLOOP_H

#include "LoopObject.h"

class QtInterface;

/**
  \short TACO loop to manange incoming TACO requests

   This class gets its information from the TACO section of the QMesyDAQ settings
   file

   \code
   [TACO]
   personal='personal name of the TACO server'
   device='TACO device name which has to be created'
   \endcode

   \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class TACOLoop : public LoopObject
{
	Q_OBJECT
public:
	//! Constructor
	TACOLoop(QtInterface *interface = 0);

	QString version(void);

protected:
	//! thread loop
	void runLoop();

private:
	//! server name = "qmesydaq"
	QString m_server;

	//! personal name, default = "srv0"
	QString m_personal;

	//! TACO detector device name, default = "puma/qmesydaq/det"
	QString m_detDevice[3];

	//! TACO timer device name, default = "puma/qmesydaq/timer"
	QString m_timerDevice;

	//! TACO counter device names, default = "puma/qmesydaq/counter[0..3]" "puma/qmesydaq/events"
	QString m_counterDevice[7];
};

#endif // TACOLOOP_H
