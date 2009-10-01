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
#include <QTimerEvent>
#include <QStringList>

#include "mdefines.h"
#include "mesydaq2.h"
#include "mcpd8.h"

/*!
    \fn Mesydaq2::Mesydaq2(QObject *parent)

    constructor

    \param parent Qt parent object
*/
Mesydaq2::Mesydaq2(QObject *parent)
	: MesydaqObject(parent) 
	, m_daq(IDLE)
	, m_acquireListfile(false)
	, m_listfilename("")
	, m_histfilename("")
	, m_listPath("/home")
	, m_histPath("/home")
	, m_configPath("/home")
	, m_timingwidth(1)
	, m_checkTimer(0)
{
	initTimers();
	initDevices();
	initHardware();
	protocol(tr("running on Qt %1").arg(qVersion()), NOTICE);
}

//! destructor
Mesydaq2::~Mesydaq2()
{
	m_mcpd.clear();
	if (m_checkTimer)
		killTimer(m_checkTimer);
	m_checkTimer = 0;
}

//! \return number of received data packages for the whole setup
quint64 Mesydaq2::receivedData(void) 
{
	quint64 dataRxd = 0;
	foreach (MCPD8	*value, m_mcpd)
		dataRxd += value->receivedData();	
	return dataRxd;
}

//! \return number of received cmd answer packages for the whole setup
quint64 Mesydaq2::receivedCmds(void)
{
	quint64 cmdRxd = 0;
	foreach (MCPD8	*value, m_mcpd)
		cmdRxd += value->receivedCmds();	
	return cmdRxd;
}

//! \return number of sent cmd packages for the whole setup
quint64 Mesydaq2::sentCmds(void) 
{
	quint64 cmdTxd = 0;
	foreach (MCPD8	*value, m_mcpd)
		cmdTxd += value->sentCmds();	
	return cmdTxd;
}

/*!
    \fn Mesydaq2::time(void)

    time of the first master MCPD
    \todo at the moment not implemented, use instead the first MCPD

    \return the time of the MCPD
 */
quint64 Mesydaq2::time(void)
{
	if (m_mcpd.empty())
		return 0;
#warning TODO what if the number of MCPD is > 1
//! \todo what if the number of MCPD is > 1
	return (*m_mcpd.begin())->time();
}

/*!
    \fn Mesydaq2::writeRegister(quint16 id, quint16 reg, quint16 val)
    
    writes a value to a register in a selected MCPD, if the MPCD with
    the id does not exist, the command will be ignored

    \param id number of the MCPD
    \param reg number of the register
    \param val new value
    \see readRegister
 */
void Mesydaq2::writeRegister(quint16 id, quint16 reg, quint16 val)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->writeRegister(reg, val);
}

/*!
    \fn float Mesydaq2::getFirmware(quint16 id)
    
    gets the firmware version of a MCPD

    \param id number of the MCPD
    \return the firmware version as float value, if MCPD does not exist 0
*/
float Mesydaq2::getFirmware(quint16 id)
{
	if (m_mcpd.empty())
		return 0;
	return m_mcpd[id]->version();
}

/*!
    \fn Mesydaq2::acqListfile(bool yesno)

    enables the writing of list mode file

    \param yesno enable/disable if true/false
 */
void Mesydaq2::acqListfile(bool yesno)
{
	m_acquireListfile = yesno;
    	protocol(tr("Listfile recording %1").arg((yesno ? "on" : "off")), NOTICE);
}

/*!
    \fn Mesydaq2::startedDaq(void)

    callback after the start was succeeded

    \see start
    \see stop
    \see stoppedDaq
 */
void Mesydaq2::startedDaq(void)
{
	if(m_acquireListfile)
	{
		m_datfile.setFileName(m_listfilename);
		m_datfile.open(QIODevice::WriteOnly);
		m_datStream.setDevice(&m_datfile);
		writeListfileHeader();
	}
	m_daq = RUNNING;
	emit statusChanged("RUNNING");
	protocol("daq started", DEBUG);
}

/*!
    \fn Mesydaq2::stoppedDaq(void)

    callback after the aquisition was stopped

    \see start
    \see stop
    \see startedDaq
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
	protocol("daq stopped", DEBUG);
}

/*!
    \fn Mesydaq2::initDevices(void)
 */
