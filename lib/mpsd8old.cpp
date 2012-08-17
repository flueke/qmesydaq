/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2012 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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
#include "logging.h"

/*!
    constructor

    \param id ID of the module
    \param parent Qt parent object
 */
MPSD8old::MPSD8old(quint8 id, QObject *parent)
	: MPSD8(id, parent)
	, m_g1(0.75)
	, m_g2(340.)	// 255.0 / 0.75
	, m_t1(5)
	, m_t2(5.66667) // 255.0 / 45.0
	, m_p1(4.167)
	, m_p2(1.2083)
{
	m_mpsdId = TYPE_MPSD8OLD;
}

/*!
    Sets the gain of the channel

    \param channel channel in the module
    \param gainv gain value of the channel in the module
    \param preset 
 */
void 	MPSD8old::setGain(quint8 channel, float gainv, bool preset)
{
// boundary check
	if(gainv > 1.5)
		gainv = 1.5;
	if(gainv < 0.75)
		gainv = 0.75;

	MPSD8::setGain(channel, gainv, preset);
}

/*!
    \fn void MPSD8old::setThreshold(quint8 threshold, bool preset)
    Sets the threshold value.

    \param threshold
    \param preset
 */
void	MPSD8old::setThreshold(quint8 threshold, bool preset)
{
// boundary check
	if(threshold > 50)
    		threshold = 50;
	if(threshold < 5)
    		threshold = 5;

	MPSD8::setThreshold(threshold, preset);
}

/*!
    \fn quint8	MPSD8old::calcGainpoti(float fval)
    \param fval floating point value
 
    \return an integer value to be set in the MPSD registers
 */
quint8	MPSD8old::calcGainpoti(float fval)
{
        float fg = (fval - m_g1) * m_g2;
        quint8 ug = quint8(fg);
        if((fg - ug) > 0.5)
                ug++;
        return ug;
}

quint8	MPSD8old::calcThreshpoti(quint8 tval)
{
	float ft = (tval - m_t1) / m_t2;
	quint8 ut = quint8(ft);
	if((ft - ut) > 0.5)
		ut++;
	return ut;
}

float	MPSD8old::calcGainval(quint8 ga)
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

quint8	MPSD8old::calcThreshval(quint8 thr)
{
	float ft = m_t1 + (float)thr * m_t2;
	quint8 t = quint8(ft);
	float diff = ft - t;
	if(diff > 0.5)
		t++;
//	MSG_ERROR << "threshpoti: " << t << ", threshval: " << thr;
	return t;
}
