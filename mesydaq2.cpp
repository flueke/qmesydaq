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
#include <QSettings>
#include <QTextStream>

#include "mdefines.h"
#include "mesydaq2.h"
#include "mcpd8.h"

Mesydaq2::Mesydaq2(QObject *parent)
	: MesydaqObject(parent) 
	, m_dataRxd(0)
	, m_cmdRxd(0)
	, m_cmdTxd(0)
	, m_daq(IDLE)
	, m_acquireListfile(false)
	, m_listfilename("")
	, m_histfilename("")
	, m_listPath("/home")
	, m_histPath("/home")
	, m_configPath("/home")
	, m_timingwidth(1)
{
	initDevices();
	initTimers();
	initHardware();
	protocol(tr("running on Qt %1").arg(qVersion()));
}

Mesydaq2::~Mesydaq2()
{
	m_mcpd.clear();
}

/*!
    \fn Mesydaq2::writeRegister(unsigned int id, unsigned int addr, unsigned int val)
 */
void Mesydaq2::writeRegister(quint16 id, quint16 addr, quint16 reg, quint16 val)
{
	m_mcpd[id]->writeRegister(addr, reg, val);
}

/*!
    \fn Mesydaq2::acqListfile(bool yesno)
 */
void Mesydaq2::acqListfile(bool yesno)
{
	m_acquireListfile = yesno;
    	protocol(tr("Listfile recording %1").arg((yesno ? "on" : "off")), 1);
}

/*!
    \fn Mesydaq2::startedDaq(void)
 */
void Mesydaq2::startedDaq(void)
{
	if(m_acquireListfile)
	{
		m_datfile.setName(m_listfilename);
		m_datfile.open(IO_WriteOnly);
//		m_textStream.setDevice(&m_datfile);
		m_datStream.setDevice(&m_datfile);
		writeListfileHeader();
	}
	m_daq = RUNNING;
	emit statusChanged("RUNNING");
	protocol("daq started", 1);
}

/*!
    \fn Mesydaq2::stoppedDaq(void)
 */
void Mesydaq2::stoppedDaq(void)
{
	if(m_acquireListfile)
	{
		writeClosingSignature();
		m_datfile.close();
	}
	m_daq = IDLE;
	emit statusChanged("IDLE");
	protocol("daq stopped", 1);
}

/*!
    \fn Mesydaq2::initDevices(void)
 */
void Mesydaq2::initDevices(void)
{
	for(quint8 i = 0; i < MCPDS; i++)
	{
		m_mcpd[i] = new MCPD8(i, this);
		connect(m_mcpd[i], SIGNAL(analyzeDataBuffer(DATA_PACKET &)), this, SLOT(analyzeBuffer(DATA_PACKET &)));
		connect(m_mcpd[i], SIGNAL(startedDaq()), this, SLOT(startedDaq()));
		connect(m_mcpd[i], SIGNAL(stoppedDaq()), this, SLOT(stoppedDaq()));
	}
}


/*!
    \fn Mesydaq2::initTimers(void)
 */
void Mesydaq2::initTimers(void)
{
// central dispatch timer
	theTimer = new QTimer(this);
	connect(theTimer, SIGNAL(timeout()), this, SLOT(centralDispatch()));
	for(quint8 c = 0; c < 10; c++)
		m_dispatch[c] = 0;
	theTimer->start(1);
}

/*!
    \fn Mesydaq2::writeListfileHeader(void)
 */
void Mesydaq2::writeListfileHeader(void)
{
	QTextStream txtStr; 
	txtStr << "mesytec psd listmode data\n";
	txtStr << QString("header length: %1 lines \n").arg(2);
	m_datStream << txtStr.string();
}


/*!
    \fn Mesydaq2::writeHeaderSeparator(void)
 */
void Mesydaq2::writeHeaderSeparator(void)
{
	m_datStream << sep0 << sep5 << sepA << sepF;
}


/*!
    \fn Mesydaq2::writeBlockSeparator(void)
 */
void Mesydaq2::writeBlockSeparator(void)
{
	m_datStream << sep0 << sepF << sep5 << sepA;
}


/*!
    \fn Mesydaq2::writeClosingSignature(void)
 */
void Mesydaq2::writeClosingSignature(void)
{
	m_datStream << sepF << sepA << sep5 << sep0;
}


/*!
    \fn Mesydaq2::readListfile(QString readfilename)
 */