void Mesydaq2::initDevices(void)
{
	QString ip[] = {"192.168.168.121", "192.168.169.121", };	
	quint16 port[] = {54321, 54321, };
	QString sourceIP[] = {"192.168.168.1", "192.168.169.1", };
}

/*!
    \fn Mesydaq2::addMCPD(quint16 id, QString ip, quint16 port, QString sourceIP)

    'adds' another MCPD to this class

    \param id the ID of the MCPD
    \param ip the IP address of the MCPD
    \param port the port number to send data and cmds
    \param sourceIP IP address to get data and cmd answers back
*/
void Mesydaq2::addMCPD(quint16 id, QString ip, quint16 port, QString sourceIP)
{
	m_mcpd[id] = new MCPD8(id, this, ip, port, sourceIP);
	connect(m_mcpd[id], SIGNAL(analyzeDataBuffer(DATA_PACKET &)), this, SLOT(analyzeBuffer(DATA_PACKET &)));
	connect(m_mcpd[id], SIGNAL(startedDaq()), this, SLOT(startedDaq()));
	connect(m_mcpd[id], SIGNAL(stoppedDaq()), this, SLOT(stoppedDaq()));
}


/*!
    \fn Mesydaq2::initTimers(void)

    initialize the check timer 
 */
void Mesydaq2::initTimers(void)
{
	m_checkTimer = startTimer(50);	// every 50 ms check the MCPD
}

/*!
    \fn Mesydaq2::writeListfileHeader(void)

    write the header of a listfile

    \see writeHeaderSeparator
    \see writeBlockSeparator
    \see writeClosingSignature
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

    write a header separator into a list mode file

    \see writeListfileHeader
    \see writeBlockSeparator
    \see writeClosingSignature
 */
void Mesydaq2::writeHeaderSeparator(void)
{
	m_datStream << sep0 << sep5 << sepA << sepF;
}


/*!
    \fn Mesydaq2::writeBlockSeparator(void)

    write a block separator into a list mode file
    
    \see writeListfileHeader
    \see writeHeaderSeparator
    \see writeClosingSignature
 */
void Mesydaq2::writeBlockSeparator(void)
{
	m_datStream << sep0 << sepF << sep5 << sepA;
}


/*!
    \fn Mesydaq2::writeClosingSignature(void)

    \see writeListfileHeader
    \see writeHeaderSeparator
    \see writeBlockSeparator
 */
void Mesydaq2::writeClosingSignature(void)
{
	m_datStream << sepF << sepA << sep5 << sep0;
}


/*!
    \fn Mesydaq2::isPulserOn()

    checks all MCPD's for a running pulser

    \return true if a running pulser found false otherwise
 */
bool Mesydaq2::isPulserOn()
{
	foreach(MCPD8 *value, m_mcpd) 
		if (value->isPulserOn())
			return true;
	return false;
}

/*!
    \fn Mesydaq2::isPulserOn(quint16 id)

    checks a MCPD with number id for a running pulser

    \param id number of the MCPD
    \return true if a running pulser found false otherwise
 */
bool Mesydaq2::isPulserOn(quint16 id)
{
	if (m_mcpd.empty())
		return false;
	return m_mcpd[id]->isPulserOn();
}

/*!
    \fn Mesydaq2::isPulserOn(quint16 id, quint8 addr)

    checks a MPSD with number addr behind a MCPD with number id for a running pulser

    \param id number of the MCPD
    \param addr number of the MPSD behind the MCPD
    \return true if a running pulser found false otherwise
 */
bool Mesydaq2::isPulserOn(quint16 id, quint8 addr)
{
	if (m_mcpd.empty())
		return false;
	return m_mcpd[id]->isPulserOn(addr);
}

/*!
    \fn Mesydaq2::scanPeriph(quint16 id)

    checks which MPSD's at the moment are connected to the MCPD

    \param id number of the MCPD
 */
void Mesydaq2::scanPeriph(quint16 id)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->readId();
}

/*!
    \fn Mesydaq2::initHardware(void)

    initializes the hardware, counts the number of MPSD's and tries
    to initialise them 

    \todo it seems that this method is not necessary anymore
 */
