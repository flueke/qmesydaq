/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2020 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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
#include <QTimer>
#include <QThread>
#include "networkdevice.h"
#include "mpsd8.h"
#include "mcpd8.h"
#include "mstd16.h"
#include "mdll.h"
#include "mdefines.h"
#include "qmlogging.h"
#include "stdafx.h"

#define MCPD8_MAX_ERRORCOUNT    5
#define MCPD8_TRY_SETCMD        3

/**
 * constructor
 *
 * \param byId       ID of the MCPD
 * \param szMcpdIp   IP address of MCPD-8 (default 192.168.168.121)
 * \param wPort      UDP port of MCPD-8 (default 54321)
 * \param szMcpdDataIp  optional different IP address of the MCPD data
 * \param wDataPort     optional different UDP port of the MCPD data
 * \param sourceIP   host IP address to bind to
 * \param bTestOnly  do not initialize, read version only
 */
MCPD8::MCPD8(quint8 byId, QString szMcpdIp, quint16 wPort, QString szMcpdDataIp, quint16 wDataPort, QString szHostIp, bool bTestOnly)
    : MCPD(byId, szMcpdIp, wPort, szMcpdDataIp, wDataPort, szHostIp)
    , m_bTestOnly(bTestOnly)
    , m_txCmdBufNum(0)
    , m_master(true)
    , m_term(true)
    , m_extsync(false)
    , m_stream(false)
    , m_iCommActive(RECV)
    , m_lastCmdBufnum(0)
    , m_lastDataBufnum(0)
    , m_runId(0)
    , m_dataRxd(0)
    , m_dataMissed(0)
    , m_cmdTxd(0)
    , m_cmdRxd(0)
    , m_headertime(0)
    , m_timemsec(0)
    , m_version(-1.0)
    , m_capabilities(0)
    , m_txMode(0)
    , m_fpgaVersion(-1.0)
{
    stdInit();
    memset(&m_cmdBuf, 0, sizeof(m_cmdBuf));

    m_mpsd.clear();
    m_mdll.clear();

//  setId(m_id);
//  version();
    if (isInitialized())
        if (scanPeriph() && !m_bTestOnly)
            init();
}

//! destructor
MCPD8::~MCPD8()
{
    m_mpsd.clear();
}

bool MCPD8::isInitialized() const
{
    if (!MCPD::isInitialized())
        return false;
    return (m_iErrorCounter < MCPD8_MAX_ERRORCOUNT);
}

/*!
    \fn MCPD8::init(void)

    initializes the MCPD and tries to set the right communication parameters

    \return true if operation was succesful or not
 */
bool MCPD8::init(void)
{
    // only for MPSDs!
    if(m_mdll.find(0) != m_mdll.end())
            return true;

    quint16 cap = capabilities(false);
    MSG_NOTICE << tr("capabilities : %1").arg(cap);

    if (m_iErrorCounter >= MCPD8_MAX_ERRORCOUNT)
        return false;

    int modus = getTxMode(false);
    if (modus == 0)
    {
        MSG_ERROR << "TX mode not set init with TPA";
        modus = TPA;
    }

    if (modus == TPA && (m_version < 8.18 || m_fpgaVersion < 5.0))
        modus = TP;

    for (quint8 c = 0; c < 8; c++)
        if (m_mpsd.find(c) != m_mpsd.end())
        {
            switch (m_mpsd[c]->getModuleId())
            {
                case TYPE_MPSD8P:
                    cap = capabilities(c);
                    MSG_NOTICE << tr("module : %1 capabilities : %2").arg(c).arg(cap);
                    modus &= cap;
                    break;
                case TYPE_MDLL :
                    modus = TPA;
                    break;
                case TYPE_NOMODULE :
                    break;
                case TYPE_MPSD8SADC:
		    if (modus == TPA)
                        modus = TP;
                    break;
                default:
                    modus = P;
                    break;
            }
        }
    MSG_NOTICE << tr("setting modus : %1").arg(modus);
    if (m_mdll.isEmpty())
    {
        if (setTxMode(modus))
            getTxMode(false);
    }
    if (m_iErrorCounter >= MCPD8_MAX_ERRORCOUNT)
        return false;
    MSG_NOTICE << tr("using modus : %1").arg(getTxMode());

    for (quint16 c = 0; c < 8; c++)
        if (m_mpsd.find(c) != m_mpsd.end())
        {
            switch (m_mpsd[c]->getModuleId())
	    {
		case TYPE_MPSD8P:
//		case TYPE_MPSD8:
		case TYPE_MPSD8SADC:
			writePeriReg(c, 1, modus);
			if (m_iErrorCounter >= MCPD8_MAX_ERRORCOUNT)
				return false;
		default:
			break;
	    }
            version(c);
            if (m_iErrorCounter >= MCPD8_MAX_ERRORCOUNT)
                return false;
        }
    return true;
}

/*!
    \fn MCPD8::width(void)

    \returns the number of possible channels
 */
int MCPD8::width(void) const
{
    int i(0);
    for (quint8 c = 0; c < 8; c++)
        if (m_mpsd.find(c) != m_mpsd.end())
            switch (m_mpsd[c]->getModuleId())
            {
                case TYPE_MSTD16:
		    i += 16;
		    break;
                case TYPE_MDLL :
                    i += 960;
                    break;
		case TYPE_MWPCHR:
		    i += 1024;
		    break;
                default:
		    i += 8;
		    break;
	    }
    return i;
}

/*!
    \fn MCPD8::reset(void)
    resets the MCPD-8

    \return true if operation was succesful or not
 */
bool MCPD8::reset(void)
{
    if (isMaster() || (version() < 9.5))
    {
        MSG_DEBUG << tr("RESET %1").arg(m_byId);
        QMutexLocker locker(m_pCommandMutex);
        initCmdBuffer(RESET);
        finishCmdBuffer(0);
        return sendCommand(false);
    }
    return true;
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
    bool bResult(true);
    if (isMaster() || (version() < 9.9))
    {
	MSG_DEBUG << tr("START %1").arg(m_byId);
	QMutexLocker locker(m_pCommandMutex);
	initCmdBuffer(START);
	finishCmdBuffer(0);
	bResult = sendCommand(false);
	MSG_DEBUG << tr("START %1, result %2").arg(m_byId).arg(bResult);
	m_lastDataBufnum = 0;
    }
    return bResult;
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
    bool bResult(true);
    if (isMaster() || (version() < 9.9))
    {
	MSG_DEBUG << tr("STOP %1").arg(m_byId);
	QMutexLocker locker(m_pCommandMutex);
	initCmdBuffer(STOP);
	finishCmdBuffer(0);
	bResult = sendCommand(false);
	MSG_DEBUG << tr("STOP %1, result %2").arg(m_byId).arg(bResult);
    }
    return bResult;
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
    if (isMaster() || (version() < 9.5))
    {
        MSG_DEBUG << tr("CONTINUE %1").arg(m_byId);
        QMutexLocker locker(m_pCommandMutex);
        initCmdBuffer(CONTINUE);
        finishCmdBuffer(0);
        return sendCommand(false);
    }
    return true;
}

/*!
    \fn MCPD8::getModuleId(quint8 addr)

    get the detected ID of the MPSD. If MPSD not exists it will return 0.

    \param addr module number
    \return module ID (type)
    \see readId
 */
quint8 MCPD8::getModuleId(quint8 addr)
{
// MDLL is always located at address 0
    if (addr == 0 && m_mdll.contains(addr))
            return m_mdll[addr]->getModuleId();
    if (m_mpsd.contains(addr))
        return m_mpsd[addr]->getModuleId();
    else
        return 0;
}

/*!
    \fn MCPD8::getChannels(quint8 addr)

    get the channel count of the MPSD. If MPSD not exists it will return 0.

    \param addr module number
    \return channel count
 */
quint8 MCPD8::getChannels(quint8 addr)
{
// MDLL is always located at address 0
    if (m_mdll.contains(addr))
        return m_mdll[addr]->bins();
    if (m_mpsd.contains(addr))
        return m_mpsd[addr]->getChannels();
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
    MSG_DEBUG << tr("SETID %1 --> %2").arg(m_byId).arg(mcpdid);
    QMutexLocker locker(m_pCommandMutex);
    MSG_NOTICE << tr("Set id for mcpd-8 #%1 to %2.").arg(m_byId).arg(mcpdid);
    initCmdBuffer(SETID);
    m_cmdBuf.data[0] = mcpdid;
    finishCmdBuffer(1);
    if (sendCommand(true))
    {
        m_byId = mcpdid;
        return true;
    }
    return false;
}

/*!
 * \fn bool MCPD8::setTxMode(const quint16 cap)
 *
 * sets the transmission protocol
 *
 *  \param mode the new transmission mode of the MCPD
 *  \return true if operation was succesful or not
 */
bool MCPD8::setTxMode(const quint16 mode)
{
// Register 103 is the TX mode register
// set tx capability
// 	writeRegister(103, cap);
	MSG_DEBUG << tr("SETCAPABILITIES %1 : %2").arg(m_byId).arg(mode);
	QMutexLocker locker(m_pCommandMutex);
	initCmdBuffer(SETCAPABILITIES);
	m_cmdBuf.data[0] = mode;
        finishCmdBuffer(1);
        return sendCommand(true);
}