bool Mesydaq2::isPulserOn()
{
	for (quint8 i = 0; i < MCPDS; ++i)
		if (m_mcpd[i]->isPulserOn())
			return true;
	return false;
}

bool Mesydaq2::isPulserOn(quint16 id)
{
	return m_mcpd[id]->isPulserOn();
}

bool Mesydaq2::isPulserOn(quint16 id, quint8 addr)
{
	return m_mcpd[id]->isPulserOn(addr);
}

/*!
    \fn Mesydaq2::scanPeriph(unsigned short id)
 */
void Mesydaq2::scanPeriph(quint16 id)
{
	m_mcpd[id]->readId();
}

/*!
    \fn Mesydaq2::initHardware(void)
 */
void Mesydaq2::initHardware(void)
{
// starts the init procedure:
// 1) check all MCPD
// 2) check all MPSD: read ID, read Registers
// setup MCPDs
// setup MPSDs
	
// set init values:
	protocol(tr("initializing hardware"), 1);
	 
// scan connected MCPDs
	quint8 p = 0;
	for(quint8 c = 0; c < MCPDS; c++)
	{
		m_mcpd[c]->readId();
		for(quint8 i = 0; i < 8; ++i)
			if(m_mcpd[c]->getMpsdId(i))
				p++;
	}
	
	protocol(tr("%1 mpsd-8 found").arg(p), 1);
	
// initialize all connected hardware modules
// try to read standard config file
	protocol(tr("load setup ?"));
	if(!loadSetup("mesycfg.mcfg"))
	{
		// initialize MPSD-8 by default values.
		for(quint8 c = 0; c < MCPDS; c++)
			for (int i = 0; i < 8; ++i)
				if(m_mcpd[c]->getMpsdId(i))
					initMpsd(c * 8 + i);
	}
	initMcpd(0);
}


/*!
    \fn Mesydaq2::saveSetup(void)
 */
bool Mesydaq2::saveSetup(const QString &name)
{
	m_configfilename = name;
	if(m_configfilename.isEmpty())
		m_configfilename = "mesycfg.mcfg";
  
	int i = m_configfilename.indexOf(".mcfg");
	if(i == -1)
		m_configfilename.append(".mcfg");

	QSettings settings(m_configfilename, QSettings::IniFormat);
	settings.setValue("general/date", QDateTime::currentDateTime());
	settings.setValue("general/Config Path", m_configPath);
	settings.setValue("general/Histogram Path", m_histPath);
	settings.setValue("general/Listfile Path", m_listPath);
	for (int i = 0; i < MCPDS; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			if (m_mcpd[i]->getMpsdId(j))
			{
				QString str;
				str.sprintf("MPSD-8 #%d/", i);
				settings.beginWriteArray(str + "gains");
				for(int k = 0; k < 8; k++)
				{
					settings.setArrayIndex(k);
					settings.setValue("gain", m_mcpd[i]->getGain(j, k));
				}
				settings.endArray();
				settings.setValue(str + "threshold", m_mcpd[i]->getThreshold(j));
			}
		}
	}

#if 0
	QFile f(configfilename);
	if ( f.open(QIODevice::WriteOnly) ) {    // file opened successfully
		QTextStream t( &f );        // use a text stream
		QString s;
		// Title
		t << "mesydaq configuration file   ";
		t << dateTime.toString("dd.MM.yy") << " " << dateTime.toString("hh.mm.ss");
		t << '\r' << '\n';
		t << '\r' << '\n';
		// default paths
		t << "Config Path";
		t << '\r' << '\n';
		t << configPath;
		t << '\r' << '\n';
		t << "Histogram Path";
		t << '\r' << '\n';
		t << histPath;
		t << '\r' << '\n';
		t << "Listfile Path";
		t << '\r' << '\n';
		t << listPath;
		t << '\r' << '\n';
		t << "Setup:";
		t << '\r' << '\n';
/*	    t << "MCPD-8 settings";
    	t << '\r' << '\n';
	    t << '\r' << '\n';
		t << "MCPD-8 #" << 0 << ":";
		t << '\r' << '\n';
*/    	
    	t << "MPSD-8 settings";
	    t << '\r' << '\n';
    	t << '\r' << '\n';
		for(int i=0; i<8*MCPDS; i++){
		if(myMpsd[i]->getMpsdId()){
			t << "MPSD-8 #" << i << ":";
			t << '\r' << '\n';
			t << "gains:";
			t << '\r' << '\n';
			for(int j=0; j<8; j++){
			t << myMpsd[i]->getGainpoti(j,0);
			t << '\r' << '\n';
			}
			t << "threshold:";
			t << '\r' << '\n';
			t << myMpsd[i]->getThreshpoti(0);
			t << '\r' << '\n';
		}
    }
  }
  f.close();
