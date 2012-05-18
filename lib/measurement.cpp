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
#include <QDateTime>
#include <QTimerEvent>
#include <QCoreApplication>
#include <QStringList>
#include <QUdpSocket>
#include "measurement.h"
#include "mdefines.h"
#include "histogram.h"
#include "mapcorrect.h"
#include "mesydaq2.h"
#include "logging.h"
#if defined(_MSC_VER)
	#include "stdafx.h"
#endif

#include <cmath>

/*!
    \fn Measurement::Measurement(Mesydaq2 *mesy, QObject *parent)

    constructor

    \param mesy Mesydaq2 object to control the hardware
    \param parent Qt parent object
*/
Measurement::Measurement(Mesydaq2 *mesy, QObject *parent)
	: QObject(parent)
	, m_mesydaq(mesy)
	, m_posHistMapCorrection(NULL)
	, m_posHistCorrected(NULL)
	, m_lastTime(0)
	, m_starttime_msec(0)
	, m_meastime_msec(0)
	, m_status(IDLE)
	, m_rateflag(false)
	, m_online(false)
	, m_working(true)
	, m_remote(false)
	, m_headertime(0)
	, m_rateTimer(0)
	, m_onlineTimer(0)
	, m_packages(0)
	, m_triggers(0)
	, m_runID(0)
	, m_mode(DataAcquisition)
	, m_histfilename("")
	, m_histPath(getenv("HOME"))
	, m_listPath(getenv("HOME"))
{
	m_Hist[PositionHistogram] = NULL;
	m_Hist[AmplitudeHistogram] = NULL;
	m_Spectrum[TimeSpectrum] = NULL;
	m_Spectrum[Diffractogram] = NULL;
	m_Spectrum[TubeSpectrum] = NULL;

    	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "MesyTec", "QMesyDAQ");
    	setConfigfilepath(settings.value("config/configfilepath", getenv("HOME")).toString());

	setRunId(settings.value("config/lastrunid", "0").toUInt());

	connect(m_mesydaq, SIGNAL(analyzeDataBuffer(DATA_PACKET)), this, SLOT(analyzeBuffer(DATA_PACKET)));
	connect(this, SIGNAL(stopSignal()), m_mesydaq, SLOT(stop()));
	for (quint8 i = 0; i < TIMERID; ++i)
	{
		m_counter[i] = new MesydaqCounter();
		connect(m_counter[i], SIGNAL(stop()), this, SLOT(requestStop()));
	}
	m_counter[TIMERID] = new MesydaqTimer();
	connect(m_counter[TIMERID], SIGNAL(stop()), this, SLOT(requestStop()));

	resizeHistogram(m_mesydaq->width(), m_mesydaq->height());

	connect(this, SIGNAL(acqListfile(bool)), m_mesydaq, SLOT(acqListfile(bool)));

	m_rateTimer = startTimer(8);	// every 8 ms calculate the rates
	m_onlineTimer = startTimer(60);	// every 60 ms check measurement
}

quint16 Measurement::height()
{
	if (m_mode == ReplayListFile)
		return qMax(m_Hist[AmplitudeHistogram]->height(), m_Hist[PositionHistogram]->height());
	return m_height;
}

quint16 Measurement::width()
{
	if (m_mode == ReplayListFile)
		return qMax(m_Hist[AmplitudeHistogram]->width(), m_Hist[PositionHistogram]->width());
	return m_width;
}

/*!
    \fn void Measurement::resizeHistogram(quint16 w, quint16 h, bool clr, bool resize)


    \param w - new width of the histogram
    \param h - new height of the histogram
    \param clr - clear existing histogram data
    \param resize - automatic resize definition
 */
void Measurement::resizeHistogram(quint16 w, quint16 h, bool clr, bool resize)
{
	m_height = h;
	m_width = w;

	if (!m_Hist[AmplitudeHistogram])
		m_Hist[AmplitudeHistogram] = new Histogram(w, h);
	else
	{
		if (clr)
			m_Hist[AmplitudeHistogram]->clear();
		m_Hist[AmplitudeHistogram]->setAutoResize(resize);
		m_Hist[AmplitudeHistogram]->resize(w, h);
	}

	if (!m_Hist[PositionHistogram])
		m_Hist[PositionHistogram] = new Histogram(w, h);
	else
	{
		if (clr)
			m_Hist[PositionHistogram]->clear();
		m_Hist[PositionHistogram]->setAutoResize(resize);
		m_Hist[PositionHistogram]->resize(w, h);
	}
	if (!m_Spectrum[TimeSpectrum])
		m_Spectrum[TimeSpectrum] = new Spectrum(h);
	else
	{
		if (clr)
			m_Spectrum[TimeSpectrum]->clear();
		m_Spectrum[TimeSpectrum]->setAutoResize(resize);
		m_Spectrum[TimeSpectrum]->resize(h);
	}
	
	if (!m_Spectrum[Diffractogram])
		m_Spectrum[Diffractogram] = new Spectrum(w);
	else
	{
		if (clr)
			m_Spectrum[Diffractogram]->clear();
		m_Spectrum[Diffractogram]->setAutoResize(resize);
		m_Spectrum[Diffractogram]->resize(w);
	}

	if (!m_Spectrum[TubeSpectrum])
		m_Spectrum[TubeSpectrum] = new Spectrum(w);
	else
	{
		if (clr)
			m_Spectrum[TubeSpectrum]->clear();
		m_Spectrum[TubeSpectrum]->setAutoResize(resize);
		m_Spectrum[TubeSpectrum]->resize(w);
	}
}