/*!
    \fn MCPD8::capabilities(const bool cached)

    read out the capabilities register of the MCPD

    \param cached take the read values
    \return capabilities register of the MCPD
 */
quint16 MCPD8::capabilities(const bool cached)
{
    if (!cached)
    {
	if (!m_capabilities)
	{
            if (m_mdll.isEmpty())
            {
                MSG_DEBUG << tr("GETCAPABILITIES %1").arg(m_byId);
#if 0
		m_capabilities = readRegister(102);
		m_txMode = readRegister(103);
#else
                QMutexLocker locker(m_pCommandMutex);
                initCmdBuffer(GETCAPABILITIES);
                finishCmdBuffer(0);
                sendCommand(false);
#endif
            }
            else
                m_capabilities = TPA;
	}
    }
    return m_capabilities;
}

/*!
    \fn MCPD8::capabilities(quint8 mod)

    read out the capabilities of the MPSD with number mod

    \param mod number of the MPSD
    \return capabilities register of the MPSD
 */
quint16 MCPD8::capabilities(quint8 mod)
{
    if (m_mpsd.find(mod) != m_mpsd.end())
        return m_mpsd[mod]->capabilities();
    return 0;
}

/*!
    \fn float MCPD8::version(void)
    \return firmware version of the MCPD whereas the integral places represents the major number
            and the decimal parts the minor number
 */
float MCPD8::version(void)
{
    return m_version;
}

/*!
    \fn float MCPD8::fpgaVersion(void)
    \return version of the FPGA inside the MCPD whereas the integral places represents the major
            number and the decimal parts the minor number
 */
float MCPD8::fpgaVersion(void)
{
    return m_fpgaVersion;
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
        tmpFloat = m_mpsd[mod]->version();
    return tmpFloat;
}

/*!
    \fn MCPD8::readId(void)

    reads the ID's of all connected MPSD-8/8+ and MSTD-16

    \return true if operation was succesful or not
    \see getModuleId
 */
bool MCPD8::readId(void)
{
    MSG_DEBUG << tr("READID %1").arg(m_byId);
    QMutexLocker locker(m_pCommandMutex);
    initCmdBuffer(READID);
    m_cmdBuf.data[0] = 2;
    finishCmdBuffer(1);
    memset(m_awReadId, 0, sizeof(m_awReadId));
    if (!sendCommand(false))
        return false;

#if defined(_MSC_VER)
#   pragma message("TODO if the configuration has changed")
#else
#   warning TODO if the configuration has changed
#endif
    //! \todo if the configuration has changed
    if (m_awReadId[0] == TYPE_MDLL || m_awReadId[0] == TYPE_MWPCHR)
    {
        QMap<int, M2D *>::iterator it = m_mdll.find(0);
        if (it == m_mdll.end())
        {
            MSG_DEBUG << tr("new MDLL detected");
            m_mdll[0] = M2D::create(0, m_awReadId[0], this);
        }
        else if ((*it)->type() != m_awReadId[0])
        {
            MSG_WARNING << tr("renewing MDLL");
            // delete m_mpsd[c];
            m_mdll.remove(0);
            if (m_awReadId[0] != 0)
                m_mdll[0] = M2D::create(0, m_awReadId[0], this);
        }
    }
    else
    {
        for (quint8 c = 0; c < 8; ++c)
        {
            QMap<int, MPSD8 *>::iterator it = m_mpsd.find(c);
            if (it == m_mpsd.end())
            {
                m_mpsd[c] = MPSD8::create(c, m_awReadId[c], this);
            }
            else if ((*it)->type() != m_awReadId[c])
            {
                // delete m_mpsd[c];
                m_mpsd.remove(c);
                if (m_awReadId[c] != 0)
                    m_mpsd[c] = MPSD8::create(c, m_awReadId[c], this);
            }
        }
    }
    return true;
}

/*!
    \fn MCPD8::scanPeriph(void)

    scans the peripherial module (all connected MPSD-8/8+ and MSTD-16) and their properties

    \return true if operation was succesful or not
    \see getModuleId, readId
 */
bool MCPD8::scanPeriph(void)
{
// get MCPD version
    MSG_DEBUG << tr("GETVER %1").arg(m_byId);
    m_pCommandMutex->lock();
    initCmdBuffer(GETVER);
    finishCmdBuffer(0);
    if(!sendCommand(false))
        m_version = -1.0;
    m_pCommandMutex->unlock();
    MSG_DEBUG << tr("GETVER %1: %2").arg(m_byId).arg(m_version);
    if (m_version < 0.0)
        return false;
    if (m_bTestOnly)
        return true;

// check the MCPD capabilities
    m_capabilities = capabilities(false);
// check the peripherial modules
    MSG_DEBUG << tr("GETCAPABILITIES %1: %2").arg(m_byId).arg(m_capabilities);
    if (m_iErrorCounter >= MCPD8_MAX_ERRORCOUNT)
        return false;
    // check the peripherial modules
    if (readId())
    {
        MSG_DEBUG << tr("READID %1").arg(m_byId);
        // get the Version for each connected module
        for (int mod = 0; mod < 8; ++mod)
            if (m_mpsd.find(mod) != m_mpsd.end())
            {
                quint16 tmp = readPeriReg(mod, 2);
                float tmpFloat = ((tmp > 4) & 0xF) /* * 10 */ + (tmp & 0xF);
                tmpFloat /= 100.;
                tmpFloat += (tmp >> 8);
                m_mpsd[mod]->setVersion(tmpFloat);
                MSG_INFO << tr("Module (ID %1): Version number : %2").arg(mod).arg(tmpFloat, 0, 'f', 2);
		switch (m_mpsd[mod]->type())
		{
			case TYPE_MPSD8P:
			case TYPE_MPSD8SADC:
                            m_mpsd[mod]->setCapabilities(readPeriReg(mod, 0));
			    break;
                        case TYPE_NOMODULE:
                            m_mpsd[mod]->setCapabilities(0);
                            break;
			default:
		            m_mpsd[mod]->setCapabilities(P);
                }
            }
        return true;
    }
    return false;
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
	if (chan > m_mpsd[addr]->getChannels())
		chan = m_mpsd[addr]->getChannels();
	// The old MSTD-16 could only set the gain for two channels simultaneously
	float ver = version() + 0.005; // uncertainty of float
	if (m_mpsd[addr]->type() == TYPE_MSTD16 && ver < 9.9)
		if (chan > 7)
			chan /= 2;

	MSG_DEBUG << tr("SETGAIN_%1 %2, addr %3, chan %4, val %5")
				.arg(m_mpsd[addr]->getType())
				.arg(m_byId).arg(addr).arg(chan).arg(gainval);
	QMutexLocker locker(m_pCommandMutex);
	m_mpsd[addr]->setGain(chan, gainval, 1);
	quint8 cmd = (m_mpsd[addr]->type() == TYPE_MSTD16 && ver >= 9.9) ? SETGAIN_MSTD : SETGAIN_MPSD;
	MSG_INFO << tr("Use setGain command: %1").arg(cmd);
	initCmdBuffer(cmd);
	m_cmdBuf.data[0] = addr;
	m_cmdBuf.data[1] = chan;
	m_cmdBuf.data[2] = gainval;
	MSG_INFO << tr("set gain to potival: %1").arg(m_cmdBuf.data[2]);
	finishCmdBuffer(3);
	return sendCommand(true);
}

/*!
	\fn MCPD8::getGainPoti(quint16 addr,  quint8 chan)

	gets the currently set gain value for a special module and channel

	if the channel number is greater 7 or 15 than all channels of the module
	will be set (MSTD-16 has 16 channels, others have 8)

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
		if (chan > m_mpsd[addr]->getChannels())
			chan = m_mpsd[addr]->getChannels();
		return m_mpsd[addr]->getGainpoti(chan, 0);
	}
	return 0;
}

/*!
	\fn float MCPD8::getGainVal(quint16 addr,  quint8 chan)

	gets the currently set gain value for a special module and channel

	if the channel number is greater 7 or 15 than all channels of the module
	will be set (MSTD-16 has 16 channels, others have 8)

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
		if (chan > m_mpsd[addr]->getChannels())
			chan = m_mpsd[addr]->getChannels();
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
        MSG_DEBUG << tr("SETTHRESH %1, addr %2, thres %3").arg(m_byId).arg(addr).arg(thresh);
        QMutexLocker locker(m_pCommandMutex);
        m_mpsd[addr]->setThreshold(thresh, 1);
        initCmdBuffer(SETTHRESH);
        m_cmdBuf.data[0] = addr;
        m_cmdBuf.data[1] = thresh;
        finishCmdBuffer(2);
        return sendCommand(true);
    }
    return false;
}

/*!
    \fn bool MCPD8::setMdllThresholds(quint8 threshX, quint8 threshY, quint8 threshA)

    set the threshold value as poti value

    \param threshX threshold for the X CFD
    \param threshY threshold for the Y CFD
    \param threshA threshold for the A CFD
    \return true if operation was succesful or not
    \see getThresh
 */
bool MCPD8::setMdllThresholds(quint8 threshX, quint8 threshY, quint8 threshA)
{
	if (m_mdll[0]->type() == TYPE_MDLL)
	{
		MSG_DEBUG << tr("SETMDLLTHRESHS %1").arg(m_byId);
		QMutexLocker locker(m_pCommandMutex);
		reinterpret_cast<MDLL *>(m_mdll[0])->setThresholds(threshX, threshY, threshA, 1);
		initCmdBuffer(SETMDLLTHRESHS);
		m_cmdBuf.data[0] = threshX;
		m_cmdBuf.data[1] = threshY;
		m_cmdBuf.data[2] = threshA;
		finishCmdBuffer(3);
		return sendCommand(true);
	}
	return true;
}

