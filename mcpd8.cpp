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
#include "mdefines.h"

MCPD8::MCPD8(quint8 id, QObject *parent, QString ip, quint16 port)
	: MesydaqObject(parent)
	, m_network(NULL)
	, m_txCmdBufNum(0)
	, m_ownIpAddress(ip)
	, m_cmdPort(port)	// original 7000
	, m_dataPort(7000)
	, m_master(true)
	, m_runId(0)
	, m_stream(false)
	, m_commActive(false)
	, m_lastBufnum(0)
	, m_commTimer(NULL)
	, m_daq(false)
	, m_dataRxd(0)
	, m_cmdTxd(0)
	, m_cmdRxd(0)
{
// TODO
//	setId(id);
	m_id = id;
	stdInit();
#warning TODO only import the found MPSD''s into the list
	for(int i = 0; i < 8; i++)
	{
		m_mpsd[i] = NULL;
		m_mpsd[i] = new MPSD8(0, this);
	}

	m_network = new NetworkDevice(this, ip, port);
	connect(m_network, SIGNAL(bufferReceived(MDP_PACKET &)), this, SLOT(analyzeBuffer(MDP_PACKET &)));

	m_commTimer = new QTimer(this);
	connect(m_commTimer, SIGNAL(timeout()), this, SLOT(commTimeout()));
	m_commTimer->setSingleShot(true);

	memset(&m_cmdBuf, 0, sizeof(m_cmdBuf));
}


MCPD8::~MCPD8()
{
#warning TODO
	for (int i = 0; i < 8; ++i)
	{
		if (m_mpsd[i])
			delete m_mpsd[i];
		m_mpsd[i] = NULL;
	}
	if (m_network)
		delete m_network;
	m_network = NULL;
	if (m_commTimer)
		delete m_commTimer;
	m_commTimer = NULL;
}

/*!
    \fn MCPD8::init(void)
 */
bool MCPD8::init(void)
{
// set tx capability to P
	initCmdBuffer(WRITEREGISTER);
	m_cmdBuf.data[0] = 1;
	m_cmdBuf.data[1] = 103;
	m_cmdBuf.data[2] = 1;
	finishCmdBuffer(3);
	return sendCommand();
}

/*!
    \fn MCPD8::reset(void)
 */
bool MCPD8::reset(void)
{	
	initCmdBuffer(RESET);	
	finishCmdBuffer(0);
	return sendCommand();
}

/*!
    \fn MCPD8::start(void)
	 */
bool MCPD8::start(void)
{	
	initCmdBuffer(START);	
	finishCmdBuffer(0);
	return sendCommand();
}

/*!
    \fn MCPD8::stop(void)
 */
bool MCPD8::stop(void)
{	
	initCmdBuffer(STOP);	
	finishCmdBuffer(0);
	return sendCommand();
}

/*!
    \fn MCPD8::cont(void)
 */
bool MCPD8::cont(void)
{	
	initCmdBuffer(CONTINUE);	
	finishCmdBuffer(0);
	return sendCommand();
}

/*!
    \fn MCPD8::getMpsdId(quint8 addr)
 */
quint8 MCPD8::getMpsdId(quint8 addr)
{
	if (m_mpsd.find(addr) != m_mpsd.end())
		return m_mpsd[addr]->getMpsdId();
	else
		return 0;
}

/*!
    \fn MCPD8::setId(quint8 mcpdid)
 */
bool MCPD8::setId(quint8 mcpdid)
{
	if(mcpdid > 8)
	{
    		protocol(tr("Error: Set id value (%1) for mcpd-8 #%2 too high! Id set to 8.").arg(mcpdid).arg(m_id), 1);
    		m_id = 8;
    	}
    	else
	{
    		m_id = mcpdid;
    		protocol(tr("Set id for mcpd-8 #%1 to %2.").arg(m_id).arg(mcpdid), 1);
	}
	initCmdBuffer(SETID);
	m_cmdBuf.data[0] = m_id;
	finishCmdBuffer(1);
	return sendCommand();
}

/*!
    \fn MCPD8::readId(void)
 */
bool MCPD8::readId(void)
{
	initCmdBuffer(READID);
	m_cmdBuf.data[0] = 2;
	finishCmdBuffer(3);
	return sendCommand();
}

/*!
    \fn MCPD8::setGain(quint16 addr, quint16 chan, float gainval, quint8 preset)
 */
