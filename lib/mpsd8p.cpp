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

#include "mpsd8.h"
#include "mdefines.h"
#include "qmlogging.h"

/*!
    \fn MPSD8plus::MPSD8plus(quint8 id, QObject *parent)

    constructor

    \param id ID of the module
    \param parent Qt parent object
*/
MPSD8plus::MPSD8plus(quint8 id, QObject *parent)
	: MPSD8(id, parent)
// set calibration factors for gain, threshold and pulser calculation
	, m_g1(0.5)
	, m_g2(184.783)
	, m_t1(0)
	, m_t2(0.4583)
	, m_p1(4.167)
	, m_p2(1.2083)
{
	m_mpsdId = TYPE_MPSD8P;
	for(quint8 c = 0; c < 9; c++)
	{
		m_gainPoti[c][0] = 92;
		m_gainVal[c][0] = 1.0;
		m_gainPoti[c][1] = 92;
		m_gainVal[c][1] = 1.0;
	}
	for (quint8 c = 0; c < 2; ++c)
	{
		m_threshPoti[c] = 48;
		m_threshVal[c] = 22;
		m_pulsPoti[c] = 128;
		m_pulsAmp[c] = 50;
	}
	
}

/*!
    \fn MPSD8plus::setGain(quint8 channel, float gain, bool preset)
 */
void MPSD8plus::setGain(quint8 channel, float gainv, bool preset)
{
// boundary check
	if(gainv > 1.88)
		gainv = 1.88;
	if(gainv < 0.5)
		gainv = 0.5;

	MPSD8::setGain(channel, gainv, preset);
}

/*!
    \fn quint8 MPSD8plus::calcGainpoti(float fval)

    \param fval floating point value
 
    \return an integer value to be set in the MPSD registers
 */
quint8 MPSD8plus::calcGainpoti(float fval)
{
        float fg = (fval - m_g1) * m_g2;
        quint8 ug = quint8(fg);
        if((fg - ug) > 0.5)
                ug++;
        return ug;
}

/*!
    \fn MPSD8plus::setThreshold(quint8 threshold, bool preset)
*/
void MPSD8plus::setThreshold(quint8 threshold, bool preset)
{
// boundary check
	if(threshold > 100)
    		threshold = 100;

	MPSD8::setThreshold(threshold, preset);
}

/*!
    \fn MPSD8plus::calcThreshpoti(quint8 tval)
 */
quint8 MPSD8plus::calcThreshpoti(quint8 tval)
{
	float ft = (tval - m_t1) / m_t2;
	quint8 ut = quint8(ft);
	if((ft - ut) > 0.5)
		ut++;
	return ut;
}


/*!
    \fn MPSD8plus::calcGainval(quint8 ga)
 */
float MPSD8plus::calcGainval(quint8 ga)
{
	float fgain = m_g1 + (float)ga/m_g2;
	// round to two decimals:
	float fg = 100.0 * fgain;
	quint16 g = quint16(fg);
	float test = fg -g;
	if(test >= 0.5)
		g++;
	fgain = g /100.0; 
	return fgain;
}

/*!
    \fn MPSD8plus::calcThreshval(quint8 thr)
 */
quint8 MPSD8plus::calcThreshval(quint8 thr)
{
	float ft = m_t1+(float)thr*m_t2;
	quint8 t = quint8(ft);
	float diff = ft - t;
	if(diff > 0.5)
		t++;
//	MSG_ERROR << tr("threshpoti: %1, threshval: %2").arg(t).arg(thr);
	return t;
}

/*!
    \fn MPSD8plus::calcPulsPoti(quint8 val, float)
 */
quint8 MPSD8plus::calcPulsPoti(quint8 val, float gv)
{
	float pamp = (val / gv - m_p1) / m_p2;
	quint8 pa = quint8(pamp);
	if(pamp - pa > 0.5)
		pa++;
//	MSG_ERROR << tr("pulsval: %1, pulspoti: %2").arg(val).arg(pa);
	return pa;
}

/*!
    \fn quint8 MPSD8plus::calcPulsAmp(quint8 val, float gv)
 */
quint8 MPSD8plus::calcPulsAmp(quint8 val, float gv)
{
	MSG_INFO << tr("MPSD8plus::calcPulsAmp(val = %1, gv = %2").arg(val).arg(gv);
	MSG_INFO << tr("m_p1: %1, m_p2: %2").arg(m_p1).arg(m_p2);
	float pa = (m_p1 + (val * m_p2)) * gv;
	quint8 pamp = (quint8) pa;
	if(pa - pamp > 0.5)
		pamp++;
	MSG_INFO << tr("pulspoti: %1, pulsamp: %2").arg(val).arg(pamp);
	return pamp;
}

QString MPSD8plus::getType(void) const
{
	return tr("MPSD-8+");
}

int MPSD8plus::type(void) const
{
	return TYPE_MPSD8P;
}
