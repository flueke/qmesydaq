/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
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
#include "mcpd8.h"
#include "mpsd8.h"

MPSD8::MPSD8(quint8 id, QObject *parent)
	: MesydaqObject(parent)
	, m_mpsdId(id)
	, m_comgain(true)
	, m_g1(0.0)
	, m_g2(1.0)
	, m_t1(5.1)
	, m_t2(5.0)
	, m_p1(0.0)
	, m_p2(1.0)
	, m_busNum(0)
{
	m_mcpdId = reinterpret_cast<MCPD8 *>(parent)->getId();
	for(quint8 c = 0; c < 9; c++)
	{
		m_gainPoti[c][1] = 128;
		m_gainVal[c][1] = 128;
	}
	for (quint8 c = 0; c < 2; ++c)
	{
		m_threshPoti[c] = 10;
		m_threshVal[c] = 10;
		m_pulsPos[c] = 2;
		m_pulsPoti[c] = 128;
		m_pulsAmp[c] = 128;
		m_pulsChan[c] = 0;
		m_pulser[c] = false;
		m_ampMode[c] = false;
	}
	
// set calibration factors for gain, threshold and pulser calculation
/*	
	, m_g1(0.5)
	, m_g2(184.783)
	, m_t1(0)
	, m_t2(0.4583)
	, m_p1(4.167)
	, m_p2(1.2083)
*/
}

MPSD8::~MPSD8()
{
}


/*!
    \fn MPSD8::setMpsdId(unsigned char id)
 */
void MPSD8::setMpsdId(quint8 bus, quint8 id, bool listIds)
{
   	m_mpsdId = id;
   	m_busNum = bus;
   	
   	if(listIds)
   	{
   		QString str;
		str.sprintf("identified MPSD id on MCPD %d, bus %d: %d", m_mcpdId, bus, id);
   		protocol(str, 1);
	}
}

/*!
    \fn MPSD8::setGain(quint8 channel, float gain, quint8 preset)
 */
void MPSD8::setGain(quint8 channel, float gainv, bool preset)
{
#if 0   
// boundary check
	if(gainv > 1.88)
		gainv = 1.88;
	if(gainv < 0.5)
		gainv = 0.5;
#endif    
	quint8 val = calcGainpoti(gainv);
    
	m_comgain = (channel == 8);
	if (m_comgain)
	{
		for(quint8 c = 0; c < 8; c++)
		{
			m_gainPoti[c][preset] = val;
			m_gainVal[c][preset] = gainv;
		}
	}
	else
	{	
		m_gainPoti[channel][preset] = val;
		m_gainVal[channel][preset] = gainv;
	}
   	protocol(tr("m_gainVal %1%2, %3,  %4 to %5 (%6)").arg(preset ? tr("preset ") : "").arg(m_mcpdId).arg(m_busNum).arg(channel).arg(gainv,6, 'f', 2).arg(val), 1);
}

/*!
    \fn MPSD8::setGain(quint8 channel, quint8 gain, quint8 preset)
 */
void MPSD8::setGain(quint8 channel, quint8 gain, bool preset)
{
	float gv = calcGainval(gain);
	m_comgain = (channel == 8);
	if (m_comgain)
	{
		for(quint8 c = 0; c < 8; c++)
		{
			m_gainPoti[c][preset] = gain;
			m_gainVal[c][preset] = gv;
		}
	}
	else
	{	
		m_gainPoti[channel][preset] = gain;
		m_gainVal[channel][preset] = gv;
	}
   	protocol(tr("gain %1%2,  %3, %4 to %5 (%6)").arg(preset ? tr("preset ") : "").arg(m_mcpdId).arg(m_busNum).arg(channel).arg(gain).arg(gv, 6, 'f', 2), 1);
}

/*!
    \fn MPSD8::setThreshold(quint8 threshold, quint8 preset)
 */
void MPSD8::setThreshold(quint8 threshold, bool preset)
{
#if 0    
// boundary check
	if(threshold > 100)
    		threshold = 100;
#endif   
	m_threshPoti[preset] = calcThreshpoti(threshold);
	m_threshVal[preset] = threshold;
    
	QString str;
   	str.sprintf("threshold %s%d, %d to %d (%d)", (preset ? "preset " : ""), m_mcpdId, m_busNum, threshold, m_threshPoti[preset]);
   	protocol(str, 1);
}

/*!
    \fn MPSD8::setThrespoti(quint8 thresh, quint8 preset)
 */
void MPSD8::setThreshpoti(quint8 thresh, bool preset)
{
	m_threshPoti[preset] = thresh;
	m_threshVal[preset] = calcThreshval(thresh);
   	
	QString str; 
   	str.sprintf("threshpoti %s%d, %d to %d (%d)", (preset ? "preset " : ""), m_mcpdId, m_busNum, thresh, m_threshVal[preset]);
   	protocol(str, 1);
}

/*!
    \fn MPSD8::setPulserpoti(quint8 thresh, quint8 chan, quint8 preset)
 */