bool MCPD8::setGain(quint16 addr, quint8 chan, quint8 gainval)
{
	if (chan > 8)
		chan = 8;
#warning TODO
	if (chan == 8)
	{
		for (int i = 0; i < 8; ++i)
			m_mpsd[addr]->setGain(chan, gainval, 1);
	}
	else
		m_mpsd[addr]->setGain(chan, gainval, 1);
	initCmdBuffer(SETGAIN);
	m_cmdBuf.data[0] = addr;
	m_cmdBuf.data[1] = chan;
	m_cmdBuf.data[2] = gainval;
	protocol(tr("set gain to potival: %1").arg(m_cmdBuf.data[2]));
	finishCmdBuffer(3);
	return sendCommand();
}

quint8 MCPD8::getGain(quint16 addr,  quint8 chan)
{
	if (m_mpsd.find(addr) != m_mpsd.end())
	{
		if (chan > 7)
			chan = 7;
		return m_mpsd[addr]->getGainpoti(chan, 0);
	}
	return 0;
}

/*!
    \fn MCPD8::setGain(quint16 addr, quint16 chan, float gainval, quint8 preset)
 */
bool MCPD8::setGain(quint16 addr, quint8 chan, float gainval)
{
	if (m_mpsd.find(addr) != m_mpsd.end())
		return setGain(addr, chan, m_mpsd[addr]->calcGainpoti(gainval)); 
	return false;
}

/*!
    \fn MCPD8::setThreshold(quint16 addr, quint8 tresh)
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

quint8 MCPD8::getThreshold(quint16 addr)
{
	if (m_mpsd.find(addr) != m_mpsd.end())
		return m_mpsd[addr]->getThreshold(0);
	return 0;
}

bool MCPD8::setMode(quint16 addr, bool mode)
{
	if (addr > 8)
		addr = 8;
#warning TODO
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

bool MCPD8::getMode(quint16 addr)
{
	if (m_mpsd.find(addr) != m_mpsd.end())
		return m_mpsd[addr]->getMode(0);
	return 0;
}

bool MCPD8::setPulser(quint16 addr, quint8 chan, quint8 pos, quint8 amp, bool onoff)
{
	protocol(tr("MCPD8::setPulser(addr = %1, chan = %2, pos = %3, amp = %4, onoff = %5)").arg(addr).arg(chan).arg(pos).arg(amp).arg(onoff));
	if (addr > 7)
		addr = 7;
	if (chan > 8)
		chan = 8;
	if (pos > 2)
		pos = 2;
	if (chan == 8)
	{
#warning TODO
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
    \fn MCPD8::setAuxTimer(unsigned char tim, unsigned short val)
 */
bool MCPD8::setAuxTimer(quint16 tim, quint16 val)
{
	if(tim > 3)
		tim = 3;
	initCmdBuffer(SETAUXTIMER);
	m_cmdBuf.data[0] = tim;
	m_cmdBuf.data[1] = val;
	finishCmdBuffer(2);
	m_auxTimer[tim] = val;
	return sendCommand();
}


/*!
    \fn MCPD8::setCounterCell(unsigned char * celldata)
 */
 // celldata[0] = cell, celldata[1] = trig, celldata[2] = comp
bool MCPD8::setCounterCell(quint16 source, quint16 trigger, quint16 compare)
{
	bool errorflag = false;
	if(source > 7)
	{
		protocol(tr("Error: mcpd %1: trying to set counter cell #%2. Range exceeded! Max. cell# is 7").arg(m_id).arg(source), 1);
		errorflag = true;
	}
	if(trigger > 7)
	{
		protocol(tr("Error: mcpd %1: trying to set counter cell trigger # to %2. Range exceeded! Max. trigger# is 7").arg(m_id).arg(trigger), 1);
		errorflag = true;
	}
	if(compare > 22)
	{
		protocol(tr("Error: mcpd %1: trying to set counter cell compare value to %2. Range exceeded! Max. value is 22").arg(m_id).arg(compare), 1);
		errorflag = true;
	}
	if(!errorflag)
	{
		m_counterCell[source][0] = trigger;
		m_counterCell[source][1] = compare;
		
		protocol(tr("mcpd %1: set counter cell %2: trigger # is %3, compare value %4.").arg(m_id).arg(source).arg(trigger).arg(compare), 1);
	
		initCmdBuffer(SETCELL);
		m_cmdBuf.data[0] = source;
		m_cmdBuf.data[1] = trigger;
		m_cmdBuf.data[2] = compare;
		finishCmdBuffer(3);
		return sendCommand();
	}
	return errorflag;
}