/*!
    \fn bool MCPD8::setMdllSpectrum(quint8 shiftX, quint8 shiftY, quint8 scaleX, quint8 scaleY)

    \param shiftX
    \param shiftY
    \param scaleX
    \param scaleY
    \return true if operation was succesful or not
 */
bool MCPD8::setMdllSpectrum(quint8 shiftX, quint8 shiftY, quint8 scaleX, quint8 scaleY)
{
	if (m_mdll[0]->type() == TYPE_MDLL)
	{
		MSG_DEBUG << tr("SETMDLLSPECTRUM %1").arg(m_byId);
		QMutexLocker locker(m_pCommandMutex);
		reinterpret_cast<MDLL *>(m_mdll[0])->setSpectrum(shiftX, shiftY, scaleX, scaleY, 1);
		initCmdBuffer(SETMDLLSPECTRUM);
		m_cmdBuf.data[0] = shiftX;
		m_cmdBuf.data[1] = shiftY;
		m_cmdBuf.data[2] = scaleX;
		m_cmdBuf.data[3] = scaleY;
		finishCmdBuffer(4);
		return sendCommand(true);
	}
	return true;
}

/*!
    \fn bool MCPD8::setMdllDataset(quint8 set)

    \param set
    \return true if operation was succesful or not
 */
bool MCPD8::setMdllDataset(quint8 set)
{
	if (m_mdll[0]->type() == TYPE_MDLL)
	{
		MSG_DEBUG << tr("SETMDLLDATASET %1").arg(m_byId);
		QMutexLocker locker(m_pCommandMutex);
		reinterpret_cast<MDLL *>(m_mdll[0])->setDataset(set, true);
		initCmdBuffer(SETMDLLDATASET);
		m_cmdBuf.data[0] = set;
		finishCmdBuffer(1);
		return sendCommand(true);
	}
	return true;
}

/*!
    \fn bool MCPD8::setMdllTimingWindow(quint16 xlo, quint16 xhi, quint16 ylo, quint16 yhi)

    \param xlo
    \param xhi
    \param ylo
    \param yhi
    \return true if operation was succesful or not
 */
bool MCPD8::setMdllTimingWindow(quint16 xlo, quint16 xhi, quint16 ylo, quint16 yhi)
{
	if (m_mdll[0]->type() == TYPE_MDLL)
	{
		MSG_DEBUG << tr("SETMDLLACQSET %1").arg(m_byId);
		QMutexLocker locker(m_pCommandMutex);
		reinterpret_cast<MDLL *>(m_mdll[0])->setTimingWindow(xlo, xhi, ylo, yhi, 1);
		initCmdBuffer(SETMDLLACQSET);
		m_cmdBuf.data[0] = 0;
		m_cmdBuf.data[1] = 0;
		m_cmdBuf.data[2] = xlo;
		m_cmdBuf.data[3] = xhi;
		m_cmdBuf.data[4] = ylo;
		m_cmdBuf.data[5] = yhi;
		finishCmdBuffer(6);
		return sendCommand(true);
	}
	return true;
}

/*!
    \fn bool MCPD8::setMdllEnergyWindow(quint8 elo, quint8 ehi)

    sets the energy window

    \param elo lower limit of the energy range
    \param ehi upper limit of the energy range
    \return true if operation was succesful or not
 */
