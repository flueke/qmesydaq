/***************************************************************************
 *   Copyright (C) 2014 by Lutz Rossa <rossa@helmholtz-berlin.de>          *
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
#include "logging.h"
#include "stdafx.h"

/** \fn MCPD(quint8 byId, QString szMcpdIp = "192.168.168.121", quint16 wPort = 54321, QString szMcpdDataIp = QString::null,
 * 		quint16 wDataPort = 0, QString szSourceIp = QString::null)
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
MCPD::MCPD(quint8 byId, QString szMcpdIp, quint16 wPort, QString szMcpdDataIp, quint16 wDataPort, QString szSourceIp)
    : m_iErrorCounter(0)
    , m_pNetwork(NULL)
    , m_pDataNetwork(NULL)
    , m_byId(byId)
    , m_szMcpdIp(szMcpdIp)
    , m_wPort(wPort)
    , m_szMcpdDataIp(szMcpdDataIp)
    , m_wDataPort(wDataPort)
    , m_pCommunicationMutex(NULL)
    , m_pCommandMutex(NULL)
    , m_bBaseMcpdInitialized(false)
    , m_pThread(NULL)
    , m_pPacketMutex(NULL)
{
    bool bResult;
    if (szSourceIp.isEmpty())
        szSourceIp = QString("0.0.0.0");
    if (QHostAddress(m_szMcpdDataIp) == QHostAddress::Any || QHostAddress(m_szMcpdDataIp) == QHostAddress::AnyIPv6)
        m_szMcpdDataIp.clear();
    if (m_wDataPort == m_wPort)
        m_wDataPort = 0;
    m_pPacketMutex = new QMutex;
    m_pThread = new MCPDThread(this);
    m_pThread->start();
    m_pCommunicationMutex = new QMutex;
    m_pCommandMutex = new QMutex;
    m_pNetwork = NetworkDevice::create(m_wPort, szSourceIp);
    bResult = m_pNetwork->connect_handler(QHostAddress(m_szMcpdIp), wPort, &staticAnalyzeBuffer, this);
    if (!m_szMcpdDataIp.isEmpty() || m_wDataPort != 0)
    {
        if (m_szMcpdDataIp.isEmpty())
            m_szMcpdDataIp = m_szMcpdIp;
        if (m_wDataPort == 0)
            m_wDataPort = wPort;
        // m_pDataNetwork = NetworkDevice::create(m_wDataPort, szSourceIp);
        bResult &= m_pNetwork->connect_handler(QHostAddress(szMcpdDataIp), wDataPort, &staticAnalyzeBuffer, this);
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

/** \fn void MCPD::staticAnalyzeBuffer(void* pPacket, void* pParam)
 *
 *  \param pPacket  pointer to MCPD packet
 *  \param pParam   pointer to MCPD instance
 */
void MCPD::staticAnalyzeBuffer(QSharedDataPointer<SD_PACKET> pPacket, void* pParam)
{
    if (pPacket.constData() == NULL || pParam == NULL)
        return;

    MCPD* pMcpd(static_cast<MCPD *>(pParam));
    if (!pMcpd->m_bBaseMcpdInitialized)
        return;
    for (;;)
    {
        pMcpd->m_pPacketMutex->lock();
        if (pMcpd->m_pThread->m_iCommand != MCPDThread::WORK)
            break;
        pMcpd->m_pPacketMutex->unlock();
        usleep(1000);
    }
    if (pMcpd->m_pThread->m_iCommand == MCPDThread::NONE)
    {
        pMcpd->m_aTodoPackets.append(pPacket);
        pMcpd->m_pThread->m_iCommand = MCPDThread::WORK;
        pMcpd->m_pThread->m_ThreadCondition.wakeOne();
    }
    pMcpd->m_pPacketMutex->unlock();
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
            pPacket = m_pMcpd->m_aTodoPackets.takeFirst();
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
