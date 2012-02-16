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
#include <QHostAddress>
#include <QApplication>
#include <QTimer>

#include "networkdevice.h"
#include "mpsd8.h"
#include "mcpd8.h"
#include "mstd16.h"
#include "mdll.h"
#include "mdefines.h"
#include "logging.h"

/**
 * constructor
 *
 * \param id ID of the MCPD
 * \param parent Qt parent object
 * \param ip source IP address
 * \param port source port
 * \param sourceIP IP address for incoming packets
 */
MCPD8::MCPD8(quint8 id, QObject *parent, QString ip, quint16 port, QString sourceIP)
	: QObject(parent)
	, m_network(NULL)
	, m_txCmdBufNum(0)
	, m_id(id)
	, m_ownIpAddress(ip)
	, m_cmdPort(port)	// original 7000
	, m_dataPort(port)	// original 7000
	, m_master(true)
	, m_term(true)
	, m_stream(false)
	, m_commActive(false)
	, m_lastBufnum(0)
	, m_commTimer(NULL)
	, m_runId(0)
	, m_daq(false)
	, m_dataRxd(0)
	, m_cmdTxd(0)
	, m_cmdRxd(0)
	, m_headertime(0)
	, m_timemsec(0)
	, m_version(0)
{
	stdInit();
	m_network = NetworkDevice::create(NULL, sourceIP, port);
	connect(m_network, SIGNAL(bufferReceived(MDP_PACKET)), this, SLOT(analyzeBuffer(MDP_PACKET)));

	m_commTimer = new QTimer(this);
	connect(m_commTimer, SIGNAL(timeout()), this, SLOT(commTimeout()));
	m_commTimer->setSingleShot(true);

	memset(&m_cmdBuf, 0, sizeof(m_cmdBuf));

	m_mpsd.clear();

//	setId(m_id);
	version();
	readId();
	init();
}

//! destructor
MCPD8::~MCPD8()
{
	m_mpsd.clear();
	NetworkDevice* network=m_network;
	m_network = NULL;
	NetworkDevice::destroy(network);
	delete m_commTimer;
	m_commTimer = NULL;
}

/*!
    \fn MCPD8::init(void)

    initializes the MCPD and tries to set the right communication parameters

    \return true if operation was succesful or not
 */
bool MCPD8::init(void)
{
	int modus = TPA;

	quint16 cap = capabilities();

	MSG_NOTICE << "capabilities : " << cap;

	if (m_version < 8.18)
		modus = TP;

	for (quint8 c = 0; c < 8; c++)
		if (m_mpsd.find(c) != m_mpsd.end())
		{
			switch (m_mpsd[c]->getMpsdId())
			{
				case TYPE_MPSD8P:
					cap = capabilities(c);
					MSG_NOTICE << "module : " << c << " capabilities : " << cap;
					modus &= cap;
					MSG_NOTICE << "modus : " << modus;
					break;
				default:
					modus = P;
			}
		}
// Register 103 is the TX mode register
// set tx capability 
	writeRegister(103, modus);

	for(quint8 c = 0; c < 8; c++)
		if (m_mpsd.find(c) != m_mpsd.end())
		{
			if (m_mpsd[c]->getMpsdId() == TYPE_MPSD8P)
				writePeriReg(c, 1, modus);
			version(c);
		}
	return true;
}

/*!
    \fn MCPD8::reset(void)
    resets the MCPD-8

    \return true if operation was succesful or not
 */
bool MCPD8::reset(void)
{	
	initCmdBuffer(RESET);	
	finishCmdBuffer(0);
	return sendCommand();
}

/*!
    \fn MCPD8::start(void)

    starts the data acquisition

    \return true if operation was succesful or not
    \see stop
    \see cont
 */
bool MCPD8::start(void)
{	
	initCmdBuffer(START);	
	finishCmdBuffer(0);
	return sendCommand();
}

/*!
    \fn MCPD8::stop(void)

    stops the data acquisition

    \return true if operation was succesful or not
    \see start
    \see cont
 */
bool MCPD8::stop(void)
{	
	initCmdBuffer(STOP);	
	finishCmdBuffer(0);
	return sendCommand();
}

/*!
    \fn MCPD8::cont(void)

    continues the data acquisition

    \return true if operation was succesful or not
    \see start
    \see stop
 */
bool MCPD8::cont(void)
{	
	initCmdBuffer(CONTINUE);	
	finishCmdBuffer(0);
	return sendCommand();
}

/*!
    \fn MCPD8::getMpsdId(quint8 addr)

    get the detected ID of the MPSD. If MPSD not exists it will return 0.

    \param addr module number
    \return module ID (type)
    \see readId
 */
quint8 MCPD8::getMpsdId(quint8 addr)
{
	if (m_mpsd.contains(addr))
		return m_mpsd[addr]->getMpsdId();
	else
		return 0;
}

/*!
    \fn bool MCPD8::online(quint8)

    returns whether the module with id was found and online or not

    \param addr module number
    \return true or false
 */
bool MCPD8::online(quint8 addr)
{
	if (m_mpsd.contains(addr))
		return m_mpsd[addr]->online();
	return false;
}

/*!
    \fn MCPD8::setId(quint8 mcpdid)

    sets the id of the MCPD

    \param mcpdid the new ID of the MCPD
    \return true if operation was succesful or not
    \see getId
 */
bool MCPD8::setId(quint8 mcpdid)
{
	MSG_NOTICE << "Set id for mcpd-8 #" << m_id << " to " << mcpdid << '.';
	initCmdBuffer(SETID);
	m_cmdBuf.data[0] = mcpdid;
	finishCmdBuffer(1);
	if (sendCommand())
	{
		m_id = mcpdid;
		return true;
	}
	return false;
}

/*!
    \fn MCPD8::capabilities()

    read out the capabilities register of the MCPD 

    \return capabilities register of the MCPD
 */
quint16 MCPD8::capabilities()
{
	return readRegister(102);
}

/*!
    \fn MCPD8::capabilities(quint16 mod)

    read out the capabilities of the MPSD with number mod

    \param mod number of the MPSD
    \return capabilities register of the MPSD
 */
quint16 MCPD8::capabilities(quint16 mod)
{
	if (m_mpsd.find(mod) != m_mpsd.end())
		return readPeriReg(mod, 0);
	return 0;
}

/*!
   \fn float MCPD8::version(void)
   \return firmware version of the MCPD whereas the integral places represents the major number 
           and the decimal parts the minor number
 */
float MCPD8::version(void)
{
	initCmdBuffer(GETVER);
	finishCmdBuffer(0);
	if(sendCommand())
		return m_version;
	return -1.0;
}

