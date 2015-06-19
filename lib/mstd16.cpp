/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2015 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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

#include "mstd16.h"

/*!
    \fn MSTD16::MSTD16(quint8 id, QObject *parent)

    constructor

    \param id ID of the module
    \param parent Qt parent object
*/
MSTD16::MSTD16(quint8 id, QObject *parent)
	: MPSD8(id, parent)
// set calibration factors for gain, threshold and pulser calculation
{
	m_mpsdId = TYPE_MSTD16;
	m_pulsPos[0] = m_pulsPos[1] = RIGHT;
	m_active.resize(16);
	m_histogram.resize(16);
	for (int i = 0; i < m_histogram.size(); ++i)
		m_active[i] = m_histogram[i] = false;
	m_ampMode[0] = m_ampMode[1] = true;
}

/*!
    \fn MSTD16::setPulser(quint8 chan, quint8 pos, quint8 amp, quint8 on, bool preset)

    set the pulser with amplitude value

    \param chan channel of the module
    \param pos position in the channel left, right, middle
    \param amp amplitude of the pulser
    \param on switch on or off
    \param preset ????
    \see setPulser
 */
void MSTD16::setPulser(quint8 chan, quint8 pos, quint8 amp, quint8 on, bool preset)
{
	if(pos != LEFT && pos != RIGHT)
		m_pulsPos[preset] = LEFT;
	else
		m_pulsPos[preset] = pos;
    
	if(chan > 7)
		m_pulsChan[preset] = 7;
	else
		m_pulsChan[preset] = chan;
    	
	m_pulsPoti[preset] = calcPulsPoti(amp, m_gainVal[chan][0]);
	m_pulsAmp[preset] = amp;
	m_pulser[preset] = on;
   	
	MSG_NOTICE << tr("pulser %1%2, bus %3 to pos %4, ampl %5 - poti %6").
		arg(preset ? "preset " : "").arg(m_mcpdId).arg(m_busNum).arg(m_pulsPos[preset]).arg(amp).arg(m_pulsPoti[preset]);
}

/*!
    \fn QList<quint16> MCPD8::getHistogramList(void)

    return the list of channels used in histograms

    \return the list of channels used in histograms
 */
QList<quint16> MSTD16::getHistogramList(void)
{
	QList<quint16> result;
	for (int i = 0; i < 16; ++i)
		if (m_histogram[i])
			result << i;
	return result;
}

QString MSTD16::getType(void) const
{
	return tr("MSTD-16");
}

int MSTD16::type(void) const
{
	return TYPE_MSTD16;
}

quint8	MSTD16::getChannels() const
{
	return 16;
}
