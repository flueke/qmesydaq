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
#include "mcpd2.h"
#include "mdefines.h"
#include "logging.h"
#if defined(_MSC_VER)
	#include "stdafx.h"
#endif

/**
 * constructor
 *
 * \param id ID of the MCPD
 * \param parent Qt parent object
 * \param ip source IP address
 * \param port source port
 * \param sourceIP IP address for incoming packets
 */
MCPD2::MCPD2(quint8 id, QObject *parent, QString ip, quint16 port, QString sourceIP)
	: QObject(parent)
	, m_network(NULL)
	, m_txCmdBufNum(0)
	, m_id(id)
	, m_ownIpAddress(ip)
	, m_cmdPort(port)	// original 7000
	, m_dataPort(7000)
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

	m_network = NetworkDevice::create(this, sourceIP, port);
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
MCPD2::~MCPD2()
{
	m_mpsd.clear();
	NetworkDevice::destroy(m_network);
	m_network = NULL;
	delete m_commTimer;
	m_commTimer = NULL;
}

/*!
    \fn MCPD2::init(void)

    initializes the MCPD and tries to set the right communication parameters

    \return true if operation was succesful or not
 */
bool MCPD2::init(void)
{
	int modus = TPA;

	quint16 cap = capabilities();

	MSG_NOTICE << "capabilities : " << cap;

	if (m_version < 8.18)
		modus = TP;

	for (quint8 c = 0; c < 8; c++)
		if (m_mpsd.find(c) != m_mpsd.end())
		{
			switch (m_mpsd[c]->getModuleId())
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
			if (m_mpsd[c]->getModuleId() == TYPE_MPSD8P)
				writePeriReg(c, 1, modus);
			version(c);
		}
	return true;
}

/*!
    \fn MCPD2::reset(void)
    resets the MCPD-8

    \return true if operation was succesful or not
 */
bool MCPD2::reset(void)
{	
	initCmdBuffer(DAQRESET);	
	finishCmdBuffer(0);
	bool tmp = sendCommand();
  	usleep (100000);
	return tmp;
}

/*!
    \fn MCPD2::start(void)

    starts the data acquisition

    \return true if operation was succesful or not
    \see stop
    \see cont
 */
bool MCPD2::start(void)
{	 
	initCmdBuffer(DAQSTART);	
	finishCmdBuffer(0);
	return sendCommand(false);
}

/*!
    \fn MCPD2::stop(void)

    stops the data acquisition

    \return true if operation was succesful or not
    \see start
    \see cont
 */
bool MCPD2::stop(void)
{	
	initCmdBuffer(DAQSTOP);	
	finishCmdBuffer(0);
	return sendCommand();
}

/*!
    \fn MCPD2::cont(void)

    continues the data acquisition

    \return true if operation was succesful or not
    \see start
    \see stop
 */
bool MCPD2::cont(void)
{	
	initCmdBuffer(CONTINUE);	
	finishCmdBuffer(0);
	return sendCommand();
}

/*!
    \fn MCPD2::getModuleId(quint8 addr)

    get the detected ID of the MPSD. If MPSD not exists it will return 0.

    \param addr module number
    \return module ID (type)
    \see readId
 */
quint8 MCPD2::getModuleId(quint8 addr)
{
	if (m_mpsd.contains(addr))
		return m_mpsd[addr]->getModuleId();
	else
		return 0;
}

/*!
    \fn MCPD2::setId(quint8 mcpdid)

    sets the id of the MCPD

    \param mcpdid the new ID of the MCPD
    \return true if operation was succesful or not
    \see getId
 */
bool MCPD2::setId(quint8 mcpdid)
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
    \fn MCPD2::capabilities()

    read out the capabilities register of the MCPD 

    \return capabilities register of the MCPD
 */
quint16 MCPD2::capabilities()
{
	return readRegister(102);
}

/*!
    \fn MCPD2::capabilities(quint16 mod)

    read out the capabilities of the MPSD with number mod

    \param mod number of the MPSD
    \return capabilities register of the MPSD
 */
quint16 MCPD2::capabilities(quint16 mod)
{
	if (m_mpsd.find(mod) != m_mpsd.end())
		return readPeriReg(mod, 0);
	return 0;
}