#endif
	return true;
}

/*!
    \fn Mesydaq2::loadSetup(void)
 */
bool Mesydaq2::loadSetup(const QString &name)
{
	m_configfilename = name;
	if(name.isEmpty())
		m_configfilename = "mesycfg.mcfg";
  
	protocol(tr("Reading configfile %1").arg(m_configfilename), 1);

	QSettings settings(m_configfilename, QSettings::IniFormat);

	m_configPath = settings.value("general/Config Path", "/home").toString();
	m_histPath = settings.value("general/Histogram Path", "/home").toString();
	m_listPath = settings.value("general/Listfile Path", "/home").toString();
	for (int i = 0; i < MCPDS; ++i)
	{
		protocol(tr("mcpd #%1").arg(i));
		for (int j = 0; j < 8; ++j)
		{
			QString str;
			str.sprintf("MPSD-8 #%d", j);
			qDebug("number of groups %d", settings.childGroups().size());
			if (settings.childGroups().contains(str))
			{
				settings.beginGroup(str);
				protocol("init " + str);
				int size = settings.beginReadArray(str + "gains");
				for (int l = 0; l < size; ++l)
				{
					settings.setArrayIndex(l);
					quint8 gain = settings.value("gain", 128).toUInt() & 0xff;
					m_mcpd[i]->setGain(j, l, gain);
				}
				settings.endArray();
				quint8 thresh = settings.value(str + "threshold", 10).toUInt() & 0xff;
				m_mcpd[i]->setThreshold(j, thresh);
				settings.endGroup();
			}
		}
	}
#if 0
	QFile f(configfilename);

	QString s, str;


	if (f.open(QIODevice::ReadOnly)) 
	{    // file opened successfully
		QTextStream t( &f );        // use a text stream

// now read from stream
    
// Headline + empty line
		s = t.readLine();
		s = t.readLine();

//  Headline + configPath
		s = t.readLine();
		configPath = t.readLine();

// Headline + histPath
		s = t.readLine();
		histPath = t.readLine();

// Headline + listPath
		s = t.readLine();
		listPath = t.readLine();
	
// three headlines
		s = t.readLine();
		s = t.readLine();
		s = t.readLine();

// now the MPSD-8, if any...
		s = t.readLine();
		char charpos = s.indexOf('#', 0);

// loop through existing entries
		while(charpos > 6)
		{
			// module address: next to # sign
			QString as = s.right(s.length()-charpos-1);
			as = as.left(1);
			unsigned char modaddr = as.toUShort();
			// one title row
			s = t.readLine();
			// 9 gain poti values
			if(myMpsd[modaddr]->getMpsdId() > 0)
			{
				for(unsigned char i = 0; i < 8; i++)
				{
					s = t.readLine();
					myMpsd[modaddr]->setGain(i, (unsigned char)s.toUShort(), 1);
				}
			}
			// one title row
			s = t.readLine();
			// threshold poti
			s = t.readLine();
			if(myMpsd[modaddr]->getMpsdId() > 0)
			{
				myMpsd[modaddr]->setThreshpoti((unsigned char)s.toUShort(), 1);
			}
			if(myMpsd[modaddr]->getMpsdId() > 0)
			{
				initMpsd(modaddr);
			}	
			// further entries?
			s = t.readLine();
			qDebug(s.toStdString().c_str());
			charpos = s.indexOf('#', 0);
		}
	
		f.close();
		mainWin->dispFiledata();
		return true;
	}
  	else
	{
		pstring.sprintf("ERROR: opening configfile '");
		pstring.append(name);
		pstring.append("' failed");
		protocol(pstring, 1);
		return false;
	}
#endif
	return true;
}

/*!
    \fn Mesydaq2::centralDispatch()
 */
