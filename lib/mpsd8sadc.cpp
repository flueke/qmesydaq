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

MPSD_8SADC::MPSD_8SADC(quint8 id, QObject *parent)
	: MPSD_8(id, parent)
#if 0
	, m_g1(0.75)
	, m_g2(340.)	// 255.0 / 0.75
	, m_t1(5)
	, m_t2(5.66667) // 255.0 / 45.0
	, m_p1(4.167)
	, m_p2(1.2083)
#endif
{
}

#if 0
void 	MPSD_8SADC::setGain(quint8 channel, float gainv, bool preset)
{
// boundary check
	if(gainv > 1.5)
		gainv = 1.5;
	if(gainv < 0.75)
		gainv = 0.75;

	MPSD_8::setGain(channel, gainv, preset);
}

void	MPSD_8SADC::setThreshold(quint8 threshold, bool preset)
{
// boundary check
	if(threshold > 50)
    		threshold = 50;
	if(threshold < 5)
    		threshold = 5;

	MPSD_8::setThreshold(threshold, preset);
}


quint8	MPSD_8SADC::calcGainpoti(float fval)
{
        float fg = (fval - m_g1) * m_g2;
        quint8 ug = quint8(fg);
        if((fg - ug) > 0.5)
                ug++;
        return ug;
}

quint8	MPSD_8SADC::calcThreshpoti(quint8 tval)
{
	float ft = (tval - m_t1) / m_t2;
	quint8 ut = quint8(ft);
	if((ft - ut) > 0.5)
		ut++;
	return ut;
}

float	MPSD_8SADC::calcGainval(quint8 ga)
{
	float fgain = m_g1 + (float)ga / m_g2;
// round to two decimals:
	float fg = 100.0 * fgain;
	quint16 g = quint16(fg);
	float test = fg -g;
	if(test >= 0.5)
		g++;
	fgain = g /100.0; 
	return fgain;
}

quint8	MPSD_8SADC::calcThreshval(quint8 thr)
{
	float ft = m_t1 + (float)thr * m_t2;
	quint8 t = quint8(ft);
	float diff = ft - t;
	if(diff > 0.5)
		t++;
//	protocol(tr("threshpoti: %1, threshval: %2").arg(t).arg(thr));	
	return t;
}
#endif