/*!
   \fn float MCPD2::version(void)
   \return firmware version of the MCPD whereas the integral places represents the major number 
           and the decimal parts the minor number
 */
float MCPD2::version(void)
{
	initCmdBuffer(GETVER);
	finishCmdBuffer(0);
	if(sendCommand())
		return m_version;
	return -1.0;
}

/*!
   \fn float MPCD2::version(quint16 mod)

   In the peripheral register 2 is the version of its firmware. The upper byte is the major 
   and the lower byte the minor number

   \param mod
   \return firmware version of the MPSD whereas the integral places represents the major number 
           and the decimal parts the minor number
 */
float MCPD2::version(quint16 mod)
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
    \fn MCPD2::readId(void)

    reads the ID's of all connected MPSD-8/8+ and MSTD-16

    \return true if operation was succesful or not
    \see getModuleId
 */
bool MCPD2::readId(void)
{
	m_mpsd.clear();
	initCmdBuffer(READID);
	m_cmdBuf.data[0] = 2;
	finishCmdBuffer(1); 
	return sendCommand();
}

/*!
    \fn MCPD2::setGain(quint16 addr, quint8 chan, quint8 gainval)

    sets the gain to a poti value

    \param addr number of the module
    \param chan channel number of the module
    \param gainval poti value of the gain
    \return true if operation was succesful or not
    \see getGain
 */
bool MCPD2::setGain(quint16 addr, quint8 chan, quint8 gainval)
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
    \fn MCPD2::getGainPoti(quint16 addr,  quint8 chan)

    gets the currently set gain value for a special module and channel

    if the channel number is greater 7 than all channels of the module
    will be set

    \param addr number of the module
    \param chan number of the channel of the module
    \return poti value of the gain
    \see setGain
    \see getGainPoti
 */
quint8 MCPD2::getGainPoti(quint16 addr,  quint8 chan)
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
    \fn float MCPD2::getGainVal(quint16 addr,  quint8 chan)

    gets the currently set gain value for a special module and channel

    if the channel number is greater 7 than all channels of the module
    will be set

    \param addr number of the module
    \param chan number of the channel of the module
    \return poti value of the gain
    \see setGain
    \see getGainPoti
 */
float MCPD2::getGainVal(quint16 addr,  quint8 chan)
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
    \overload MCPD2::setGain(quint16 addr, quint8 chan, float gainval)

    the gain value will be set as a user value
    \param addr number of the module
    \param chan channel number of the module
    \param gainval user value of the gain
    \return true if operation was succesful or not
    \see getGain
 */
bool MCPD2::setGain(quint16 addr, quint8 chan, float gainval)
{
	if (m_mpsd.find(addr) != m_mpsd.end())
		return setGain(addr, chan, m_mpsd[addr]->calcGainpoti(gainval)); 
	return false;
}

/*!
    \fn MCPD2::setThreshold(quint16 addr, quint8 thresh)

    set the threshold value as poti value

    \param addr number of the module
    \param thresh threshold value as poti value
    \return true if operation was succesful or not
    \see getThreshold
 */
bool MCPD2::setThreshold(quint16 addr, quint8 thresh)
{
	if (m_mpsd.find(addr) != m_mpsd.end())
	{
		m_mpsd[addr]->setThreshold(thresh, 1);
		initCmdBuffer(DAQSETTHRESH);
		m_cmdBuf.result = thresh;
		m_cmdBuf.coll = addr;
		finishCmdBuffer(0);
        	return sendCommand();
	}
	return false;
}

/*!
    \fn MCPD2::getThreshold(quint16 addr)

    get the threshold value as poti value

    \param addr module number
    \return the threshold as poti value
    \see setThreshold
 */
quint8 MCPD2::getThreshold(quint16 addr)
{
	if (m_mpsd.find(addr) != m_mpsd.end())
		return m_mpsd[addr]->getThreshold(0);
	return 0;
}