void Measurement::destroyHistogram(void)
{
	if (m_Hist[AmplitudeHistogram])
		delete m_Hist[AmplitudeHistogram];
	m_Hist[AmplitudeHistogram] = NULL;

	if (m_Hist[PositionHistogram])
		delete m_Hist[PositionHistogram];
	m_Hist[PositionHistogram] = NULL;

	if (m_Spectrum[TimeSpectrum])
		delete m_Spectrum[TimeSpectrum];
	m_Spectrum[TimeSpectrum] = NULL;

	if (m_Spectrum[Diffractogram])
		delete m_Spectrum[Diffractogram];
	m_Spectrum[Diffractogram] = NULL;
	
	if (m_Spectrum[TubeSpectrum])
		delete m_Spectrum[TubeSpectrum];
	m_Spectrum[TubeSpectrum] = NULL;
}

//! destructor
Measurement::~Measurement()
{
        if (m_rateTimer)
		killTimer(m_rateTimer);
	m_rateTimer = 0;

	if (m_onlineTimer)
		killTimer(m_onlineTimer);
	m_onlineTimer = 0;

	if (m_posHistCorrected)
		delete m_posHistCorrected;
	m_posHistCorrected = NULL;

	if (m_posHistMapCorrection)
		delete m_posHistMapCorrection;
	m_posHistMapCorrection = NULL;

	destroyHistogram();
}

/*!
    \fn Measurement::timerEvent(QTimerEvent *event)

    callback for the timer events

    \param event event structure
*/
void Measurement::timerEvent(QTimerEvent *event)
{
	int id = event->timerId(); 
	if (id == m_rateTimer)
		calcRates();
	else if (id == m_onlineTimer)
	{
		if(!isOk())
			setOnline(false);
	}
}
/*!
    \fn Measurement::setCurrentTime(quint64 msecs)
  
    sets the current time for the measurement
    the timer will be updated during the events 

    \param msecs current time in ms
 */
void Measurement::setCurrentTime(quint64 msecs)
{
	if(m_status == STARTED)
	{
    		m_meastime_msec = msecs - m_starttime_msec;
		for (quint8 i = 0; i < TIMERID; ++i)
			m_counter[i]->setTime(m_meastime_msec);
	}
}

/*!
    \fn Measurement::getMeastime(void)

    \return measurement time
 */
quint64 Measurement::getMeastime(void)
{
	return m_meastime_msec;
}

/*!
    \fn Measurement::start()

    starts the measurement and initialize all counters and timers
 */
void Measurement::start()
{
	m_mode = DataAcquisition;
	resizeHistogram(m_mesydaq->width(), m_mesydaq->height());
	foreach (MesydaqCounter *c, m_counter)
		c->reset();	
	m_packages = 0;
	m_triggers = 0;
	m_mesydaq->setRunId(m_mesydaq->runId() + 1);
	m_mesydaq->start();
	m_status = STARTED;
	m_starttime_msec = m_mesydaq->time();
	MSG_INFO << "event counter limit : " << m_counter[EVID]->limit();
	MSG_INFO << "timer limit : " << m_counter[TIMERID]->limit() / 1000;
	foreach (MesydaqCounter *c, m_counter)
	{
		c->start(m_starttime_msec);
		MSG_INFO<< "counter " << *c << " value : " << c->value() << " limit : " << c->limit();
	}
	setROI(QRectF(0,0, m_mesydaq->width(), m_mesydaq->height()));
}

/*!
    \fn Measurement::requestStop()

    callback for the request to stop the measurement
 */
void Measurement::requestStop()
{
	if (m_status == STARTED)
	{
		m_status = STOPPED;
		emit stopSignal(false);
		MSG_NOTICE << "Max " << m_Hist[PositionHistogram]->max(0) << " was at pos " << m_Hist[PositionHistogram]->maxpos(0);
	}
}

/*!
    \fn Measurement::stop()
    
    really stop the measurement
 */
void Measurement::stop()
{
	if (m_status != IDLE)
	{ 
		if (m_status == STARTED)
			m_mesydaq->stop(); 
		quint64 time = m_mesydaq->time();
		foreach (MesydaqCounter *c, m_counter)
			c->stop(time);
		MSG_ERROR << "packages : " << m_packages << " triggers : " << m_triggers;
		if (m_triggers)
		{
			for(int i = 0; i < m_counter.size(); ++i)
				MSG_NOTICE << "Counter " << i << " gots " << m_counter[i]->value() << " events";
		}
	}
	m_status = IDLE;
} 

