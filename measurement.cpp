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
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

#include "measurement.h"
#include "mdefines.h"
#include "histogram.h"
#include "mesydaq2.h"

Measurement::Measurement(Mesydaq2 *mesy, QObject *parent)
	: MesydaqObject(parent)
	, m_mesydaq(mesy)
	, m_posHist(NULL)
	, m_ampHist(NULL)
	, m_timeSpectrum(NULL)
	, m_lastTime(0)
	, m_starttime_msec(0)
	, m_meastime_msec(0)
	, m_running(false)
	, m_stopping(false)
	, m_rateflag(false)
	, m_online(false)
	, m_working(true)
	, m_remote(false)
	, m_headertime(0)
{
	connect(m_mesydaq, SIGNAL(analyzeDataBuffer(DATA_PACKET &)), this, SLOT(analyzeBuffer(DATA_PACKET &)));
	connect(this, SIGNAL(stop()), m_mesydaq, SLOT(stop()));
	for (quint8 i = 0; i < 8; ++i)
		connect(&m_counter[i], SIGNAL(stop()), this, SLOT(requestStop()));
	m_ampHist = new Histogram(64, 960);
	m_posHist = new Histogram(64, 960);
	m_timeSpectrum = new Spectrum(960);

	connect(this, SIGNAL(stopSignal()), m_mesydaq, SLOT(stop()));
	connect(this, SIGNAL(acqListfile(bool)), m_mesydaq, SLOT(acqListfile(bool)));
}

Measurement::~Measurement()
{
	if (m_ampHist)
		delete m_ampHist;
	m_ampHist = NULL;
	if (m_posHist)
		delete m_posHist;
	m_posHist = NULL;
	if (m_timeSpectrum)
		delete m_timeSpectrum;
	m_timeSpectrum = NULL;
}

/*!
    \fn Measurement::setCurrentTime(unsigend long msecs)
 */
void Measurement::setCurrentTime(quint64 msecs)
{
	if(m_running)
	{
    		m_meastime_msec = msecs - m_starttime_msec;
		for (quint8 i = 0; i < 8; ++i)
			m_counter[i].setTime(m_meastime_msec);
	}
}

/*!
    \fn Measurement::getMeastime(void)
 */
quint64 Measurement::getMeastime(void)
{
	return m_meastime_msec;
}

/*!
    \fn Measurement::start(quint64 time)
		m_theApp->getMTime());
 */
void Measurement::start()
{
	m_mesydaq->start();
	m_starttime_msec = m_mesydaq->time();
	m_running = true;
	m_stopping = false;
	protocol(tr("event counter limit : %1").arg(m_counter[EVCT].limit()));
	for (quint8 c = 0; c < 8; ++c)
	{
		m_counter[c].start(m_starttime_msec);
		protocol(tr("counter %1 limit : %2").arg(c).arg(m_counter[c].limit()));
	}
}

/*!
    \fn Measurement::requestStop()
 */
void Measurement::requestStop()
{
	m_stopping = true;
	emit stopSignal();
	protocol(tr("Max %1 was at pos %2").arg(m_posHist->max(0)).arg(m_posHist->maxpos(0)));
}

/*!
    \fn Measurement::stop(unsigned long time)
 */
void Measurement::stop()
{
	m_mesydaq->stop(); 
	quint64 time = m_mesydaq->time();
	m_running = false;
	m_stopping = false;
	for (quint8 c = 0; c < 8; ++c)
		m_counter[c].stop(time);
#if 0
 	m_counterOffset[TCT] = m_counter[1][TCT];
#endif
} 


/*!
    \fn Measurement::setCounter(unsigned int cNum, quint64 val)
 */
void Measurement::setCounter(quint32 cNum, quint64 val)
{
// set counter
	if(cNum < 8)
	{
		if (val == 0)
			m_counter[cNum].reset();
		else
   			m_counter[cNum].set(val);
// is counter master and is limit reached?
		if(m_counter[cNum].isStopped() && !m_stopping)
		{
			protocol(tr("stop on counter %1, value: %2, preset: %3").arg(cNum).arg(m_counter[cNum].value()).arg(m_counter[cNum].limit()));
			m_stopping = true;
			emit stopSignal();
		}
	}
}

/*!
    \fn Measurement::calcRates()
 */
void Measurement::calcRates()
{
	for(quint8 c = 0; c < 8; c++)
		m_counter[c].calcRate();
}

/*!
    \fn Measurement::calcMeanRates()
 */