void Mesydaq2::initHardware(void)
{
// starts the init procedure:
// 1) check all MCPD
// 2) check all MPSD: read ID, read Registers
// setup MCPDs
// setup MPSDs
	
// set init values:
	protocol(tr("initializing hardware"), NOTICE);
	 
// scan connected MCPDs
	quint16 p = 0;
	foreach(MCPD8 *value, m_mcpd)
		p += value->numModules();
	
	protocol(tr("%1 mpsd-8 found").arg(p), NOTICE);
	
// initialize all connected hardware modules
// try to read standard config file
	protocol(tr("load setup ?"), DEBUG);
	if(!loadSetup("mesycfg.mcfg"))
	{
		// initialize MPSD-8 by default values.
		foreach(MCPD8 *value, m_mcpd)
		{
			for (int i = 0; i < 8; ++i)
				if(value->getMpsdId(i))
					value->initMpsd(i);
		}
	}
}


/*!
    \fn Mesydaq2::saveSetup(const QString &name)

    stores the setup in a file
 
    \param name file name
    \return true if successfully saved otherwise false
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
	i = 0;
	foreach(MCPD8 *value, m_mcpd) 
	{
		for (int j = 0; j < 8; ++j)
		{
			if (value->getMpsdId(j))
			{
				QString str;
				str.sprintf("MPSD-8 #%d/", i);
				settings.beginWriteArray(str + "gains");
				for(int k = 0; k < 8; k++)
				{
					settings.setArrayIndex(k);
					settings.setValue("gain", value->getGain(j, k));
				}
				settings.endArray();
				settings.setValue(str + "threshold", value->getThreshold(j));
			}
		}
		++i;
	}

#if 0
	QFile f(configfilename);
	if ( f.open(QIODevice::WriteOnly) ) 
	{    // file opened successfully
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
/*		t << "MCPD-8 settings";
		t << '\r' << '\n';
		t << '\r' << '\n';
		t << "MCPD-8 #" << 0 << ":";
		t << '\r' << '\n';
*/    	
		t << "MPSD-8 settings";
		t << '\r' << '\n';
		t << '\r' << '\n';
		for(int i=0; i<8*MCPDS; i++)
		{
			if(myMpsd[i]->getMpsdId())
			{
				t << "MPSD-8 #" << i << ":";
				t << '\r' << '\n';
				t << "gains:";
				t << '\r' << '\n';
				for(int j=0; j<8; j++)
				{
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
    \fn Mesydaq2::loadSetup(const QString &name)

    loads the setup from a file
 
    \param name file name
    \return true if successfully loaded otherwise false
 */
bool Mesydaq2::loadSetup(const QString &name)
{
	m_configfilename = name;
	if(name.isEmpty())
		m_configfilename = "mesycfg.mcfg";
  
	protocol(tr("Reading configfile %1").arg(m_configfilename), DEBUG);

	QSettings settings(m_configfilename, QSettings::IniFormat);

	m_configPath = settings.value("general/Config Path", "/home").toString();
	m_histPath = settings.value("general/Histogram Path", "/home").toString();
	m_listPath = settings.value("general/Listfile Path", "/home").toString();
	int i(0);
	foreach(MCPD8 *value, m_mcpd)
	{
		protocol(tr("mcpd #%1 : number of groups %2").arg(i).arg(settings.childGroups().size()), NOTICE);
		for (int j = 0; j < 8; ++j)
		{
			QString str;
			str.sprintf("MPSD-8 #%d", j);
			if (settings.childGroups().contains(str))
			{
				settings.beginGroup(str);
				protocol("init " + str, NOTICE);
				int size = settings.beginReadArray("gains");
				protocol(tr("init size = %1").arg(size), DEBUG);
				for (int l = 0; l < size; ++l)
				{
					settings.setArrayIndex(l);
					quint8 gain = settings.value("gain", 128).toUInt() & 0xff;
					value->setGain(j, l, gain);
				}
				settings.endArray();
				quint8 thresh = settings.value(str + "threshold", 10).toUInt() & 0xff;
				value->setThreshold(j, thresh);
				settings.endGroup();
			}
		}
		++i;
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
		protocol(pstring, ERROR);
		return false;
	}
#endif
	return true;
}

/*!
    \fn Mesydaq2::timerEvent(QTimerEvent *event)

    callback for the timer
 */
void Mesydaq2::timerEvent(QTimerEvent * /* event */)
{
#warning TODO if(cInt->caressTaskPending() && (!cInt->asyncTaskPending()))
#warning TODO 		cInt->caressTask();
#warning TODO
//! \todo CARESS binding and checks for the hardware
#if 0
	if (event->timerId() == m_checkTimer)	
		checkMcpd(0);
#endif
}



/*!
    \fn Mesydaq2::initMcpd(quint8 id)

    initializes a MCPD

    \todo is this function really needed

    \param id number of the MCPD
 */
void Mesydaq2::initMcpd(quint8 id)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->init();
}


/*!
    \fn Mesydaq2::allPulserOff()

    switches all pulsers off
 */
void Mesydaq2::allPulserOff()
{
// send pulser off to all connected MPSD
	foreach(MCPD8 *value, m_mcpd)
		for(quint8 j = 0; j < 8; j++)
			if(value->getMpsdId(j))
				value->setPulser(j, 0, 2, 40, 0);
}

/*!
    \fn Mesydaq2::setTimingwidth(quint8 width)

    defines a timing width ????
 
    \todo not really implemented

    \param width ????
 */
void Mesydaq2::setTimingwidth(quint8 width)
{
	m_timingwidth = width;
	if(width > 48)
    		m_timingwidth = 48;
#warning TODO
//! \todo set the timing width of the histogram
#if 0
	if (m_hist)
   		m_hist->setWidth(m_timingwidth); 
#endif
}


/*!
    \fn Mesydaq2::readPeriReg(quint16 id, quint16 mod, quint16 reg)

    reads the content of a register in a module

    \param id number of the MCPD
    \param mod number of the module
    \param reg number of the register
    \return content of the register
    \see writePeriReg
 
 */
quint16 Mesydaq2::readPeriReg(quint16 id, quint16 mod, quint16 reg)
{
	if (m_mcpd.empty())
		return 0;
	return m_mcpd[id]->readPeriReg(mod, reg);
}

/*!
    \fn Mesydaq2::writePeriReg(quint16 id, quint16 mod, quint16 reg, quint16 val)

    writes a value into a module register

    \param id number of the MCPD
    \param mod number of the module
    \param reg number of the register
    \param val new value
    \see readPeriReg
 */
void Mesydaq2::writePeriReg(quint16 id, quint16 mod, quint16 reg, quint16 val)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->writePeriReg(mod, reg, val);
}

// command shortcuts for simple operations:
/*!
    \fn Mesydaq2::start(void)

    starts a data acquisition
 */
void Mesydaq2::start(void)
{
   	protocol("remote start", NOTICE);
	foreach(MCPD8 *it, m_mcpd)
		it->start();
	emit statusChanged("STARTED");
}

/*!
    \fn Mesydaq2::stop(void)

    stops a data acquisition
 */
void Mesydaq2::stop(void)
{
	foreach(MCPD8 *it, m_mcpd)
		it->stop();
	emit statusChanged("STOPPED");
   	protocol("remote stop", NOTICE);
}

/*!
    \fn Mesydaq2::cont(void)

    continues a data acquisition
 */
void Mesydaq2::cont(void)
{
	protocol("remote cont", NOTICE);
	foreach(MCPD8 *it, m_mcpd)
		it->cont();
	emit statusChanged("STARTED");
}

/*!
    \fn Mesydaq2::reset(void)

    resets the MCPD's

    \todo implement me
 */
void Mesydaq2::reset(void)
{
}

/*!
    \fn Mesydaq2::checkMcpd(quint8 device)
    
    rescan all MCPD's for the connected modules

    \return true if successfully finished otherwise false
 */
bool Mesydaq2::checkMcpd(quint8 /* device */)
{
	quint8 c(0);
	foreach(MCPD8 *value, m_mcpd)
		scanPeriph(c++);    	
	return true;
}

/*!
    \fn Mesydaq2::setProtocol(const quint16 id, const QString &mcpdIP, const QString dataIP, const quint16 dataPort, const QString cmdIP, const quint16 cmdPort)

    configures a MCPD for the communication it will set the IP address of the module, the IP address and ports of the data and command sink

    \param id number of the MCPD
    \param mcpdIP new IP address of the module
    \param dataIP IP address to which data packets should be send (if 0.0.0.0 the sender will be receive them)
    \param dataPort port number for data packets (if 0 the port number won't be changed)
    \param cmdIP IP address to which cmd answer packets should be send (if 0.0.0.0 the sender will be receive them)
    \param cmdPort port number for cmd answer packets (if 0 the port number won't be changed)
    \see getProtocol
 */
void Mesydaq2::setProtocol(const quint16 id, const QString &mcpdIP, const QString dataIP, const quint16 dataPort, const QString cmdIP, const quint16 cmdPort)
{
	m_mcpd[id]->setProtocol(mcpdIP, dataIP, dataPort, cmdIP, cmdPort);
}	

/*!
    \fn Mesydaq2::getMode(const quint16 id, quint8 addr)

    get the mode: amplitude or position

    \param id number of the MCPD
    \param addr module number
    \see setMode
 */
bool Mesydaq2::getMode(const quint16 id, quint8 addr)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getMode(addr);
	return false;
}