/*!
    \fn MCPD8::getCounterCell(unsigned char *)
 */
void MCPD8::getCounterCell(quint8 cell, quint16 *celldata)
{
	celldata[0] = m_counterCell[cell][0];
	celldata[1] = m_counterCell[cell][1];
}


/*!
    \fn MCPD8::setParam(unsigned char param, unsigned char source)
 */
bool MCPD8::setParamSource(quint16 param, quint16 source)
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
    \fn MCPD8::getParamSource(unsigned short param)
 */
quint16 MCPD8::getParamSource(quint16 param)
{
	return param > 3 ? 0 : m_paramSource[param];
}


/*!
    \fn MCPD8::setProtocol(const QString addr, const QString datasink, const quint16 dataport, const QString cmdsink, const quint16 cmdport)
 */
bool MCPD8::setProtocol(const QString addr, const QString datasink, const quint16 dataport, const QString cmdsink, const quint16 cmdport)
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
	if (ip > 0x00FFFFFF) 
	{
		m_ownIpAddress = addr;
		protocol(tr("mcpd #%1: ip address set to %2").arg(m_id).arg(m_ownIpAddress), 1);
	}

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
		protocol(tr("mcpd #%1: data ip address set to %2").arg(m_id).arg(m_dataIpAddress), 1);
	}

// UDP port of command receiver
	m_cmdBuf.data[8] = cmdport;
	if (cmdport > 0)
	{
		m_cmdPort = cmdport;
		protocol(tr("mcpd #%1: cmd port set to %2").arg(m_id).arg(m_cmdPort), 1);
	}

// UDP port of data receiver
	m_cmdBuf.data[9] = dataport;
	if (dataport > 0)
	{
		m_dataPort = dataport;
		protocol(tr("mcpd #%1: data port set to %2").arg(m_id).arg(m_dataPort), 1);
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
		protocol(tr("mcpd #%1: cmd ip address set to %2").arg(m_id).arg(m_cmdIpAddress), 1);
	}

	finishCmdBuffer(14);
	return sendCommand();
}

/*!
    \fn MCPD8::getProtocol(unsigned short addr*)
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
    \fn MCPD8::setDac(unsigned char dac, unsigned short val)
 */
bool MCPD8::setDac(quint16 /* dac */, quint16 /* val */)
{
	return true;
}


/*!
    \fn MCPD8::setOutstring(QString str)
 */
bool MCPD8::sendSerialString(QString /* str*/)
{
	return true;
}


/*!
    \fn MCPD8::setRunId(unsigned short runid)
 */
bool MCPD8::setRunId(quint16 runid)
{
	if(m_master)
	{
    		m_runId = runid;
    		protocol(tr("mcpd %1: set run ID to %2").arg(m_id).arg(m_runId), 1);
  	}
	else
		protocol(tr("Error: trying to set run ID on mcpd %1 - not master!").arg(m_id), 1);
	return m_master;
}


/*!
    \fn MCPD8::setParameter(unsigned char param, unsigned long val)
 */
bool MCPD8::setParameter(quint16 param, quint64 val)
{
	if(param > 3)
		return false;
	m_parameter[param] = val;
	return true;
}


/*!
    \fn MCPD8::getParameter(unsigned char param)
 */
quint64 MCPD8::getParameter(quint16 param)
{
	return param > 3 ?  0 : m_parameter[param];
}


/*!
    \fn MCPD8::getAuxTimer(unsigned short timer)
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
		m_parameter[c] = c;
	} 
}

/*!
    \fn MCPD8::setStream(unsigned short strm)
 */
bool MCPD8::setStream(quint16 strm)
{
	m_stream = bool(strm);
	return true;
}

/*!
    \fn MCPD8::serialize(qDatastream ds)
 */
bool MCPD8::serialize(QDataStream /* ds */)
{
    /// @todo implement me
	return false;
}

// general buffer preparations:
void MCPD8::initCmdBuffer(quint16 cmd)
{
	m_cmdBuf.bufferType = BUFTYPE;
	m_cmdBuf.headerLength = CMDHEADLEN;
	m_cmdBuf.cmd = cmd;
	m_cmdBuf.bufferNumber = m_txCmdBufNum;
	m_cmdBuf.deviceId = m_id;
}

void MCPD8::finishCmdBuffer(quint16 buflen)
{
	m_cmdBuf.bufferNumber =	m_txCmdBufNum++;
	m_cmdBuf.bufferLength = CMDHEADLEN + buflen + 1;
	m_cmdBuf.data[buflen] = 0xFFFF;
	m_cmdBuf.headerChksum = 0;
	m_cmdBuf.headerChksum = calcChksum(m_cmdBuf);
}

