/***************************************************************************
 *   Copyright (C) 2009 by Gregor Montermann, mesytec GmbH & Co. KG        *
 *      g.montermann@mesytec.com                                           *
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
#ifndef MDLL_H
#define MDLL_H

#include <QObject>

#include <QString>
#include <QFile>

#include "structures.h"
#include "mesydaqobject.h"
#include "mdefines.h"

class mesydaq3;

/*!
    \short Base class for MDLL

    \author Gregor Montermann, mesytec GmbH  Co. KG <g.montermann@mesytec.com>
*/
class Mdll : public MesydaqObject
{
Q_OBJECT
public:
    Mdll(mesydaq3 *, QObject *parent = 0);

    ~Mdll();
    void initDefaults(void);
    void setMdll(P_MDLL_SETTING pMdllSet, bool check);
    void getMdllSet(P_MDLL_SETTING pMdllSet);
    void setPacket(PMDP_PACKET packet);
    void setEnergy(void);
    void setAcqset(void);
    void setDatareg(void);
    void setSlide(void);
    void setMode(void);
    void setHistogram(void);
    void setSpectrum();
    void setThreshold(void);;
    void setCountercells(void);;
    void setAuxtimers(void);;
    void setParams(void);;
    void initMdll(void);
    void setMaster(bool truth);
    void setIpAddress(QString addrStr);
    void setTermination(bool truth);
    bool setId(quint16 mcpdid);
    bool setOutstring(QString str);
    bool setProtocol(quint16* addr);
    bool setParamSource(quint16 param, quint16 source);
    bool setCounterCell(quint16 * celldata);
    bool setAuxTimer(quint16 tim, quint16 val);
    quint8 getId(void);
    void getProtocol(quint16 * addr);
    quint16 getRunId(void);
    bool setRunId(quint16 runid);
    void getCounterCell(quint8 cell, quint16 * celldata);
    quint32 getParameter(quint16 param);
    bool setParameter(quint16 param, quint32 val);
    quint16 getAuxTimer(quint16 timer);
//    void stdInit(void);
    quint16 getParamSource(quint16 param);
    bool setStream(quint16 strm);
    bool getStream(void);
    void serialize(QFile * fi);
    void setTiming(void);
    void setAll(P_MDLL_SETTING pMdllSet);

protected:
    MDLL_SETTING m_mdllSet;
    PMDP_PACKET m_pCmdPacket;
    mesydaq3* theApp;

    void communicate(bool yesno);
    bool isBusy(void);
    QString getIpAddress(void);
    void buildIpStr();
//    sockaddr_in getInetAddr();
    void fillTiming(quint16 * buf);
    void fillCounterCell(quint8 cell, quint16 * buf);
    void fillAuxTimer(quint8 timer, quint16 * buf);
    void fillParamSource(quint8 param, quint16 * buf);
    bool isMaster();
    bool isTerminated();
    bool isOnline(void);
    void timeout();
    void setVersion(quint8 maj, quint8 min);
    void setOnline();
    bool isConfigured();
    void setConfigured(bool truth);
    void answered(void);
    bool isResponding(void);

public:
    bool modified;
protected:
    // communication params
    quint8 ipAddress[4];
    QString ipAddrStr;
//    sockaddr_in inetAddr;

    quint8 cmdIpAddress[4];
    quint8 dataIpAddress[4];
    quint16 cmdPort;
    quint16 dataPort;

    bool master;
    bool terminate;
/*
    // 3 counter cells, trig source in [0], compare reg in [1]
    quint8 counterCell[3][2];
    // four auxiliary timers, capture values
    quint16 auxTimer[4];
    // four parameters (transmitted in buffer header), 9 possible sources
    quint8 paramSource[4];
*/
    // one output, one input string for RS-232
    QString outString;
    QString inString;
    QString str;
    quint16 runId;
    bool stream;
    quint32 parameter[4];
    bool commActive;
    bool commFailed;
    quint8 timeoutCounter;
    quint8 majVer;
    quint8 minVer;
    QString verString;
    bool online;
    QString pstring;
    bool configured;
};

#endif