/*!
    \fn Mesydaq2::setMode(const quint16 id, quint8 addr, bool mode)

    set the mode to amplitude or position

    \param id number of the MCPD
    \param addr number of the module
    \param mode if true amplitude mode otherwise position mode
    \see getMode
*/
void Mesydaq2::setMode(const quint16 id, quint8 addr, bool mode)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setMode(addr, mode);
}

/*!
    \fn Mesydaq2::setPulser(const quint16 id, quint8 addr, quint8 chan, quint8 pos, quint8 amp, bool onoff)

    set the pulser of the module to a position, channel, and amplitude
    and switch it on or off

    \param id number of the MCPD
    \param addr number of the module
    \param chan number of the channel of the module
    \param pos set the position to left, middle or right of the 'tube'
    \param amp the amplitude of a test pulse (event)
    \param onoff true the pulser will be switch on, otherwise off
    \see isPulserOn
 */
void Mesydaq2::setPulser(const quint16 id, quint8 addr, quint8 channel, quint8 position, quint8 amp, bool onoff)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setPulser(addr, channel, position, amp, onoff);
}

/*!
    \fn Mesydaq2::setCounterCell(quint16 id, quint16 source, quint16 trigger, quint16 compare)

    map the counter cell

    \param id number of the MCPD
    \param source source of the counter
    \param trigger trigger level
    \param compare ????
    \see getCounterCell
 */
