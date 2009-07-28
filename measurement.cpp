/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann   *
 *   g.montermann@mesytec.com   *
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
#include "measurement.h"
#include "mdefines.h"
#include "histogram.h"

Measurement::Measurement(QObject *parent)
	: QObject(parent)
	, m_events(0)
	, m_mon1(0)
	, m_mon2(0)
	, m_starttime_msec(0)
	, m_meastime_msec(0)
	, m_ratetime_msec(0)
	, m_running(false)
	, m_stopping(false)
	, m_rateflag(false)
	, m_online(false)
	, m_working(true)
	, m_remote(false)
	, m_runNumber(0)
	, m_carHistHeight(128)
	, m_carHistWidth(128)
	, m_carStep(0)
{
		
	for(quint8 c = 0; c < 8; c++)
	{
    		m_rate[10][c] = 0;
    		m_counter[0][c] = 0;
    		m_counter[1][c] = 0;
    		m_preset[c] = 0;
    		m_counterStart[c] = 0;
    		m_counterOffset[c] = 0;
    		m_master[c] = false;
    		m_stopped[c] = false;
		m_ratecount[c] = 0;
		m_ratepointer[c] = 0;
	}
}

Measurement::~Measurement()
{
}

/*!
    \fn Measurement::setCurrentTime(unsigend long msecs)
 */
void Measurement::setCurrentTime(ulong msecs)
{
	if(m_running)
	{
    		m_meastime_msec = msecs - m_starttime_msec;
		setCounter(TCT, msecs/1000);
	}
}

/*!
    \fn Measurement::getMeastime(void)
 */
ulong Measurement::getMeastime(void)
{
	return m_meastime_msec;
}

/*!
    \fn Measurement::start(unsigned long time)
 */
void Measurement::start(ulong time)
{
	m_running = true;
	m_stopping = false;
	m_starttime_msec = time;
	m_rateflag = false;
	for(quint8 c = 0; c < 8; c++)
	{
		m_stopped[c] = false;
    		m_rate[10][c] = 0;
		m_ratecount[c] = 0;
		m_ratepointer [c] = 0;
		if(c == TCT)
		{
			m_counter[1][c] = m_counterOffset[c];
			m_counterStart[TCT] = time / 1000;
		}
	}
	// remember current value for timer counter
}


/*!
    \fn Measurement::stop(unsigned long time)
 */
void Measurement::stop(ulong /*time */)
{
	m_running = false;
	m_stopping = false;
 	m_counterOffset[TCT] = m_counter[1][TCT];
} 


/*!
    \fn Measurement::setCounter(unsigned int cNum, unsigned long long val)
 */
void Measurement::setCounter(quint32 cNum, quint64 val)
{
	QString str;
    
// set counter
	if(cNum < 8)
   		m_counter[1][cNum] = m_counterOffset[cNum] + val - m_counterStart[cNum];
    	
// if counter is reset: clear rate buffers
	if(val == 0)
	{
		for(quint8 c = 0; c < 11; c++)
			m_rate[c][cNum] = 0;
		m_counterOffset[cNum] = 0;
		m_counterStart[cNum] = 0;
	}
	
// is counter master and is limit reached?
	if(m_master[cNum] && (m_preset[cNum] > 0))
	{
		if(m_counter[1][cNum] >= m_preset[cNum] && !m_stopping)
		{
			str.sprintf("stop on counter %d, value: %lld, preset: %lld", cNum, m_counter[1][cNum], m_preset[cNum]);
			emit protocol(str);
			m_stopped[cNum] = true;
			m_stopping = true;
			emit stop();
		}
	}
}


/*!
    \fn Measurement::calcRates()
 */
void Measurement::calcRates()
{
	if(m_meastime_msec == 0)
		return;
	if(m_ratetime_msec >= m_meastime_msec)
	{
		m_ratetime_msec = m_meastime_msec;
		return;
	}
	if (m_rateflag = true)
	{
		ulong tval = (m_meastime_msec - m_ratetime_msec);
		
		for(quint8 c = 0; c < 8; c++)
		{
			if(m_ratecount[c] < 10)
				m_ratecount[c]++;
			if(m_ratecount[c] > 1)
				m_rate[m_ratepointer[c]][c] = (m_counter[1][c] - m_counter[0][c]) * 1000 / tval;
			else
				m_rate[m_ratepointer[c]][c] = 0;
			m_counter[0][c] = m_counter[1][c];
			m_ratepointer[c]++;
			if(m_ratepointer[c] == 10)
				m_ratepointer[c] = 0;
		}
	}
	m_ratetime_msec = m_meastime_msec;
	m_rateflag = true;
}

/*!
    \fn Measurement::calcMeanRates()
 */
void Measurement::calcMeanRates()
{
	for(quint8 c = 0; c < 8; c++)
	{
		ulong val2 = 0;
		for(quint8 d = 1; d < m_ratecount[c]; d++)
			val2 += m_rate[d][c];
		if(m_ratecount[c] > 1)
			m_rate[10][c] = val2 / (m_ratecount[c] - 1);
		else
			m_rate[10][c] = 0;			
	}
}

/*!
    \fn Measurement::getCounter(unsigned char cNum)
 */