/*!
    \fn Measurement::cont()
    \todo implement me
*/
void Measurement::cont()
{
}

/*!
    \fn Measurement::setCounter(quint32 cNum, quint64 val)

    fill a counter with a value

    \param cNum number of the counter
    \param val value of the counter
 */
void Measurement::setCounter(quint32 cNum, quint64 val)
{
// set counter
	MSG_NOTICE << "Measurement::setCounter(cNum = " << cNum << ", val = " << val << ')';
	if(m_counter.contains(cNum))
	{
		if (val == 0)
			m_counter[cNum]->reset();
		else
   			m_counter[cNum]->set(val);
// is counter master and is limit reached?
		if(m_counter[cNum]->isStopped() && m_status != STOPPED)
		{
			MSG_NOTICE << "stop on counter " << cNum << ", value: " << m_counter[cNum]->value() << ", preset: " << m_counter[cNum]->limit();
			m_status = STOPPED;
			emit stopSignal();
		}
	}
}

/*!
    \fn Measurement::calcRates()

    calculates the rates for all counters
 */
void Measurement::calcRates()
{
	foreach(MesydaqCounter *c, m_counter)
		c->calcRate();
}

/*!
    \fn Measurement::calcMeanRates()

    calculates the mean rates for all counters
 */
void Measurement::calcMeanRates()
{
	foreach(MesydaqCounter *c, m_counter)
		c->calcMeanRate();
}

/*!
    \fn Measurement::getCounter(quint8 cNum)

    get the current value of the counter

    \param cNum number of the counter
    \return counter value
 */
quint64 Measurement::getCounter(quint8 cNum)
{
	if(m_counter.contains(cNum))
		return m_counter[cNum]->value();
	return 0;
}

/*!
    \fn Measurement::getRate(quint8 cNum)

    get the rate of a counter

    \param cNum number of the counter
    \return counter rate
 */
ulong Measurement::getRate(quint8 cNum)
{
	if(m_counter.contains(cNum))
		return m_counter[cNum]->rate();
	return 0;
}

/*!
    \fn Measurement::isOk(void)

    return the status of the measurement
	- 0 - online and working
	- 1 - online and not working
	- 2 - not oline
 */
quint8 Measurement::isOk(void)
{
	quint8 ret(2);
	if (m_online)
	{
		if (m_working)
    			ret = 0;
    		else 
    			ret = 1;
   	}
    	return ret;
}

/*!
    \fn Measurement::setOnline(bool truth)

    sets the measurement online/offline

    \param truth online = true, offline = false
 */
void Measurement::setOnline(bool truth)
{
	m_online = truth;
	MSG_NOTICE << "MCPD " << (const char*)(m_online ? "online" : "offline");
}

/*!
    \fn Measurement::setPreset(quint8 cNum, quint64 prval, bool mast)

    sets the counter preset and master counter, all other counters will be set to slave
    if the counter is set to master

    \param cNum number of the counter
    \param prval preset value
    \param mast should the counter be master or not
 */
void Measurement::setPreset(quint8 cNum, quint64 prval, bool mast)
{
	if(m_counter.contains(cNum))
	{
		MSG_NOTICE << "setPreset counter: " << cNum << " to " << prval << ' ' << (const char*)(mast ? "master" : "slave");
		if (mast)
		{
    			// clear all other master flags
			foreach(MesydaqCounter *c, m_counter)
				c->setMaster(false);
    		}
    		m_counter[cNum]->setMaster(mast);
    		m_counter[cNum]->setLimit(prval);
	}
}


/*!
    \fn Measurement::getPreset(quint8 cNum)

    get the preset value of the counter

    \param cNum number of the counter
    \return preset value
 */
quint64 Measurement::getPreset(quint8 cNum)
{
	if(m_counter.contains(cNum))
		return m_counter[cNum]->limit();
	return 0;
}

/*!
    \fn Measurement::setListmode(bool truth)

    sets the list mode 

    \param truth list mode
 */
void Measurement::setListmode(bool truth)
{
	emit acqListfile(truth);
}

/*!
    \fn Measurement::setRemote(bool truth)

    sets the flag for remote control

    \param truth remote on/off
 */
void Measurement::setRemote(bool truth)
{
	m_remote = truth;
}


/*!
    \fn Measurement::remoteStart(void)

     gets information whether the remote control is switched on or not
	
     \return status of remote control
 */
bool Measurement::remoteStart(void)
{
	return m_remote;
}

/*!
    \fn Measurement::isMaster(quint8 cNum)
    
    is counter master or not

    \param cNum number of the counter
    \return master state
 */
bool Measurement::isMaster(quint8 cNum)
{
	if (m_counter.contains(cNum))
		return m_counter[cNum]->isMaster();
	return false;
}