void Mesydaq2::setCounterCell(quint16 id, quint16 source, quint16 trigger, quint16 compare)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setCounterCell(source, trigger, compare);
}

/*!
    \fn Mesydaq2::setParamSource(quint16 id, quint16 param, quint16 source)

    set the source of a parameter

    \param id number of the MCPD
    \param param number of the parameter
    \param source number of source
    \see getParamSource
 */
void Mesydaq2::setParamSource(quint16 id, quint16 param, quint16 source)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setParamSource(param, source);
}

/*!
    \fn Mesydaq2::setAuxTimer(quint16 id, quint16 tim, quint16 val)

    sets the auxiliary timer to a new value

    \param id number of the MCPD
    \param tim number of the timer
    \param val new timer value
    \see getAuxTimer
 */
void Mesydaq2::setAuxTimer(quint16 id, quint16 tim, quint16 val)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setAuxTimer(tim, val);
}

/*!
    \fn Mesydaq2::setMasterClock(quint16 id, quint64 val)

    sets the master clock to a new value

    \todo check for the masters

    \param id number of the MCPD
    \param val new clock value
 */
void Mesydaq2::setMasterClock(quint16 id, quint64 val)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setMasterClock(val);
}

/*!
    \fn Mesydaq2::setTimingSetup(quint16 id, bool master, bool sync)

    sets the communication parameters between the MCPD's

    \param id number of the MCPD
    \param master is this MCPD master or not
    \param sync should the MCPD synchronized with the other MCPD's
 */
void Mesydaq2::setTimingSetup(quint16 id, bool master, bool sync)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setTimingSetup(master, sync);
}

/*!
    \fn Mesydaq2::setId(quint16 id, quint8 mcpdid)

    sets the id of the MCPD

    \param id number of the MCPD
    \param mcpdid the new ID of the MCPD
    \see getId
 */
void Mesydaq2::setId(quint16 id, quint8 mcpdid)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setId(mcpdid);
}