/*!
    \fn MCPD2::setMode(quint16 addr, bool mode)

    set the mode to amplitude or position

    \param addr number of the module
    \param mode if true amplitude mode otherwise position mode
    \return true if operation was succesful or not
    \see getMode
*/
bool MCPD2::setMode(quint16 addr, bool mode)
{
	if (m_mpsd.find(addr) == m_mpsd.end())
		return false;
	if (addr > 8)
		addr = 8;
#if defined(_MSC_VER)
#	pragma message("TODO common mode handling")
#else
#	warning TODO common mode handling
#endif
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
    \fn MCPD2::getMode(quint16 addr)

    get the mode: amplitude or position

    \param addr module number
    \return true if in amplitude mode otherwise in position mode
    \see setMode
 */
bool MCPD2::getMode(quint16 addr)
{
	if (m_mpsd.find(addr) != m_mpsd.end())
		return m_mpsd[addr]->getMode(0);
	return false;
}

/*!
    \fn MCPD2::setPulser(quint16 addr, quint8 chan, quint8 pos, quint8 amp, bool onoff)

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
bool MCPD2::setPulser(quint16 addr, quint8 chan, quint8 pos, quint8 amp, bool onoff)
{
	MSG_INFO << "MCPD2::setPulser(addr = " << addr << ", chan = " << chan << ", pos = " << pos
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
#if defined(_MSC_VER)
#	pragma message("TODO common pulser handling")
#else
#	warning TODO common pulser handling
#endif
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
    \fn MCPD2::setAuxTimer(quint16 tim, quint16 val)

    sets the auxiliary timer to a new value

    \param tim number of the timer
    \param val new timer value
    \return true if operation was succesful or not
    \see getAuxTimer
 */
bool MCPD2::setAuxTimer(quint16 tim, quint16 val)
{
	MSG_ERROR << "MCPD2::setAuxTimer(" << tim << ", " << val << ')';
	if(tim > 3)
		tim = 3;
	initCmdBuffer(SETAUXTIMER);
	m_cmdBuf.data[0] = tim;
	m_cmdBuf.data[1] = val;
	finishCmdBuffer(2);
	if(sendCommand())
	{
		m_auxTimer[tim] = val;
		return true;
	}
	return false;
}

/*!
    \fn MCPD2::setCounterCell(quint16 source, quint16 trigger, quint16 compare)

    map the counter cell

    \param source source of the counter
    \param trigger trigger level
    \param compare ????
    \see getCounterCell
 */
bool MCPD2::setCounterCell(quint16 source, quint16 trigger, quint16 compare)
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
		MSG_INFO << "mcpd " << m_id << ": set counter cell " << source << ": trigger # is " << trigger << ", compare value " << compare << '.';
	
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
    \fn MCPD2::getCounterCell(quint8 cell, quint16 *celldata)

    celldata[0] = trig, celldata[1] = comp

    \param cell cell number
    \param celldata return data
    \see setCounterCell
 */
void MCPD2::getCounterCell(quint8 cell, quint16 *celldata)
{
	if (cell > 7)
		cell = 7;
	celldata[0] = m_counterCell[cell][0];
	celldata[1] = m_counterCell[cell][1];
}


/*!
    \fn MCPD2::setParamSource(quint16 param, quint16 source)

    set the source of a parameter

    \param param number of the parameter
    \param source number of source
    \return true if operation was succesful or not
    \see getParamSource
 */
bool MCPD2::setParamSource(quint16 param, quint16 source)
{
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
    \fn MCPD2::getParamSource(quint16 param)

    get the source of parameter param

    \param param the parameter number
    \return source of the parameter
    \see setParamSource
 */
quint16 MCPD2::getParamSource(quint16 param)
{
	return param > 3 ? 0 : m_paramSource[param];
}


/*!
    \fn MCPD2::setProtocol(const QString& addr, const QString& datasink, const quint16 dataport, const QString& cmdsink, const quint16 cmdport)

    configures the MCPD for the communication it will set the IP address of the module, the IP address and ports of the data and command sink

    \param addr new IP address of the module
    \param datasink IP address to which data packets should be send (if 0.0.0.0 the sender will be receive them)
    \param dataport port number for data packets (if 0 the port number won't be changed)
    \param cmdsink IP address to which cmd answer packets should be send (if 0.0.0.0 the sender will be receive them)
    \param cmdport port number for cmd answer packets (if 0 the port number won't be changed)
    \return true if operation was succesful or not
    \see getProtocol
 */
bool MCPD2::setProtocol(const QString& addr, const QString& datasink, const quint16 dataport, const QString& cmdsink, const quint16 cmdport)
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
	quint32 ip = cmd.toIPv4Address();

	m_cmdBuf.data[0] = (ip >> 24) & 0xff;
	m_cmdBuf.data[1] = (ip >> 16) & 0xff;
	m_cmdBuf.data[2] = (ip >> 8) & 0xff;
	m_cmdBuf.data[3] = (ip & 0xff);

// IP address of data receiver
	cmd = QHostAddress(datasink);
	ip = cmd.toIPv4Address();
	m_cmdBuf.data[4] = (ip >> 24) & 0xff;
	m_cmdBuf.data[5] = (ip >> 16) & 0xff;
	m_cmdBuf.data[6] = (ip >> 8) & 0xff;
	m_cmdBuf.data[7] = ip & 0xff;
	if (ip > 0x00FFFFFF) 
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
	ip = cmd.toIPv4Address();
	m_cmdBuf.data[10] = (ip >> 24) & 0xff;
	m_cmdBuf.data[11] = (ip >> 16) & 0xff;
	m_cmdBuf.data[12] = (ip >> 8) & 0xff;
	m_cmdBuf.data[13] = ip & 0xff;
	if (ip > 0x00FFFFFF) 
	{
		m_cmdIpAddress = cmdsink;
		MSG_NOTICE << "mcpd #" << m_id << ": cmd ip address set to " << m_cmdIpAddress;
	}
	finishCmdBuffer(14);
	if (sendCommand())
	{
		if (ip > 0x00FFFFFF) 
		{
			m_ownIpAddress = addr;
			MSG_NOTICE << "mcpd #" << m_id << ": ip address set to " << m_ownIpAddress;
		}
		return true;
	}
	return false;
}

/*!
    \fn MCPD2::getProtocol(quint16 *addr)

    \param addr ????
    \see setProtocol
 */
void MCPD2::getProtocol(quint16 * addr)
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
    \fn MCPD2::setDac(quint16 dac, quint16 val)
    \todo this function has to be implemented

    the MCPD has a analogue output which may be set by programmer 

    \param dac
    \param val
    \return true if operation was succesful or not
 */
bool MCPD2::setDac(quint16 /* dac */, quint16 /* val */)
{
	return true;
}

/*!
    \fn MCPD2::sendSerialString(QString str)
    \todo this function has to be implemented

    \param str
    \return true if operation was succesful or not
 */
bool MCPD2::sendSerialString(QString /* str*/)
{
	return true;
}

/*!
    \fn MCPD2::setRunId(quint16 runid)

    sets the run ID of the measurement

    \param runid the new run ID
    \return true if operation was succesful or not
    \see getRunId
 */
bool MCPD2::setRunId(quint16 runid)
{
	if(m_master)
	{
    		m_runId = runid;
		initCmdBuffer(SETRUNID);
		m_cmdBuf.data[0] = m_runId;
		finishCmdBuffer(1);
		MSG_ERROR << "mcpd " << m_id << ": set run ID to " << m_runId;
		return sendCommand();
  	}
	MSG_ERROR << "Error: trying to set run ID on mcpd " << m_id << " - not master!";
	return false;
}

/*!
    \fn MCPD2::setParameter(quint16 param, quint64 val)

    sets a parameter param to a new value
  
    \param param parameter number
    \param val new value
    \return true if operation was succesful or not
    \see getParameter
 */
bool MCPD2::setParameter(quint16 param, quint64 val)
{
	if(param > 3)
		return false;
	m_parameter[param] = val;
	return true;
}

/*!
    \fn MCPD2::getParameter(quint16 param)

    gets the value of the parameter number param

    \param param parameter number
    \return parameter value
    \see setParameter
 */
quint64 MCPD2::getParameter(quint16 param)
{
	return param > 3 ?  0 : m_parameter[param];
}

/*!
    \fn MCPD2::getAuxTimer(quint16 timer)

    get the value of auxiliary counter

    \param timer number of the timer
    \return counter value
    \see setAuxTimer
 */
quint16 MCPD2::getAuxTimer(quint16 timer)
{
	return timer > 3 ? 0 : m_auxTimer[timer];
}

/*!
    \fn MCPD2::stdInit(void)
 */
void MCPD2::stdInit(void)
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
		m_parameter[c] = c;
	} 
}