int MCPD8::sendCommand(void)
{
	if(m_network->sendBuffer(m_cmdBuf))
	{
		QString pstring;
		pstring.sprintf("%d. sent cmd: %d to id: %d", m_txCmdBufNum, m_cmdBuf.cmd, m_cmdBuf.deviceId);
		protocol(pstring, 2 /*3 */);
		communicate(true);
		m_commTimer->start(500);
// wait for answer
		while(isBusy())
			qApp->processEvents();
		m_cmdTxd++;
	}
	return 1;
}

/*!
    \fn MCPD8::calcChksm(unsigned int * buffer, unsigned int len)
 */
quint16 MCPD8::calcChksum(MDP_PACKET &buffer)
{
	quint16 chksum = 0;
	quint16 *p = reinterpret_cast<quint16 *>(&buffer);
	for (quint32 i = 0; i < buffer.bufferLength; i++)
		chksum ^= p[i];
	return chksum;
}

/*!
    \fn MCPD8::analyzeCmd(MDP_PACKET &recBuf)
 */
void MCPD8::analyzeBuffer(MDP_PACKET &recBuf)
{
	m_commTimer->stop();
	quint8 id = recBuf.deviceId;
	
	protocol(tr("id %1").arg(id), 3);
	communicate(false);
	MPSD8	*ptrMPSD;

	protocol(tr("MCPD8::analyzeBuffer(MDP_PACKET &recBuf) 0x%1 : %2").arg(recBuf.bufferType, 0, 16).arg(recBuf.cmd), 3);
		
	if(recBuf.bufferType & 0x8000)
	{
		++m_cmdRxd;
		quint16 chksum = recBuf.headerChksum;
		recBuf.headerChksum = 0;
		if (chksum != calcChksum(recBuf))
			protocol(tr("packet (%3) is not valid (CHKSUM error) %1 != %2").arg(chksum).arg(calcChksum(recBuf)).arg(recBuf.bufferLength));
		switch(recBuf.cmd)
		{
			case START:
				m_daq = true;
				emit startedDaq();
				break;
			case STOP:
				m_daq = false;
				emit stoppedDaq();
				break;
			case SETPROTOCOL:
				// extract ip and eth addresses in case of "this pc"
				break;
			case READID: // extract the retrieved MPSD-8 IDs:
				for(quint8 c = 0; c < 8; c++)
					m_mpsd[c]->setMpsdId(c, recBuf.data[c]);
				break;
			case SETAUXTIMER:
				ptrMPSD = m_mpsd[recBuf.data[0]];
				if (recBuf.data[2] != m_auxTimer[recBuf.data[1]])
				{
					protocol(tr("Error setting auxiliary timer, tim %1, is: %2, should be %3").
						arg(recBuf.data[1]).arg(recBuf.data[2]).arg(m_auxTimer[recBuf.data[1]]), 1);
				}
				break;
			case SETMODE: // extract the set mode:
				m_mpsd[recBuf.data[0]]->setMode(recBuf.data[1] == 1, 0);
				break;
			case SETPULSER:
				ptrMPSD = m_mpsd[recBuf.data[0]];
				if(recBuf.data[3] != ptrMPSD->getPulsPoti(1))
				{
					protocol(tr("Error setting pulspoti, mod %1, is: %2, should be: %3").
						arg(8 * recBuf.deviceId + recBuf.data[0]).arg(recBuf.data[3]).arg(ptrMPSD->getPulsPoti(1)), 1);
				}
				ptrMPSD->setPulserPoti(recBuf.data[1], recBuf.data[2], recBuf.data[3], recBuf.data[4], 0);
				break;
			case SETTHRESH: // extract the set thresh value:
				ptrMPSD = m_mpsd[recBuf.data[0]];
				if (recBuf.data[1] != ptrMPSD->getThreshpoti(1))
				{
					protocol(tr("Error setting threshold, mod %1, is: %2, should be: %3").
						arg(8 * recBuf.deviceId + recBuf.data[0]).arg(recBuf.data[1]).arg(ptrMPSD->getThreshpoti(1)), 1);
				}
				ptrMPSD->setThreshpoti(recBuf.data[1], 0);
				break;
			case WRITEPERIREG:
				ptrMPSD = m_mpsd[recBuf.data[0]];
				if(recBuf.data[2] != ptrMPSD->getInternalreg(recBuf.data[1], 1))
				{
					protocol(tr("Error setting internal mpsd-register, mod %1, is: %2, should be: %3").
						arg(8 * recBuf.deviceId + recBuf.data[0]).arg(recBuf.data[3]).arg(ptrMPSD->getPulsPoti(1)), 1);
				}
				ptrMPSD->setInternalreg(recBuf.data[1], recBuf.data[2], 0);			
				break;
			case SETGAIN: // extract the set gain values: 
				if(recBuf.bufferLength == 21) // set common gain
				{
					for(quint8 c = 0; c < 8; c++)
					{
						ptrMPSD = m_mpsd[recBuf.data[0]];
						if(recBuf.data[2 + c] != ptrMPSD->getGainpoti(c, 1))
						{
							protocol(tr("Error setting gain, mod %1, chan %2 is: %3, should be: %4").
								arg(8 * recBuf.deviceId + recBuf.data[0]).arg(c).arg(recBuf.data[2+c]).arg(ptrMPSD->getGainpoti(c, 1)), 1);
							// set back to received value
						}
						ptrMPSD->setGain(c, (quint8)recBuf.data[2 + c], 0);
					}
				}
				else// set one channel
				{
					ptrMPSD = m_mpsd[recBuf.data[0]];
					if(recBuf.data[2] != ptrMPSD->getGainpoti(recBuf.data[1], 1))
					{
						protocol(tr("Error setting gain, mod %1, chan %2 is: %3, should be: %4").
							arg(8 * recBuf.deviceId + recBuf.data[0]).arg(recBuf.data[1]).arg(recBuf.data[2]).arg(ptrMPSD->getGainpoti(recBuf.data[1], 1)), 1);
						// set back to received value
					}
					ptrMPSD->setGain(recBuf.data[1], (quint8)recBuf.data[2], 0);
				}
				break;
			case WRITEREGISTER :
				break;
			default:
				protocol(tr("not handled command : %1").arg(recBuf.cmd));
				break;
		}
	}
	else
	{
		++m_dataRxd;
		protocol(tr("ID %1 : emit analyzeBuffer(recBuf)").arg(m_id), 3);
		emit analyzeDataBuffer((DATA_PACKET &)recBuf); 
	}
}