/*!
   \fn float MCPD8::version(quint16 mod)

   Returns the version number of a connected MPSD module.

   In the peripheral register 2 is the version of its firmware. The upper byte is the major 
   and the lower byte the minor number

   \param mod the modul number

   \return firmware version of the MPSD whereas the integral places represents the major number 
           and the decimal parts the minor number
 */
float MCPD8::version(quint16 mod)
{
	float tmpFloat(0.0);
	if (m_mpsd.find(mod) != m_mpsd.end())
	{
		quint16 tmp = readPeriReg(mod, 2);
		tmpFloat = ((tmp > 4) & 0xF) * 10 + (tmp & 0xF);
		tmpFloat /= 100.;
		tmpFloat += (tmp >> 8);
	}
	MSG_INFO << "MPSD (ID " << mod << "): Version number : " << tmpFloat;
	return tmpFloat;
}

/*!
    \fn MCPD8::readId(void)

    reads the ID's of all connected MPSD-8/8+ and MSTD-16

    \return true if operation was succesful or not
    \see getMpsdId
 */
bool MCPD8::readId(void)
{
//	m_mpsd.clear();
	initCmdBuffer(READID);
	m_cmdBuf.data[0] = 2;
	finishCmdBuffer(1); 
	return sendCommand();
}

/*!
    \fn MCPD8::setGain(quint16 addr, quint8 chan, quint8 gainval)

    sets the gain to a poti value

    \param addr number of the module
    \param chan channel number of the module
    \param gainval poti value of the gain
    \return true if operation was succesful or not
    \see getGain
 */
bool MCPD8::setGain(quint16 addr, quint8 chan, quint8 gainval)
{
	if (!m_mpsd.size() || m_mpsd.find(addr) == m_mpsd.end())
		return false;
	if (chan > 8)
		chan = 8;

	m_mpsd[addr]->setGain(chan, gainval, 1);
	initCmdBuffer(SETGAIN);
	m_cmdBuf.data[0] = addr;
	m_cmdBuf.data[1] = chan;
	m_cmdBuf.data[2] = gainval;
	MSG_INFO << "set gain to potival: " << m_cmdBuf.data[2];
	finishCmdBuffer(3);
	return sendCommand();
}

/*!
    \fn MCPD8::getGainPoti(quint16 addr,  quint8 chan)

    gets the currently set gain value for a special module and channel

    if the channel number is greater 7 than all channels of the module
    will be set

    \param addr number of the module
    \param chan number of the channel of the module
    \return poti value of the gain
    \see setGain
    \see getGainPoti
 */
quint8 MCPD8::getGainPoti(quint16 addr,  quint8 chan)
{
	if (m_mpsd.find(addr) != m_mpsd.end())
	{
		if (chan > 7)
			chan = 8;
		return m_mpsd[addr]->getGainpoti(chan, 0);
	}
	return 0;
}

/*!
    \fn float MCPD8::getGainVal(quint16 addr,  quint8 chan)

    gets the currently set gain value for a special module and channel

    if the channel number is greater 7 than all channels of the module
    will be set

    \param addr number of the module
    \param chan number of the channel of the module
    \return poti value of the gain
    \see setGain
    \see getGainPoti
 */
float MCPD8::getGainVal(quint16 addr,  quint8 chan)
{
	if (m_mpsd.find(addr) != m_mpsd.end())
	{
		if (chan > 7)
			chan = 8;
		return m_mpsd[addr]->getGainval(chan, 0);
	}
	return 0;
}

/*!
    \overload bool MCPD8::setGain(quint16 addr, quint8 chan, float gainval)

    the gain value will be set as a user value
    \param addr number of the module
    \param chan channel number of the module
    \param gainval user value of the gain
    \return true if operation was succesful or not
    \see getGain
 */
bool MCPD8::setGain(quint16 addr, quint8 chan, float gainval)
{
	if (m_mpsd.find(addr) != m_mpsd.end())
		return setGain(addr, chan, m_mpsd[addr]->calcGainpoti(gainval)); 
	return false;
}

/*!
    \fn bool MCPD8::setThreshold(quint16 addr, quint8 thresh)

    set the threshold value as poti value

    \param addr number of the module
    \param thresh threshold value as poti value
    \return true if operation was succesful or not
    \see getThresh
 */
bool MCPD8::setThreshold(quint16 addr, quint8 thresh)
{
	if (m_mpsd.find(addr) != m_mpsd.end())
	{
		m_mpsd[addr]->setThreshold(thresh, 1);
		initCmdBuffer(SETTHRESH);
		m_cmdBuf.data[0] = addr;
		m_cmdBuf.data[1] = thresh;
		finishCmdBuffer(2);
        	return sendCommand();
	}
	return false;
}

/*!
    \fn quint8 MCPD8::getThreshold(quint16 addr)

    get the threshold value as poti value

    \param addr module number
    \return the threshold as poti value
    \see setThreshold
 */
quint8 MCPD8::getThreshold(quint16 addr)
{
	if (m_mpsd.find(addr) != m_mpsd.end())
		return m_mpsd[addr]->getThreshold(0);
	return 0;
}

/*!
    \fn bool MCPD8::setMode(quint16 addr, bool mode)

    set the mode to amplitude or position

    \param addr number of the module
    \param mode if true amplitude mode otherwise position mode
    \return true if operation was succesful or not
    \see getMode
*/
bool MCPD8::setMode(quint16 addr, bool mode)
{
	if (m_mpsd.find(addr) == m_mpsd.end())
		return false;
	if (addr > 8)
		addr = 8;
#warning TODO common mode handling
//! \todo common mode handling
	if (addr == 8)
	{
		for (int i = 0; i < 8; ++i)
			m_mpsd[i]->setMode(mode, 1);
	}
	else
		m_mpsd[addr]->setMode(mode, 1);
	initCmdBuffer(SETMODE);
	m_cmdBuf.data[0] = addr;
	m_cmdBuf.data[1] = mode;
	finishCmdBuffer(2);
	return sendCommand();
}

/*!
    \fn MCPD8::getMode(quint16 addr)

    get the mode: amplitude or position

    \param addr module number
    \return true if in amplitude mode otherwise in position mode
    \see setMode
 */
bool MCPD8::getMode(quint16 addr)
{
	if (m_mpsd.find(addr) != m_mpsd.end())
		return m_mpsd[addr]->getMode(0);
	return false;
}

/*!
    \fn MCPD8::setPulser(quint16 addr, quint8 chan, quint8 pos, quint8 amp, bool onoff)

    set the pulser of the module to a position, channel, and amplitude
    and switch it on or off

    \param addr number of the module
    \param chan number of the channel of the module
    \param pos set the position to left, middle or right of the 'tube'
    \param amp the amplitude of a test pulse (event)
    \param onoff true the pulser will be switch on, otherwise off
    \return true if operation was succesful or not
    \see isPulserOn
 */