/*!
    \fn Measurement::clearCounter(quint8 cNum)

    resets the counter, clear preset and set value to 0

    \param cNum number of the counter
 */
void Measurement::clearCounter(quint8 cNum)
{
	if(m_counter.contains(cNum))
		m_counter[cNum]->reset();
}

/*!
    \fn Measurement::clearAllHist(void)

    clears all histograms
 */
void Measurement::clearAllHist(void)
{
	if (m_Hist[PositionHistogram])
		m_Hist[PositionHistogram]->clear();
	if (m_Hist[AmplitudeHistogram])
		m_Hist[AmplitudeHistogram]->clear();
	if (m_Spectrum[TimeSpectrum])
		m_Spectrum[TimeSpectrum]->clear();
	if (m_Spectrum[Diffractogram])
		m_Spectrum[Diffractogram]->clear();
	if (m_Spectrum[TubeSpectrum])
		m_Spectrum[TubeSpectrum]->clear();
	if (m_posHistCorrected)
		m_posHistCorrected->clear();
	m_lastTime = 0;
}

/*!
    \fn Measurement::clearChanHist(quint16 chan)

    clears the spectra of a tube

    \param chan tube number
 */
void Measurement::clearChanHist(quint16 chan)
{
	if (m_Hist[PositionHistogram])
		m_Hist[PositionHistogram]->clear(chan);
	if (m_Hist[AmplitudeHistogram])
		m_Hist[AmplitudeHistogram]->clear(chan);
//	if (m_posHistCorrected)
//		m_posHistCorrected->clear(chan);
}

/*!
    \fn Spectrum *Measurement::posData(quint16 line)

    gets a position spectrum of a tube

    \param line tube number
    \return spectrum if line exist otherwise NULL pointer
*/
Spectrum *Measurement::posData(quint16 line)
{
	if (m_Hist[PositionHistogram])
		return m_Hist[PositionHistogram]->spectrum(line);
	else
		return NULL;
}

/*!
    \fn Spectrum *Measurement::posData()

    gets the position histogram

    \return position histogram if exist otherwise NULL pointer
*/
Spectrum *Measurement::posData()
{
	if (m_Hist[PositionHistogram])
		return m_Hist[PositionHistogram]->spectrum();
	else
		return NULL;
}

/*!
    \fn Spectrum *Measurement::ampData()

    gets the amplitude histogram

    \return amplitude histogram if exist otherwise NULL pointer
*/
Spectrum *Measurement::ampData()
{
	if (m_Hist[AmplitudeHistogram])
		return m_Hist[AmplitudeHistogram]->spectrum();
	else
		return NULL;
}

/*!
    \fn Spectrum *Measurement::ampData(quint16 line)

    gets a amplitude spectrum of a tube

    \param line tube number
    \return spectrum if line exist otherwise NULL pointer
*/
Spectrum *Measurement::ampData(quint16 line)
{
	if (m_Hist[AmplitudeHistogram])
		return m_Hist[AmplitudeHistogram]->spectrum(line);
	else
		return NULL;
}

/*!
    \fn Spectrum *Measurement::timeData()

    gets a time spectrum of all events

    \return spectrum if line exist otherwise NULL pointer
*/
Spectrum *Measurement::timeData()
{
	return m_Spectrum[TimeSpectrum];
}

/*!
    \fn Measurement::writeHistograms(const QString &name)

    writes the position and amplitude histogram to a file

    \param name file name
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
		if (m_Hist[PositionHistogram])
		{
			t << "position data: 1 row title (8 x 8 detectors), position data in columns" << '\r' << '\n';
    			t << m_Hist[PositionHistogram]->format() << '\r' << '\n';
		}
		if (m_Hist[AmplitudeHistogram])
		{
			t << "amplitude/energy data: 1 row title (8 x 8 detectors), amplitude data in columns" << '\r' << '\n';
    			t << m_Hist[AmplitudeHistogram]->format() << '\r' << '\n';
		}
		f.close();
	}
}

/*!
    \fn Measurement::readHistograms(const QString &name)

    reads the position and amplitude histogram from a file

    \param name file name
 */
void Measurement::readHistograms(const QString &name)
{
	if(name.isEmpty())
		return;

	QFile f;
	f.setFileName(name);
	if (f.open(QIODevice::ReadOnly)) 
	{    
		m_mode = HistogramLoad;
// use a text stream
		QTextStream t(&f);
// Title
		QStringList list = t.readLine().split(QRegExp("\\s+"));
		if (list.size() >= 3 && list[0] == "mesydaq" && list[1] == "Histogram" && list[2] == "File")
		{
			setHistfilename(name);
			clearAllHist();
			resizeHistogram(0, 0);

			while(!t.atEnd())
			{
				list = t.readLine().split(QRegExp("\\s+"));
				if (list.size() >= 2 && list[1].startsWith("data"))
				{
					if (list[0] == "position")
						fillHistogram(t, m_Hist[PositionHistogram]);
					else if (list[0] == "amplitude/energy")
						fillHistogram(t, m_Hist[AmplitudeHistogram]);
				}
			}	
			resizeHistogram(m_Hist[PositionHistogram]->width() ? m_Hist[PositionHistogram]->width() : m_Hist[AmplitudeHistogram]->width(), m_Hist[PositionHistogram]->width() ?  m_Hist[PositionHistogram]->height() : m_Hist[AmplitudeHistogram]->height(), false);
		}
		f.close();
	}
}

