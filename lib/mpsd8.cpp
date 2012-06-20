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
#include "logging.h"

/*!
    constructor

    \param id module number
    \param parent Qt parent object
 */
MPSD8::MPSD8(quint8 id, QObject *parent)
	: QObject(parent)
	, m_mpsdId(TYPE_MPSD8)
	, m_comgain(true)
#if 0
	, m_g1(0.0)
	, m_g2(1.0)
	, m_t1(5.1)
	, m_t2(5.0)
	, m_p1(0.0)
	, m_p2(1.0)
#endif
	, m_busNum(id)
{
	for(int i = 0; i < 8; ++i)
	{
		m_active[i] = true;
		m_histogram[i] = true;
	}
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
	MSG_DEBUG << "identified MPSD id on MCPD " << m_mcpdId << ", bus " << m_busNum << ": " << id;
}

//! desctructor
MPSD8::~MPSD8()
{
}

bool MPSD8::online(void)
{
	return true;
}

/*!
    \fn MPSD8::setGain(quint8 channel, float gainv, bool preset)

    sets the gain values for a single channel or all (if channel > 7)

    \param channel number of the channel
    \param gainv user value of the gain
    \param preset ????
    \see getGainpoti
    \see getGainval
 */
void MPSD8::setGain(quint8 channel, float gainv, bool preset)
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
	MSG_NOTICE << "m_gainVal " << (const char*)(preset?"preset ":"") << m_mcpdId << ", " << m_busNum
						 << ",  " << channel << " to " << tr("%1").arg(gainv,6,'f',2) << " (" << val << ')';
}

/*!
    \overload MPSD8::setGain(quint8 channel, quint8 gain, bool preset)

    sets the gain values for a single channel or all (if channel > 7)

    \param channel number of the channel
    \param gain poti value of the gain
    \param preset ????
    \see getGainpoti
    \see getGainval
 */
void MPSD8::setGain(quint8 channel, quint8 gain, bool preset)
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
	MSG_NOTICE << "gain " << (const char*)(preset?"preset ":"") << m_mcpdId << ", " << m_busNum
						 << ",  " << channel << " to " << gain << " (" << tr("%1").arg(gv,6,'f',2) << ')';
}

/*!
    \fn MPSD8::setThreshold(quint8 threshold, bool preset)

    set the threshold value for the MPSD

    \param threshold threshold value 
    \param preset ????
    \see setThreshpoti
    \see getThreshold
    \see getThrespoti
 */
void MPSD8::setThreshold(quint8 threshold, bool preset)
{
	m_threshPoti[preset] = calcThreshpoti(threshold);
	m_threshVal[preset] = threshold;

	MSG_NOTICE << "threshold " << (const char*)(preset?"preset ":"") << m_mcpdId << ", " << m_busNum << " to " << threshold << " (" << m_threshPoti[preset] << ')';
}

/*!
    \fn MPSD8::setThreshpoti(quint8 thresh, bool preset)

    set the threshold poti value for the MPSD

    \param thresh threshold poti value 
    \param preset ????
    \see setThreshold
    \see getThreshold
    \see getThrespoti
 */
void MPSD8::setThreshpoti(quint8 thresh, bool preset)
{
	m_threshPoti[preset] = thresh;
	m_threshVal[preset] = calcThreshval(thresh);
   	
	MSG_NOTICE << "threspoti " << (const char*)(preset?"preset ":"") << m_mcpdId << ", " << m_busNum << " to " << thresh << " (" << m_threshVal[preset] << ')';
}

/*!
    \fn MPSD8::setPulserPoti(quint8 chan, quint8 pos, quint8 poti, quint8 on, bool preset)

    set the pulser with poti value for amplitude

    \param chan channel of the module
    \param pos position in the channel left, right, middle
    \param poti poti value of the amplitude of the pulser
    \param on switch on or off
    \param preset ????
    \see setPulser
 */
void MPSD8::setPulserPoti(quint8 chan, quint8 pos, quint8 poti, quint8 on, bool preset)
{
	MSG_NOTICE << "MPSD8::setPulserPoti(chan = " << chan << ", pos = " << pos << ", poti = " << poti
						 << ", on = " << on << ", preset = " << preset << ')';
	if(pos > MIDDLE)
		m_pulsPos[preset] = MIDDLE;
	else
    		m_pulsPos[preset] = pos;
    
	if(chan > 7)
		m_pulsChan[preset] = 7;
	else
		m_pulsChan[preset] = chan;
    
	m_pulsPoti[preset] = poti;
	m_pulsAmp[preset] = calcPulsAmp(poti, m_gainVal[chan][0]);
	m_pulser[preset] = on;

	MSG_NOTICE << "pulser " << (const char*)(preset?"preset ":"") << m_mcpdId << ", bus " << m_busNum
						 << " to pos " << m_pulsPos[preset] << ", poti " << poti << " - ampl " << tr("%1").arg(m_pulsAmp[preset],6,'f',2);
}

/*!
    \fn MPSD8::setPulser(quint8 chan, quint8 pos, quint8 amp, quint8 on, bool preset)

    set the pulser with amplitude value

    \param chan channel of the module
    \param pos position in the channel left, right, middle
    \param amp amplitude of the pulser
    \param on switch on or off
    \param preset ????
    \see setPulser
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
   	
	MSG_NOTICE << "pulser " << (const char*)(preset?"preset ":"") << m_mcpdId << ", bus " << m_busNum
						 << " to pos " << m_pulsPos[preset] << ", ampl " << amp << " - poti " << m_pulsPoti[preset];
}

/*!
    \fn MPSD8::calcGainpoti(float fval)

    calculates the poti value of the gain for a user value

    \param fval user value
    \return poti value
 */