/*!
    \fn Mesydaq2::setGain(quint16 id, quint8 addr, quint8 chan, quint8 gainval)

    sets the gain to a poti value

    \param id number of the MCPD
    \param addr number of the module
    \param chan channel number of the module
    \param gainval poti value of the gain
    \see getGain
 */
void Mesydaq2::setGain(quint16 id, quint8 addr, quint8 channel, quint8 gain)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setGain(addr, channel, gain);
}

/*!
    \overload Mesydaq2::setGain(quint16 id, quint8 addr, quint8 chan, float gainval)

    the gain value will be set as a user value

    \param id number of the MCPD
    \param addr number of the module
    \param chan channel number of the module
    \param gainval user value of the gain
    \see getGain
 */
void Mesydaq2::setGain(quint16 id, quint8 addr, quint8 chan, float gain)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setGain(addr, chan, gain);
}

/*!
    \fn Mesydaq2::setThreshold(quint16 id, quint8 addr, quint8 thresh)

    set the threshold value as poti value

    \param id number of the MCPD
    \param addr number of the module
    \param thresh threshold value as poti value
    \see getThresh
 */
void Mesydaq2::setThreshold(quint16 id, quint8 addr, quint8 thresh)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setThreshold(addr, thresh);
}

/*!
    \overload Mesydaq2::setThreshold(quint16 id, quint8 addr, quint16 thresh)

    set the threshold value 

    \param id number of the MCPD
    \param addr number of the module
    \param thresh threshold value 
    \see getThresh
*/
void Mesydaq2::setThreshold(quint16 id, quint8 addr, quint16 thresh)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setThreshold(addr, thresh);
}

/*!
    \fn Mesydaq2::getMpsdId(quint16 id, quint8 addr)

    get the detected ID of the MPSD. If MPSD not exists it will return 0.

    \param id number of the MCPD
    \param addr module number
    \return module ID (type)
    \see readId
 */
quint8 Mesydaq2::getMpsdId(quint16 id, quint8 addr)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getMpsdId(addr);
	return 0;
}

/*!
    \fn Mesydaq2::getMpsdType(quint16 id, quint8 addr)

    get the detected ID of the MPSD. If MPSD not exists it will return 0.
    the value is a human readable value.

    \param id number of the MCPD
    \param addr module number
    \return module ID (type)
    \see readId
 */
QString Mesydaq2::getMpsdType(quint16 id, quint8 addr)
{
	if (m_mcpd.contains(id))
		switch(m_mcpd[id]->getMpsdId(addr))
		{
			case MPSD8 :
				return "MPSD-8";
			case MPSD8P:
				return "MPSD-8+";
			case 0:
				return "-";
			default:
				return "MPSD-8??";
		}
	return "-";
}

/*!
    \fn Mesydaq2::getMspdVersion(quint16 id, quint8 addr)

    get the detected version of the MPSD. If MPSD not exists it will return 0.

    \param id number of the MCPD
    \param addr module number
    \return module ID (type)
    \see readId
*/
float Mesydaq2::getMpsdVersion(quint16 id, quint8 addr)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->version(addr);
	return 0.0;
}

/*!
    \fn Mesydaq2::getPulsChan(quint16 id, quint8 addr)

    gets the current set channel of the pulser of a module

    \param id number of the MCPD
    \param addr module number
    \return the pulser channel
    \see setPulser
    \see getPulsAmp
    \see getPulsPos
 */
quint8 Mesydaq2::getPulsChan(quint16 id, quint8 addr)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getPulsChan(addr);
	return 0;
}

/*!
    \fn Mesydaq2::getPulsAmp(quint16 id, quint8 addr)

    gets the current set pulser amplitude of a module

    \param id number of the MCPD
    \param addr module number
    \return the pulser amplitude
    \see setPulser
    \see getPulsPos
    \see getPulsChan
 */
quint8 Mesydaq2::getPulsAmp(quint16 id, quint8 addr)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getPulsAmp(addr);
	return 0;
}

/*!
    \fn Mesydaq2::getPulsPos(quint16 id, quint8 addr)
    gets the current set pulser position of a module

    \param id number of the MCPD
    \param addr module number
    \return the pulser position
    \see setPulser
    \see getPulsAmp
    \see getPulsChan
 */
