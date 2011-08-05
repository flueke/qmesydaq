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
#include "mdefines.h"

/*!
    constructor

    \param id module number
    \param parent Qt parent object
 */
MPSD_8::MPSD_8(quint8 id, QObject *parent)
	: MesydaqObject(parent)
	, m_mpsdId(id)
	, m_comgain(true)
#if 0
	, m_g1(0.0)
	, m_g2(1.0)
	, m_t1(5.1)
	, m_t2(5.0)
	, m_p1(0.0)
	, m_p2(1.0)
#endif
	, m_busNum(0)
{
	m_mcpdId = reinterpret_cast<MCPD8 *>(parent)->getId();
	for(quint8 c = 0; c < 9; c++)
	{
		m_gainPoti[c][0] = 128;
		m_gainVal[c][0] = 128;
		m_gainPoti[c][1] = 128;
		m_gainVal[c][1] = 128;
	}
	for (quint8 c = 0; c < 2; ++c)
	{
		m_threshPoti[c] = 10;
		m_threshVal[c] = 10;
		m_pulsPos[c] = MIDDLE;
		m_pulsPoti[c] = 128;
		m_pulsAmp[c] = 128;
		m_pulsChan[c] = 0;
		m_pulser[c] = false;
		m_ampMode[c] = false;
	}
	
}

//! desctructor
MPSD_8::~MPSD_8()
{
}

bool MPSD_8::online(void)
{
	return true;
}

/*!
    \fn MPSD_8::setMpsdId(quint8 bus, quint8 id, bool listIds)

    sets the ID of the MPSD

    \param bus number of the bus
    \param id ID number
    \param listIds log the action or not
 */
void MPSD_8::setMpsdId(quint8 bus, quint8 id, bool listIds)
{
   	m_mpsdId = id;
   	m_busNum = bus;
   	
   	if(listIds)
   		protocol(tr("identified MPSD id on MCPD %1, bus %2: %3").arg(m_mcpdId).arg(bus).arg(id), NOTICE);
}

/*!
    \fn MPSD_8::setGain(quint8 channel, float gainv, bool preset)

    sets the gain values for a single channel or all (if channel > 7)

    \param channel number of the channel
    \param gainv user value of the gain
    \param preset ????
    \see getGainpoti
    \see getGainval
 */
void MPSD_8::setGain(quint8 channel, float gainv, bool preset)
{
	quint8 val = calcGainpoti(gainv);
    
	m_comgain = (channel > 7);
	if (m_comgain)
	{
		for(quint8 c = 0; c < 9; c++)
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
   	protocol(tr("m_gainVal %1%2, %3,  %4 to %5 (%6)").arg(preset ? tr("preset ") : "").arg(m_mcpdId).arg(m_busNum).arg(channel).arg(gainv,6, 'f', 2).arg(val), NOTICE);
}

/*!
    \overload MPSD_8::setGain(quint8 channel, quint8 gain, bool preset)

    sets the gain values for a single channel or all (if channel > 7)

    \param channel number of the channel
    \param gain poti value of the gain
    \param preset ????
    \see getGainpoti
    \see getGainval
 */
void MPSD_8::setGain(quint8 channel, quint8 gain, bool preset)
{
	float gv = calcGainval(gain);
	m_comgain = (channel > 7);
	if (m_comgain)
	{
		for(quint8 c = 0; c < 9; c++)
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
   	protocol(tr("gain %1%2,  %3, %4 to %5 (%6)").arg(preset ? tr("preset ") : "").arg(m_mcpdId).arg(m_busNum).arg(channel).arg(gain).arg(gv, 6, 'f', 2), NOTICE);
}

/*!
    \fn MPSD_8::setThreshold(quint8 threshold, bool preset)

    set the threshold value for the MPSD

    \param threshold threshold value 
    \param preset ????
    \see setThreshpoti
    \see getThreshold
    \see getThrespoti
 */
void MPSD_8::setThreshold(quint8 threshold, bool preset)
{
	m_threshPoti[preset] = calcThreshpoti(threshold);
	m_threshVal[preset] = threshold;
    
	QString str;
   	str.sprintf("threshold %s%d, %d to %d (%d)", (preset ? "preset " : ""), m_mcpdId, m_busNum, threshold, m_threshPoti[preset]);
   	protocol(str, NOTICE);
}

/*!
    \fn MPSD_8::setThreshpoti(quint8 thresh, bool preset)

    set the threshold poti value for the MPSD

    \param thresh threshold poti value 
    \param preset ????
    \see setThreshold
    \see getThreshold
    \see getThrespoti
 */
void MPSD_8::setThreshpoti(quint8 thresh, bool preset)
{
	m_threshPoti[preset] = thresh;
	m_threshVal[preset] = calcThreshval(thresh);
   	
	QString str; 
   	str.sprintf("threshpoti %s%d, %d to %d (%d)", (preset ? "preset " : ""), m_mcpdId, m_busNum, thresh, m_threshVal[preset]);
   	protocol(str, NOTICE);
}

/*!
    \fn MPSD_8::setPulserPoti(quint8 chan, quint8 pos, quint8 poti, quint8 on, bool preset)

    set the pulser with poti value for amplitude

    \param chan channel of the module
    \param pos position in the channel left, right, middle
    \param poti poti value of the amplitude of the pulser
    \param on switch on or off
    \param preset ????
    \see setPulser
 */
void MPSD_8::setPulserPoti(quint8 chan, quint8 pos, quint8 poti, quint8 on, bool preset)
{
	protocol(tr("MPSD_8::setPulserPoti(chan = %1, pos = %2, poti = %3, on = %4, preset = %5)").arg(chan).arg(pos).arg(poti).arg(on).arg(preset), NOTICE);
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
			.arg(preset ? "preset ": "").arg(m_mcpdId).arg(m_busNum).arg(m_pulsPos[preset]).arg(poti).arg(m_pulsAmp[preset], 6, 'f', 2), NOTICE);
}

/*!
    \fn MPSD_8::setPulser(quint8 chan, quint8 pos, quint8 amp, quint8 on, bool preset)

    set the pulser with amplitude value

    \param chan channel of the module
    \param pos position in the channel left, right, middle
    \param amp amplitude of the pulser
    \param on switch on or off
    \param preset ????
    \see setPulser
 */
void MPSD_8::setPulser(quint8 chan, quint8 pos, quint8 amp, quint8 on, bool preset)
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
			.arg(preset ? "preset " : "").arg(m_mcpdId).arg(m_busNum).arg(m_pulsPos[preset]).arg(amp).arg(m_pulsPoti[preset]), NOTICE);
}