/*!
    \fn MCPD8::commTimeout()
 */
void MCPD8::commTimeout()
{
	communicate(false);
	protocol(tr("timeout while waiting for cmd answer from ID: %1").arg(m_id));
}

/*!
    \fn MCPD8::readPeriReg(unsigned short mod, unsigned short reg)
 */
bool MCPD8::readPeriReg(quint16 mod, quint16 reg)
{
	initCmdBuffer(READPERIREG);
	m_cmdBuf.data[0] = mod;
	m_cmdBuf.data[1] = reg;
	finishCmdBuffer(2);
	return sendCommand();
}

/*!
    \fn MCPD8::writePeriReg(unsigned short mod, unsigned short reg, unsigned short val)
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
    \fn MCPD8::writeRegister(unsigned int addr, unsigned int val)
 */
bool MCPD8::writeRegister(quint16 addr, quint16 reg, quint16 val)
{
	initCmdBuffer(WRITEREGISTER);
	m_cmdBuf.data[0] = addr;
	m_cmdBuf.data[1] = reg;
	m_cmdBuf.data[2] = val;
	finishCmdBuffer(3);
	return sendCommand();
}

bool MCPD8::setMasterClock(quint64 val)
{
	initCmdBuffer(SETCLOCK);
	m_cmdBuf.data[0] = val & 0xFFFF;
	m_cmdBuf.data[1] = (val >> 16) & 0xFFFF;
	m_cmdBuf.data[2] = (val >> 32) & 0xFFFF;
	finishCmdBuffer(3);
	return sendCommand();
}

bool MCPD8::setTimingSetup(bool master, bool sync)
{
	initCmdBuffer(SETTIMING);
	m_cmdBuf.data[0] = master;
	m_cmdBuf.data[1] = sync;
	finishCmdBuffer(2);
	return sendCommand();
}

bool MCPD8::isPulserOn(quint8 addr)
{
	if (m_mpsd[addr]->getMpsdId() && m_mpsd[addr]->isPulserOn())
		return true;
	return false;
}

bool MCPD8::isPulserOn()
{
	for (quint8 i = 0; i < 8; ++i)
		if (m_mpsd[i]->getMpsdId() && m_mpsd[i]->isPulserOn())
			return true;
	return false;
}