void Mesydaq2::centralDispatch()
{
#warning TODO if(cInt->caressTaskPending() && (!cInt->asyncTaskPending()))
#warning TODO 		cInt->caressTask();
    	
#warning TODO
#if 0
	if(++m_dispatch[0] == 8)		// every 8 ms calculate the rates
	{
		m_dispatch[0] = 0;
		meas->calcRates();
	}
#endif
	
	if(++m_dispatch[1] == 50)		// every 50 ms check the MCPD
	{
//		checkMcpd(0);
	}
	else if(m_dispatch[1] == 60)		// every 60 ms check measurement
	{
		//if(meas->isOk() == 0)
			//meas->setOnline(false);
		m_dispatch[1] = 0;
	}
}


/*!
    \fn Mesydaq2::initMpsd(unsigned char id)
 */
void Mesydaq2::initMpsd(unsigned char id)
{
	quint8 	start = 8,
		stop = 9;
	
#warning TODO
#if 0
	// gains:
	if(!myMpsd[id]->comGain())
	{
		// iterate through all channels
		start = 0;
		stop = 8;
	}
	for (quint8 c = start; c < stop; ++c)
		m_mcpd[id]->setGain(c, myMpsd[id]->getGainpoti(c, 1));
#endif
	
// threshold:
	m_mcpd[0]->setThreshold(id, m_mcpd[0]->getThreshold(id));

// pulser
	m_mcpd[0]->setPulser(id, 0, 2, 50, false);

// mode
	m_mcpd[0]->setMode(id, false);
	
// now set tx capabilities, if id == 105
	if(m_mcpd[0]->getMpsdId(id) == 105)
	{
		// write register 1
		m_mcpd[0]->writePeriReg(id, 1, 4);
	}
}


/*!
    \fn Mesydaq2::initMcpd(unsigned char id)
 */
void Mesydaq2::initMcpd(quint8 id)
{
	m_mcpd[id]->init();
}


/*!
    \fn Mesydaq2::allPulserOff()
 */
void Mesydaq2::allPulserOff()
{
// send pulser off to all connected MPSD
	for(quint32 i = 0; i < MCPDS; i++)
		for(quint32 j = 0; j < 8; j++)
			if(m_mcpd[i]->getMpsdId(j))
				m_mcpd[i]->setPulser(j, 0, 2, 40, 0);
}

/*!
    \fn Mesydaq2::setTimingwidth(unsigned char width)
 */
void Mesydaq2::setTimingwidth(quint8 width)
{
	m_timingwidth = width;
	if(width > 48)
    		m_timingwidth = 48;
#warning TODO
#if 0
	if (m_hist)
   		m_hist->setWidth(m_timingwidth); 
#endif
}


/*!
    \fn Mesydaq2::readPeriReg(unsigned short id, unsigned short mod, unsigned short reg)
 */
quint16 Mesydaq2::readPeriReg(quint16 id, quint16 mod, quint16 reg)
{
	return m_mcpd[id]->readPeriReg(mod, reg);
}


/*!
    \fn Mesydaq2::writePeriReg(unsigned short id, unsigned short mod, unsigned short reg, unsigned short val)
 */
void Mesydaq2::writePeriReg(quint16 id, quint16 mod, quint16 reg, quint16 val)
{
	m_mcpd[id]->writePeriReg(mod, reg, val);
}

// command shortcuts for simple operations:
/*!
    \fn Mesydaq2::start(void)
 */
void Mesydaq2::start(void)
{
   	protocol("remote start", 1);
	m_mcpd[0]->start();
	emit statusChanged("STARTED");
}

/*!
    \fn Mesydaq2::stop(void)
 */
void Mesydaq2::stop(void)
{
	m_mcpd[0]->stop();
	emit statusChanged("STOPPED");
   	protocol("remote stop", 1);
}

/*!
    \fn Mesydaq2::continue(void)
 */
void Mesydaq2::cont(void)
{
	protocol("remote cont", 1);
	m_mcpd[0]->cont();
	emit statusChanged("STARTED");
}

/*!
    \fn Mesydaq2::reset(void)
 */
void Mesydaq2::reset(void)
{
    /// @todo implement me
}


/*!
    \fn Mesydaq2::checkMcpd(unsigned char device)
 */
bool Mesydaq2::checkMcpd(quint8 /* device */)
{
	for(quint8 c = 0; c < MCPDS; c++)
		scanPeriph(c);    	
	return true;
}

void Mesydaq2::setProtocol(const quint16 id, const QString &mcpdIP, const QString dataIP, const qint16 dataPort, const QString cmdIP, const qint16 cmdPort)
{
	m_mcpd[id]->setProtocol(mcpdIP, dataIP, dataPort, cmdIP, cmdPort);
}	