bool MCPD8::setPulser(quint16 addr, quint8 chan, quint8 pos, quint8 amp, bool onoff)
{
	MSG_INFO << "MCPD8::setPulser(addr = " << addr << ", chan = " << chan << ", pos = " << pos
					 << ", amp = " << amp << ", onoff = " << onoff << ')';
	if (m_mpsd.find(addr) == m_mpsd.end())
		return false;
	if (addr > 7)
		addr = 7;
	if (chan > 8)
		chan = 8;
	if (pos > 2)
		pos = 2;
	if (chan == 8)
	{
#warning TODO common pulser handling
//! \todo common pulser handling
		for (int i = 0; i < 8; ++i)
			m_mpsd[addr]->setPulser(i, pos, amp, onoff, 1);
	}
	else
		m_mpsd[addr]->setPulserPoti(chan, pos, amp, onoff, 1);
	initCmdBuffer(SETPULSER);
	m_cmdBuf.data[0] = addr;
	m_cmdBuf.data[1] = chan;
	m_cmdBuf.data[2] = pos;
	m_cmdBuf.data[3] = amp;
	m_cmdBuf.data[4] = onoff;
	finishCmdBuffer(5);
	return sendCommand();
}

/*!
    \fn MCPD8::setAuxTimer(quint16 tim, quint16 val)

    sets the auxiliary timer to a new value

    \param tim number of the timer
    \param val new timer value
    \return true if operation was succesful or not
    \see getAuxTimer
 */
bool MCPD8::setAuxTimer(quint16 tim, quint16 val)
{
	MSG_INFO << "MCPD8::setAuxTimer(" << tim << ", " << val << ')';
	if(tim > 3)
		tim = 3;
	initCmdBuffer(SETAUXTIMER);
	m_cmdBuf.data[0] = tim;
	m_cmdBuf.data[1] = val;
	m_auxTimer[tim] = val;
	finishCmdBuffer(2);
	return sendCommand();
}

/*!
    \fn MCPD8::setCounterCell(quint16 source, quint16 trigger, quint16 compare)

    map the counter cell

    \param source source of the counter
    \param trigger trigger level
    \param compare ????
    \see getCounterCell
 */
bool MCPD8::setCounterCell(quint16 source, quint16 trigger, quint16 compare)
{
	bool errorflag = true;
	if(source > 7)
	{
		MSG_ERROR << "Error: mcpd " << m_id << ": trying to set counter cell #" << source << ". Range exceeded! Max. cell# is 7";
		errorflag = false;
	}
	if(trigger > 7)
	{
		MSG_ERROR << "Error: mcpd " << m_id << ": trying to set counter cell trigger # to " << trigger << ". Range exceeded! Max. trigger# is 7";
		errorflag = false;
	}
	if(compare > 22)
	{
		MSG_ERROR << "Error: mcpd " << m_id << ": trying to set counter cell compare value to " << compare << ". Range exceeded! Max. value is 22";
		errorflag = false;
	}
	if(errorflag)
	{
		MSG_NOTICE << "mcpd " << m_id << ": set counter cell " << source << ": trigger # is " << trigger << ", compare value " << compare << '.';
	
		initCmdBuffer(SETCELL);
		m_cmdBuf.data[0] = source;
		m_cmdBuf.data[1] = trigger;
		m_cmdBuf.data[2] = compare;
		finishCmdBuffer(3);
		if (sendCommand())
		{
			m_counterCell[source][0] = trigger;
			m_counterCell[source][1] = compare;
		}
		else
			errorflag = false;
	}
	return errorflag;
}

/*!
    \fn MCPD8::getCounterCell(quint8 cell, quint16 *celldata)

    celldata[0] = trig, celldata[1] = comp

    \param cell cell number
    \param celldata return data
    \see setCounterCell
 */
void MCPD8::getCounterCell(quint8 cell, quint16 *celldata)
{
	if (cell > 7)
		cell = 7;
	celldata[0] = m_counterCell[cell][0];
	celldata[1] = m_counterCell[cell][1];
}


/*!
    \fn MCPD8::setParamSource(quint16 param, quint16 source)

    set the source of a parameter

    \param param number of the parameter
    \param source number of source
    \return true if operation was succesful or not
    \see getParamSource
 */
bool MCPD8::setParamSource(quint16 param, quint16 source)
{
	MSG_NOTICE << "set parameter source " << param << " to " << source;
	if(param > 3 || source > 8)
		return false;
	m_paramSource[param] = source;
	initCmdBuffer(SETPARAM);
	m_cmdBuf.data[0] = param;
	m_cmdBuf.data[1] = source;
	finishCmdBuffer(2);
	return sendCommand();
}

/*!
    \fn MCPD8::getParamSource(quint16 param)

    get the source of parameter param

    \param param the parameter number
    \return source of the parameter
    \see setParamSource
 */
quint16 MCPD8::getParamSource(quint16 param)
{
	return param > 3 ? 0 : m_paramSource[param];
}

/*!
    \fn bool MCPD8::getParameter(void)
    
    \return true if operation was succesful or not
 */
bool MCPD8::getParameter(void)
{
	initCmdBuffer(GETPARAM);
	finishCmdBuffer(0);
	return sendCommand();
}

/*!
    \fn bool MCPD8::setProtocol(const QString& addr, const QString& datasink, const quint16 dataport, const QString& cmdsink, const quint16 cmdport)

    configures the MCPD for the communication it will set the IP address of the module, the IP address and ports of the data and command sink

    \param addr new IP address of the module
    \param datasink IP address to which data packets should be send (if 0.0.0.0 the sender will be receive them)
    \param dataport port number for data packets (if 0 the port number won't be changed)
    \param cmdsink IP address to which cmd answer packets should be send (if 0.0.0.0 the sender will be receive them)
    \param cmdport port number for cmd answer packets (if 0 the port number won't be changed)
    \return true if operation was succesful or not
    \see getProtocol
 */