/*!
    \fn MPSD_8::calcGainpoti(float fval)

    calculates the poti value of the gain for a user value

    \param fval user value
    \return poti value
 */
quint8 MPSD_8::calcGainpoti(float fval)
{
	quint8 ug = (quint8) fval;
//	protocol(tr("m_gainVal: %1, m_gainPoti: %2").arg(fval).arg(ug));		
	return ug;
}


/*!
    \fn MPSD_8::calcThreshpoti(quint8 tval)
    calculates the poti value for the threshold for a user value

    \param tval user value
    \return poti value
 */
quint8 MPSD_8::calcThreshpoti(quint8 tval)
{
	quint8 ut = tval;
//	protocol(tr("threshold: %1, threshpoti: %2").arg(tval).arg(ut));	
	return ut;
}


/*!
    \fn MPSD_8::calcGainval(quint8 ga)

    calculates the user value for a gain poti value

    \param ga poti gain value
    \return user gain value
 */
float MPSD_8::calcGainval(quint8 ga)
{
	float fgain = float(ga);
//	protocol(tr("m_gainPoti: %1, m_gainVal: %2").arg(ga).arg(fgain));	
	return fgain;
}


/*!
    \fn MPSD_8::calcThreshval(quint8 thr)

    calculates the user value for the threshold poti value

    \param thr threshold poti value
    \return user threshold value
 */
quint8 MPSD_8::calcThreshval(quint8 thr)
{
	quint8 t = thr;
//	protocol(tr("threshpoti: %1, threshval: %2").arg(t).arg(thr));	
	return t;
}

/*!
    \fn MPSD_8::calcPulsPoti(quint8 val, float gv)

    calculates the pulser poti value ????

    \param val
    \param gv gain value ??? 
    \return pulser poti value
 */
quint8 MPSD_8::calcPulsPoti(quint8 val, float /* gv */)
{
	quint8 pa = val;
//	protocol(tr("pulsval: %1, pulspoti: %2").arg(val).arg(pa));
	return pa;
}

/*!
    \fn MPSD_8::calcPulsAmp(quint8 val, float gv)

    calculates the amplitude value for ????

    \param val value 
    \param gv gain value ???
    \return amplitude value
 */
quint8 MPSD_8::calcPulsAmp(quint8 val, float gv)
{
	protocol(tr("MPSD_8::calcPulsAmp(val = %1, gv = %2)").arg(val).arg(gv), NOTICE);
	return val;
}

/*!
    \fn MPSD_8::setInternalreg(quint8 reg, quint16 val, bool preset)

    set the value of the internal registers 

    \param reg register number
    \param val vale of the register
    \param preset ????
    \see getInternalreg
 */
void MPSD_8::setInternalreg(quint8 reg, quint16 val, bool preset)
{
	m_internalReg[reg][preset] = val;
   	
   	protocol(tr("register %1%2, %3, %4, to %5").arg(preset ? tr("preset ") : "").arg(m_mcpdId).arg(m_busNum).arg(reg).arg(m_internalReg[reg][preset]), NOTICE);
}

