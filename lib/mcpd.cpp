/***************************************************************************
 *   Copyright (C) 2014-2020 by Lutz Rossa <rossa@helmholtz-berlin.de>     *
 *   Copyright (C) 2014-2020 by Jens Krüger <jens.krueger@frm2.tum.de>  *
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
#include "mcpd.h"
#include "networkdevice.h"
#include "qmlogging.h"
#include "stdafx.h"

/** \fn MCPD(quint8 byId, QString szMcpdIp = "192.168.168.121", quint16 wPort = 54321, QString szMcpdDataIp = QString(),
 * 		quint16 wDataPort = 0, QString szSourceIp = QString())
 *
 *  Initialize base class for a MCPD.
 *
 *  \param byId          id of the MCPD
 *  \param szMcpdIp      IP address of the MCPD, default: 192.168.168.121
 *  \param wPort         UDP port of the MCPD, default: 54321
 *  \param szMcpdDataIp  optional different IP address of the MCPD data
 *  \param wDataPort     optional different UDP port of the MCPD data
 *  \param szSourceIp    optional host IP address
 */
MCPD::MCPD(quint8 byId, QString mcpdAddress, quint16 wPort, QString szMcpdDataIp, quint16 wDataPort, QString szSourceIp)
    : m_iErrorCounter(0)
    , m_pNetwork(NULL)
    , m_pDataNetwork(NULL)
    , m_byId(byId)
    , m_szMcpdIp(lookup_ipv4(mcpdAddress).toString())
    , m_wPort(wPort)
    , m_szMcpdDataIp(szMcpdDataIp)
    , m_wDataPort(wDataPort)
    , m_pCommunicationMutex(NULL)
    , m_pCommandMutex(NULL)
    , m_bBaseMcpdInitialized(false)
    , m_bIsSynced(true)
    , m_pThread(NULL)
    , m_pPacketMutex(NULL)
{
    bool bResult;
    if (szSourceIp.isEmpty())
        szSourceIp = QString("0.0.0.0");
    if (
#if QT_VERSION < 0x050000
        QHostAddress(m_szMcpdDataIp) == QHostAddress::Any ||
#else
        QHostAddress(m_szMcpdDataIp) == QHostAddress::AnyIPv4 ||
#endif
        QHostAddress(m_szMcpdDataIp) == QHostAddress::AnyIPv6)
        m_szMcpdDataIp.clear();

    if (m_wDataPort == m_wPort)
        m_wDataPort = 0;

    MSG_NOTICE << tr("%1(%2): host lookup result: %3 -> %4")
                  .arg(m_szMcpdIp).arg(m_wPort)
                  .arg(mcpdAddress).arg(m_szMcpdIp);

    m_pPacketMutex = new QMutex;
    m_pThread = new MCPDThread(this);
    m_pThread->start();
    m_pCommunicationMutex = new QMutex;
    m_pCommandMutex = new QMutex;
    m_pNetwork = NetworkDevice::create(m_wPort, szSourceIp);

    bResult = m_pNetwork->connect_handler(QHostAddress(m_szMcpdIp), m_wPort, &staticAnalyzeBuffer, this);

    if (!m_szMcpdDataIp.isEmpty() || m_wDataPort != 0)
    {
        if (m_szMcpdDataIp.isEmpty())
            m_szMcpdDataIp = m_szMcpdIp;
        if (m_wDataPort == 0)
            m_wDataPort = wPort;
        // m_pDataNetwork = NetworkDevice::create(m_wDataPort, szSourceIp);
        bResult &= m_pNetwork->connect_handler(QHostAddress(m_szMcpdDataIp), wDataPort, &staticAnalyzeBuffer, this);
    }
    //! this is normally true and only false, if there is already such a MCPD
    m_bBaseMcpdInitialized = bResult;
}

/** \fn MCPD::~MCPD()
 *
 *  clean up base class of a MCPD.
 */
MCPD::~MCPD()
{
    m_pCommandMutex->lock();
    delete m_pThread;
    if (m_pDataNetwork != NULL)
    {
        m_pDataNetwork->disconnect_handler(QHostAddress(m_szMcpdDataIp), m_wDataPort, &staticAnalyzeBuffer);
        NetworkDevice::destroy(m_pDataNetwork);
    }
    m_pNetwork->disconnect_handler(QHostAddress(m_szMcpdIp), m_wPort, &staticAnalyzeBuffer);
    NetworkDevice::destroy(m_pNetwork);
    m_pCommandMutex->unlock();
    delete m_pCommunicationMutex;
    delete m_pCommandMutex;
    delete m_pPacketMutex;
}

/** \fn bool MCPD::connect_handler(analyzeBufferFunction pFunction, void* pParam = NULL)
 *
 *  connect a new data packet handler to this MCPD
 *
 *  \param pFunction  pointer to data handler
 *  \param pParam     the data handler will be called with this pointer
 */
bool MCPD::connect_handler(analyzeBufferFunction pFunction, void* pParam /*= NULL*/)
{
    QMutexLocker locker(m_pCommandMutex);
    struct handler h;
    if (!m_bBaseMcpdInitialized || pFunction == NULL)
        return false;
    foreach(const struct handler &tmp, m_aHandler)
    {
        if (tmp.pFunction == pFunction)
            return false;
    }
    h.pFunction = pFunction;
    h.pParam = pParam;
    m_aHandler.append(h);
    return true;
}

/** \fn void MCPD::disconnect_handler(analyzeBufferFunction pFunction)
 *
 *  \param pFunction  pointer to data handler
 */
