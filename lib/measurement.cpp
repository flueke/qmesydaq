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
#include <QDateTime>
#include <QTimerEvent>
#include <QCoreApplication>
#include <QRegExp>
#include <QStringList>

#include <QDebug>

#include "measurement.h"
#include "mdefines.h"
#include "histogram.h"
#include "mesydaq2.h"

/*!
    \fn Measurement::Measurement(Mesydaq2 *mesy, QObject *parent)

    constructor

    \param mesy Mesydaq2 object to control the hardware
    \param parent Qt parent object
*/
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
	, m_rateTimer(0)
	, m_onlineTimer(0)
	, m_packages(0)
	, m_triggers(0)
{
	connect(m_mesydaq, SIGNAL(analyzeDataBuffer(DATA_PACKET &)), this, SLOT(analyzeBuffer(DATA_PACKET &)));
	connect(this, SIGNAL(stopSignal()), m_mesydaq, SLOT(stop()));
	for (quint8 i = 0; i < 8; ++i)
	{
		if (i == TCT)
			m_counter[i] = new MesydaqTimer();
		else
			m_counter[i] = new MesydaqCounter();
		connect(m_counter[i], SIGNAL(stop()), this, SLOT(requestStop()));
	}
	m_ampHist = new Histogram(0, 960);
	m_posHist = new Histogram(0, 960);
	m_timeSpectrum = new Spectrum(960);

	connect(this, SIGNAL(stopSignal()), m_mesydaq, SLOT(stop()));
	connect(this, SIGNAL(acqListfile(bool)), m_mesydaq, SLOT(acqListfile(bool)));

	m_rateTimer = startTimer(8);	// every 8 ms calculate the rates
	m_onlineTimer = startTimer(60);	// every 60 ms check measurement
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
	delete m_ampHist;
	m_ampHist = NULL;
	delete m_posHist;
	m_posHist = NULL;
	delete m_timeSpectrum;
	m_timeSpectrum = NULL;
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

    \param msecs current time in ms
 */
void Measurement::setCurrentTime(quint64 msecs)
{
	if(m_running)
	{
    		m_meastime_msec = msecs - m_starttime_msec;
		for (quint8 i = 0; i < 8; ++i)
			if (i != TCT)	// this will be updated during the events 
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
	for (quint8 c = 0; c < 8; ++c)
		m_counter[c]->reset();	
	m_packages = 0;
	m_triggers = 0;
	m_mesydaq->start();
	m_starttime_msec = m_mesydaq->time();
	m_running = true;
	m_stopping = false;
	protocol(tr("event counter limit : %1").arg(m_counter[EVCT]->limit()), INFO);
	for (quint8 c = 0; c < 8; ++c)
	{
		m_counter[c]->start(m_starttime_msec);
		protocol(tr("counter %1 value : %2 limit : %3").arg(c).arg(m_counter[c]->value()).arg(m_counter[c]->limit()), INFO);
	}
}

/*!
    \fn Measurement::requestStop()

    callback for the request to stop the measurement
 */
void Measurement::requestStop()
{
	m_stopping = true;
	emit stopSignal(false);
	protocol(tr("Max %1 was at pos %2").arg(m_posHist->max(0)).arg(m_posHist->maxpos(0)), NOTICE);
}

/*!
    \fn Measurement::stop()
    
    really stop the measurement
 */
void Measurement::stop()
{
	m_mesydaq->stop(); 
	quint64 time = m_mesydaq->time();
	m_running = false;
	m_stopping = false;
	for (quint8 c = 0; c < 8; ++c)
		m_counter[c]->stop(time);

	protocol(tr("packages : %1 triggers : %2").arg(m_packages).arg(m_triggers));
#if 0
 	m_counterOffset[TCT] = m_counter[1][TCT];
#endif
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
	protocol(tr("Measurement::setCounter(cNum = %1, val = %2)").arg(cNum).arg(val));
	if(cNum < 8)
	{
		if (val == 0)
			m_counter[cNum]->reset();
		else
   			m_counter[cNum]->set(val);
// is counter master and is limit reached?
		if(m_counter[cNum]->isStopped() && !m_stopping)
		{
			protocol(tr("stop on counter %1, value: %2, preset: %3").arg(cNum).arg(m_counter[cNum]->value()).arg(m_counter[cNum]->limit()), NOTICE);
			m_stopping = true;
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
	for(quint8 c = 0; c < 8; c++)
		m_counter[c]->calcRate();
}

/*!
    \fn Measurement::calcMeanRates()

    calculates the mean rates for all counters
 */
void Measurement::calcMeanRates()
{
	for(quint8 c = 0; c < 8; c++)
		m_counter[c]->calcMeanRate();
}

/*!
    \fn Measurement::getCounter(quint8 cNum)

    get the current value of the counter

    \param cNum number of the counter
    \return counter value
 */
quint64 Measurement::getCounter(quint8 cNum)
{
	if(cNum < 8)
		return m_counter[cNum]->value();
	else
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
	if(cNum < 8)
		return m_counter[cNum]->rate();
	else
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

    sets the measurement online/offline

    \param truth online = true, offline = false
 */
void Measurement::setOnline(bool truth)
{
	m_online = truth;
	protocol(tr("MCPD %1").arg(m_online ? tr("online") : tr("offline")), NOTICE);	
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
	if(cNum < 8)
	{
		protocol(tr("setPreset counter: %1 to %2 %3").arg(cNum).arg(prval).arg(mast ? tr("master") : tr("slave")), NOTICE);	
		if (mast)
		{
    			// clear all other master flags
    			for (quint8 c = 0; c < 8;c++)
    				m_counter[cNum]->setMaster(false);
    			// set new master
    			m_counter[cNum]->setMaster(true);
    		}
    		else
    		// just clear master
    			m_counter[cNum]->setMaster(false);
    		m_counter[cNum]->setLimit(prval);
	}
}


/*!
    \fn Measurement::getPreset(quint8 cNum)

    get the preset value of the counter

    \param cNum number of the counter
    \return preset value
 */
ulong Measurement::getPreset(quint8 cNum)
{
	if(cNum < 8)
		return m_counter[cNum]->limit();
	else
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
	return m_counter[cNum]->isMaster();
}

/*!
    \fn Measurement::clearCounter(quint8 cNum)

    resets the counter, clear preset and set value to 0

    \param cNum number of the counter
 */
void Measurement::clearCounter(quint8 cNum)
{
	if(cNum > 7)
		return;
	m_counter[cNum]->reset();
}

/*!
    \fn Measurement::clearAllHist(void)

    clears all histograms
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
    \fn Measurement::clearChanHist(quint16 chan)

    clears the spectra of a tube

    \param chan tube number
 */
void Measurement::clearChanHist(quint16 chan)
{
	if (m_posHist)
		m_posHist->clear(chan);
	if (m_ampHist)
		m_ampHist->clear(chan);
}

/*!
    \fn Spectrum *Measurement::posData(quint16 line)

    gets a position spectrum of a tube

    \param line tube number
    \return spectrum if line exist otherwise NULL pointer
*/
Spectrum *Measurement::posData(quint16 line)
{
	if (m_posHist)
		return m_posHist->spectrum(line);
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
	if (m_posHist)
		return m_posHist->spectrum();
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
	if (m_ampHist)
		return m_ampHist->spectrum();
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
	if (m_ampHist)
		return m_ampHist->spectrum(line);
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
	return m_timeSpectrum;
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
		if (m_posHist)
    			m_posHist->writeHistogram(&f, "position data: 1 row title (8 x 8 detectors), position data in columns");
		if (m_ampHist)
			m_ampHist->writeHistogram(&f, "amplitude/energy data: 1 row title (8 x 8 detectors), amplitude data in columns");
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
	{    // file opened successfully
		QTextStream t( &f );        // use a text stream
		QString tmp = t.readLine();
		// Title
		QStringList list = tmp.split(QRegExp("\\s+"));
		if (list.size() >= 3 && list[0] == "mesydaq" && list[1] == "Histogram" && list[2] == "File")
		{
			m_posHist->clear();
			m_ampHist->clear();

			while(!t.atEnd())
			{
				tmp = t.readLine();
				list = tmp.split(QRegExp("\\s+"));
				if (list.size() >= 2 && list[1].startsWith("data"))
				{
					if (list[0] == "position")
						fillHistogram(t, m_posHist);
					else if (list[0] == "amplitude/energy")
						fillHistogram(t, m_ampHist);
				}
			}	
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

	while(!(tmp = t.readLine()).isEmpty())
	{
		list = tmp.split(QRegExp("\\s+"), QString::SkipEmptyParts);
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
    \fn Measurement::analyzeBuffer(DATA_PACKET &pd)

    analyze the data packet and put all events into the right counters and/or histogram

    \param pd data packet
 */
void Measurement::analyzeBuffer(DATA_PACKET &pd)
{
	quint16 time;
	ulong 	data;
	quint32 i, j;
	quint16 neutrons = 0;
	quint16 triggers = 0;
	quint64 tim;
	quint16 mod = pd.deviceId;	
	m_headertime = pd.time[0] + (quint64(pd.time[1]) << 16) + (quint64(pd.time[2]) << 32);
	setCurrentTime(m_headertime / 10000); // headertime is in 100ns steps

	m_packages++;
	if(pd.bufferType < 0x0002) 
	{
// extract parameter values:
		QChar c('0');
 		quint32 datalen = (pd.bufferLength - pd.headerLength) / 3;
		if (datalen == 0)
			m_counter[TCT]->setTime(m_headertime / 10000);
		quint16 counter = 0;
		for(i = 0; i < datalen && !m_stopping; ++i, counter += 3)
		{
			tim = pd.data[counter + 1] & 0x7;
			tim <<= 16;
			tim += pd.data[counter];
			tim += m_headertime;
// id stands for the trigId and modId depending on the package type
			quint8 id = (pd.data[counter + 2] >> 12) & 0x7;
// not neutron event (counter, chopper, ...)
			m_counter[TCT]->setTime(tim / 10000);
			if((pd.data[counter + 2] & 0x8000))
			{
				triggers++;
				quint8 dataId = (pd.data[counter + 2] >> 8) & 0x0F;
				data = ((pd.data[counter + 2] & 0xFF) << 13) + ((pd.data[counter + 1] >> 3) & 0x7FFF);
				time = (quint16)tim;
#warning TODO remove mysterious mapping
//! \todo remove mysterious mapping
				switch(dataId)
				{
					case MON1ID :
					case MON2ID :
					case MON3ID :
					case MON4ID :
						++(*m_counter[dataId]);
//						protocol(tr("counter %1 : (%3 - %4)%2").arg(dataId).arg(m_counter[dataId]->value()).arg(i).arg(triggers));
						break;
					default:
						protocol(tr("counter %1 : %2").arg(dataId).arg(i));
						break;
				}
			}
// neutron event:
			else
			{
				neutrons++;
				quint8 slotId = (pd.data[counter + 2] >> 7) & 0x1F;
				quint8 modChan = (id << 3) + slotId;
				quint8 chan = modChan + (mod << 6);
				quint16 amp = ((pd.data[counter+2] & 0x7F) << 3) + ((pd.data[counter+1] >> 13) & 0x7),
					pos = (pd.data[counter+1] >> 3) & 0x3FF;
// BUG in firmware, every first neutron event seems to be "buggy" or virtual
// Only on newer modules with a distinct CPLD firmware
// BUG is reported
				if (neutrons == 1 && modChan == 0 && pos == 0 && amp == 0)
				{
					protocol(tr("GHOST EVENT: SlotID %1 Mod %2 %3").arg(slotId).arg(id), WARNING);
					continue;
				}
				++(*m_counter[EVCT]);
				if (m_posHist)
					m_posHist->incVal(chan, pos);
				if (m_ampHist)
					m_ampHist->incVal(chan, amp);
			}
		}
		m_triggers += triggers;
		for(i = 0; i < 4; i++)
		{
			quint64 var = 0;
			for(j = 0; j < 3; j++)
			{
				var <<= 16;
				var |= pd.param[i][2 - j];
			}
			if (var && m_counter[i]->value() != var)
			{
				protocol(tr("counter %1 : %3 <-> %2").arg(i).arg(m_counter[i]->value()).arg(var));
				setCounter(i, var);
			}
		}		
	}
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
	protocol(tr("readListfile : %1").arg(ok), NOTICE);
	clearAllHist();
	QChar c('0');
	if (m_running)
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
			protocol(tr("EOF reached after %1 buffers").arg(blocks), NOTICE);
			break;
		}

//		memset(&dataBuf, 0, sizeof(dataBuf));
		dataBuf.bufferLength = sep1;
		dataBuf.bufferType = sep2;
		dataBuf.headerLength = sep3;
		dataBuf.bufferNumber = sep4;
		if(dataBuf.bufferLength > 729)
		{
			protocol(tr("erroneous length: %1 - aborting").arg(dataBuf.bufferLength), ERROR);
			datStream >> sep1 >> sep2 >> sep3 >> sep4;
			protocol(tr("Separator: %1 %2 %3 %4").arg(sep1, 2, 16, c).arg(sep2, 2, 16, c).arg(sep3, 2, 16, c).arg(sep4, 2, 16, c), ERROR);
			break;
		}
		quint16 *pD = (quint16 *)&dataBuf.bufferLength;
		for(int i = 4; i < dataBuf.bufferLength; i++)
			datStream >> pD[i];
		if (!blocks)
		{
			quint64 tmp = dataBuf.time[0] + (quint64(dataBuf.time[1]) << 16) + (quint64(dataBuf.time[2]) << 32);
			for (quint8 i = 0; i < 8; ++i)
				m_counter[i]->reset();
			m_counter[TCT]->start(tmp / 10000);
		}
// hand over data buffer for processing
		analyzeBuffer(dataBuf);
// increment local counters
		blocks++;
		bcount++;
// check for next separator:
//		qint64 p = textStream.device()->pos();
//		protocol(tr("at position : %1 (0x%2)").arg(p).arg(p, 8, 16, c), DEBUG);
		datStream >> sep1 >> sep2 >> sep3 >> sep4;
//		protocol(tr("Separator: %1 %2 %3 %4").arg(sep1, 2, 16, c).arg(sep2, 2, 16, c).arg(sep3, 2, 16, c).arg(sep4, 2, 16, c), DEBUG);
		ok = ((sep1 == sep0) && (sep2 == sepF) && (sep3 == sep5) && (sep4 == sepA));
		if (!ok)
		{
			protocol(tr("File structure error - read aborted after %1 buffers").arg(blocks), ERROR);
			qint64 p = textStream.device()->pos();
			protocol(tr("at position : %1 (0x%2)").arg(p).arg(p, 8, 16, c), ERROR);
			protocol(tr("Separator: %1 %2 %3 %4").arg(sep1, 2, 16, c).arg(sep2, 2, 16, c).arg(sep3, 2, 16, c).arg(sep4, 2, 16, c), ERROR);
		}
		if(!(bcount % 1000))
		{
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
	m_posHist->getMean(mean, sigma);
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
	m_posHist->getMean(chan, mean, sigma);
}

/*!
    \fn Measurement::getAmpMean(float &mean, float &sigma)

    gives the mean value and the standard deviation of the last amplitude events
  
    \param mean mean value
    \param sigma standard deviation
 */
void Measurement::getAmpMean(float &mean, float &sigma)
{
	m_ampHist->getMean(mean, sigma);
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
	m_ampHist->getMean(chan, mean, sigma);
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
	mean = m_timeSpectrum->mean(sigma);
}