/*!
    \fn MCPD2::setStream(quint16 strm)

    ????

    \param strm ????	
    \return true if operation was succesful or not
    \see getStream
 */
bool MCPD2::setStream(quint16 strm)
{
	m_stream = bool(strm);
#if defined(_MSC_VER)
#	pragma message("TODO MCPD2::setStream(quint16 strm)")
#else
#	warning TODO MCPD2::setStream(quint16 strm)
#endif
//! \todo implement me
#if 0
	unsigned short id = (unsigned short) deviceId->value();	
	initCmdBuffer(QUIET);	
	m_cmdBuf.data[0] = strm;
	finishCmdBuffer(1);
	MSG_WARNING << "Set stream " << strm;
	return sendCommand();
#endif
	return true;
}

#if 0
/*!
    \fn MCPD2::serialize(QDataStream ds)
 */
bool MCPD2::serialize(QDataStream /* ds */)
{
    /// @todo implement me
	return false;
}

#endif

// general buffer preparations:
void MCPD2::initCmdBuffer(quint8 cmd)
{
  	memset(&m_cmdBuf, 0, sizeof(m_cmdBuf));
	m_cmdBuf.cmd = cmd;
#if 0
	m_cmdBuf.sender = 0;
	m_cmdBuf.cpu = 0;
	m_cmdBuf.headerlength = 0;
	m_cmdBuf.packet = 0;
#endif
}