bool MCPD8::setProtocol(const QString& addr, const QString& datasink, const quint16 dataport, const QString& cmdsink, const quint16 cmdport)
{
// addresses are in addr buffer like follows:
// own addr: [0].[1].[2].[3]
// data addr: [4].[5].[6].[7]
// cmd port [8]
// data port [9]
// cmd addr [10].[11].[12].[13]
// if first address byte == 0, or port == 0: don't change!

	memset(&m_cmdBuf, 0, sizeof(m_cmdBuf));

	initCmdBuffer(SETPROTOCOL);

// IP address of MCPD-8
	QHostAddress cmd(addr);
	quint32 ownip = cmd.toIPv4Address();

	m_cmdBuf.data[0] = (ownip >> 24) & 0xff;
	m_cmdBuf.data[1] = (ownip >> 16) & 0xff;
	m_cmdBuf.data[2] = (ownip >> 8) & 0xff;
	m_cmdBuf.data[3] = (ownip & 0xff);

// IP address of data receiver
	cmd = QHostAddress(datasink);
	quint32 dataip = cmd.toIPv4Address();
	m_cmdBuf.data[4] = (dataip >> 24) & 0xff;
	m_cmdBuf.data[5] = (dataip >> 16) & 0xff;
	m_cmdBuf.data[6] = (dataip >> 8) & 0xff;
	m_cmdBuf.data[7] = dataip & 0xff;
	if (dataip > 0x00FFFFFF)
	{
		m_dataIpAddress = datasink;
		MSG_NOTICE << "mcpd #" << m_id << ": data ip address set to " << m_dataIpAddress;
	}

// UDP port of command receiver
	m_cmdBuf.data[8] = cmdport;
	if (cmdport > 0)
	{
		m_cmdPort = cmdport;
		MSG_NOTICE << "mcpd #" << m_id << ": cmd port set to " << m_cmdPort;
	}

// UDP port of data receiver
	m_cmdBuf.data[9] = dataport;
	if (dataport > 0)
	{
		m_dataPort = dataport;
		MSG_NOTICE << "mcpd #" << m_id << ": data port set to " << m_dataPort;
	}

// IP address of command receiver
	cmd = QHostAddress(cmdsink);
	quint32 cmdip = cmd.toIPv4Address();
	m_cmdBuf.data[10] = (cmdip >> 24) & 0xff;
	m_cmdBuf.data[11] = (cmdip >> 16) & 0xff;
	m_cmdBuf.data[12] = (cmdip >> 8) & 0xff;
	m_cmdBuf.data[13] = cmdip & 0xff;
	if (cmdip > 0x00FFFFFF)
	{
		m_cmdIpAddress = cmdsink;
		MSG_NOTICE << "mcpd #" << m_id << ": cmd ip address set to " << m_cmdIpAddress;
	}
	finishCmdBuffer(14);
	if (sendCommand())
	{
		sleep(1);
		if (ownip > 0x00FFFFFF)
		{
			m_ownIpAddress = addr;
			MSG_NOTICE << "mcpd #" << m_id << ": ip address set to " << m_ownIpAddress;
		}
		return true;
	}
	return false;
}

/*!
    \fn void MCPD8::getProtocol(QString& ip, QString &cmdip, quint16& cmdport, QString& dataip, quint16& dataport) const

    \see setProtocol
 */
void MCPD8::getProtocol(QString& ip, QString& cmdip, quint16& cmdport, QString& dataip, quint16& dataport) const
{
	ip = m_ownIpAddress;
	cmdip = m_cmdIpAddress;
	cmdport = m_cmdPort;
	dataip = m_dataIpAddress;
	dataport = m_dataPort;
}

/*!
    \fn void MCPD8::getProtocol(quint16 *addr)

    \param addr ????
    \see setProtocol
 */
void MCPD8::getProtocol(quint16 * addr)
{
	quint32 cmdIP = QHostAddress(m_cmdIpAddress).toIPv4Address();
	quint32 ownIP = QHostAddress(m_ownIpAddress).toIPv4Address();
	quint32 dataIP = QHostAddress(m_dataIpAddress).toIPv4Address();

	for(quint8 c = 0; c < 4; c++)
	{
		quint8 shift = ((3 - c) * 8);
		addr[c] = (cmdIP >> shift) & 0xFF;
		addr[c + 4] = (dataIP >> shift) & 0xFF;
		addr[c + 10] = (ownIP >> shift) & 0xFF;
	}
	addr[8] = m_cmdPort;
	addr[9] = m_dataPort;
}

/*!
    \fn MCPD8::setDac(quint16 dac, quint16 val)
    \todo this function has to be implemented

    the MCPD has a analogue output which may be set by programmer 

    \param dac
    \param val
    \return true if operation was succesful or not
 */
bool MCPD8::setDac(quint16 /* dac */, quint16 /* val */)
{
	return true;
}

/*!
    \fn MCPD8::sendSerialString(QString str)
    \todo this function has to be implemented

    \param str
    \return true if operation was succesful or not
 */
bool MCPD8::sendSerialString(QString /* str*/)
{
	return true;
}

/*!
    \fn MCPD8::setRunId(quint16 runid)

    sets the run ID of the measurement

    \param runid the new run ID
    \return true if operation was succesful or not
    \see getRunId
 */
bool MCPD8::setRunId(quint16 runid)
{
	m_runId = runid;
	if(m_master)
	{
		initCmdBuffer(SETRUNID);
		m_cmdBuf.data[0] = runid;
		finishCmdBuffer(1);
		MSG_NOTICE << "mcpd " << m_id << ": set run ID to " << runid;
		return sendCommand();
	}
	MSG_ERROR << "Error: trying to set run ID on mcpd " << m_id << " - not master!";
	return false;
}

/*!
    \fn MCPD8::setParameter(quint16 param, quint64 val)

    sets a parameter param to a new value
  
    \param param parameter number
    \param val new value
    \return true if operation was succesful or not
    \see getParameter
 */
bool MCPD8::setParameter(quint16 param, quint64 val)
{
	MSG_DEBUG << "Set parameter " << param << " to " << val;
	if(param > 3)
		return false;
	m_parameter[param] = val;
	return true;
}

/*!
    \fn MCPD8::getParameter(quint16 param)

    gets the value of the parameter number param

    \param param parameter number
    \return parameter value
    \see setParameter
 */
quint64 MCPD8::getParameter(quint16 param)
{
	return param > 3 ?  0 : m_parameter[param];
}

/*!
    \fn MCPD8::getAuxTimer(quint16 timer)

    get the value of auxiliary counter

    \param timer number of the timer
    \return counter value
    \see setAuxTimer
 */
quint16 MCPD8::getAuxTimer(quint16 timer)
{
	return timer > 3 ? 0 : m_auxTimer[timer];
}

/*!
    \fn MCPD8::stdInit(void)
 */
void MCPD8::stdInit(void)
{
	quint8 c;
    
	for(c = 0; c < 4; c++)
	{
		m_counterCell[c][0] = 7; 
		m_counterCell[c][1] = 22; 
	}
	
	for(c = 4; c < 7; c++)
	{
		m_counterCell[c][0] = 0; 
		m_counterCell[c][1] = 0; 
	}

	for(c = 0; c < 4; c++)
	{
		m_auxTimer[c] = 0;
		m_paramSource[c] = c;
		m_parameter[c] = 0;
	} 
}