bool MCPD8::setMdllEnergyWindow(quint8 elo, quint8 ehi)
{
	if (m_mdll[0]->type() == TYPE_MDLL)
	{
		MSG_DEBUG << tr("SETMDLLEWINDOW %1").arg(m_byId);
		QMutexLocker locker(m_pCommandMutex);
		reinterpret_cast<MDLL *>(m_mdll[0])->setEnergyWindow(elo, ehi, 1);
		initCmdBuffer(SETMDLLEWINDOW);
		m_cmdBuf.data[0] = elo;
		m_cmdBuf.data[1] = ehi;
		m_cmdBuf.data[2] = 0;
		m_cmdBuf.data[3] = 0;
		finishCmdBuffer(4);
		return sendCommand(true);
	}
	return true;
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
    \fn quint8 MCPD8::getMdllThreshold(quint8 val)

    \param val
    \return the threshold value
 */
quint8 MCPD8::getMdllThreshold(quint8 val)
{
	if (m_mdll.find(0) != m_mdll.end() && m_mdll[0]->type() == TYPE_MDLL)
		return reinterpret_cast<MDLL *>(m_mdll[0])->getThreshold(val);
	return 0;
}

/*!
    \fn quint8 MCPD8::getMdllSpectrum(quint8 val)

    \param val
    \return the spectrum
 */
quint8 MCPD8::getMdllSpectrum(quint8 val)
{
	if (m_mdll.find(0) != m_mdll.end() && m_mdll[0]->type() == TYPE_MDLL)
		return reinterpret_cast<MDLL *>(m_mdll[0])->getSpectrum(val);
	return 0;
}

/*!
    \fn quint16 MCPD8::getMdllTimingWindow(quint8 val)

    \param val
    \return timing window
 */
quint16 MCPD8::getMdllTimingWindow(quint8 val)
{
	if (m_mdll.find(0) != m_mdll.end() && m_mdll[0]->type() == TYPE_MDLL)
		return reinterpret_cast<MDLL *>(m_mdll[0])->getTimingWindow(val);
	return 0;
}

/*!
    \fn quint8 MCPD8::getMdllEnergyWindow(quint8 val)

    \param val
    \return energy window
 */
quint8 MCPD8::getMdllEnergyWindow(quint8 val)
{
	if (m_mdll.find(0) != m_mdll.end() && m_mdll[0]->type() == TYPE_MDLL)
		return reinterpret_cast<MDLL *>(m_mdll[0])->getEnergyWindow(val);
	return 0;
}

/*!
    \fn quint8 MCPD8::getMdllDataset(void)

    \return the type of data set
 */
quint8 MCPD8::getMdllDataset(void)
{
	if (m_mdll.find(0) != m_mdll.end() && m_mdll[0]->type() == TYPE_MDLL)
		return reinterpret_cast<MDLL *>(m_mdll[0])->getDataset();
	return 0;
}

/*!
    \fn quint8 MCPD8::getMdllPulser(quint8 val)

    \param val
    \return
 */
quint8 MCPD8::getMdllPulser(quint8 val)
{
    if (m_mdll.find(0) != m_mdll.end() && m_mdll[0]->type() == TYPE_MDLL)
    {
        switch(val)
        {
            case 0:
                if(reinterpret_cast<MDLL *>(m_mdll[0])->isPulserOn())
                    return 1;
                else
                    return 0;
                break;
            case 1:
                return reinterpret_cast<MDLL *>(m_mdll[0])->getPulsAmp();
                break;
            case 2:
                return reinterpret_cast<MDLL *>(m_mdll[0])->getPulsPos();
                break;
        }
    }
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
    MSG_DEBUG << tr("SETMODE %1").arg(m_byId);
    QMutexLocker locker(m_pCommandMutex);
    if (addr >= 8)
    {
	bool ret(true);
	//! \todo current firmware does not support addr 8 to set all
        for (int i = 0; ret && i < 8; ++i)
	{
            if (!m_mpsd.contains(i))
                return false;
            m_mpsd[i]->setMode(mode, 1);
            initCmdBuffer(SETMODE);
            m_cmdBuf.data[0] = i;
            m_cmdBuf.data[1] = mode;
            finishCmdBuffer(2);
            ret &= sendCommand(false);
	}
	return ret;
    }

    if (!m_mpsd.contains(addr))
        return false;
    m_mpsd[addr]->setMode(mode, 1);
    initCmdBuffer(SETMODE);
    m_cmdBuf.data[0] = addr;
    m_cmdBuf.data[1] = mode;
    finishCmdBuffer(2);
    return sendCommand(false);
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
    MSG_INFO << tr("MCPD8::setPulser(addr = %1, chan = %2, pos = %3, amp = %4, onoff = %5").arg(addr).arg(chan).arg(pos).arg(amp).arg(onoff);

    if (pos > 2)
        pos = 2;
    MSG_DEBUG << tr("SETPULSER %1").arg(m_byId);
    QMutexLocker locker(m_pCommandMutex);
    if (addr == 0 && getModuleId(addr) == TYPE_MDLL)
    {
        if (amp > 3)
            amp = 3;
        m_cmdBuf.data[1] = amp;
        reinterpret_cast<MDLL *>(m_mdll[0])->setPulser(pos, amp, onoff, 1);
        initCmdBuffer(SETMDLLPULSER);
        m_cmdBuf.data[0] = onoff;
        m_cmdBuf.data[2] = pos;
        finishCmdBuffer(3);
    }
    else if (getModuleId(addr) != TYPE_MWPCHR)
    {
        if (m_mpsd.find(addr) == m_mpsd.end())
            return false;
        if (addr > 7)
            addr = 7;
        if (chan > m_mpsd[addr]->getChannels())
            chan = m_mpsd[addr]->getChannels();
        if (chan == m_mpsd[addr]->getChannels())
        {
#if defined(_MSC_VER)
#			pragma message("TODO common pulser handling")
#else
#			warning TODO common pulser handling
#endif
//! \todo common pulser handling
            for (int i = 0; i < chan; ++i)
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
    }
    return sendCommand(true);
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
    MSG_INFO << tr("MCPD8::setAuxTimer(%1, %2)").arg(tim).arg(val);
    if(tim > 3)
        tim = 3;
    MSG_DEBUG << tr("SETAUXTIMER %1").arg(m_byId);
    QMutexLocker locker(m_pCommandMutex);
    initCmdBuffer(SETAUXTIMER);
    m_cmdBuf.data[0] = tim;
    m_cmdBuf.data[1] = val;
    m_auxTimer[tim] = val;
    finishCmdBuffer(2);
    return sendCommand(false);
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
    if (source > 7)
    {
        MSG_ERROR << tr("Error: mcpd %1: trying to set counter cell #%2. Range exceeded! Max. cell# is 7").arg(m_byId).arg(source);
        errorflag = false;
    }
    if (errorflag && trigger > 7)
    {
        MSG_ERROR << tr("Error: mcpd %1: trying to set counter cell trigger # to %2. Range exceeded! Max. trigger# is 7").arg(m_byId).arg(trigger);
        errorflag = false;
    }
    if (errorflag && compare > 22)
    {
        MSG_ERROR << tr("Error: mcpd %1: trying to set counter cell compare value to %2. Range exceeded! Max. value is 22").arg(m_byId).arg(compare);
        errorflag = false;
    }
    if (errorflag)
    {
        MSG_NOTICE << tr("mcpd %1: set counter cell %2: trigger # is %3, compare value %4.").arg(m_byId).arg(source).arg(trigger).arg(compare);

        MSG_DEBUG << tr("SETCELL %1").arg(m_byId);
        QMutexLocker locker(m_pCommandMutex);
        initCmdBuffer(SETCELL);
        m_cmdBuf.data[0] = source;
        m_cmdBuf.data[1] = trigger;
        m_cmdBuf.data[2] = compare;
        finishCmdBuffer(3);
        if (sendCommand(true))
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
    MSG_NOTICE << tr("set parameter source %1 to %2").arg(param).arg(source);
    if(param > 3 || source > 8)
        return false;
    MSG_DEBUG << tr("SETPARAM %1").arg(m_byId);
    QMutexLocker locker(m_pCommandMutex);
    m_paramSource[param] = source;
    initCmdBuffer(SETPARAM);
    m_cmdBuf.data[0] = param;
    m_cmdBuf.data[1] = source;
    finishCmdBuffer(2);
    return sendCommand(true);
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
    MSG_DEBUG << tr("GETPARAM %1").arg(m_byId);
    QMutexLocker locker(m_pCommandMutex);
    initCmdBuffer(GETPARAM);
    finishCmdBuffer(0);
    return sendCommand(false);
}

/*!
    \fn bool MCPD8::setProtocol(const QString &addr, const QString &datasink, const quint16 dataport, const QString& cmdsink, const quint16 cmdport)

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

    MSG_DEBUG << tr("SETPROTOCOL %1").arg(m_byId);
    QMutexLocker locker(m_pCommandMutex);
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
        m_szMcpdDataIp = datasink;
        MSG_NOTICE << tr("mcpd #%1: data ip address set to %2").arg(m_byId).arg(m_szMcpdDataIp);
    }

// UDP port of command receiver
    m_cmdBuf.data[8] = cmdport;
    if (cmdport > 0)
    {
        m_wPort = cmdport;
        MSG_NOTICE << tr("mcpd #%1: cmd port set to %2").arg(m_byId).arg(m_wPort);
    }

// UDP port of data receiver
    m_cmdBuf.data[9] = dataport;
    if (dataport > 0)
    {
        m_wDataPort = dataport;
        MSG_NOTICE << tr("mcpd #%1: data port set to %2").arg(m_byId).arg(m_wDataPort);
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
        m_szMcpdIp = cmdsink;
        MSG_NOTICE << tr("mcpd #%1: cmd ip address set to %2").arg(m_byId).arg(m_szMcpdIp);
    }
    finishCmdBuffer(14);
    if (sendCommand(false))
    {
        sleep(1);
        if (ownip > 0x00FFFFFF)
        {
#if defined(_MSC_VER)
#   pragma message("m_ownIpAddress = addr;")
#   pragma message(MSG_NOTICE << tr("mcpd #%1: ip address set to %2").arg(m_byId).arg(m_ownIpAddress);)
#else
#   warning m_ownIpAddress = addr;
#   warning MSG_NOTICE << tr("mcpd #%1: ip address set to %2").arg(m_byId).arg(m_ownIpAddress);
#endif
        }
        return true;
    }
    return false;
}

/*!
    \fn void MCPD8::getProtocol(QString& ip, QString &cmdip, quint16& cmdport, QString& dataip, quint16& dataport) const

    \see setProtocol
 */
void MCPD8::getProtocol(QString &ip, QString &cmdip, quint16 &cmdport, QString &dataip, quint16 &dataport) const
{
    ip = m_szMcpdIp;
    cmdip = m_szMcpdIp;
    cmdport = m_wPort;
    dataip = m_szMcpdDataIp;
    dataport = m_wDataPort;
}

/*!
    \fn void MCPD8::getProtocol(quint16 *addr)

    \param addr ????
    \see setProtocol
 */
void MCPD8::getProtocol(quint16 *addr)
{
    quint32 cmdIP = QHostAddress(m_szMcpdIp).toIPv4Address();
    quint32 ownIP = QHostAddress(m_szMcpdIp).toIPv4Address();
    quint32 dataIP = QHostAddress(m_szMcpdDataIp).toIPv4Address();

    for (quint8 c = 0; c < 4; c++)
    {
        quint8 shift = ((3 - c) * 8);
        addr[c] = (cmdIP >> shift) & 0xFF;
        addr[c + 4] = (dataIP >> shift) & 0xFF;
        addr[c + 10] = (ownIP >> shift) & 0xFF;
    }
    addr[8] = m_wPort;
    addr[9] = m_wDataPort;
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
    \fn MCPD8::setRunId(quint32 runid)

    sets the run ID of the measurement

    \param runid the new run ID
    \return true if operation was succesful or not
    \see getRunId
 */
bool MCPD8::setRunId(quint32 runid)
{
    bool bResult = false;
    if(m_master)
    {
        MSG_DEBUG << tr("SETRUNID %1").arg(m_byId);
        QMutexLocker locker(m_pCommandMutex);
        initCmdBuffer(SETRUNID);
        m_cmdBuf.data[0] = (quint16)(runid & 0xFFFF);
        finishCmdBuffer(1);
        MSG_NOTICE << tr("mcpd %1: set run ID to %2").arg(m_byId).arg(m_cmdBuf.data[0]);
        m_runId = runid;
        bResult = sendCommand(false);
        MSG_DEBUG << tr("SETRUNID %1, result %2").arg(m_byId).arg(bResult);
    }
    else
        MSG_ERROR << tr("Error: trying to set run ID on mcpd %1 - not master!").arg(m_byId);
    return bResult;
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
    MSG_DEBUG << tr("Set parameter %1 to %2").arg(param).arg(val);
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

    for (c = 0; c < 4; c++)
    {
        m_counterCell[c][0] = 7;
        m_counterCell[c][1] = 22;
    }

    for (c = 4; c < 7; c++)
    {
        m_counterCell[c][0] = 0;
        m_counterCell[c][1] = 0;
    }

    for (c = 0; c < 4; c++)
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
#if defined(_MSC_VER)
#	pragma message("TODO MCPD8::setStream(quint16 strm)")
#else
#	warning TODO MCPD8::setStream(quint16 strm)
#endif
//! \todo implement me
#if 0
    MSG_DEBUG << tr("QUIET %1").arg(m_byId);
    QMutexLocker locker(m_pCommandMutex);
    unsigned short id = (unsigned short) deviceId->value();
    initCmdBuffer(QUIET);
    m_cmdBuf.data[0] = strm;
    finishCmdBuffer(1);
    MSG_NOTICE << tr("Set stream %1").arg(strm);
    return sendCommand(false);
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
    m_cmdBuf.deviceId = m_byId;
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
    m_cmdBuf.bufferNumber = m_txCmdBufNum++;
    m_cmdBuf.bufferLength = CMDHEADLEN + buflen;
    m_cmdBuf.data[buflen] = 0xFFFF;
    m_cmdBuf.headerChksum = 0;
    m_cmdBuf.headerChksum = calcChksum(m_cmdBuf);
}

/*!
    \fn int MCPD8::sendCommand(bool bCheckAnswer)

    sends the command buffer to the MCPD

    \return
 */
bool MCPD8::sendCommand(bool bCheckAnswer)
{
    bool bOK(false);
    bool bTimeout(false);
    int iTryCount(0);
    m_pCommunicationMutex->lock();
tryagain:
    if (m_pNetwork->sendBuffer(m_szMcpdIp, m_wPort, m_cmdBuf))
    {
#if QT_VERSION >= 0x040700
        quint64 qwStart = QDateTime::currentMSecsSinceEpoch();
#else
        quint64 qwStart = quint64(QDateTime::currentDateTime().toTime_t()) * 1000;
#endif
        MSG_INFO << tr("%1(%2) : %3. sent cmd: %4 to id: %5").
		arg(m_pNetwork->ip()).arg(m_pNetwork->port()).arg(m_cmdBuf.bufferNumber).arg(m_cmdBuf.cmd).arg(m_cmdBuf.deviceId);
// block other commands due to the writing on flash to avoid crashes on MCPD-8
        if (m_cmdBuf.cmd == SETPROTOCOL)
            sleep(3);
        bOK = true;
        m_iCommActive = SEND;
        m_pCommunicationMutex->unlock();
        forever // wait for answer
        {
            usleep(1000);
            if (m_iCommActive == RECV || m_iCommActive == RECV_INVALID)
                break;
            // 5sec timeout: block other commands due to the writing on flash to avoid crashes on MCPD-8
#if QT_VERSION >= 0x040700
            if (quint64(QDateTime::currentMSecsSinceEpoch()-qwStart) > ((m_cmdBuf.cmd == SETPROTOCOL) ? 5000 : 500))
#else
            quint64 tmp = quint64(QDateTime::currentDateTime().toTime_t()) * 1000;
            if (quint64(tmp - qwStart) > ((m_cmdBuf.cmd == SETPROTOCOL) ? 5000 : 500))
#endif
            {
                bOK = false;
                bTimeout = true;
                break;
            }
        }
        m_pCommunicationMutex->lock();
        m_cmdTxd++;
        if (bCheckAnswer || bTimeout)
        {
            switch (m_iCommActive)
            {
            case RECV_INVALID: // command was not correctly processed
                m_iErrorCounter = 0;
                if ((++iTryCount) < MCPD8_TRY_SETCMD)
                    goto tryagain;
                bOK = false;
                break;
            case SEND: // timeout
                if (m_iErrorCounter < MCPD8_MAX_ERRORCOUNT)
                {
                    ++m_iErrorCounter;
                    goto tryagain;
                }
                break;
            default: // normal
                m_iErrorCounter = 0;
                bOK = true;
                break;
            }
        }
        m_iCommActive = RECV;
        m_pCommunicationMutex->unlock();
        if (!bOK)
        {
            const char* szCmd;
            switch (m_cmdBuf.cmd)
            {
#define CMD(x) case x: szCmd=#x; break
            CMD(RESET); CMD(START); CMD(STOP); CMD(CONTINUE); CMD(SETID);
            CMD(SETPROTOCOL); CMD(SETTIMING); CMD(SETCLOCK); CMD(SETRUNID);
            CMD(SETCELL); CMD(SETAUXTIMER); CMD(SETPARAM); CMD(GETPARAM);
            CMD(SETGAIN_MPSD); CMD(SETTHRESH); CMD(SETPULSER); CMD(SETMODE);
            CMD(SETDAC); CMD(SENDSERIAL); CMD(READSERIAL); CMD(SCANPERI);
            CMD(GETCAPABILITIES); CMD(SETCAPABILITIES); CMD(WRITEFPGA);
            CMD(WRITEREGISTER); CMD(READREGISTER); CMD(READFPGA); CMD(SETPOTI);
            CMD(GETPOTI); CMD(READID); CMD(DATAREQUEST); CMD(QUIET);
            CMD(GETVER); CMD(READPERIREG); CMD(WRITEPERIREG); CMD(SETMDLLTHRESHS);
            CMD(SETMDLLSPECTRUM); CMD(SETMDLLPULSER); CMD(SETMDLLDATASET);
            CMD(SETMDLLACQSET); CMD(SETMDLLEWINDOW); CMD(SETGAIN_MSTD);
#undef CMD
            default: szCmd="???"; break;
            }

            if (bTimeout)
                MSG_ERROR << tr("T I M E O U T : timeout while waiting for cmd %1/%2 answer from ID: %3").arg(m_cmdBuf.cmd).arg(szCmd).arg(m_byId);
            else
                MSG_ERROR << tr("E R R O R : wrong cmd %1/%2 answer from ID: %3").arg(m_cmdBuf.cmd).arg(szCmd).arg(m_byId);
            switch (m_cmdBuf.cmd)
            {
            default:
                break;
            case START:
                emit startedDaq();
                break;
            case STOP:
                emit stoppedDaq();
                break;
            case CONTINUE:
                emit continuedDaq();
                break;
            }
        }
    }
    else
        m_pCommunicationMutex->unlock();
    return bOK;
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
    \fn MCPD8::calcChksum(const QSharedDataPointer<SD_PACKET> &buffer)

    calculates the checksum of the buffer

    \param buffer
    \return calculated checksum
 */
quint16 MCPD8::calcChksum(const QSharedDataPointer<SD_PACKET> &buffer)
{
    quint16 chksum = buffer->mdp.headerChksum;
    const quint16 *p = reinterpret_cast<const quint16 *>(&buffer->mdp);
    for (quint32 i = 0; i < buffer->mdp.bufferLength; i++)
        chksum ^= p[i];
    return chksum;
}

/*!
    \fn bool MCPD8::analyzeBuffer(QSharedDataPointer<SD_PACKET> pPacket)

    analyze the data package coming from the MCPD-8

    \param pPacket command/data package
 */
bool MCPD8::analyzeBuffer(QSharedDataPointer<SD_PACKET> pPacket)
{
    const MDP_PACKET *pMdp = &pPacket.constData()->mdp;
    bool bAnswerOk(true);
    if (pMdp->deviceId != m_byId)
    {
        MSG_WARNING << tr("deviceId : %1 <-> %2").arg(pMdp->deviceId).arg(m_byId);
        return false;
    }

    if(pMdp->bufferType & CMDBUFTYPE)
    {
        quint16 diff = pMdp->bufferNumber - m_lastCmdBufnum;
        if(diff > 1 && pMdp->bufferNumber > 0 && m_lastCmdBufnum > 0)
             MSG_ERROR << tr("%1(%2) %3: Lost %4 command buffers: current: %5, last: %6").
		arg(m_pNetwork->ip()).arg(m_pNetwork->port()).arg(getId()).arg(diff - 1).arg(pMdp->bufferNumber).arg(m_lastCmdBufnum);
        m_lastCmdBufnum = pMdp->bufferNumber;

        ++m_cmdRxd;
//	MSG_DEBUG << tr("%1(%2) : id %3").arg(m_pNetwork->ip()).arg(m_pNetwork->port()).arg(pMdp->deviceId);

        m_headertime = pMdp->time[0] + (quint64(pMdp->time[1]) << 16) + (quint64(pMdp->time[2]) << 32);
        m_timemsec = (m_headertime / 10000); // headertime is in 100ns steps
//	MSG_DEBUG << tr("MCPD8::analyzeBuffer(QSharedDataPointer<SD_MDP_PACKET> pPacket) 0x%1 : %2").arg(pMdp->bufferType, 0, 16).arg(pMdp->cmd);

	emit headerTimeChanged(m_headertime);

        MPSD8	*ptrMPSD;
        MDLL    *ptrMDLL;

        switch (pMdp->cmd)
        {
            case RESET:
                MSG_WARNING << tr("not handled command : RESET");
                break;
            case START:
                emit startedDaq();
                break;
            case STOP:
                emit stoppedDaq();
                break;
            case CONTINUE:
                emit continuedDaq();
                break;
            case SETID:
		if (pMdp->cmd & 0x80)
		{
			bAnswerOk = false;
			MSG_ERROR << tr("SETID : failed");
		}
		else
			MSG_NOTICE << tr("SETID = %1").arg(pMdp->data[0]);
                break;
            case SETPROTOCOL:
                // extract ip and eth addresses in case of "this pc"
                break;
            case SETTIMING:
		if (pMdp->cmd & 0x80)
		{
			bAnswerOk = false;
			MSG_ERROR << tr("SETTIMING : failed");
		}
		else
			MSG_INFO << tr("SETTIMING : master %1 terminate %2").arg(pMdp->data[0]).arg(pMdp->data[1]);
                break;
            case SETCLOCK:
                MSG_WARNING << tr("not handled command : SETCLOCK");
                break;
            case SETRUNID:
                MSG_WARNING << tr("not handled command : SETRUNID");
                break;
            case SETCELL:
		if (pMdp->cmd & 0x80)
		{
			bAnswerOk = false;
			MSG_ERROR << tr("SETCELL : failed");
		}
		else
			MSG_INFO << tr(": SETCELL");
                break;
            case SETAUXTIMER:
		if (pMdp->data[1] != m_auxTimer[pMdp->data[0]])
		{
			MSG_ERROR << tr("Error setting auxiliary timer, tim %1, is: %2, should be %3").
				arg(pMdp->data[0]).arg(pMdp->data[1]).arg(m_auxTimer[pMdp->data[0]]);
		}
                break;
            case SETPARAM:
		if (pMdp->cmd & 0x80)
		{
			bAnswerOk = false;
			MSG_ERROR << tr("SETPARAM : failed");
		}
		else
			MSG_INFO << tr("SETPARAM");
                break;
            case GETPARAM:
                MSG_WARNING << tr("not handled command : GETPARAM");
                {
			quint64 val = pMdp->data[9] + (quint64(pMdp->data[10]) << 16) + (quint64(pMdp->data[11]) << 32);
			setParameter(0, val);
			val = pMdp->data[12] + (quint64(pMdp->data[13]) << 16) + (quint64(pMdp->data[14]) << 32);
			setParameter(1, val);
			val = pMdp->data[15] + (quint64(pMdp->data[16]) << 16) + (quint64(pMdp->data[17]) << 32);
			setParameter(2, val);
			val = pMdp->data[18] + (quint64(pMdp->data[19]) << 16) + (quint64(pMdp->data[20]) << 32);
			setParameter(3, val);
                }
                break;
            case SETGAIN_MPSD: // extract the set gain values:
            case SETGAIN_MSTD:
                if (pMdp->bufferLength == 21) // set common gain
		{
                        for(quint8 c = 0; c < 8; c++)
			{
				ptrMPSD = m_mpsd[pMdp->data[0]];
				if(pMdp->data[2 + c] != ptrMPSD->getGainpoti(c, 1))
				{
					bAnswerOk = false;
					MSG_ERROR << tr("Error setting gain, mod %1, chan %2 is: %3, should be: %4").
						arg(8 * pMdp->deviceId + pMdp->data[0]).arg(c).arg(pMdp->data[2 + c]).arg(ptrMPSD->getGainpoti(c, 1));
					// set back to received value
					ptrMPSD->setGain(ptrMPSD->getChannels(), (quint8)pMdp->data[c + 2], 0);
				}
			}
				ptrMPSD->setGain(ptrMPSD->getChannels(), (quint8)pMdp->data[2], 0);
		}
		else // set one channel
		{
			// If there was a gain set to module which do not answer right, the data area is empty and this would lead to NULL pointers
			if (pMdp->bufferLength > pMdp->headerLength + 1)
			{
				ptrMPSD = m_mpsd[pMdp->data[0]];
				if(pMdp->data[2] != ptrMPSD->getGainpoti(pMdp->data[1], 1))
				{
					bAnswerOk = false;
					MSG_ERROR << tr("Error setting gain, mod %1, chan %2 is: %3, should be: %4").
						arg(8 * pMdp->deviceId + pMdp->data[0]).arg(pMdp->data[1]).arg(pMdp->data[2]).arg(ptrMPSD->getGainpoti(pMdp->data[1], 1));
					// set back to received value
				}
				ptrMPSD->setGain(pMdp->data[1], (quint8)pMdp->data[2], 0);
			}
			else
			{
				bAnswerOk = false;
				MSG_ERROR << tr("Error setting gain, perhaps an MSTD-16 module");
			}
		}
		break;
            case SETTHRESH: // extract the set thresh value:
		if (pMdp->bufferLength > pMdp->headerLength + 1)
		{
			ptrMPSD = m_mpsd[pMdp->data[0]];
			if (pMdp->data[1] != ptrMPSD->getThreshold(1))
			{
				bAnswerOk = false;
				MSG_ERROR << tr("Error setting threshold, mod %1, is: %2, should be: %3").
					arg(8 * pMdp->deviceId + pMdp->data[0]).arg(pMdp->data[1]).arg(ptrMPSD->getThreshold(1));
			}
			ptrMPSD->setThreshold(pMdp->data[1], 0);
		}
		else
		{
			bAnswerOk = false;
			MSG_ERROR << tr("Error setting threshold, answer from hardware incomplete");
		}
		break;
            case SETPULSER:
		ptrMPSD = m_mpsd[pMdp->data[0]];
		if(pMdp->data[3] != ptrMPSD->getPulsPoti(1))
		{
			bAnswerOk = false;
			MSG_ERROR << tr("Error setting pulspoti, mod %1, is: %2, should be: %3").
			       arg(8 * pMdp->deviceId + pMdp->data[0]).arg(pMdp->data[3]).arg(ptrMPSD->getPulsPoti(1));
		}
		ptrMPSD->setPulserPoti(pMdp->data[1], pMdp->data[2], pMdp->data[3], pMdp->data[4], 0);
                break;
            case SETMODE: // extract the set mode:
		if (pMdp->bufferLength > pMdp->headerLength + 1)
		{
		    int mod = pMdp->data[0];
		    if (mod == 8)
		        for (int i = 0; i < 8; ++i)
		            m_mpsd[i]->setMode(pMdp->data[1] == 1, 0);
                    else
		        m_mpsd[mod]->setMode(pMdp->data[1] == 1, 0);
		}
		else
		{
			bAnswerOk = false;
			MSG_ERROR << tr("Error setting mode, answer from hardware incomplete");
		}
                break;
            case SETDAC:
                MSG_WARNING << tr("not handled command : SETDAC");
                break;
            case SENDSERIAL:
                MSG_WARNING << tr("not handled command : SENDSERIAL");
                break;
            case READSERIAL:
                MSG_WARNING << tr("not handled command : READSERIAL");
                break;
            case SCANPERI:
                MSG_WARNING << tr("not handled command : SCANPERI");
                break;
            case WRITEFPGA:
		if (pMdp->cmd & 0x80)
		{
			bAnswerOk = false;
			MSG_ERROR << tr("WRITEFPGA : failed");
		}
                break;
            case WRITEREGISTER:
		if (pMdp->cmd & 0x80)
		{
			bAnswerOk = false;
			MSG_ERROR << tr("WRITEREGISTER failed");
		}
                break;
            case GETCAPABILITIES:
		if (pMdp->bufferLength - pMdp->headerLength > 1)
		{
			m_capabilities = pMdp->data[0];
			m_txMode = pMdp->data[1];
		}
                else
                {
			m_capabilities = m_txMode = 0;
			m_reg = 0xFFF;
                }
                MSG_NOTICE << tr("GETCAPABILITIES %1").arg(m_reg);
                break;
            case SETCAPABILITIES:
                if(pMdp->data[0] == m_txMode)
                        MSG_DEBUG << tr("MCPD txMode set to %1").arg(m_txMode);
                else
                {
                        MSG_ERROR << tr("Error setting MCPD txMode to %1, is now: %2").arg(m_txMode).arg(pMdp->data[0]);
                        m_txMode = pMdp->data[0];
                }
                break;
            case READREGISTER:
		for (int i = 0; i < (pMdp->bufferLength - pMdp->headerLength); ++i)
			MSG_NOTICE << tr("READREGISTER : %1 = %2").arg(i).arg(pMdp->data[i]);
		m_reg = pMdp->data[0];
		MSG_NOTICE << tr("READREGISTER : %1 %2").arg(m_reg).arg(pMdp->bufferLength);
                break;
            case READFPGA:
                MSG_WARNING << tr("not handled command : READFPGA");
                break;
            case SETPOTI:
                MSG_WARNING << tr("not handled command : SETPOTI");
                break;
            case GETPOTI:
                MSG_WARNING << tr("not handled command : GETPOTI");
                break;
            case READID: // extract the retrieved MPSD-8 IDs:
		for (quint8 c = 0; c < 8; ++c)
			m_awReadId[c] = pMdp->data[c];
                MSG_DEBUG << tr("READID finished");
                break;
            case DATAREQUEST:
                MSG_WARNING << tr("not handled command : DATAREQUEST");
                break;
            case QUIET:
                MSG_WARNING << tr("not handled command : QUIET");
                break;
            case GETVER:
                m_version = pMdp->data[1];
                while (m_version > 1)
                    m_version /= 10.;
                m_version += pMdp->data[0];
                m_fpgaVersion = pMdp->data[2] & 0xFF;
                while (m_fpgaVersion > 1)
                    m_fpgaVersion /= 10.;
                m_fpgaVersion += pMdp->data[2] >> 8;
                MSG_DEBUG << tr("Modul (ID %1): Version number : %2, FPGA version : %3").arg(m_byId).arg(m_version, 0, 'f', 2).arg(m_fpgaVersion, 0, 'f', 2);
                break;
            case READPERIREG:
		ptrMPSD = m_mpsd[pMdp->data[0]];
		m_periReg = pMdp->data[2];
		MSG_DEBUG << tr("READPERIREG %1 : %2 = %3").arg(pMdp->data[0]).arg(pMdp->data[1]).arg(m_periReg);
                break;
            case WRITEPERIREG:
		ptrMPSD = m_mpsd[pMdp->data[0]];
		if(pMdp->data[2] != ptrMPSD->getInternalreg(pMdp->data[1], 1))
		{
			bAnswerOk = false;
			MSG_ERROR << tr("Error setting internal mpsd-register, mod %1, is: %2, should be: %3").
			       arg(8 * pMdp->deviceId + pMdp->data[0]).arg(pMdp->data[3]).arg(ptrMPSD->getPulsPoti(1));
		}
		ptrMPSD->setInternalreg(pMdp->data[1], pMdp->data[2], 0);
                break;
// MDLL commands:
            case SETMDLLTHRESHS:
		if (m_mdll[0]->type() == TYPE_MDLL)
		{
			ptrMDLL = reinterpret_cast<MDLL *>(m_mdll[0]);
			MSG_DEBUG << tr("MDLL command : SETMDLLTHRESHS");
			quint8 thresh[3];
			for (quint8 c = 0; c < 3; c++)
			{
				if (pMdp->data[c] != ptrMDLL->getThreshold(c))
				{
					bAnswerOk = false;
					MSG_ERROR << tr("Error setting threshold%1, mod %2, is: %3, should be: %4").
					       arg(c).arg(8 * pMdp->deviceId).arg(pMdp->data[c]).arg(ptrMDLL->getThreshold(c));
					// leave old threshold setting
					thresh[c] = ptrMDLL->getThreshold(c);
				}
				else
					thresh[c] = pMdp->data[c];
			}
			ptrMDLL->setThresholds(thresh[0], thresh[1], thresh[2], false);
		}
		break;
            case SETMDLLSPECTRUM:
		if (m_mdll[0]->type() == TYPE_MDLL)
                {
			MSG_DEBUG << tr("MDLL command : SETMDLLSPECTRUM");
			ptrMDLL = reinterpret_cast<MDLL *>(m_mdll[0]);
			quint8 spect[4];
			for(quint8 c = 0; c < 4; c++)
			{
				if (pMdp->data[c] != ptrMDLL->getSpectrum(c))
				{
					bAnswerOk = false;
					MSG_ERROR << tr("Error setting spectrum%1, mod %2, is: %3, should be: %4").
						arg(c).arg(8 * pMdp->deviceId).arg(pMdp->data[c]).arg(ptrMDLL->getSpectrum(c));
					// leave old spectrum setting
					spect[c] = ptrMDLL->getSpectrum(c);
				}
				else
					spect[c] = pMdp->data[c];
			}
			ptrMDLL->setSpectrum(spect[0], spect[1], spect[2], spect[3], false);
		}
                break;
            case SETMDLLPULSER:
		if (m_mdll[0]->type() == TYPE_MDLL)
                {
			MSG_DEBUG << tr("MDLL command : SETMDLLPULSER");
			ptrMDLL = reinterpret_cast<MDLL *>(m_mdll[0]);
			quint8 puls[3];

			quint8 val = ptrMDLL->isPulserOn(true);
			if(pMdp->data[0] != val)
			{
				bAnswerOk = false;
				MSG_ERROR << tr("Error switching pulser, mod %1, is: %2, should be: %3").
					arg(8 * pMdp->deviceId).arg(pMdp->data[0]).arg(val);
				// leave old threshold setting
				puls[0] = ptrMDLL->isPulserOn(false);
			}
			else
				puls[0] = pMdp->data[0];

			if (pMdp->data[1] != ptrMDLL->getPulsAmp(true))
			{
				bAnswerOk = false;
				MSG_ERROR << tr("Error switching pulser, mod %1, is: %2, should be: %3").
					arg(8 * pMdp->deviceId).arg(pMdp->data[1]).arg(ptrMDLL->getPulsAmp(true));
				// leave old threshold setting
				puls[1] = ptrMDLL->getPulsAmp(false);
			}
			else
				puls[1] = pMdp->data[1];

			if (pMdp->data[2] != ptrMDLL->getPulsPos(true))
			{
				bAnswerOk = false;
				MSG_ERROR << tr("Error switching pulser, mod %1, is: %2, should be: %3").
					arg(8 * pMdp->deviceId).arg(pMdp->data[1]).arg(ptrMDLL->getPulsPos(true));
				// leave old threshold setting
				puls[2] = ptrMDLL->getPulsPos(false);
			}
			else
				puls[2] = pMdp->data[2];
			ptrMDLL->setPulser(puls[2], puls[1], puls[0], false);
                }
                break;
            case SETMDLLDATASET:
		if (m_mdll[0]->type() == TYPE_MDLL)
                {
			MSG_DEBUG << tr("MDLL command : SETMDLLDATASET");
			ptrMDLL = reinterpret_cast<MDLL *>(m_mdll[0]);
			quint8 set;
			if (pMdp->data[0] != ptrMDLL->getDataset())
			{
				bAnswerOk = false;
				MSG_ERROR << tr("Error setting dataset, mod %1, is: %2, should be: %3").
					arg(8 * pMdp->deviceId).arg(pMdp->data[0]).arg(ptrMDLL->getDataset());
				// leave old threshold setting
				set = ptrMDLL->getDataset();
			}
			else
				set = pMdp->data[0];
			ptrMDLL->setDataset(set, false);
                }
                break;
            case SETMDLLACQSET:
		if (m_mdll[0]->type() == TYPE_MDLL)
                {
			MSG_DEBUG << tr("MDLL command : SETMDLLACQSET");
			ptrMDLL = reinterpret_cast<MDLL *>(m_mdll[0]);
			quint16 tsum[4];
			for(quint8 c = 0; c < 4; c++)
			{
				if (pMdp->data[c+2] != ptrMDLL->getTimingWindow(c))
				{
					bAnswerOk = false;
					MSG_ERROR << tr("Error setting timing window%1, mod %2, is: %3, should be: %4").
					       arg(c).arg(8 * pMdp->deviceId).arg(pMdp->data[c + 2]).arg(ptrMDLL->getTimingWindow(c));
					// leave old threshold setting
					tsum[c] = ptrMDLL->getTimingWindow(c);
				}
				else
					tsum[c] = pMdp->data[c+2];
			}
			ptrMDLL->setTimingWindow(tsum[0], tsum[1], tsum[2], tsum[3], false);
                }
                break;
            case SETMDLLEWINDOW:
		if (m_mdll[0]->type() == TYPE_MDLL)
                {
			MSG_DEBUG << tr("MDLL command : SETMDLLEWINDOW");
			ptrMDLL = reinterpret_cast<MDLL *>(m_mdll[0]);
			quint8 e[2];
			for(quint8 c = 0; c < 2; c++)
			{
				if (pMdp->data[c] != ptrMDLL->getEnergyWindow(c))
				{
					bAnswerOk = false;
					MSG_ERROR << tr("Error setting energy window%1, mod %2, is: %3, should be: %4").
					       arg(c).arg(8 * pMdp->deviceId).arg(pMdp->data[c]).arg(ptrMDLL->getEnergyWindow(c));
					// leave old threshold setting
					e[c] = ptrMDLL->getEnergyWindow(c);
				}
				else
					e[c] = pMdp->data[c];
			}
			ptrMDLL->setEnergyWindow(e[0], e[1], false);
                }
                break;
            default:
                MSG_WARNING << tr("not handled command : %1").arg(pMdp->cmd);
                break;
        }

        m_pCommunicationMutex->lock();
        m_iCommActive = bAnswerOk ? RECV : RECV_INVALID;
        m_pCommunicationMutex->unlock();
    }
    else
    {
        quint16 diff = pMdp->bufferNumber - m_lastDataBufnum;
        if(diff > 1 && pMdp->bufferNumber > 0 && m_lastDataBufnum > 0)
        {
             m_dataMissed += (diff - 1);
             MSG_ERROR << tr("%1(%2) %3: Lost %4 data buffers: current: %5, last: %6").
                 arg(m_pNetwork->ip()).arg(m_pNetwork->port()).arg(getId()).arg(diff - 1).arg(pMdp->bufferNumber).arg(m_lastDataBufnum);
        }
        m_lastDataBufnum = pMdp->bufferNumber;
        ++m_dataRxd;
//      MSG_DEBUG << tr("ID %1 : emit analyzeBuffer(pPacket)").arg(m_id);
	emit analyzeDataBuffer(pPacket);
    }
    return true;
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
    MSG_DEBUG << tr("READPERIREG %1, mod %2, reg %3").arg(m_byId).arg(mod).arg(reg);
    QMutexLocker locker(m_pCommandMutex);
    quint16 wValue = 0xFFFF;
    initCmdBuffer(READPERIREG);
    m_cmdBuf.data[0] = mod;
    m_cmdBuf.data[1] = reg;
    finishCmdBuffer(2);
    if (sendCommand(false))
        wValue = m_periReg;
    return wValue;
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
    MSG_DEBUG << tr("WRITEPERIREG %1, mod %2, reg %3, val %4").arg(m_byId).arg(mod).arg(reg).arg(val);
    QMutexLocker locker(m_pCommandMutex);
    m_mpsd[mod]->setInternalreg(reg, val, 1);
    initCmdBuffer(WRITEPERIREG);
    m_cmdBuf.data[0] = mod;
    m_cmdBuf.data[1] = reg;
    m_cmdBuf.data[2] = val;
    finishCmdBuffer(3);
    return sendCommand(true);
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
    MSG_DEBUG << tr("WRITEREGISTER %1, reg %2, val %3").arg(m_byId).arg(reg).arg(val);
    QMutexLocker locker(m_pCommandMutex);
    initCmdBuffer(WRITEREGISTER);
    m_cmdBuf.data[0] = 1;
    m_cmdBuf.data[1] = reg;
    m_cmdBuf.data[2] = val;
    finishCmdBuffer(3);
    return sendCommand(true);
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
    MSG_DEBUG << tr("READREGISTER %1, reg %2").arg(m_byId).arg(reg);
    QMutexLocker locker(m_pCommandMutex);
    quint16 wValue = 0xFFFF;
    initCmdBuffer(READREGISTER);
    m_cmdBuf.data[0] = 1;
    m_cmdBuf.data[1] = reg;
    finishCmdBuffer(2);
    if (sendCommand(false))
        wValue = m_reg;
    return wValue;
}

/*!
    \fn MCPD8::setMasterClock(quint64 val)

    sets the master clock to a new value

    \param val new clock value
    \return true if operation was succesful or not
 */
bool MCPD8::setMasterClock(quint64 val)
{
    MSG_DEBUG << tr("SETCLOCK %1, val %2").arg(m_byId).arg(val);
    QMutexLocker locker(m_pCommandMutex);
    initCmdBuffer(SETCLOCK);
    m_cmdBuf.data[0] = val & 0xFFFF;
    m_cmdBuf.data[1] = (val >> 16) & 0xFFFF;
    m_cmdBuf.data[2] = (val >> 32) & 0xFFFF;
    finishCmdBuffer(3);
    return sendCommand(false);
}

/*!
    \fn MCPD8::setTimingSetup(bool master, bool term, bool extsync)

    sets the communication parameters between the MCPD's

    \param master is this MCPD master or not
    \param term should the MCPD synchronization bus terminated or not
    \param extsync is external sync enabled (only master)
    \return true if operation was succesful or not
 */
bool MCPD8::setTimingSetup(bool master, bool term, bool extsync)
{
    MSG_NOTICE << tr("mcpd %1: set timing setup : master = %2, terminate = %3, external sync %4").arg(m_byId).arg(master).arg(term).arg(extsync);
    if (master)
    {
        term = true;
        if (extsync)
            setCounterCell(MON4ID, NOTRIGGER, 0);
    }
    MSG_DEBUG << tr("SETTIMING %1, %2, %3, %4").arg(m_byId).arg(master).arg(term).arg(extsync);
    QMutexLocker locker(m_pCommandMutex);
    initCmdBuffer(SETTIMING);
// This enables the external sync capability
    m_cmdBuf.data[0] = master + 2 * extsync;
    m_cmdBuf.data[1] = term;
    finishCmdBuffer(2);
    if (sendCommand(true))
    {
        m_master = master;
        m_term = term;
        m_extsync = extsync;
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
    if (getModuleId(addr) && m_mpsd[addr]->isPulserOn())
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

// mdll has fixed bins
    if(m_mdll.contains(0))
            return m_mdll[0]->type() == TYPE_MDLL ? 960 : 1024;

    for (quint8 i = 0; i < 8; ++i)
        if (m_mpsd.find(i) != m_mpsd.end() && getModuleId(i))
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
    \fn MCPD8::initModule(quint8 id)

    initializes a Module:
    - sets threshold
    - sets pulser
    - sets mode
    - writes peripheral registers
    - ...

    \param id number of the MPSD
 */
void MCPD8::initModule(quint8 id)
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
	stop = myMpsd[id]->getChannels();
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
    if(getModuleId(id) == TYPE_MPSD8P)
    {
        // write register 1
        writePeriReg(id, 1, TPA);
    }
}

/*!
    \fn QString MCPD8::getModuleType(quint8 addr)

    \param addr id number of the MPSD
    \return the type of the MPSD
 */
QString MCPD8::getModuleType(quint8 addr)
{
    if (addr == 0 && m_mdll.find(addr) != m_mdll.end())
        return m_mdll[addr]->getType();
    if (m_mpsd.find(addr) != m_mpsd.end())
        return m_mpsd[addr]->getType();
    return "-";
}


/*!
    \fn void MCPD8::setHistogram(bool hist)

    \param hist
 */
void MCPD8::setHistogram(bool hist)
{
    foreach(MPSD8 *it, m_mpsd)
    {
        Q_ASSERT_X(it != NULL, "MCPD8::setHistogram", "one of the MPSD's is NULL");
        it->setHistogram(hist);
    }
    if (!hist)
        setActive(false);
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
    if (!hist)
        setActive(id, false);
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
        setActive(id, chan, false);
}

/*!
    \fn void MCPD8::setActive(bool act)

    \param act
 */
void MCPD8::setActive(bool act)
{
    foreach(MPSD8 *it, m_mpsd)
    {
        Q_ASSERT_X(it != NULL, "MCPD8::setActive", "one of the MPSD's is NULL");
        it->setActive(act);
    }
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
    if(m_mdll.find(0) != m_mdll.end())
            return true;
    foreach(MPSD8 *it, m_mpsd)
    {
        Q_ASSERT_X(it != NULL, "MCPD8::active", "one of the MPSD's is NULL");
        result |= it->active();
    }
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
    {
        Q_ASSERT_X(it != NULL, "MCPD8::histogram", "one of the MPSD's is NULL");
        result |= it->histogram();
    }
    return result;
}

/*!
    \fn bool MCPD8::active(quint16 id)

    \param id
    \return whether the MPSD id is used or not
 */
bool MCPD8::active(quint16 id)
{
    if(m_mdll.find(0) != m_mdll.end())
            return true;
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
    if(m_mdll.find(0) != m_mdll.end())
            return true;
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
    \fn QList<quint16> MCPD8::getHistogramList(void)

    return the list of channels used in histograms

    \return the list of channels used in histograms
 */
QList<quint16> MCPD8::getHistogramList(void)
{
    QList<quint16> result;

// MDLL has fixed channels
    if(m_mdll.contains(0))
        result = m_mdll[0]->getHistogramList();
    else
    {
        foreach(MPSD8 *it, m_mpsd)
        {
            Q_ASSERT_X(it != NULL, "MCPD8::getHistogramList", "one of the MPSD's is NULL");
            QList<quint16> tmpHistList = it->getHistogramList();
            foreach(quint16 hit, tmpHistList)
	    {
#if 0
	        quint16 busNr = it->busNumber();
                for (quint16 i = result.size(); i < (busNr  * it->getChannels()); ++i)
                    result.append(0);
#endif
                result.append(hit + it->busNumber() * it->getChannels());
	    }
        }
    }
    qStableSort(result);
    return result;
}

/*!
    \fn QList<quint16> MCPD8::getActiveList(void)

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
    for(int i = 0; i < m_mpsd.keys().size(); ++i)
    {
        MPSD8 *it = m_mpsd.value(i);
        Q_ASSERT_X(it != NULL, "MCPD8::numModules", "one of the MPSD's is NULL");
        if (it->getModuleId())
            n++;
    }
    return n;
}

quint16 MCPD8::getTxMode(const bool cached)
{
    MSG_DEBUG << tr("GETCAPABILITIES %1").arg(m_byId);
    if (!cached)
    {
        QMutexLocker locker(m_pCommandMutex);
        initCmdBuffer(GETCAPABILITIES);
        finishCmdBuffer(0);
        sendCommand(false);
    }
    return m_txMode;
}

quint16 MCPD8::getTxMode(quint8 mod)
{
	if (!m_mpsd.contains(mod))
		return 0;
        switch (m_mpsd[mod]->type())
        {
                case TYPE_MPSD8P:
                case TYPE_MPSD8SADC:
                        return readPeriReg(mod, 1);
                case TYPE_NOMODULE:
                        return 0;
                default:
                        return P;
        }
}

QMap<quint16, quint16> MCPD8::getTubeMapping(void)
{
    QMap<quint16, quint16> result;
    quint16 n(0);

// MDLL has fixed channels
    if (m_mdll.contains(0))
    {
	QList<quint16> tmpList = m_mdll[0]->getHistogramList();
        foreach(quint16 i, tmpList)
	{
            result.insert(n, n);
	    n++;
	}
    }
    else
    {
        quint16 offset(0);
        for(int i = 0; i < m_mpsd.keys().size(); ++i)
        {
            MPSD8 *it = m_mpsd.value(i);
            Q_ASSERT_X(it != NULL, "MCPD8::getTubeMapping", "one of the MPSD's is NULL");
            QList<quint16> tmpList = it->getHistogramList();
            quint16 busNr = it->busNumber();
            quint16 buswidth = busNr * it->getChannels();
	    int j = 0; // reset list offset
	    for (int i = 0; i < tmpList.size(); ++i)
		if (tmpList.at(i) == i) // ignore not used channels
		{
		    result.insert(buswidth + i, offset + j);
		    j++;
		}
	    offset += tmpList.size() - tmpList.count(65535); // Ignore not used channels
        }
    }
    return result;
}

QList<int> MCPD8::mpsdId(void)
{
	QList<int> modList;
	for (int i = 0; i < 8; ++i)
		if (getModuleId(i))
			modList << i;
	return modList;
}

QList<int> MCPD8::histogrammedId(void)
{
	QList<int> modList;
	for (int i = 0; i < 8; ++i)
		if (m_mpsd.contains(i) && !m_mpsd[i]->getHistogramList().isEmpty())
			modList << i;
	return modList;
}

QList<int> MCPD8::channelId(const int mod)
{
	QList<int> channelList;
	if (m_mpsd.contains(mod))
	{
		QList<quint16> tmpList = m_mpsd[mod]->getHistogramList();
		foreach(int i, tmpList)
			if (i != 0xFFFF)
				channelList.append(i);
	}
	return channelList;
}

bool MCPD8::isMaster(void) const
{
	return m_master;
}

bool MCPD8::isTerminated(void) const
{
	return isMaster() ? true : m_term;
}

bool MCPD8::isExtsynced(void) const
{
	return m_extsync;
}

quint64 MCPD8::receivedData() const
{
	return m_dataRxd;
}

quint64 MCPD8::missedData() const
{
	return m_dataMissed;
}

quint64 MCPD8::receivedCmds() const
{
	return m_cmdRxd;
}

quint64 MCPD8::sentCmds() const
{
	return m_cmdTxd;
}

bool MCPD8::getStream(void) const
{
	return m_stream;
}

quint64 MCPD8::time(void) const
{
	return m_timemsec;
}

quint32 MCPD8::getRunId(void) const
{
	return m_runId;
}

void MCPD8::setBuffer(quint8 index, quint16 value)
{
	m_cmdBuf.data[index] = value;
}