bool Mesydaq2::getMode(const quint16 id, quint16 addr)
{
	return m_mcpd[id]->getMode(addr);
}

void Mesydaq2::setMode(const quint16 id, quint16 addr, bool mode)
{
	m_mcpd[id]->setMode(addr, mode);
}

void Mesydaq2::setPulser(const quint16 id, quint16 addr, quint8 channel, quint8 position, quint8 amp, bool onoff)
{
	m_mcpd[id]->setPulser(addr, channel, position, amp, onoff);
}

void Mesydaq2::setCounterCell(quint16 id, quint16 source, quint16 trigger, quint16 compare)
{
	m_mcpd[id]->setCounterCell(source, trigger, compare);
}

void Mesydaq2::setParamSource(quint16 id, quint16 param, quint16 source)
{
	m_mcpd[id]->setParamSource(param, source);
}

void Mesydaq2::setAuxTimer(quint16 id, quint16 tim, quint16 val)
{
	m_mcpd[id]->setAuxTimer(tim, val);
}

void Mesydaq2::setMasterClock(quint16 id, quint64 val)
{
	m_mcpd[id]->setMasterClock(val);
}

void Mesydaq2::setTimingSetup(quint16 id, bool master, bool sync)
{
	m_mcpd[id]->setTimingSetup(master, sync);
}

void Mesydaq2::setId(quint16 id, quint8 mcpdid)
{
	m_mcpd[id]->setId(mcpdid);
}

void Mesydaq2::setGain(quint16 id, quint16 addr, quint8 channel, quint8 gain)
{
	m_mcpd[id]->setGain(addr, channel, gain);
}

void Mesydaq2::setGain(quint16 id, quint16 addr, quint8 channel, float gain)
{
	m_mcpd[id]->setGain(addr, channel, gain);
}

void Mesydaq2::setThreshold(quint16 id, quint16 addr, quint8 thresh)
{
	m_mcpd[id]->setThreshold(addr, thresh);
}

quint8 Mesydaq2::getMpsdId(quint16 id, quint8 addr)
{
	return m_mcpd[id]->getMpsdId(addr);
}

quint8 Mesydaq2::getGain(quint16 id, quint16 addr, quint8 chan)
{
	return m_mcpd[id]->getGain(addr, chan);
}

quint8 Mesydaq2::getThreshold(quint16 id, quint16 addr)
{
	return m_mcpd[id]->getThreshold(addr);
}

void Mesydaq2::setHistfilename(QString name) 
{
	m_histfilename = name;
	if(m_histfilename.indexOf(".mtxt") == -1)
		m_histfilename.append(".mtxt");
}
     
/*!
    \fn Mesydaq2::analyzeBuffer(DATA_PACKET &pd)
 */
void Mesydaq2::analyzeBuffer(DATA_PACKET &pd)
{
	m_dispatch[1] = 0;
	protocol(tr("Mesydaq2::analyzeBuffer(): %1").arg(m_daq), 3);
	if(m_daq == RUNNING)
	{
		quint32 i, j;
		quint16 mod = pd.deviceId;	
		quint16 diff = pd.bufferNumber - m_lastBufnum;
		if(diff > 1)
			protocol(tr("Lost %1 Buffers: current: %2, last: %3").arg(diff).arg(pd.bufferNumber).arg(m_lastBufnum));
		m_lastBufnum = pd.bufferNumber;
		if(m_acquireListfile)
		{
			quint16 *pD = (quint16 *) &pd.bufferLength;
			for(quint16 i = 0; i < pd.bufferLength; i++)
				m_datStream << pD[i];
			writeBlockSeparator();
//			qDebug("------------------");
		}
		protocol(tr("dataRxd : %1").arg(++m_dataRxd), 3);
		protocol(tr("buffer : length : %1").arg(pd.bufferLength), 3);
		if(pd.bufferType < 0x0002) 
		{
// extract parameter values:
			for(i = 0; i < 4; i++)
			{
				quint64 var = 0;
				for(j = 0; j < 3; j++)
				{
					var <<= 16;
					var |= pd.param[i][2-j];
				}
				m_mcpd[mod]->setParameter((unsigned char)i, var);
			}
			emit analyzeDataBuffer(pd);
		}
	}
}