/*!
    \fn MCPD8::setStream(quint16 strm)

    ????

    \param strm ????	
    \return true if operation was succesful or not
    \see getStream
 */
bool MCPD8::setStream(quint16 strm)
{
	m_stream = bool(strm);
#warning TODO MCPD8::setStream(quint16 strm)
//! \todo implement me
#if 0
	unsigned short id = (unsigned short) deviceId->value();	
	initCmdBuffer(QUIET);	
	m_cmdBuf.data[0] = strm;
	finishCmdBuffer(1);
	MSG_NOTICE << "Set stream " << strm;
	return sendCommand();
#endif
	return true;
}

#if 0
/*!
    \fn MCPD8::serialize(QDataStream ds)
 */
bool MCPD8::serialize(QDataStream /* ds */)
{
    /// @todo implement me
	return false;
}

#endif

// general buffer preparations:

/*!
    \fn void MCPD8::initCmdBuffer(quint16 cmd)

    initializes the command buffer structure with the command cmd
    
    \param cmd command
 */
void MCPD8::initCmdBuffer(quint16 cmd)
{
	m_cmdBuf.bufferType = CMDBUFTYPE;
	m_cmdBuf.headerLength = CMDHEADLEN;
	m_cmdBuf.cmd = cmd;
	m_cmdBuf.bufferNumber = m_txCmdBufNum;
	m_cmdBuf.deviceId = m_id;
}

/*!
    \fn void MCPD8::finishCmdBuffer(quint16 buflen)

    finishes the command buffer:
      - sets the buffer number
      - sets the buffer length
      - adds the checksum
 
    \param buflen size of the parameter buffer
 */
void MCPD8::finishCmdBuffer(quint16 buflen)
{
	m_cmdBuf.bufferNumber =	m_txCmdBufNum++;
	m_cmdBuf.bufferLength = CMDHEADLEN + buflen;
	m_cmdBuf.data[buflen] = 0xFFFF;
	m_cmdBuf.headerChksum = 0;
	m_cmdBuf.headerChksum = calcChksum(m_cmdBuf);
}

/*!
    \fn int MCPD8::sendCommand(void)

    sends the command buffer to the MCPD
 
    \return 
 */
int MCPD8::sendCommand(void)
{
	if(m_network->sendBuffer(m_ownIpAddress, m_cmdBuf))
	{
		MSG_DEBUG << m_network->ip().toLocal8Bit().constData() << '(' << m_network->port() << ") : " << m_cmdBuf.bufferNumber << ". sent cmd: "
							<< m_cmdBuf.cmd << " to id: " << m_cmdBuf.deviceId;
// block other commands due to the writing on flash to avoid crashes on MCPD-8
		if (m_cmdBuf.cmd == SETPROTOCOL)
			sleep(3);
		communicate(true);
		m_commTimer->start(500);
		MSG_DEBUG << m_network->ip().toLocal8Bit().constData() << '(' << m_network->port() << ") : timer started";
// wait for answer
		while(isBusy())
			qApp->processEvents();
		m_cmdTxd++;
	}
	return 1;
}

/*!
    \fn MCPD8::calcChksum(const MDP_PACKET &buffer)

    calculates the checksum of the buffer

    \param buffer 
    \return calculated checksum
 */
quint16 MCPD8::calcChksum(const MDP_PACKET &buffer)
{
	quint16 chksum = buffer.headerChksum;
	const quint16 *p = reinterpret_cast<const quint16 *>(&buffer);
	for (quint32 i = 0; i < buffer.bufferLength; i++)
		chksum ^= p[i];
	return chksum;
}

/*!
    \fn MCPD8::analyzeBuffer(MDP_PACKET recBuf)
	
    analyze the data package coming from the MCPD-8

    \param recBuf data package
 */