/*!
    \fn Measurement::fillHistogram(QTextStream &t, Histogram *hist)

    analyzes the text stream down to a empty line and put the values
    into a histogram
*/

void Measurement::fillHistogram(QTextStream &t, Histogram *hist)
{
	QString tmp = t.readLine();
	QStringList list = tmp.split(QRegExp("\\s+"), QString::SkipEmptyParts);
	int tubes = list.size();

	QStringList lines;

	while(!(tmp = t.readLine()).isEmpty())
		lines << tmp;
	
	hist->resize(lines.size(), tubes);

	for (int j = 0; j < lines.size(); ++j)
	{
		list = lines[j].split(QRegExp("\\s+"), QString::SkipEmptyParts);
		if (list.size() == (tubes + 1))
		{
//			add values to histogram
			quint16 bin = list[0].toUShort();
			for(int i = 1; i < list.size(); ++i)
				hist->addValue((i - 1), bin, list[i].toULongLong());
		}
	}
}

/*!
    \fn Measurement::analyzeBuffer(DATA_PACKET pd)

    analyze the data packet and put all events into the right counters and/or histogram

    \param pd data packet
 */
void Measurement::analyzeBuffer(DATA_PACKET pd)
{
	ulong 	data;
	quint16 neutrons = 0;
	quint16 triggers = 0;
	quint64 tim;
	quint16 mod = pd.deviceId;
	m_headertime = pd.time[0] + (quint64(pd.time[1]) << 16) + (quint64(pd.time[2]) << 32);
	setCurrentTime(m_headertime / 10000); // headertime is in 100ns steps
	m_counter[TIMERID]->setTime(m_headertime / 10000);
	m_runID = pd.runID;

	m_packages++;
	if(pd.bufferType < 0x0003)
	{
// extract parameter values:
		QChar c('0');
		quint32 datalen = (pd.bufferLength - pd.headerLength) / 3;
//
// status IDLE is for replaying files
// 
		for(quint32 counter = 0, i = 0; i < datalen && (m_status == STARTED || m_status == IDLE); ++i, counter += 3)
		{
			tim = pd.data[counter + 1] & 0x7;
			tim <<= 16;
			tim += pd.data[counter];
			tim += m_headertime;
// id stands for the trigId and modId depending on the package type
			quint8 id = (pd.data[counter + 2] >> 12) & 0x7;
// not neutron event (counter, chopper, ...)
			m_counter[TIMERID]->setTime(tim / 10000);
			if((pd.data[counter + 2] & TRIGGEREVENTTYPE))
			{
				triggers++;
				quint8 dataId = (pd.data[counter + 2] >> 8) & 0x0F;
				data = ((pd.data[counter + 2] & 0xFF) << 13) + ((pd.data[counter + 1] >> 3) & 0x7FFF);
				switch(dataId)
				{
					case MON1ID :
					case MON2ID :
					case MON3ID :
					case MON4ID :
						++(*m_counter[dataId]);
//						MSG_DEBUG << "counter " << dataId << " : (" << i << " - " << triggers << ')' << m_counter[dataId]->value() << " : " << data;
						break;
					case TTL1ID :
					case TTL2ID :
						++(*m_counter[dataId]);
						MSG_NOTICE << "counter " << dataId << " : (" << i << " - " << triggers << ')' << m_counter[dataId]->value() << " : " << data;
						break;
					case ADC1ID :
					case ADC2ID :
						++(*m_counter[dataId]);
						MSG_NOTICE << "counter " << dataId << " : (" << i << " - " << triggers << ')' << m_counter[dataId]->value() << " : " << data;
						break;
					default:
						MSG_ERROR << "counter " << dataId << " : " << i;
						break;
				}
			}
// neutron event:
			else
			{
				neutrons++;
				quint8 slotId = (pd.data[counter + 2] >> 7) & 0x1F;
				quint8 modChan = (id << 3) + slotId;
				quint16 chan = modChan + (mod << 6);
				quint16 amp = ((pd.data[counter+2] & 0x7F) << 3) + ((pd.data[counter+1] >> 13) & 0x7),
					pos = (pd.data[counter+1] >> 3) & 0x3FF;
//
// in MDLL, data format is different:
// y position (10 bit) is at MPSD "Amplitude" data
// amplitude (8 bit) is at MPSD "chan" data
//
				if(pd.bufferType == 0x0002)
				{
					quint16 val = chan;
					chan = amp;
					amp = val;
				}
//
// old MPSD-8 are running in 8-bit mode and the data are stored left in the ten bits
//
				if (m_mesydaq->getMpsdId(mod, id) == TYPE_MPSD8OLD)
				{
					amp >>= 2;
					pos >>= 2;
				}
// BUG in firmware, every first neutron event seems to be "buggy" or virtual
// Only on newer modules with a distinct CPLD firmware
// BUG is reported
				if (neutrons == 1 && modChan == 0 && pos == 0 && amp == 0)
				{
					MSG_WARNING << "GHOST EVENT: SlotID " << slotId << " Mod " << id;
					continue;
				}
				++(*m_counter[EVID]);
				if (m_Hist[PositionHistogram])
					m_Hist[PositionHistogram]->incVal(chan, pos);
				if (m_Hist[AmplitudeHistogram])
					m_Hist[AmplitudeHistogram]->incVal(chan, amp);
#if 0
				if (m_Spectrum[Diffractogram])
					m_Spectrum[Diffractogram]->incVal(chan);
#endif
				if (m_posHistCorrected)
					m_posHistCorrected->incVal(chan, pos);
				if (m_mesydaq->getMpsdId(mod, id) == TYPE_MSTD16)
				{
//					MSG_INFO << "MSTD-16 event : chan : " << chan << " : pos : " << pos << " : id : " << id;
					chan <<= 1;
					chan += (pos >> 9) & 0x1;
					amp &= 0x1FF;
//					if (pos >= 480)
//						++chan;
					MSG_INFO << "Put this event into channel : " << chan;
					m_Spectrum[TubeSpectrum]->incVal(chan);
//					MSG_INFO << "Value of this channel : " << m_Spectrum[TubeSpectrum]->value(chan);
				}
				else if (m_Spectrum[TubeSpectrum])
					m_Spectrum[TubeSpectrum]->incVal(chan);
			}
		}
		m_triggers += triggers;
#if 0
		for(int i = 0; i < 4; i++)
		{
			quint64 var = 0;
			for(int j = 0; j < 3; j++)
			{
				var <<= 16;
				var |= pd.param[i][2 - j];
			}
			quint64 tmp = m_counter[i]->value();
			if (var)
			{
// only differences > 1 should be logged
				if ((tmp > var && tmp > (var + 1))|| (tmp < var && (tmp + 1) < var))
				{
					MSG_ERROR << m_packages << " counter " << i << " : is " << var << " <-> should be " << m_counter[i]->value();
					setCounter(i, var);
				}
			}
		}		
#endif
	}
	else
		MSG_INFO << "buffer type : " << pd.bufferType;
}

