/***************************************************************************
 *   Copyright (C) 2013 by Lutz Rossa <rossa@helmholtz-berlin.de>          *
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
#ifndef __MCPD_H__58A1A592_FA2E_44DD_92F8_34558D3C6073__
#define __MCPD_H__58A1A592_FA2E_44DD_92F8_34558D3C6073__

#include <QObject>
#include <QMap>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>

#include "libqmesydaq_global.h"
#include "structures.h"

class NetworkDevice;
class MCPDThread;

/**
 * \short base class for MCPD central module
 *
 * There should be an instance for every used MCPD module. Every instance connects to
 * a "NetworkDevice", receives incoming packets for the specific MCPD only. A worker
 * thread (MCPDThread) handles the incoming packets and does a synchronous call to
 * the virtual function "analyzeBuffer" and if it does not handle packets, it calls
 * any handler which was registered via "connect_handler".
 *
 * \author Lutz Rossa <rossa@helmholtz-berlin.de>
 */
class LIBQMESYDAQ_EXPORT MCPD : public QObject
{
    friend class MCPDThread;
    Q_OBJECT
    Q_DISABLE_COPY(MCPD)
    Q_PROPERTY(bool initialized READ isInitialized)
public:
    //! type of handler for data packets
    typedef void (*analyzeBufferFunction)(MCPD* pMcpd, QSharedDataPointer<SD_PACKET> pMdpPacket, void* pParam);

    MCPD(quint8 byId, QString szMcpdIp = "192.168.168.121", quint16 wPort = 54321, QString szMcpdDataIp = QString::null, quint16 wDataPort = 0, QString szSourceIp = QString::null);

    virtual ~MCPD();

    virtual bool isInitialized() const {return m_bBaseMcpdInitialized;}
    virtual int errorCount(bool bClear = false)
    {
	    int r(m_iErrorCounter);
	    if (bClear)
		    m_iErrorCounter = 0;
	    return r;
    }

    //! connect a new data packet handler to this MCPD
    bool connect_handler(analyzeBufferFunction pFunction, void* pParam = NULL);

    //! disconnect a data packets handler
    void disconnect_handler(analyzeBufferFunction pFunction);

    //! \return the ID of this MCPD
    quint8 getId(void) {return m_byId;}

    /**
     * gets the address of the MCPD module
     *
     * \return IP address of the MCPD
     */
    QString ip(void) {return m_szMcpdIp;}

    /**
     * gets the command port of the MCPD
     *
     * \return port number of the command port
     */
    quint16 port(void) {return m_wPort;}

protected:
    //! analyse a network packet, \return true to prevent calling other packet handlers
    virtual bool analyzeBuffer(QSharedDataPointer<SD_PACKET> pPacket) = 0;

    //! error counter
    int m_iErrorCounter;

    //! communication device
    NetworkDevice* m_pNetwork;

    //! communication device for data only
    NetworkDevice* m_pDataNetwork;

    //! ID of the MCPD
    quint8 m_byId;

    //! IP address of the module
    QString m_szMcpdIp;

    //! Port of the module
    quint16 m_wPort;

    //! IP address of the module data
    QString m_szMcpdDataIp;

    //! Port of the module data
    quint16 m_wDataPort;

    //! communication mutex
    QMutex* m_pCommunicationMutex;

    //! command mutex
    QMutex* m_pCommandMutex;

private:
    struct handler
    {
        analyzeBufferFunction pFunction;
        void* pParam;
    };

    //! wrapper for calling the real handler for new network packets
    static void staticAnalyzeBuffer(QSharedDataPointer<SD_PACKET> pPacket, void* pParam);

    //! true, if base class was successful initialized
    bool m_bBaseMcpdInitialized;

    //! worker thread
    MCPDThread* m_pThread;

    //! thread mutex
    QMutex* m_pPacketMutex;

    //! all packet handlers
    QList<struct handler> m_aHandler;

    //! all incoming MCPD packets
    QList<QSharedDataPointer<SD_PACKET> > m_aTodoPackets;
};

/**
 * \short worker thread class for MCPDs
 *
 * \author Lutz Rossa <rossa@helmholtz-berlin.de>
 */
class LIBQMESYDAQ_EXPORT MCPDThread : public QThread
{
    friend class MCPD;
    Q_OBJECT
    Q_DISABLE_COPY(MCPDThread)
protected:
    MCPDThread(MCPD* pMcpd);
    virtual ~MCPDThread();
    virtual void run();

    //! reference to MCPD
    MCPD*          m_pMcpd;

    enum Command {
	    NONE,
	    WORK,
	    QUIT,
    } m_iCommand;

    //! wake thread up
    QWaitCondition m_ThreadCondition;
};

#endif /*__MCPD_H__58A1A592_FA2E_44DD_92F8_34558D3C6073__*/