void MCPD8::analyzeBuffer(MDP_PACKET recBuf)
{
	if (recBuf.deviceId != m_id)
	{
		MSG_INFO << "deviceId : " << recBuf.deviceId << " <-> " << m_id;
		return;
	}

	quint16 diff = recBuf.bufferNumber - m_lastBufnum;
	if(diff > 1 && recBuf.bufferNumber > 0 && m_lastBufnum != 255)
		MSG_ERROR << m_network->ip().toLocal8Bit().constData() << '(' << m_network->port() << ')' << m_id << " : Lost "
							<< diff << " Buffers: current: " << recBuf.bufferNumber << ", last: " << m_lastBufnum;
	m_lastBufnum = recBuf.bufferNumber;

	if(recBuf.bufferType & CMDBUFTYPE)
	{
		communicate(false);
		m_commTimer->stop();
//		MSG_DEBUG << m_network->ip().toLocal8Bit().constData() << '(' << m_network->port() << ") : timer stopped";
	
		++m_cmdRxd;
//		MSG_DEBUG << m_network->ip().toLocal8Bit().constData() << '(' << m_network->port() << ") : id " << recBuf.deviceId;

		m_headertime = recBuf.time[0] + (quint64(recBuf.time[1]) << 16) + (quint64(recBuf.time[2]) << 32);
		m_timemsec = (m_headertime / 10000); // headertime is in 100ns steps

//		MSG_DEBUG << tr("MCPD8::analyzeBuffer(MDP_PACKET recBuf) 0x%1 : %2").arg(recBuf.bufferType, 0, 16).arg(recBuf.cmd);
		
		MPSD8	*ptrMPSD;
		quint16 chksum = recBuf.headerChksum;
		if (chksum != calcChksum(recBuf))
			MSG_INFO << "cmd packet (cmd = " << recBuf.cmd << ", size = " << recBuf.bufferLength
							 << ") is not valid (CHKSUM error) " << chksum << " != (expected)" << calcChksum(recBuf);
		switch(recBuf.cmd)
		{
			case RESET:
				MSG_ERROR << "not handled command : RESET";
				break;
			case START:
				m_daq = true;
				emit startedDaq();
				break;
			case STOP:
				m_daq = false;
				emit stoppedDaq();
				break;
			case CONTINUE:
				m_daq = true;
				emit continuedDaq();
				break;
			case SETID:
				if (recBuf.cmd & 0x80)
					MSG_ERROR << "SETID : failed";
				else
					MSG_NOTICE << "SETID = " << recBuf.data[0];
				break;
			case SETPROTOCOL:
				// extract ip and eth addresses in case of "this pc"
				break;
			case SETTIMING:
				if (recBuf.cmd & 0x80)
					MSG_ERROR << "SETTIMING : failed";
				else
					MSG_INFO << "SETTIMING : master " << recBuf.data[0] << " terminate " << recBuf.data[1];
				break;
			case SETCLOCK:
				MSG_ERROR << "not handled command : SETCLOCK";
				break;
			case SETRUNID: 
				MSG_ERROR << "not handled command : SETRUNID";
				break;
			case SETCELL:
				if (recBuf.cmd & 0x80)
					MSG_ERROR << "SETCELL : failed";
				else
					MSG_INFO << ": SETCELL";
				break;
			case SETAUXTIMER:
				if (recBuf.data[1] != m_auxTimer[recBuf.data[0]])
				{
					MSG_ERROR << "Error setting auxiliary timer, tim " << recBuf.data[0] << ", is: " << recBuf.data[1]
										<< ", should be " << m_auxTimer[recBuf.data[0]];
				}
				break;
			case SETPARAM:
				if (recBuf.cmd & 0x80)
					MSG_ERROR << "SETPARAM : failed";
				else
					MSG_INFO << "SETPARAM";
				break;
			case GETPARAM:
				MSG_ERROR << "not handled command : GETPARAM";
				{
					quint64 val = recBuf.data[9] + (quint64(recBuf.data[10]) << 16) + (quint64(recBuf.data[11]) << 32);
					setParameter(0, val);
					val = recBuf.data[12] + (quint64(recBuf.data[13]) << 16) + (quint64(recBuf.data[14]) << 32);
					setParameter(1, val);
					val = recBuf.data[15] + (quint64(recBuf.data[16]) << 16) + (quint64(recBuf.data[17]) << 32);
					setParameter(2, val);
					val = recBuf.data[18] + (quint64(recBuf.data[19]) << 16) + (quint64(recBuf.data[20]) << 32);
					setParameter(3, val);
				}
				break;
			case SETGAIN: // extract the set gain values: 
				if(recBuf.bufferLength == 21) // set common gain
				{
					for(quint8 c = 0; c < 8; c++)
					{
						ptrMPSD = m_mpsd[recBuf.data[0]];
						if(recBuf.data[2 + c] != ptrMPSD->getGainpoti(c, 1))
						{
							MSG_ERROR << "Error setting gain, mod " << (8 * recBuf.deviceId + recBuf.data[0]) << ", chan "
												<< c << " is: " << recBuf.data[2+c] << ", should be: " << ptrMPSD->getGainpoti(c, 1);
							// set back to received value
							ptrMPSD->setGain(8, (quint8)recBuf.data[c + 2], 0);
						}
					}
					ptrMPSD->setGain(8, (quint8)recBuf.data[2], 0);
				}
				else// set one channel
				{
					ptrMPSD = m_mpsd[recBuf.data[0]];
					if(recBuf.data[2] != ptrMPSD->getGainpoti(recBuf.data[1], 1))
					{
						MSG_ERROR << "Error setting gain, mod " << (8 * recBuf.deviceId + recBuf.data[0]) << ", chan "
											<< recBuf.data[1] << " is: " << recBuf.data[2] << ", should be: " << ptrMPSD->getGainpoti(recBuf.data[1], 1);
						// set back to received value
					}
					ptrMPSD->setGain(recBuf.data[1], (quint8)recBuf.data[2], 0);
				}
				break;
			case SETTHRESH: // extract the set thresh value:
				ptrMPSD = m_mpsd[recBuf.data[0]];
				if (recBuf.data[1] != ptrMPSD->getThreshold(1))
				{
					MSG_ERROR << "Error setting threshold, mod " << (8 * recBuf.deviceId + recBuf.data[0]) << ", is: "
										<< recBuf.data[1] << ", should be: " << ptrMPSD->getThreshold(1);
				}
				ptrMPSD->setThreshold(recBuf.data[1], 0);
				break;
			case SETPULSER:
				ptrMPSD = m_mpsd[recBuf.data[0]];
				if(recBuf.data[3] != ptrMPSD->getPulsPoti(1))
				{
					MSG_ERROR << "Error setting pulspoti, mod " << (8 * recBuf.deviceId + recBuf.data[0]) << ", is: "
										<< recBuf.data[3] << ", should be: " << ptrMPSD->getPulsPoti(1);
				}
				ptrMPSD->setPulserPoti(recBuf.data[1], recBuf.data[2], recBuf.data[3], recBuf.data[4], 0);
				break;
			case SETMODE: // extract the set mode:
				m_mpsd[recBuf.data[0]]->setMode(recBuf.data[1] == 1, 0);
				break;
			case SETDAC:
				MSG_ERROR << "not handled command : SETDAC";
				break;
			case SENDSERIAL:
				MSG_ERROR << "not handled command : SENDSERIAL";
				break;
			case READSERIAL:
				MSG_ERROR << "not handled command : READSERIAL";
				break;
			case SCANPERI:
				MSG_ERROR << "not handled command : SCANPERI";
				break;
			case WRITEFPGA:
				if (recBuf.cmd & 0x80)
					MSG_ERROR << "WRITEFPGA : failed";
				break;
			case WRITEREGISTER:
				if (recBuf.cmd & 0x80)
					MSG_ERROR << "WRITEREGISTER failed";
				break;
			case READREGISTER:
				for (int i = 0; i < (recBuf.bufferLength - recBuf.headerLength); ++i)
					MSG_DEBUG << "READREGISTER : " << i << " = " << recBuf.data[i];
				m_reg = recBuf.data[0];	
				MSG_INFO << "READREGISTER : " << m_reg << ' ' << recBuf.bufferLength;
				break;
			case READFPGA:
				MSG_ERROR << "not handled command : READFPGA";
				break;
			case SETPOTI:
				MSG_ERROR << "not handled command : SETPOTI";
				break;
			case GETPOTI:
				MSG_ERROR << "not handled command : GETPOTI";
				break;
			case READID: // extract the retrieved MPSD-8 IDs:
#warning TODO if the configuration has changed
//! \todo if the configuration has changed
				for(quint8 c = 0; c < 8; c++)
				{
					QMap<int, MPSD8 *>::iterator it = m_mpsd.find(c);
					if (it == m_mpsd.end())
					{
						m_mpsd[c] = MPSD8::create(c, recBuf.data[c], this);
					}
					else if ((*it)->type() != recBuf.data[c])
					{
//						delete m_mpsd[c];
						m_mpsd.remove(c);
						if (recBuf.data[c] != 0)
							m_mpsd[c] = MPSD8::create(c, recBuf.data[c], this);
					}
				}
				MSG_DEBUG << "READID finished";
				break;
			case DATAREQUEST:
				MSG_ERROR << "not handled command : DATAREQUEST";
				break;
			case QUIET:
				MSG_ERROR << "not handled command : QUIET";
				break;
			case GETVER:
				m_version = recBuf.data[1];
				while (m_version > 1)
					m_version /= 10.;
				m_version += recBuf.data[0];
				MSG_DEBUG << "Modul (ID " << m_id << "): Version number : " << m_version;
				break;
			case READPERIREG:
				ptrMPSD = m_mpsd[recBuf.data[0]];
				m_periReg = recBuf.data[2];
				MSG_DEBUG << "READPERIREG " << recBuf.data[0] << " : " << recBuf.data[1] << " = " << m_periReg;
				break;
			case WRITEPERIREG:
				ptrMPSD = m_mpsd[recBuf.data[0]];
				if(recBuf.data[2] != ptrMPSD->getInternalreg(recBuf.data[1], 1))
				{
					MSG_ERROR << "Error setting internal mpsd-register, mod " << (8 * recBuf.deviceId + recBuf.data[0])
										<< ", is: " << recBuf.data[3] << ", should be: " << ptrMPSD->getPulsPoti(1);
				}
				ptrMPSD->setInternalreg(recBuf.data[1], recBuf.data[2], 0);			
				break;
			default:
				MSG_ERROR << "not handled command : " << recBuf.cmd;
				break;
		}
	}
	else
	{
		++m_dataRxd;
//		MSG_DEBUG << tr("ID " << m_id << " : emit analyzeBuffer(recBuf)";
		emit analyzeDataBuffer(*((DATA_PACKET*)(&recBuf)));
	}
}