quint8 Mesydaq2::getPulsPos(quint16 id, quint8 addr)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getPulsPos(addr);
	return 0;
}

/*!
    \fn Mesydaq2::getCounterCell(quint16 id, quint8 cell, quint16 *celldata)

    celldata[0] = trig, celldata[1] = comp

    \param id number of the MCPD
    \param cell cell number
    \param celldata return data
    \see setCounterCell
 */
void Mesydaq2::getCounterCell(quint16 id, quint8 cell, quint16 *celldata)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->getCounterCell(cell, celldata);
}

/*!
    \fn Mesydaq2::getParamSource(quint16 id, quint16 param)

    get the source of parameter param

    \param id number of the MCPD
    \param param the parameter number
    \return source of the parameter
    \see setParamSource
 */
quint16 Mesydaq2::getParamSource(quint16 id, quint16 param)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getParamSource(param);
	return 0;
}
	
/*!
    \fn Mesydaq2::getAuxTimer(quint16 id, quint16 timer)

    get the value of auxiliary counter

    \param id number of the MCPD
    \param timer number of the timer
    \return counter value
    \see setAuxTimer
 */
quint16 Mesydaq2::getAuxTimer(quint16 id, quint16 timer)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getAuxTimer(timer);
	return 0;
}

/*!
    \fn Mesydaq2::getParameter(quint16 id, quint16 param)

    gets the value of the parameter number param

    \param id number of the MCPD
    \param param parameter number
    \return parameter value
    \see setParameter
 */
quint64 Mesydaq2::getParameter(quint16 id, quint16 param)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getParameter(param);
	return 0;
}

/*!
    \fn Mesydaq2::getGain(quint16 id, quint8 addr,  quint8 chan)

    gets the currently set gain value for a special module and channel

    if the channel number is greater 7 than all channels of the module
    will be set

    \param id number of the MCPD
    \param addr number of the module
    \param chan number of the channel of the module
    \return poti value of the gain
    \see setGain
 */
quint8 Mesydaq2::getGain(quint16 id, quint8 addr, quint8 chan)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getGain(addr, chan);
	return 0;
}

/*!
    \fn Mesydaq2::getThreshold(quint16 id, quint8 addr)

    get the threshold value as poti value

    \param id number of the MCPD
    \param addr module number
    \return the threshold as poti value
    \see setThreshold
 */
quint8 Mesydaq2::getThreshold(quint16 id, quint8 addr)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getThreshold(addr);
	return 0;
}

/*!
    \fn Mesydaq2::setRunId(quint16 id, quint16 runid)

    sets the run ID of the measurement

    \param id number of the MCPD
    \param runid the new run ID
    \return true if operation was succesful or not
    \see getRunId
 */
void Mesydaq2::setRunId(quint16 id, quint16 runid)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setRunId(runid); 
}

void Mesydaq2::setHistfilename(QString name) 
{
	m_histfilename = name;
	if(m_histfilename.indexOf(".mtxt") == -1)
		m_histfilename.append(".mtxt");
}
     
/*!
    \fn Mesydaq2::analyzeBuffer(DATA_PACKET &pd)

    callback to analyze input data packet

    \param pd data packet
 */
void Mesydaq2::analyzeBuffer(DATA_PACKET &pd)
{
	protocol(tr("Mesydaq2::analyzeBuffer(): %1").arg(m_daq), DEBUG);
	if(m_daq == RUNNING)
	{
		quint16 mod = pd.deviceId;	
		if(m_acquireListfile)
		{
			quint16 *pD = (quint16 *) &pd.bufferLength;
			for(quint16 i = 0; i < pd.bufferLength; i++)
				m_datStream << pD[i];
			writeBlockSeparator();
//			qDebug("------------------");
		}
		protocol(tr("buffer : length : %1").arg(pd.bufferLength), DEBUG);
		if(pd.bufferType < 0x0002) 
		{
// extract parameter values:
			for(quint8 i = 0; i < 4; i++)
			{
				quint64 var = 0;
				for(quint8 j = 0; j < 3; j++)
				{
					var <<= 16;
					var |= pd.param[i][2 - j];
				}
				m_mcpd[mod]->setParameter(i, var);
			}
			emit analyzeDataBuffer(pd);
		}
	}
}