quint8 MPSD8::calcGainpoti(float fval)
{
	quint8 ug = (quint8) fval;
//	MSG_ERROR << "m_gainVal: " << fval << ", m_gainPoti: " << ug;
	return ug;
}


/*!
    \fn MPSD8::calcThreshpoti(quint8 tval)
    calculates the poti value for the threshold for a user value

    \param tval user value
    \return poti value
 */
quint8 MPSD8::calcThreshpoti(quint8 tval)
{
	quint8 ut = tval;
//	MSG_ERROR << "threshold: " << tval << ", threshpoti: " << ut;
	return ut;
}


/*!
    \fn MPSD8::calcGainval(quint8 ga)

    calculates the user value for a gain poti value

    \param ga poti gain value
    \return user gain value
 */
float MPSD8::calcGainval(quint8 ga)
{
	float fgain = float(ga);
//	MSG_ERROR << "m_gainPoti: " << ga << ", m_gainVal: " << fgain;
	return fgain;
}


/*!
    \fn MPSD8::calcThreshval(quint8 thr)

    calculates the user value for the threshold poti value

    \param thr threshold poti value
    \return user threshold value
 */
quint8 MPSD8::calcThreshval(quint8 thr)
{
	quint8 t = thr;
//	MSG_ERROR << "threshpoti: " << t << ", threshval: " << thr;
	return t;
}

/*!
    \fn MPSD8::calcPulsPoti(quint8 val, float gv)

    calculates the pulser poti value ????

    \param val
    \param gv gain value ??? 
    \return pulser poti value
 */
quint8 MPSD8::calcPulsPoti(quint8 val, float /* gv */)
{
	quint8 pa = val;
//	MSG_ERROR << "pulsval: " << val << ", pulspoti: " << pa;
	return pa;
}

/*!
    \fn MPSD8::calcPulsAmp(quint8 val, float gv)

    calculates the amplitude value for ????

    \param val value 
    \param gv gain value ???
    \return amplitude value
 */
quint8 MPSD8::calcPulsAmp(quint8 val, float gv)
{
	MSG_NOTICE << "MPSD8::calcPulsAmp(val = " << val << ", gv = " << gv << ')';
	return val;
}

/*!
    \fn MPSD8::setInternalreg(quint8 reg, quint16 val, bool preset)

    set the value of the internal registers 

    \param reg register number
    \param val vale of the register
    \param preset ????
    \see getInternalreg
 */
void MPSD8::setInternalreg(quint8 reg, quint16 val, bool preset)
{
	m_internalReg[reg][preset] = val;
   	
	MSG_NOTICE << "register " << (const char*)(preset?"preset ":"") << m_mcpdId << ", " << m_busNum
						 << ", " << reg << ", to " << m_internalReg[reg][preset];
}

/*!
    \fn void MPSD8::setActive(bool act) 
 
    \param act
 */
void MPSD8::setActive(bool act) 
{
	for (int i = 0; i < 8; ++i)
		setActive(i, act); 
}

/*!
    \fn void MPSD8::setActive(quint16 id, bool act)

    \param id
    \param act
 */
void MPSD8::setActive(quint16 chan, bool act)
{
	if (chan < 8)
		m_active[chan] = act;
}

/*!
    \fn void MPSD8::setHistogram(quint16 id, bool act)

    \param id
    \param act
 */
void MPSD8::setHistogram(quint16 chan, bool hist)
{
	if (chan < 8)
		m_histogram[chan] = hist;
	if (!hist)
		setActive(chan, false);
}

/*!
    \fn void MPSD8::setHistogram(bool hist) 
 
    \param hist
 */
void MPSD8::setHistogram(bool hist) 
{
	for (int i = 0; i < 8; ++i)
		setHistogram(i, hist); 
}

/*!
    \fn bool MPSD8::active()

    \return
 */
bool MPSD8::active()
{
	bool result(false);
	for (quint16 i = 0; i < 8; ++i)
		result |= m_active[i];
	return result;
}

/*!
    \fn bool MPSD8::histogram()

    \return
 */
bool MPSD8::histogram()
{
	bool result(false);
	for (quint16 i = 0; i < 8; ++i)
		result |= histogram(i);
	return result;
}

/*!
    \fn bool MPSD8::active(quint16 chan)

    \param chan
    \return
 */
bool MPSD8::active(quint16 chan)
{
	if (chan > 7)
		return false;
	return m_active[chan];
}

/*!
    \fn bool MPSD8::histogram(quint16 chan)

    \param chan
    \return
 */
bool MPSD8::histogram(quint16 chan)
{
	if (chan > 7)
		return false;
	return m_histogram[chan];
}

/*!
    \fn QList<quint16> MPSD8::getHistogramList(void)
    
    \return the list of channels used in histograms
 */
QList<quint16> MPSD8::getHistogramList(void)
{
	QList<quint16> result;
	for (int i = 0; i < 8; ++i)
		if (m_histogram[i])
			result << i;
	return result;
}

/*!
    \fn QList<quint16> MPSD8::getActiveList(void)

    \return the list of channels which are active
 */
QList<quint16> MPSD8::getActiveList(void)
{
	QList<quint16> result;
	for (int i = 0; i < 8; ++i)
		if (m_active[i])
			result << i;
	return result;
}