/*!
    \fn MCPD8::commTimeout()
 */
void MCPD8::commTimeout()
{
	communicate(false);
	MSG_ERROR << "timeout while waiting for cmd " << m_cmdBuf.cmd << " answer from ID: " << m_id;
}

/*!
    \fn MCPD8::readPeriReg(quint16 mod, quint16 reg)

    reads the content of a register in a module

    \param mod number of the module
    \param reg number of the register
    \return content of the register
    \see writePeriReg
 */
quint16 MCPD8::readPeriReg(quint16 mod, quint16 reg)
{
	initCmdBuffer(READPERIREG);
	m_cmdBuf.data[0] = mod;
	m_cmdBuf.data[1] = reg;
	finishCmdBuffer(2);
	if (sendCommand())
		return m_periReg;
	return 0xFFFF;
}

/*!
    \fn MCPD8::writePeriReg(quint16 mod, quint16 reg, quint16 val)

    writes a value into a module register

    \param mod number of the module
    \param reg number of the register
    \param val new value
    \return true if operation was succesful or not
    \see readPeriReg
 */
bool MCPD8::writePeriReg(quint16 mod, quint16 reg, quint16 val)
{
	m_mpsd[mod]->setInternalreg(reg, val, 1);
	initCmdBuffer(WRITEPERIREG);
	m_cmdBuf.data[0] = mod;
	m_cmdBuf.data[1] = reg;
	m_cmdBuf.data[2] = val;
	finishCmdBuffer(3);
	return sendCommand();
}

/*!
    \fn MCPD8::writeRegister(quint16 reg, quint16 val)

    writes a value into a register of the MCPD

    \param reg number of the register
    \param val new value
    \return true if operation was succesful or not
    \see readRegister
 */
bool MCPD8::writeRegister(quint16 reg, quint16 val)
{
	initCmdBuffer(WRITEREGISTER);
	m_cmdBuf.data[0] = 1;
	m_cmdBuf.data[1] = reg;
	m_cmdBuf.data[2] = val;
	finishCmdBuffer(3);
	return sendCommand();
}

/*!
    \fn  MCPD8::readRegister(quint16 reg)
    reads the content of a register 

    \param reg number of the register
    \return content of the register
    \see writeRegister
 */
quint16 MCPD8::readRegister(quint16 reg)
{
	initCmdBuffer(READREGISTER);
	m_cmdBuf.data[0] = 1;
	m_cmdBuf.data[1] = reg;
	finishCmdBuffer(2);
	if (sendCommand())
		return m_reg;
	return 0xFFFF;
}

/*!
    \fn MCPD8::setMasterClock(quint64 val)

    sets the master clock to a new value
   
    \param val new clock value
    \return true if operation was succesful or not
 */
bool MCPD8::setMasterClock(quint64 val)
{
	initCmdBuffer(SETCLOCK);
	m_cmdBuf.data[0] = val & 0xFFFF;
	m_cmdBuf.data[1] = (val >> 16) & 0xFFFF;
	m_cmdBuf.data[2] = (val >> 32) & 0xFFFF;
	finishCmdBuffer(3);
	return sendCommand();
}

/*!
    \fn MCPD8::setTimingSetup(bool master, bool term)

    sets the communication parameters between the MCPD's

    \param master is this MCPD master or not
    \param term should the MCPD synchronization bus terminated or not
    \return true if operation was succesful or not
 */
bool MCPD8::setTimingSetup(bool master, bool term)
{
	if (master)
		term = true;
	initCmdBuffer(SETTIMING);
	m_cmdBuf.data[0] = master;
	m_cmdBuf.data[1] = term;
	finishCmdBuffer(2);
	if (sendCommand())
	{
		m_master = master;
		m_term = term;
		return true;
	}
	return false;
}

/*!
    \fn  MCPD8::isPulserOn(quint8 addr)

    checks if the pulser of module number addr

    \param addr number of the module to query
    \return is pulser on or not
 */
bool MCPD8::isPulserOn(quint8 addr)
{
	if (m_mpsd.find(addr) == m_mpsd.end())
		return false;
	if (getMpsdId(addr) && m_mpsd[addr]->isPulserOn())
		return true;
	return false;
}

/*! 
    \fn MCPD8::bins()

    \return the maximum number of bins over all MPSD
 */
quint16 MCPD8::bins()
{
	quint16 bins(0);
	for (quint8 i = 0; i < 8; ++i)
		if (m_mpsd.find(i) != m_mpsd.end() && getMpsdId(i))
			if (m_mpsd[i]->bins() > bins)
				bins = m_mpsd[i]->bins();
	return bins;
}

/*! 
    \fn MCPD8::isPulserOn()

    \return true if one of the connected modules has a switched on pulser
 */
bool MCPD8::isPulserOn()
{
	for (quint8 i = 0; i < 8; ++i)
		if (isPulserOn(i))
			return true;
	return false;
}

/*!
    \fn	MCPD8::getPulsPos(quint8 addr, bool preset)
    gets the current set pulser position of a module

    \param addr module number
    \param preset ???
    \return the pulser position
    \see setPulser
    \see getPulsAmp
    \see getPulsChan
 */
quint8	MCPD8::getPulsPos(quint8 addr, bool preset)
{
	if (m_mpsd.find(addr) == m_mpsd.end())
		return -1;
	return m_mpsd[addr]->getPulsPos(preset);
}