void MCPD2::finishCmdBuffer(quint16)
{
	m_cmdBuf.hchksm = calcChksum(m_cmdBuf);
}

int MCPD2::sendCommand(bool wait)
{
	if(m_network->sendBuffer(m_ownIpAddress, m_cmdBuf))
	{
#if defined(_MSC_VER)
#	pragma message("TODO	MSG_DEBUG << m_network->ip().toLocal8Bit().constData() << '(' << m_network->port() \") : \" << m_cmdBuf.bufferNumber << \". sent cmd: \" << m_cmdBuf.cmd << \" to id: \" << m_cmdBuf.deviceId;")
#else
#	warning TODO	MSG_DEBUG << m_network->ip().toLocal8Bit().constData() << '(' << m_network->port() ") : " << m_cmdBuf.bufferNumber << ". sent cmd: " << m_cmdBuf.cmd << " to id: " << m_cmdBuf.deviceId;
#endif
		communicate(wait);
		if (wait)
		{
			m_commTimer->start(500);
			MSG_DEBUG << m_network->ip().toLocal8Bit().constData() << '(' << m_network->port() << ") : timer started";
// wait for answer
			while(isBusy())
				qApp->processEvents();
		}
		m_cmdTxd++;
	}
	return 1;
}

/*!
    \fn MCPD2::calcChksum(const MDP_PACKET2 &buffer)
 */
quint8 MCPD2::calcChksum(const MDP_PACKET2 &buffer)
{
	const quint8 *cmdBuf = reinterpret_cast<const quint8 *>(&buffer);
	quint8 chksum = buffer.hchksm;

	for (int i = 0; i < 7; i++)
                chksum ^= cmdBuf[i];
	return chksum;
}

/*!
    \fn MCPD2::analyzeBuffer(MDP_PACKET recBuf)
	
    analyze the data package coming from the MCPD-8

    \param recBuf data package
 */