quint64 Measurement::getCounter(quint8 cNum)
{
	if(cNum < 8)
		return m_counter[1][cNum];
	else
		return 0;
}


/*!
    \fn Measurement::getRate(unsigned char cNum)
 */
ulong Measurement::getRate(quint8 cNum)
{
	if(cNum < 8)
		return m_rate[10][cNum];
	else
		return 0;
}




/*!
    \fn Measurement::isOk(void)
 */
quint8 Measurement::isOk(void)
{
	if(m_online)
	{
		if(m_working)
		{
//    			qDebug("online&working");
    			return 0;
		}
    		else 
		{
//    			qDebug("online not working");
    			return 1;
    		}
   	}
//	qDebug("not online, not working");
    	return 2;
}


/*!
    \fn Measurement::setOnline(bool truth)
 */
void Measurement::setOnline(bool truth)
{
	QString str;
	
	m_online = truth;
	if (m_online)
		str.sprintf("MCPD online");
	else
		str.sprintf("MCPD offline");
	emit protocol(str, 1);	
}


/*!
    \fn Measurement::setPreset(unsigned char cNum, unsigned long prval, bool mast)
 */
void Measurement::setPreset(quint8 cNum, ulong prval, bool mast)
{
	if(cNum < 8)
	{
		if(mast)
		{
    			// clear all other master flags
    			for(quint8 c = 0; c < 8;c++)
    				m_master[cNum] = false;
    			// set new master
    			m_master[cNum] = true;
    		}
    		else
    		// just clear master
    			m_master[cNum] = false;
    		
    		m_preset[cNum] = prval;
    		if(cNum == EVCT || cNum == M1CT || cNum == M2CT)
    			if(m_master[cNum])
    				emit setCountlimit(cNum, prval);
			else	
    				emit setCountlimit(cNum, 0);
	}
}


/*!
    \fn Measurement::getPreset(unsigned char cNum)
 */
ulong Measurement::getPreset(quint8 cNum)
{
	if(cNum < 8)
		return m_preset[cNum];
	else
		return 0;
}


/*!
    \fn Measurement::setRunnumber(unsigned int number)
 */
void Measurement::setRunnumber(quint32 number)
{
	m_runNumber = number;
}


/*!
    \fn Measurement::setListmode(bool truth)
 */
void Measurement::setListmode(bool truth)
{
	emit acqListfile(truth);
}


/*!
    \fn Measurement::setCarHistSize(unsigned int h, unsigned int w)
 */
void Measurement::setCarHistSize(quint32 h, quint32 w)
{
	m_carHistWidth = w;
	m_carHistHeight = h;
}


/*!
    \fn Measurement::remote(bool truth)
 */
void Measurement::setRemote(bool truth)
{
	m_remote = truth;
}


/*!
    \fn Measurement::remoteStart(void)
 */
bool Measurement::remoteStart(void)
{
	return m_remote;
}


/*!
    \fn Measurement::setStep(unsigned int step)
 */
void Measurement::setStep(quint32 step)
{
	m_carStep = step;
}


/*!
    \fn Measurement::getCarWidth()
 */
quint32 Measurement::getCarWidth()
{
	return m_carHistWidth;
}


/*!
    \fn Measurement::getCarHeight()
 */
quint32 Measurement::getCarHeight()
{
	return m_carHistHeight;
}


/*!
    \fn Measurement::getRun()
 */
quint32 Measurement::getRun()
{
	return m_runNumber;
}

/*!
    \fn Measurement::isMaster(unsigned char cNum)
 */
bool Measurement::isMaster(quint8 cNum)
{
	return(m_master[cNum]);
}



/*!
    \fn Measurement::clearCounter(unsigned char cNum)
 */
void Measurement::clearCounter(quint8 cNum)
{
	if(cNum > 7)
		return;
		
	if(cNum == TCT)
	{
		if(m_running)
		{
			m_counterStart[cNum] += m_counter[1][cNum];
			m_counterStart[cNum] -= m_counterOffset[cNum];
			m_counterOffset[cNum] = 0;
		}
	}

	if(cNum == EVCT)
    		m_events = 0;
	if(cNum == M1CT)
    		m_mon1 = 0;
	if(cNum == M2CT)
		m_mon2 = 0;

	m_counter[1][cNum] = 0;
	
	for(quint8 c = 0; c < 11; c++)
		m_rate[c][cNum] = 0;
	m_counterOffset[cNum] = 0;

	m_ratecount[cNum] = 0;
	m_ratepointer[cNum] = 0;
}

/*!
    \fn Measurement::hasStopped(unsigned char cNum)
 */
bool Measurement::hasStopped(quint8 cNum)
{
	if(cNum < 8)
		return m_stopped[cNum];
	else
    		return false;
}

/*!
    \fn Measurement::copyCounters(void)
 */
void Measurement::copyCounters(void)
{
	setCounter(EVCT, m_events);
	setCounter(M1CT, m_mon1);
	setCounter(M2CT, m_mon2);
}


/*!
    \fn Measurement::limitReached(unsigned char cNum)
 */
bool Measurement::limitReached(quint8 cNum)
{
	if(cNum >= 8)
		return false;
    
	if(m_master[cNum] && (m_counter[1][cNum] >= m_preset[cNum]))
		return true;
	else 
		return false;
}