bool Measurement::acqListfile() const
{
	return m_mesydaq ? m_mesydaq->acqListfile() : true;
}

/*!
    \fn Measurement::readListfile(QString readfilename)

    reads a list mode data file and handles the read events as same as events coming
    over the network interfaces

    \param readfilename file name for the list mode data
*/
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

	m_mode = ReplayListFile;

	quint32 blocks(0),
		bcount(0);

	str = textStream.readLine();
	qint64	seekPos;
	seekPos = str.size() + 1;
	str = textStream.readLine();
	seekPos += str.size() + 1;
	textStream.seek(seekPos);
	datStream >> sep1 >> sep2 >> sep3 >> sep4;

	bool ok = ((sep1 == sep0) && (sep2 == sep5) && (sep3 == sepA) && (sep4 == sepF));
	MSG_NOTICE << "readListfile : " << ok;

	resizeHistogram(0 /* 1024 */, 0 /* 128 */, true, true);

	QChar c('0');
	if (m_status == STARTED)
	{
		stop();
		QCoreApplication::processEvents();
	}
	DATA_PACKET 	dataBuf;
	while(ok)
	{
		datStream >> sep1 >> sep2 >> sep3 >> sep4;
// check for closing signature:
		if((sep1 == sepF) && (sep2 == sepA) && (sep3 == sep5) && (sep4 == sep0))
		{
			MSG_NOTICE << "EOF reached after " << blocks << " buffers";
			break;
		}

//		memset(&dataBuf, 0, sizeof(dataBuf));
		dataBuf.bufferLength = sep1;
		dataBuf.bufferType = sep2;
		dataBuf.headerLength = sep3;
		dataBuf.bufferNumber = sep4;
		if(dataBuf.bufferLength > 729)
		{
			MSG_ERROR << "erroneous length: " << dataBuf.bufferLength << " - aborting";
			datStream >> sep1 >> sep2 >> sep3 >> sep4;
			MSG_ERROR << tr("Separator: %1 %2 %3 %4").arg(sep1, 2, 16, c).arg(sep2, 2, 16, c).arg(sep3, 2, 16, c).arg(sep4, 2, 16, c);
			break;
		}
		quint16 *pD = (quint16 *)&dataBuf.bufferLength;
		for(int i = 4; i < dataBuf.bufferLength; i++)
			datStream >> pD[i];
		if (!blocks)
		{
			quint64 tmp = dataBuf.time[0] + (quint64(dataBuf.time[1]) << 16) + (quint64(dataBuf.time[2]) << 32);
			foreach(MesydaqCounter *c, m_counter)
				c->reset();
			m_counter[TIMERID]->start(tmp / 10000);
		}

// hand over data buffer for processing
		analyzeBuffer(dataBuf);
// increment local counters
		blocks++;
		bcount++;
// check for next separator:
//		qint64 p = textStream.device()->pos();
//		MSG_DEBUG << tr("at position : %1 (0x%2)").arg(p).arg(p, 8, 16, c);
		datStream >> sep1 >> sep2 >> sep3 >> sep4;
//		MSG_ERROR << tr("Separator: %1 %2 %3 %4").arg(sep1, 2, 16, c).arg(sep2, 2, 16, c).arg(sep3, 2, 16, c).arg(sep4, 2, 16, c);
		ok = ((sep1 == sep0) && (sep2 == sepF) && (sep3 == sep5) && (sep4 == sepA));
		if (!ok)
		{
			MSG_ERROR << "File structure error - read aborted after " << blocks << " buffers";
			qint64 p = textStream.device()->pos();
			MSG_ERROR << tr("at position : %1 (0x%2)").arg(p).arg(p, 8, 16, c);
			MSG_ERROR << tr("Separator: %1 %2 %3 %4").arg(sep1, 2, 16, c).arg(sep2, 2, 16, c).arg(sep3, 2, 16, c).arg(sep4, 2, 16, c);
		}
		if(!(bcount % 1000))
		{
			emit draw();
			QCoreApplication::processEvents();
		}
	}	
	datfile.close();
}

