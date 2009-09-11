/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009 by Jens Kr�ger <jens.krueger@frm2.tum.de>          *
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
// #include "mcpd8.h"

MPSD_8p::MPSD_8p(quint8 id, QObject *parent)
	: MPSD_8(id, parent)
// set calibration factors for gain, threshold and pulser calculation
	, m_g1(0.5)
	, m_g2(184.783)
	, m_t1(0)
	, m_t2(0.4583)
	, m_p1(4.167)
	, m_p2(1.2083)
{
	protocol(tr("ID = %1").arg(id));
}

/*!
    \fn MPSD_8p::setGain(unsigned char channel, float gain, bool preset)
 */
void MPSD_8p::setGain(quint8 channel, float gainv, quint8 preset)
{
// boundary check
	if(gainv > 1.88)
		gainv = 1.88;
	if(gainv < 0.5)
		gainv = 0.5;

	MPSD_8::setGain(channel, gainv, preset);
}

/*!
    \fn mpsd8::calcGainpoti(float fval)
 */
quint8 MPSD_8p::calcGainpoti(float fval)
{
        float fg = (fval - m_g1) * m_g2;
        quint8 ug = quint8(fg);
        if((fg - ug) > 0.5)
                ug++;
//      protocol(tr("gainval: %1, gainpoti: %2").arg(fval).arg(ug));
        return ug;
}

void MPSD_8p::setThreshold(quint8 threshold, bool preset)
{
// boundary check
	if(threshold > 100)
    		threshold = 100;

	MPSD_8::setThreshold(threshold, preset);
}

/*!
    \fn MPSD_8::calcThreshpoti(quint8 tval)
 */
quint8 MPSD_8p::calcThreshpoti(quint8 tval)
{
	float ft = (tval - m_t1) / m_t2;
	quint8 ut = quint8(ft);
	if((ft - ut) > 0.5)
		ut++;
//	protocol(tr("threshold: %1, threshpoti: %2").arg(tval).arg(ut));	
	return ut;
}


/*!
    \fn MPSD_8::calcGainval(quint8 ga)
 */
float MPSD_8p::calcGainval(quint8 ga)
{
	float fgain = m_g1 + (float)ga/m_g2;
	// round to two decimals:
	float fg = 100.0 * fgain;
	quint16 g = quint16(fg);
	float test = fg -g;
	if(test >= 0.5)
		g++;
	fgain = g /100.0; 
//	protocol(tr("m_gainPoti: %1, m_gainVal: %2").arg(ga).arg(fgain));	
	return fgain;
}

/*!
    \fn MPSD_8p::calcThreshval(quint8 thr)
 */
quint8 MPSD_8p::calcThreshval(quint8 thr)
{
	float ft = m_t1+(float)thr*m_t2;
	quint8 t = quint8(ft);
	float diff = ft - t;
	if(diff > 0.5)
		t++;
//	protocol(tr("threshpoti: %1, threshval: %2").arg(t).arg(thr));	
	return t;
}

/*!
    \fn MPSD_8::calcPulspoti(quint8 val, float)
 */
quint8 MPSD_8p::calcPulsPoti(quint8 val, float gv)
{
	float pamp = (val / gv - m_p1) / m_p2;
	quint8 pa = quint8(pamp);
	if(pamp - pa > 0.5)
		pa++;
//	protocol(tr("pulsval: %1, pulspoti: %2").arg(val).arg(pa));
	return pa;
}

/*!
    \fn MPSD_8::calcPulsamp(quint8 val, float gv)
 */
quint8 MPSD_8p::calcPulsAmp(quint8 val, float gv)
{
	protocol(tr("MPSD_8p::calcPulsAmp(val = %1, gv = %2)").arg(val).arg(gv));
	protocol(tr("m_p1: %1, m_p2: %2").arg(m_p1).arg(m_p2));
	float pa = (m_p1 + (val * m_p2)) * gv;
	quint8 pamp = (quint8) pa;
	if(pa - pamp > 0.5)
		pamp++;
	protocol(tr("pulspoti: %1, pulsamp: %2").arg(val).arg(pamp));
	return pamp;
}