void MCPD::disconnect_handler(analyzeBufferFunction pFunction)
{
    QMutexLocker locker(m_pCommandMutex);
    for (int i = 0; i < m_aHandler.count(); ++i)
    {
        struct handler &tmp = m_aHandler[i];
        if (tmp.pFunction == pFunction)
            m_aHandler.removeAt(i--);
    }
}

/** \fn void MCPD::staticAnalyzeBuffer(void *pPacket, void *pParam)
 *
 *  \param pPacket  pointer to MCPD packet
 *  \param pParam   pointer to MCPD instance
 */
void MCPD::staticAnalyzeBuffer(QSharedDataPointer<SD_PACKET> pPacket, void *pParam)
{
    if (pPacket.constData() == NULL || pParam == NULL)
        return;

    MCPD *pMcpd(static_cast<MCPD *>(pParam));
    if (!pMcpd->m_bBaseMcpdInitialized)
        return;

#if !defined(_MSC_VER) || _MSC_VER >= 1700
    if ((pPacket.constData()->mdp.deviceStatus & 0b1000))
#else
    if ((pPacket.constData()->mdp.deviceStatus & 0x8))
#endif
    {
        if (pMcpd->m_bIsSynced)
        {
            MSG_NOTICE << tr("MODULE : %1 lost sync").arg(pPacket.constData()->mdp.deviceId);
            MSG_NOTICE << tr("STATUS : 0x%1").arg(pPacket.constData()->mdp.deviceStatus, 4, 16, QChar('0'));
            emit pMcpd->lostSync(pPacket.constData()->mdp.deviceId, true);
            pMcpd->m_bIsSynced = false;
        }
    }
    else
    {
        if (!pMcpd->m_bIsSynced)
        {
            MSG_NOTICE << tr("MODULE : %1 gained sync").arg(pPacket.constData()->mdp.deviceId);
            emit pMcpd->lostSync(pPacket.constData()->mdp.deviceId, false);
            pMcpd->m_bIsSynced = true;
        }
    }

    for (;;)
    {
        pMcpd->m_pPacketMutex->lock();
        if (pMcpd->m_pThread->m_iCommand != MCPDThread::WORK)
            break;
        pMcpd->m_pPacketMutex->unlock();
        usleep(1);
    }
    if (pMcpd->m_pThread->m_iCommand == MCPDThread::NONE)
    {
        pMcpd->m_aTodoPackets.enqueue(pPacket);
        pMcpd->m_pThread->m_iCommand = MCPDThread::WORK;
        pMcpd->m_pThread->m_ThreadCondition.wakeOne();
    }
    pMcpd->m_pPacketMutex->unlock();
}

/** \fn bool MCPD::isSynced() const
 *
 *  \return if MCPD is synchronized with other MCPDs
 */
bool MCPD::isSynced() const
{
    return m_bIsSynced;
}

bool MCPD::isInitialized() const
{
    return m_bBaseMcpdInitialized;
}

int MCPD::errorCount(bool bClear)
{
    int r(m_iErrorCounter);
    if (bClear)
        m_iErrorCounter = 0;
    return r;
}

quint8 MCPD::getId(void) const
{
    return m_byId;
}

QString MCPD::ip(void) const
{
    return m_szMcpdIp;
}

quint16 MCPD::port(void) const
{
    return m_wPort;
}

quint64 MCPD::queueLength()
{
    return m_aTodoPackets.length();
}

/** \fn MCPDThread::MCPDThread(MCPD* pMcpd)
 *
 *  constructor
 *
 *  \param pMcpd pointer to matching MCPD instance
 */
MCPDThread::MCPDThread(MCPD* pMcpd)
    : m_pMcpd(pMcpd)
    , m_iCommand(NONE)
{
}

/** \fn MCPDThread::~MCPDThread()
 *
 *  destructor
 */
MCPDThread::~MCPDThread()
{
    m_pMcpd->m_pPacketMutex->lock();
    m_iCommand = QUIT;
    m_ThreadCondition.wakeOne();
    m_pMcpd->m_pPacketMutex->unlock();
    wait();
}

/** \fn void MCPDThread::run()
 *
 *  worker thread for analysing incoming MCPD command/data packets
 */
void MCPDThread::run()
{
    QSharedDataPointer<SD_PACKET> pPacket;
    for (;;)
    {
        m_pMcpd->m_pPacketMutex->lock();
        switch (m_iCommand)
        {
            case QUIT:
                m_pMcpd->m_pPacketMutex->unlock();
                return;
            case NONE:
                m_ThreadCondition.wait(m_pMcpd->m_pPacketMutex);
                m_pMcpd->m_pPacketMutex->unlock();
                continue;
            default:
                break;
        }

        m_iCommand = NONE;
        while (!m_pMcpd->m_aTodoPackets.isEmpty())
        {
            bool bPacketLost(false);
            pPacket = m_pMcpd->m_aTodoPackets.dequeue();
            m_pMcpd->m_pPacketMutex->unlock();
            if (!m_pMcpd->analyzeBuffer(pPacket))
            {
                bPacketLost = true;
                foreach(const struct MCPD::handler &tmp, m_pMcpd->m_aHandler)
                {
                    bPacketLost = false;
                    (tmp.pFunction)(m_pMcpd, pPacket, tmp.pParam);
                }
            }
            if (bPacketLost)
                MSG_DEBUG << tr("lost packet id %1: %2").arg(m_pMcpd->m_byId).arg(QString(HexDump(&pPacket->mdp, sizeof(pPacket->mdp))));
            m_pMcpd->m_pPacketMutex->lock();
            pPacket = NULL;
        }
        m_pMcpd->m_pPacketMutex->unlock();
    }
}