void MCPD2::analyzeBuffer(MDP_PACKET recBuf)
{
	if (recBuf.deviceId != m_id)
		return;

	quint16 diff = recBuf.bufferNumber - m_lastBufnum;
	if(diff > 1 && recBuf.bufferNumber > 0 && m_lastBufnum != 255)
		MSG_ERROR << m_network->ip().toLocal8Bit().constData() << '(' << m_network->port() << ')' << m_id << " : Lost " << diff << " Buffers: current: "
							<< recBuf.bufferNumber << ", last " << m_lastBufnum;
	m_lastBufnum = recBuf.bufferNumber;

	if(recBuf.bufferType & CMDBUFTYPE)
	{
		communicate(false);
		m_commTimer->stop();
//	MSG_DEBUG << m_network->ip().toLocal8Bit().constData() << '(' << m_network->port() << ") : timer stopped";
	
		++m_cmdRxd;
//	MSG_DEBUG << m_network->ip().toLocal8Bit().constData() << '(' << m_network->port() << ") : id " << recBuf.deviceId;

		m_headertime = recBuf.time[0] + (quint64(recBuf.time[1]) << 16) + (quint64(recBuf.time[2]) << 32);
		m_timemsec = (m_headertime / 10000); // headertime is in 100ns steps

//	MSG_DEBUG << tr("MCPD2::analyzeBuffer(MDP_PACKET recBuf) 0x%1 : %2").arg(recBuf.bufferType, 0, 16).arg(recBuf.cmd);
		
		MPSD8	*ptrMPSD;
		quint16 chksum = recBuf.headerChksum;
#if defined(_MSC_VER)
#	pragma message("if (chksum != calcChksum(recBuf))")
#	pragma message("MSG_INFO << \"cmd packet (cmd = \" << recBuf.cmd << \", size = \" << recBuf.bufferLength")
#	pragma message("<< \") is not valid (CHKSUM error) \" << chksum << \" != (expected)\" << calcChksum(recBuf);")
#else
#	warning TODO	if (chksum != calcChksum(recBuf))
#	warning TODO		MSG_INFO << "cmd packet (cmd = " << recBuf.cmd << ", size = " << recBuf.bufferLength
#	warning TODO			<< ") is not valid (CHKSUM error) " << chksum << " != (expected)" << calcChksum(recBuf);
#endif
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
			case SETCELL:
				if (recBuf.cmd & 0x80)
					MSG_ERROR << "SETCELL : failed";
				else
					MSG_INFO << ": SETCELL";
				break;
			case SETAUXTIMER:
				if (recBuf.data[2] != m_auxTimer[recBuf.data[1]])
				{
					MSG_ERROR << "Error setting auxiliary timer, tim " << recBuf.data[1] << ", is: "
										<< recBuf.data[2] << ", should be " << m_auxTimer[recBuf.data[1]];
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
#if defined(_MSC_VER)
#	pragma message("TODO if the configuration has changed")
#else
#	warning TODO if the configuration has changed
#endif
//! \todo if the configuration has changed
				for(quint8 c = 0; c < 8; c++)
				{
					MSG_ERROR << "module ID : " << recBuf.data[c];
					if (m_mpsd.find(c) == m_mpsd.end())
					{
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
				MSG_DEBUG << "Modul (ID  " << m_id << "): Version number : " << m_version;
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
//		MSG_DEBUG << "ID " << m_id << " : emit analyzeBuffer(recBuf)";
		emit analyzeDataBuffer(*((DATA_PACKET*)(&recBuf)));
	}
}

/*!
    \fn MCPD2::commTimeout()
 */
void MCPD2::commTimeout()
{
	communicate(false);
	MSG_ERROR << "timeout while waiting for cmd " << m_cmdBuf.cmd << " answer from ID: " << m_id;
}

/*!
    \fn MCPD2::readPeriReg(quint16 mod, quint16 reg)

    reads the content of a register in a module

    \param mod number of the module
    \param reg number of the register
    \return content of the register
    \see writePeriReg
 */
quint16 MCPD2::readPeriReg(quint16 mod, quint16 reg)
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
    \fn MCPD2::writePeriReg(quint16 mod, quint16 reg, quint16 val)

    writes a value into a module register

    \param mod number of the module
    \param reg number of the register
    \param val new value
    \return true if operation was succesful or not
    \see readPeriReg
 */
bool MCPD2::writePeriReg(quint16 mod, quint16 reg, quint16 val)
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
    \fn MCPD2::writeRegister(quint16 reg, quint16 val)

    writes a value into a register of the MCPD

    \param reg number of the register
    \param val new value
    \return true if operation was succesful or not
    \see readRegister
 */
bool MCPD2::writeRegister(quint16 reg, quint16 val)
{
	initCmdBuffer(WRITEREGISTER);
	m_cmdBuf.data[0] = 1;
	m_cmdBuf.data[1] = reg;
	m_cmdBuf.data[2] = val;
	finishCmdBuffer(3);
	return sendCommand();
}

/*!
    \fn  MCPD2::readRegister(quint16 reg)
    reads the content of a register 

    \param reg number of the register
    \return content of the register
    \see writeRegister
 */
quint16 MCPD2::readRegister(quint16 reg)
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
    \fn MCPD2::setMasterClock(quint64 val)

    sets the master clock to a new value
   
    \param val new clock value
    \return true if operation was succesful or not
 */
bool MCPD2::setMasterClock(quint64 val)
{
	initCmdBuffer(SETCLOCK);
	m_cmdBuf.data[0] = val & 0xFFFF;
	m_cmdBuf.data[1] = (val >> 16) & 0xFFFF;
	m_cmdBuf.data[2] = (val >> 32) & 0xFFFF;
	finishCmdBuffer(3);
	return sendCommand();
}

/*!
    \fn MCPD2::setTimingSetup(bool master, bool term)

    sets the communication parameters between the MCPD's

    \param master is this MCPD master or not
    \param term should the MCPD synchronization bus terminated or not
    \return true if operation was succesful or not
 */
bool MCPD2::setTimingSetup(bool master, bool term)
{
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
    \fn  MCPD2::isPulserOn(quint8 addr)

    checks if the pulser of module number addr

    \param addr number of the module to query
    \return is pulser on or not
 */
bool MCPD2::isPulserOn(quint8 addr)
{
	if (m_mpsd.find(addr) == m_mpsd.end())
		return false;
	if (getModuleId(addr) && m_mpsd[addr]->isPulserOn())
		return true;
	return false;
}

/*! 
    \fn MCPD2::isPulserOn()

    \return true if one of the connected modules has a switched on pulser
 */
bool MCPD2::isPulserOn()
{
	for (quint8 i = 0; i < 8; ++i)
		if (isPulserOn(i))
			return true;
	return false;
}

/*!
    \fn	MCPD2::getPulsPos(quint8 addr, bool preset)
    gets the current set pulser position of a module

    \param addr module number
    \param preset ???
    \return the pulser position
    \see setPulser
    \see getPulsAmp
    \see getPulsChan
 */
quint8	MCPD2::getPulsPos(quint8 addr, bool preset)
{
	if (m_mpsd.find(addr) == m_mpsd.end())
		return -1;
	return m_mpsd[addr]->getPulsPos(preset);
}

/*!
    \fn	MCPD2::getPulsAmp(quint8 addr, bool preset)

    gets the current set pulser amplitude of a module

    \param addr module number
    \param preset ???
    \return the pulser amplitude
    \see setPulser
    \see getPulsPos
    \see getPulsChan
 */
quint8	MCPD2::getPulsAmp(quint8 addr, bool preset)
{
	if (m_mpsd.find(addr) == m_mpsd.end())
		return -1;
	return m_mpsd[addr]->getPulsAmp(preset);
} 

/*!
    \fn	MCPD2::getPulsChan(quint8 addr, bool preset)

    gets the current set channel of the pulser of a module

    \param addr module number
    \param preset ???
    \return the pulser channel
    \see setPulser
    \see getPulsAmp
    \see getPulsPos
 */
quint8	MCPD2::getPulsChan(quint8 addr, bool preset)
{
	if (m_mpsd.find(addr) == m_mpsd.end())
		return -1;
	return m_mpsd[addr]->getPulsChan(preset);
} 

/*!
    \fn MCPD2::initModule(quint8 id)
    
    initializes a Module:
	- sets threshold
	- sets pulser
	- sets mode
	- writes peripheral registers
	- ...
    
    \param id number of the MPSD
 */
void MCPD2::initModule(quint8 id)
{
#if defined(_MSC_VER)
#	pragma message("TODO gain initialization")
#else
#	warning TODO gain initialization
#endif

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
	if(getModuleId(id) == 105)
	{
		// write register 1
		writePeriReg(id, 1, 4);
	}
}