/*!
    \fn Measurement::getPosMean(float &mean, float &sigma)

    gives the mean value and the standard deviation of the last position events
  
    \param mean mean value
    \param sigma standard deviation
 */
void Measurement::getPosMean(float &mean, float &sigma)
{
	m_Hist[PositionHistogram]->getMean(mean, sigma);
}

/*!
    \fn Measurement::getPosMean(quint16 chan, float &mean, float &sigma)

    gives the mean value and the standard deviation of the last events in the tube chan of the position histogram

    \param chan the number of the tube
    \param mean mean value
    \param sigma standard deviation
 */
void Measurement::getPosMean(quint16 chan, float &mean, float &sigma)
{
	m_Hist[PositionHistogram]->getMean(chan, mean, sigma);
}

/*!
    \fn Measurement::getAmpMean(float &mean, float &sigma)

    gives the mean value and the standard deviation of the last amplitude events
  
    \param mean mean value
    \param sigma standard deviation
 */
void Measurement::getAmpMean(float &mean, float &sigma)
{
	m_Hist[AmplitudeHistogram]->getMean(mean, sigma);
}

/*!
    \fn Measurement::getAmpMean(quint16 chan, float &mean, float &sigma)

    gives the mean value and the standard deviation of the last events in the tube chan of the amplitude histogram

    \param chan the number of the tube
    \param mean mean value
    \param sigma standard deviation
 */
void Measurement::getAmpMean(quint16 chan, float &mean, float &sigma)
{
	m_Hist[AmplitudeHistogram]->getMean(chan, mean, sigma);
}

/**
    \fn Measurement::getTimeMean(float &mean, float &sigma)

    gives the mean value and the standard deviation of the last events in the time spectrum

    \param mean mean value
    \param sigma standard deviation of the mean value
    \return mean value
 */
void Measurement::getTimeMean(float &mean, float &sigma)
{
	mean = m_Spectrum[TimeSpectrum]->mean(sigma);
}

/*!
    \fn void Measurement::setROI(QRectF r)

    define a region of interest (ROI) for counting all events in it
 
    \param r rectangle as ROI
 */
void Measurement::setROI(QRectF r)
{
	int x = round(r.x()),
	    y = round(r.y()),
	    w = round(r.width()),
	    h = round(r.height());
	MSG_NOTICE << "setROI : " << x << ',' << y << ',' << w << ',' << h;
	m_roi = QRect(x, y, w, h);
}

QRectF Measurement::getROI(void)
{
	return m_roi;
}

quint64 Measurement::ampEventsInROI()
{
	quint64 tmp = m_Hist[AmplitudeHistogram]->getCounts(m_roi);
	return tmp;
}

quint64 Measurement::posEventsInROI()
{
	quint64 tmp = m_Hist[PositionHistogram]->getCounts(m_roi);
	return tmp;
}

Spectrum *Measurement::diffractogram()
{
#if defined(_MSC_VER)
#	pragma message("TODO tube spectrum !!!")
#else
#	warning TODO tube spectrum !!!
#endif
#if 0
	if (m_Spectrum[TubeSpectrum]->width() > 0)
		return m_Spectrum[TubeSpectrum];
#endif
	m_Spectrum[Diffractogram]->resize(m_Hist[PositionHistogram]->height());
	for (int i = 0; i < m_Spectrum[Diffractogram]->width(); ++i)
	{
		Spectrum *spec = m_Hist[AmplitudeHistogram]->spectrum(i);
		if (spec)
			m_Spectrum[Diffractogram]->setValue(i, spec->getTotalCounts());
	}
	return m_Spectrum[Diffractogram];
}