void MPSD8::setPulserPoti(quint8 chan, quint8 pos, quint8 poti, quint8 on, bool preset)
{
	protocol(tr("MPSD8::setPulserPoti(chan = %1, pos = %2, poti = %3, on = %4, preset = %5)").arg(chan).arg(pos).arg(poti).arg(on).arg(preset));
	if(pos > 2)
		m_pulsPos[preset] = 2;
	else
    		m_pulsPos[preset] = pos;
    
	if(chan > 7)
		m_pulsChan[preset] = 7;
	else
		m_pulsChan[preset] = chan;
    
	m_pulsPoti[preset] = poti;
	m_pulsAmp[preset] = calcPulsAmp(poti, m_gainVal[chan][0]);
	m_pulser[preset] = on;
   	
   	protocol(tr("pulser %1%2, bus %3 to pos %4, poti %5 - ampl %6")
			.arg(preset ? "preset ": "").arg(m_mcpdId).arg(m_busNum).arg(m_pulsPos[preset]).arg(poti).arg(m_pulsAmp[preset], 6, 'f', 2), 1);
}

/*!
    \fn MPSD8::setPulser(quint8 chan, quint8 amp = 128, quint8 pos = 2, quint8 on = true, quint8 preset = 0)
 */
void MPSD8::setPulser(quint8 chan, quint8 pos, quint8 amp, quint8 on, bool preset)
{
	if(pos > 2)
		m_pulsPos[preset] = 2;
	else
		m_pulsPos[preset] = pos;
    
	if(chan > 7)
		m_pulsChan[preset] = 7;
	else
		m_pulsChan[preset] = chan;
    	
	m_pulsPoti[preset] = calcPulsPoti(amp, m_gainVal[chan][0]);
	m_pulsAmp[preset] = amp;
	m_pulser[preset] = on;
   	
   	protocol(tr("pulser %1%2, bus %3 to pos %4, ampl %5 - poti %6")
			.arg(preset ? "preset " : "").arg(m_mcpdId).arg(m_busNum).arg(m_pulsPos[preset]).arg(amp).arg(m_pulsPoti[preset]), 1);
}

/*!
    \fn MPSD8::calcGainpoti(float fval)
 */
quint8 MPSD8::calcGainpoti(float fval)
{
	quint8 ug = (quint8) fval;
#if 0	
	float fg;
	
	fg = (fval-m_g1)*m_g2;
	ug = (unsigned char) fg;
	if((fg - ug) > 0.5)
		ug++;
//	qDebug("m_gainVal: %1.2f, m_gainPoti: %d", fval, ug);		
#endif
	return ug;
}


/*!
    \fn MPSD8::calcThreshpoti(quint8 tval)
 */
quint8 MPSD8::calcThreshpoti(quint8 tval)
{
	quint8 ut = tval;
#if 0
	float ft;
	
	ft = (tval-m_t1)/m_t2;
	ut = (unsigned char) ft;
	if((ft - ut) > 0.5)
		ut++;
		
//	qDebug("threshold: %d, threshpoti: %d", tval, ut);	
#endif
	return ut;
}


/*!
    \fn MPSD8::calcGainval(quint8 ga)
 */
float MPSD8::calcGainval(quint8 ga)
{
	float fgain = (float) ga;
#if 0	
	float fgain = m_g1 + (float)ga/m_g2;
	// round to two decimals:
	float fg = 100.0 * fgain;
	unsigned short g = (unsigned short) fg;
	float test = fg -g;
	if(test >= 0.5)
		g++;
	 
	fgain = (float)g /100.0; 
//	qDebug("m_gainPoti: %d, m_gainVal: %1.2f", ga, fgain);	
#endif
	return fgain;
}


/*!
    \fn MPSD8::calcThreshval(quint8 thr)
 */
quint8 MPSD8::calcThreshval(quint8 thr)
{
	quint8 t = thr;
#if 0
	float ft = m_t1+(float)thr*m_t2;
	unsigned char t = (unsigned char) ft;
	float diff = ft - t;
	if(diff > 0.5)
		t++;
//	qDebug("threshpoti: %d, threshval: %d", t, thr);	
#endif
	return t;
}

/*!
    \fn MPSD8::calcPulspoti(quint8 val, float)
 */
quint8 MPSD8::calcPulsPoti(quint8 val, float /* gv */)
{
	quint8 pa = val;
#if 0    
	float pamp = (val / gv - m_p1) / m_p2;
	unsigned char pa = (unsigned char) pamp;
     
	if(pamp - pa > 0.5)
		pa++;
     
//	qDebug("pulsval: %d, pulspoti: %d", val, pa);
#endif
	return pa;
}

/*!
    \fn MPSD8::calcPulsamp(quint8 val, float gv)
 */
quint8 MPSD8::calcPulsAmp(quint8 val, float gv)
{
	protocol(tr("MPSD8::calcPulsAmp(val = %1, gv = %2)").arg(val).arg(gv));
	protocol(tr("m_p1: %1, m_p2: %2").arg(m_p1).arg(m_p2));
#if 0
	float pa = (m_p1 + (val * m_p2)) * gv;
	quint8 pamp = (quint8) pa;
	if(pa - pamp > 0.5)
		pamp++;
	protocol(tr("pulspoti: %1, pulsamp: %2").arg(val).arg(pamp));
	return pamp;
#endif
	return val;
}

/*!
    \fn MPSD8::setInternalreg(quint8 reg, quint16 val, quint8 preset)
 */
void MPSD8::setInternalreg(quint8 reg, quint16 val, bool preset)
{
	m_internalReg[reg][preset] = val;
   	
   	protocol(tr("register %1%2, %3, %4, to %5").arg(preset ? tr("preset ") : "").arg(m_mcpdId).arg(m_busNum).arg(reg).arg(m_internalReg[reg][preset]), 1);
}