/*!
    \fn	MCPD8::getPulsAmp(quint8 addr, bool preset)

    gets the current set pulser amplitude of a module

    \param addr module number
    \param preset ???
    \return the pulser amplitude
    \see setPulser
    \see getPulsPos
    \see getPulsChan
 */
quint8	MCPD8::getPulsAmp(quint8 addr, bool preset)
{
	if (m_mpsd.find(addr) == m_mpsd.end())
		return -1;
	return m_mpsd[addr]->getPulsAmp(preset);
} 

/*!
    \fn	MCPD8::getPulsChan(quint8 addr, bool preset)

    gets the current set channel of the pulser of a module

    \param addr module number
    \param preset ???
    \return the pulser channel
    \see setPulser
    \see getPulsAmp
    \see getPulsPos
 */
quint8	MCPD8::getPulsChan(quint8 addr, bool preset)
{
	if (m_mpsd.find(addr) == m_mpsd.end())
		return -1;
	return m_mpsd[addr]->getPulsChan(preset);
} 

/*!
    \fn MCPD8::initMpsd(quint8 id)
    
    initializes a MPSD:
	- sets threshold
	- sets pulser
	- sets mode
	- writes peripheral registers
	- ...
    
    \param id number of the MPSD
 */
void MCPD8::initMpsd(quint8 id)
{
#warning TODO gain initialization
//! \todo gain initialization
#if 0
	quint8 	start = 8,
		stop = 9;
	
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
	setThreshold(id, getThreshold(id));

// pulser
	setPulser(id, 0, 2, 50, false);

// mode
	setMode(id, false);
	
// now set tx capabilities, if id == 105
	if(getMpsdId(id) == 105)
	{
		// write register 1
		writePeriReg(id, 1, 4);
	}
}

/*!
    \fn QString MPCD8::getMpsdType(quint8 addr)
 
    \param addr id number of the MPSD
    \return the type of the MPSD
 */
QString MCPD8::getMpsdType(quint8 addr)
{
	if (m_mpsd.find(addr) != m_mpsd.end())
	{
		return m_mpsd[addr]->getType();
	}
	return "-";
}
	

/*!
    \fn void MCPD8::setHistogram(bool hist)

    \param hist
 */
void MCPD8::setHistogram(bool hist)
{
	foreach(MPSD8 *it, m_mpsd)
		it->setHistogram(hist);
}

/*!
    \fn void MCPD8::setHistogram(quint16 id, bool hist)

    \param id 
    \param hist
 */

void MCPD8::setHistogram(quint16 id, bool hist)
{
	if (m_mpsd.contains(id))
		m_mpsd[id]->setHistogram(hist);
}

/*!
    \fn void MCPD8::setHistogram(quint16 id, quint16 chan, bool hist)

    \param id 
    \param chan
    \param hist
 */
void MCPD8::setHistogram(quint16 id, quint16 chan, bool hist)
{
	if (m_mpsd.contains(id))
		m_mpsd[id]->setHistogram(chan, hist);
	if (!hist)
		setActive(false);
}

/*!
    \fn void MCPD8::setActive(bool act)

    \param act
 */
void MCPD8::setActive(bool act)
{
	foreach(MPSD8 *it, m_mpsd)
		it->setActive(act);
}

/*!
    \fn void MCPD8::setActive(quint16 id, bool act)

    \param id 
    \param act
 */
void MCPD8::setActive(quint16 id, bool act)
{
	if (m_mpsd.contains(id))
		m_mpsd[id]->setActive(act);
}

/*!
    \fn void MCPD8::setActive(quint16 id, quint16 chan, bool act)

    \param id 
    \param chan
    \param act
 */
void MCPD8::setActive(quint16 id, quint16 chan, bool act)
{
	if (m_mpsd.contains(id))
		m_mpsd[id]->setActive(chan, act);
}

/*!
    \fn bool MCPD8::active(void)

    \return true if one of the connected modules is active
 */
bool MCPD8::active(void)
{
	bool result(false);
	foreach(MPSD8 *it, m_mpsd)
		result |= it->active();
	return result;
}

/*!
    \fn bool MCPD8::histogram(void)

    \return true if one of the connected modules contributes to the histogram
 */
bool MCPD8::histogram(void)
{
	bool result(false);
	foreach(MPSD8 *it, m_mpsd)
		result |= it->histogram();
	return result;
}

/*!
    \fn bool MCPD8::active(quint16 id)
 
    \param id
    \return whether the MPSD id is used or not
 */
bool MCPD8::active(quint16 id)
{
	if (m_mpsd.contains(id))
		return m_mpsd[id]->active();
	return false;
}

/*!
    \fn bool MCPD8::active(quint16 id, quint16 chan)
 
    \param id
    \param chan
    \return whether the channel chan of the MPSD id is used or not
 */
bool MCPD8::active(quint16 id, quint16 chan)
{
	if (m_mpsd.contains(id))
		return m_mpsd[id]->active(chan);
	return false;
}

/*!
    \fn bool MCPD8::histogram(quint16 id)
 
    \param id
    \return whether the MPSD id should be integrated in histogram or not
 */
bool MCPD8::histogram(quint16 id)
{
	if (m_mpsd.contains(id))
		return m_mpsd[id]->histogram();
	return false;
}

/*!
    \fn bool MCPD8::histogram(quint16 id, quint16 chan)
 
    \param id
    \param chan
    \return whether the channel chan MPSD id should be integrated in histogram or not
 */
bool MCPD8::histogram(quint16 id, quint16 chan)
{
	if (m_mpsd.contains(id))
		return m_mpsd[id]->histogram(chan);
	return false;
}

/*!
    \fn QList<quint16> MPSD8::getHistogramList(void)
    
    \return the list of channels used in histograms
 */
QList<quint16> MCPD8::getHistogramList(void)
{
	QList<quint16> result;
	foreach(MPSD8 *it, m_mpsd)
	{
		QList<quint16> tmpHistList = it->getHistogramList();
		foreach(quint16 hit, tmpHistList)
			result.append(hit + it->busNumber() * 8);
	}
	qStableSort(result);
	return result;
}

/*!
    \fn QList<quint16> MPSD8::getActiveList(void)

    provides list of active modules

    \return the list of channels which are active
 */
QList<quint16> MCPD8::getActiveList(void)
{
	QList<quint16> result;
#if 0
	for (int i = 0; i < 8; ++i)
		if (m_active[i])
			result << i;
#endif
	return result;
}

quint8 MCPD8::numModules(void)
{
	quint8 n(0);
	foreach(MPSD8 *it, m_mpsd)	
		if (it->getMpsdId())
			n++;
	return n;
}