void Measurement::setListFileHeader(const QByteArray& header)
{
	if (m_mesydaq)
		m_mesydaq->setListFileHeader(header);
}

void Measurement::setHistfilename(QString name) 
{
	m_histfilename = name;
	if(m_histfilename.indexOf(".mtxt") == -1)
		m_histfilename.append(".mtxt");
}
     
void Measurement::setConfigfilename(const QString &name)
{
	if (name.isEmpty())
		m_configfile.setFile("mesycfg.mcfg");
	else
		m_configfile.setFile(name);
}

QString Measurement::getConfigfilename(void) 
{
	if (m_configfile.exists())
		return m_configfile.absoluteFilePath();
	else
		return QString("");
}

/*!
    \fn bool Measurement::loadSetup(const QString &name)

    Loads the setup from a file. This function should be able to load
    "MesyDAQ" files and also "QMesyDAQ" files (using QSettings class which is
    not easy human readable).

    \note MesyDAQ INI file format is not used correctly, because the section
	  names are not unique: imagine you don't have a single MCPD-8 + MPSD-8 ...

    \param name file name
    \return true if successfully loaded otherwise false
 */
bool Measurement::loadSetup(const QString &name)
{
	setConfigfilename(name);
	if (getConfigfilename().isEmpty())
		return false;

	bool 		bOK(false);

	MSG_NOTICE << "Reading configfile " << getConfigfilename();

	QSettings settings(getConfigfilename(), QSettings::IniFormat);

	settings.beginGroup("MESYDAQ");
	QString	home(getenv("HOME"));
	m_histPath = settings.value("histogramPath", home).toString();
	m_listPath = settings.value("listfilePath", home).toString();
	QString sz = settings.value("debugLevel", QString("%1").arg(NOTICE)).toString();
	int n = sz.toInt(&bOK);
	if (bOK)
		DEBUGLEVEL = n;
	else
	{
		if (sz.contains("fatal", Qt::CaseInsensitive)) 
			DEBUGLEVEL = FATAL;
		else if (sz.contains("error", Qt::CaseInsensitive)) 
			DEBUGLEVEL = ERROR;
		else if (sz.contains("standard", Qt::CaseInsensitive) || sz.contains("warning", Qt::CaseInsensitive) || sz.contains("default", Qt::CaseInsensitive))
			DEBUGLEVEL = WARNING;
		else if (sz.contains("notice", Qt::CaseInsensitive)) 
			DEBUGLEVEL = NOTICE;
		else if (sz.contains("details", Qt::CaseInsensitive) || sz.contains("info", Qt::CaseInsensitive)) 
			DEBUGLEVEL = INFO;
		else if (sz.contains("debug", Qt::CaseInsensitive) || sz.contains("debug", Qt::CaseInsensitive)) 
			DEBUGLEVEL = DEBUG;
	}
//	m_acquireListfile = settings.value("listmode", "true").toBool();
	settings.endGroup();

	m_mesydaq->loadSetup(settings);
	storeLastFile();
	return true;
}

/*!
    \fn bool Measurement::saveSetup(const QString &name)

    Stores the setup in a file. This function stores INI files in format of
    "MesyDAQ" instead of "QMesyDAQ" using QSettings class (which is not easy
    human readable).

    Note: MesyDAQ INI file format is not used correctly, because the section
	  names are not unique: imagine you don't have a single MCPD-8 + MPSD-8 ...

    \param name file name
    \return true if successfully saved otherwise false
 */
bool Measurement::saveSetup(const QString &name)
{
	if(name.isEmpty())
		m_configfile.setFile("mesycfg.mcfg");
	m_configfile.setFile(name);
  
	if(m_configfile.fileName().indexOf(".mcfg") == -1)
		m_configfile.setFile(m_configfile.absoluteFilePath().append(".mcfg"));

	QSettings settings(m_configfile.absoluteFilePath(), QSettings::IniFormat);

	settings.beginGroup("MESYDAQ");
	settings.setValue("comment", "QMesyDAQ configuration file");
	settings.setValue("date", QDateTime::currentDateTime().toString(Qt::ISODate));
	settings.setValue("histogramPath", m_histPath);
	settings.setValue("listfilePath", m_listPath);
	settings.setValue("debugLevel", QString("%1").arg(DEBUGLEVEL));
	settings.endGroup();

	m_mesydaq->saveSetup(settings);
	settings.sync();
	storeLastFile();
	return true;
}

void Measurement::storeLastFile(void)
{
        QSettings settings(QSettings::IniFormat, QSettings::UserScope, "MesyTec", "QMesyDAQ");
	settings.setValue("lastconfigfile", getConfigfilename());
	settings.sync();
}