void Measurement::calcMeanRates()
{
	for(quint8 c = 0; c < 8; c++)
		m_counter[c].calcMeanRate();
}

/*!
    \fn Measurement::getCounter(unsigned char cNum)
 */
quint64 Measurement::getCounter(quint8 cNum)
{
	if(cNum < 8)
		return m_counter[cNum].value();
	else
		return 0;
}

/*!
    \fn Measurement::getRate(unsigned char cNum)
 */
ulong Measurement::getRate(quint8 cNum)
{
	if(cNum < 8)
		return m_counter[cNum].rate();
	else
		return 0;
}

/*!
    \fn Measurement::isOk(void)
 */
quint8 Measurement::isOk(void)
{
	if (m_online)
	{
		if (m_working)
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
	m_online = truth;
	protocol(tr("MCPD %1").arg(m_online ? tr("online") : tr("offline")), 1);	
}


/*!
    \fn Measurement::setPreset(unsigned char cNum, unsigned long prval, bool mast)
 */
void Measurement::setPreset(quint8 cNum, quint64 prval, bool mast)
{
	if(cNum < 8)
	{
		protocol(tr("setPreset counter: %1 to %2 %3").arg(cNum).arg(prval).arg(mast ? tr("master") : tr("slave")), 1);	
		if (mast)
		{
    			// clear all other master flags
    			for (quint8 c = 0; c < 8;c++)
    				m_counter[cNum].setMaster(false);
    			// set new master
    			m_counter[cNum].setMaster(true);
    		}
    		else
    		// just clear master
    			m_counter[cNum].setMaster(false);
    		m_counter[cNum].setLimit(prval);
	}
}


/*!
    \fn Measurement::getPreset(unsigned char cNum)
 */
ulong Measurement::getPreset(quint8 cNum)
{
	if(cNum < 8)
		return m_counter[cNum].limit();
	else
		return 0;
}

/*!
    \fn Measurement::setListmode(bool truth)
 */
void Measurement::setListmode(bool truth)
{
	emit acqListfile(truth);
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
    \fn Measurement::isMaster(unsigned char cNum)
 */
bool Measurement::isMaster(quint8 cNum)
{
	return m_counter[cNum].isMaster();
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
#if 0
		if(m_running)
		{
			m_counterStart[cNum] += m_counter[1][cNum];
			m_counterStart[cNum] -= m_counterOffset[cNum];
			m_counterOffset[cNum] = 0;
		}
#endif
	}
	m_counter[cNum].reset();
}

/*!
    \fn Measurement::hasStopped(unsigned char cNum)
 */
bool Measurement::hasStopped(quint8 cNum)
{
	if(cNum < 8)
		return m_counter[cNum].isStopped();
    	return false;
}

/*!
    \fn Measurement::limitReached(unsigned char cNum)
 */
bool Measurement::limitReached(quint8 cNum)
{
	if(cNum >= 8)
		return false;
	return m_counter[cNum].isStopped();
#if 0   
	if(m_master[cNum] && (m_counter[1][cNum] >= m_preset[cNum]))
		return true;
	else 
		return false;
#endif
}

/*!
    \fn Measurement::clearAllHist(void)
 */
void Measurement::clearAllHist(void)
{
	if (m_posHist)
		m_posHist->clear();
	if (m_ampHist)
		m_ampHist->clear();
	if (m_timeSpectrum)
		m_timeSpectrum->clear();
	m_lastTime = 0;
}


/*!
    \fn Measurement::clearChanHist(unsigned long chan)
 */
void Measurement::clearChanHist(quint16 chan)
{
	if (m_posHist)
		m_posHist->clear(chan);
	if (m_ampHist)
		m_ampHist->clear(chan);
//	if (m_timeSpectrum)
//		m_timeSpectrum->clear(chan);
//	m_lastTime = 0;
}

/*!
    \fn Measurement::copyPosData(quint32 line, ulong *data)
 */
void Measurement::copyPosData(quint32 line, ulong *data)
{
	if (m_posHist)
		m_posHist->copyLine(line, data);
}

/*!
    \fn Measurement::copyAmpData(quint32 line, ulong *data)
 */
void Measurement::copyAmpData(quint32 line, ulong *data)
{
	if (m_ampHist)
		m_ampHist->copyLine(line, data);
}

/*!
    \fn Measurement::copyPosData(ulong *data)
 */
void Measurement::copyPosData(ulong *data)
{
	if (m_posHist)
		m_posHist->copyLine(data);
}

/*!
    \fn Measurement::copyAmpData(ulong *data)
 */
void Measurement::copyAmpData(ulong *data)
{
	if (m_ampHist)
		m_ampHist->copyLine(data);
}

/*!
    \fn Measurement::copyTimeData(ulong *data)
 */
void Measurement::copyTimeData(ulong *data)
{
	if (m_timeSpectrum)
	{
		for(quint16 i = 0; i < m_timeSpectrum->width(); i++)
    			data[i] = m_timeSpectrum->val(i);
	}
}

/*!
    \fn Measurement::writeHistograms()
 */
void Measurement::writeHistograms(const QString &name)
{
	if(name.isEmpty())
		return;

	QFile f;
	f.setFileName(name);
	if (f.open(QIODevice::WriteOnly)) 
	{    // file opened successfully
		QTextStream t( &f );        // use a text stream
		// Title
		t << "mesydaq Histogram File    " << QDateTime::currentDateTime().toString("dd.MM.yy  hh:mm:ss") << '\r' << '\n';
		t.flush();
		if (m_posHist)
    			m_posHist->writeHistogram(&f, "position data: 1 row title (8 x 8 detectors), position data in columns");
		if (m_ampHist)
			m_ampHist->writeHistogram(&f, "amplitude/energy data: 1 row title (8 x 8 detectors), amplitude data in columns");
		f.close();
	}
}

/*!
    \fn Measurement::analyzeBuffer(DATA_PACKET &pd, quint8 daq)
 */
void Measurement::analyzeBuffer(DATA_PACKET &pd)
{
	quint16 time, counter = 0;
	ulong 	data;
	quint32 i, j;
	quint16 neutrons = 0;
	quint16 triggers = 0;
	quint64 tim;
	quint16 mod = pd.deviceId;	
	m_headertime = pd.time[0] + (quint64(pd.time[1]) << 16) + (quint64(pd.time[2]) << 32);
	setCurrentTime(m_headertime / 10000); // headertime is in 100ns steps
	if(pd.bufferType < 0x0002) 
	{
		protocol(tr("Measurement::analyzeBuffer()"), 3);
// extract parameter values:
		QChar c('0');
		for(i = 0; i < 4; i++)
		{
			quint64 var = 0;
			for(j = 0; j < 3; j++)
			{
				var *= 0x10000ULL;
				var += pd.param[i][2-j];
			}
			protocol(tr("set counter %1 to %2").arg(i).arg(var), 3);
			setCounter(i, var);
		}		
// 		data length = (buffer length - header length) / (3 words per event) - 4 parameters.
 		quint32 datalen = (pd.bufferLength - pd.headerLength) / 3;
		protocol(tr("datalen = %1, m_stopping = %2").arg(datalen).arg(m_stopping), 3);
		for(i = 0; i < datalen && !m_stopping; ++i, counter += 3)
		{
			protocol(tr("i = %1").arg(i), 3);
			tim = pd.data[counter + 1] & 0x7;
			tim <<= 16;
			tim += pd.data[counter];
//			protocol(tr("time : %1 (%2 %3)").arg(tim).arg(pd.data[counter + 1] & 0x7).arg(pd.data[counter])); //, 8, 16, c));
			tim += m_headertime;
// id stands for the trigId and modId depending on the package type
			quint8 id = (pd.data[counter + 2] >> 12) & 0x7;
//			ulong delta = tim - m_lastTime;
//			m_lastTime = tim;
// not neutron event (counter, chopper, ...)
//			protocol(tr("%1 %2 %3").arg(pd.data[counter + 2],4,16,c).arg(pd.data[counter + 1],4,16,c).arg(pd.data[counter], 4, 16,c));
			if((pd.data[counter + 2] & 0x8000))
			{
				triggers++;
				quint8 dataId = (pd.data[counter + 2] >> 8) & 0x0F;
				data = (pd.data[counter + 2] & 0xFF) << 13 + (pd.data[counter + 1] >> 3) & 0x7FFF;
				time = (quint16)tim;
				switch(dataId)
				{
					case MON1ID:
						++m_counter[M1CT];
						break;
					case MON2ID:
						++m_counter[M2CT];
						break;
					default:
						break;
				}
				protocol(tr("Trigger : %1 id %2 data %3").arg(triggers).arg(id).arg(dataId));
			}
// neutron event:
			else
			{
				neutrons++;
				quint8 slotId = (pd.data[counter + 2] >> 7) & 0x1F;
				quint8 chan = (id << 3) + slotId;
				quint16 amp(0), 
					pos(0);
				if (m_mesydaq->getMpsdId(mod, slotId) == 103)
				{
					if (m_mesydaq->getMode(mod, id))
					{
						amp = (pd.data[counter+1] >> 3) & 0x3FF;
						if (amp != 0)
							protocol(tr("amp = %1").arg(amp));
					}
					else
						pos = (pd.data[counter+1] >> 3) & 0x3FF;
				}
				else
				{
					amp = (pd.data[counter+2] & 0x7F) << 3 + (pd.data[counter+1] >> 13) & 0x7;
					pos = (pd.data[counter+1] >> 3) & 0x3FF;
				}
				++m_counter[EVCT];
				protocol(tr("events %1 %2").arg(quint64(m_counter[EVCT])).arg(m_counter[EVCT].limit()), 3);
				if (m_posHist)
				{
					m_posHist->incVal(chan, pos);
				}
				if (m_ampHist)
				{
					m_ampHist->incVal(chan, amp);
				}
#if 0
// already a value?
				if(m_lastTime)
				{
					quint64 deltat = time - m_lastTime;
#warning TODO 960 ??????
					if(deltat > 959)		
						deltat = 959; 
#warning TODO				m_timeSpectrum->incVal((quint16)deltat);
				}
				m_lastTime = time;
#endif
				protocol(tr("Neutron : %1 id %2 slot %3 pos 0x%4 amp 0x%5").arg(neutrons).arg(id).arg(slotId).arg(pos, 4, 16, c).arg(amp, 4, 16, c), 3);
			}
		}
	}
}

void Measurement::readListfile(QString readfilename)
{
	QDataStream datStream;
	QTextStream textStream;
	QFile datfile;
	QString str;
	quint16 sep1, sep2, sep3, sep4;
    
	datfile.setFileName(readfilename);
	datfile.open(QIODevice::ReadOnly);
	datStream.setDevice(&datfile);
	textStream.setDevice(&datfile);

	quint32 blocks(0),
		bcount(0);

	str = textStream.readLine();
//	qDebug(str.toStdString().c_str());
	str = textStream.readLine();
//	qDebug(str.toStdString().c_str());
	datStream >> sep1 >> sep2 >> sep3 >> sep4;
	bool ok = ((sep1 == sep0) && (sep2 == sep5) && (sep3 == sepA) && (sep4 == sepF));
	protocol(tr("readListfile : %1").arg(ok));
	QChar c('0');
	while(ok)
	{
		datStream >> sep1 >> sep2 >> sep3 >> sep4;
// check for closing signature:
		if((sep1 == sepF) && (sep2 == sepA) && (sep3 == sep5) && (sep4 == sep0))
		{
			protocol(tr("EOF reached after %1 buffers").arg(blocks));
			break;
		}
		DATA_PACKET 	dataBuf;
		dataBuf.bufferLength = sep1;
		dataBuf.bufferType = sep2;
		dataBuf.headerLength = sep3;
		dataBuf.bufferNumber = sep4;
		if(dataBuf.bufferLength > 729)
		{
			protocol(tr("erroneous length: %1 - aborting").arg(dataBuf.bufferLength));
			datStream >> sep1 >> sep2 >> sep3 >> sep4;
			protocol(tr("Separator: %1 %2 %3 %4").arg(sep1, 2, 16, c).arg(sep2, 2, 16, c).arg(sep3, 2, 16, c).arg(sep4, 2, 16, c));
			break;
		}
		quint16 *pD = (quint16 *)&dataBuf.bufferLength;
		for(int i = 4; i < dataBuf.bufferLength; i++)
			datStream >> pD[i];
// hand over data buffer for processing
		analyzeBuffer(dataBuf);
// increment local counters
		blocks++;
		bcount++;
// check for next separator:
		datStream >> sep1 >> sep2 >> sep3 >> sep4;
		protocol(tr("Separator: %1 %2 %3 %4").arg(sep1, 2, 16, c).arg(sep2, 2, 16, c).arg(sep3, 2, 16, c).arg(sep4, 2, 16, c));
		ok = ((sep1 == sep0) && (sep2 == sepF) && (sep3 == sep5) && (sep4 == sepA));
		if (!ok)
			protocol(tr("File structure error - read aborted after %1 buffers").arg(blocks));
		if(bcount == 1000)
		{
			bcount = 0;
			emit draw();
		}  
	}	
	datfile.close();
	emit draw();
}

