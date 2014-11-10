/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2014 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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

#include "pulsertest.h"
#include "mdefines.h"
#include "mesydaq2.h"

/**
 * create a list of all actions to set the pulser on/off over the whole set of settings:
 * - all found MCPD
 *   - all found MPSD
 *   	- all positions
 *   	  - all amplitude values (30, 60)
 */
QList<puls> PulserTest::sequence(Mesydaq2 *mesy)
{
	QList<puls> retVal;
	QList<quint8> amps;
	amps << 30 << 60;
	QList<int> m_mcpd = mesy->mcpdId();
	QList<int> positions;
	positions << LEFT << MIDDLE << RIGHT;
	foreach(int mod, m_mcpd)
	{
		QList<int> mpsd = mesy->mpsdId(mod);
		foreach(int addr, mpsd)
			for (quint8 channel = 0; channel < 8; ++channel)
				foreach (quint8 position, positions)
					foreach(quint8 amp, amps)
					{
						puls p = {mod, addr, channel, position, amp, 125};
						retVal << p;
					}
	}
	return retVal;
}
