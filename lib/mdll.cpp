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
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <sys/types.h>
// #include <arpa/inet.h>

#include "mdll.h"
#include "mdefines.h"
#include "mesydaq3.h"
#include "networkdevice.h"

Mdll::Mdll(mesydaq3 *app, QObject *parent)
	: MesydaqObject(parent)
{
	theApp = (mesydaq3*)app;
	initDefaults();
}

Mdll::~Mdll()
{
}

/*!
    \fn Mdll::initDefaults(void)
 */
void Mdll::initDefaults(void)
{
    m_mdllSet.id = 0;
    m_mdllSet.master = true;
    m_mdllSet.terminate = true;
    m_mdllSet.threshX = 20;
    m_mdllSet.threshY = 20;
    m_mdllSet.threshA = 20;
    m_mdllSet.shiftX = 100;
    m_mdllSet.shiftY = 100;
    m_mdllSet.scaleX = 40;
    m_mdllSet.scaleY = 40;
/*
    m_mdllSet.mode = 0;
    m_mdllSet.previewHistsize = 0;
    m_mdllSet.previewHistrate = 0xFFF;
    m_mdllSet.histSize = 2;
    m_mdllSet.histType = 0;
    m_mdllSet.slscOff = 1;
*/    m_mdllSet.datareg = 0;
    
    m_mdllSet.eventCounter0 = 0;
    m_mdllSet.eventCounter1 = 0;
    m_mdllSet.eventLimit0 = 0xFFFF;
    m_mdllSet.eventLimit1 = 0xFFFF;
    
    m_mdllSet.tsumXlo = 100;
    m_mdllSet.tsumXhi = 1000;
    m_mdllSet.tsumYlo = 100;
    m_mdllSet.tsumYhi = 1000;
    
    m_mdllSet.pulserOn = 0;
    m_mdllSet.pulserAmpl = 3;
    m_mdllSet.pulserPos = 1;
    
    m_mdllSet.energyLow = 20;
    m_mdllSet.energyHi = 240;
    m_mdllSet.eScaleX = 1;
    m_mdllSet.eScaleY = 1;

    m_mdllSet.counterCell[0][0] = 7;
    m_mdllSet.counterCell[0][1] = 22;
    m_mdllSet.counterCell[1][0] = 7;
    m_mdllSet.counterCell[1][1] = 22;
    m_mdllSet.counterCell[2][0] = 0;
    m_mdllSet.counterCell[2][1] = 0;

    m_mdllSet.auxTimer[0] = 0;
    m_mdllSet.auxTimer[1] = 0;
    m_mdllSet.auxTimer[2] = 0;
    m_mdllSet.auxTimer[3] = 0;

    m_mdllSet.paramSource[0] = 0;
    m_mdllSet.paramSource[1] = 1;
    m_mdllSet.paramSource[2] = 2;
    m_mdllSet.paramSource[3] = 7;
}


/*!
    \fn void Mdll::getMdllSet(P_MDLL_SETTING)
 */
void Mdll::getMdllSet(P_MDLL_SETTING pMdllSet)
{
	memcpy(pMdllSet, &m_mdllSet, sizeof(MDLL_SETTING));
}

/*!
    \fn Mdll::setAll(P_MDLL_SETTING)
 */
void Mdll::setAll(P_MDLL_SETTING pMdllSet)
{
	memcpy(&m_mdllSet, pMdllSet, sizeof(MDLL_SETTING));
	setTiming();
	setSpectrum();
	setAcqset();
	setDatareg();
	setEnergy();
	setThreshold();
	setCountercells();
	setParams();
	setAuxtimers();
//	setMode();
//	setSlide();
//	setHistogram();
//	setSpectrum();
}


/*!
    \fn void Mdll::setMdll(P_MDLL_SETTING pMdllSet, bool dontCheck)
 */
void Mdll::setMdll(P_MDLL_SETTING pMdllSet, bool dontCheck)
{
	qDebug("set MDLL");
	bool change(false);
	m_pCmdPacket->deviceId = 0;
// check for different command blocks:
    
// MDLL part
// thresholds
	if (m_mdllSet.threshX != pMdllSet->threshX)
    		change = true;
	if (m_mdllSet.threshY != pMdllSet->threshY)
    		change = true;
	if (m_mdllSet.threshA != pMdllSet->threshA)
		change = true;
// new settings?
	if (change || dontCheck)
	{
    		m_mdllSet.threshX = pMdllSet->threshX;
    		m_mdllSet.threshY = pMdllSet->threshY;
    		m_mdllSet.threshA = pMdllSet->threshA;
        	setThreshold();
    	}
// spectrum
	if (m_mdllSet.shiftX != pMdllSet->shiftX)
		change = true;
	if (m_mdllSet.shiftY != pMdllSet->shiftY)
		change = true;
	if (m_mdllSet.scaleX != pMdllSet->scaleX)
		change = true;
	if (m_mdllSet.scaleY != pMdllSet->scaleY)
		change = true;
// testreg (= datareg)
	if (m_mdllSet.datareg != pMdllSet->datareg)
		change = true;
// acq settings
	if (m_mdllSet.eventLimit0 != pMdllSet->eventLimit0)
		change = true;
	if (m_mdllSet.eventLimit1 != pMdllSet->eventLimit1)
		change = true;
	if (m_mdllSet.tsumXlo != pMdllSet->tsumXlo)
		change = true;
	if (m_mdllSet.tsumXhi != pMdllSet->tsumXhi)
		change = true;
	if (m_mdllSet.tsumYlo != pMdllSet->tsumYlo)
		change = true;
	if (m_mdllSet.tsumYhi != pMdllSet->tsumYhi)
		change = true;
// energy window
	if (m_mdllSet.energyLow != pMdllSet->energyLow)
		change = true;
	if (m_mdllSet.energyHi !=pMdllSet->energyHi)
		change = true;

	if (change || dontCheck)
	{
		qDebug("change in MDLL part");
		m_mdllSet.threshX = pMdllSet->threshX;
		m_mdllSet.threshY = pMdllSet->threshY;
		m_mdllSet.threshA = pMdllSet->threshA;
		m_mdllSet.shiftX = pMdllSet->shiftX;
		m_mdllSet.shiftY = pMdllSet->shiftY;
		m_mdllSet.scaleX = pMdllSet->scaleX;
		m_mdllSet.scaleY = pMdllSet->scaleY;
		m_mdllSet.datareg = pMdllSet->datareg;
		m_mdllSet.eventLimit0 = pMdllSet->eventLimit0;
		m_mdllSet.eventLimit1 = pMdllSet->eventLimit1;
		m_mdllSet.tsumXlo = pMdllSet->tsumXlo;
		m_mdllSet.tsumXhi = pMdllSet->tsumXhi;
		m_mdllSet.tsumYlo = pMdllSet->tsumYlo;
		m_mdllSet.tsumYhi = pMdllSet->tsumYhi;
		m_mdllSet.energyLow = pMdllSet->energyLow;
		m_mdllSet.energyHi = pMdllSet->energyHi;

		setEnergy();
		setThreshold();
		setSpectrum();
		setDatareg();
		setAcqset();
	}
#if 0
// mode
	if (m_mdllSet.mode != pMdllSet->mode)
	{
    		m_mdllSet.mode = pMdllSet->mode;
		setMode();
	}

// histogram
	change = true;
	if (m_mdllSet.previewHistsize != pMdllSet->previewHistsize)
		change = true;
	if (m_mdllSet.previewHistrate != pMdllSet->previewHistrate)
    		change = true;
	if (m_mdllSet.histSize != pMdllSet->histSize)
		change = true;
	if (m_mdllSet.histType != pMdllSet->histType)
		change = true;
// new settings?
	if(change)
	{
		m_mdllSet.previewHistsize = pMdllSet->previewHistsize;
		m_mdllSet.previewHistrate = pMdllSet->previewHistrate;
		m_mdllSet.histSize = pMdllSet->histSize;
		m_mdllSet.histType = pMdllSet->histType;
		setHistogram();
	}

// sliding scale
	if (m_mdllSet.slscOff != pMdllSet->slscOff)
	{
		m_mdllSet.slscOff = pMdllSet->slscOff;
		setSlide();
	}
#endif

// pulser settings
	change = false;
	if (m_mdllSet.pulserOn != pMdllSet->pulserOn)
    		change = true;
	if (m_mdllSet.pulserAmpl != pMdllSet->pulserAmpl)
		change = true;
	if (m_mdllSet.pulserPos != pMdllSet->pulserPos)
		change = true;
	if (change)
	{
		qDebug("change in Pulser part");
		m_mdllSet.pulserOn = pMdllSet->pulserOn;
		m_mdllSet.pulserAmpl = pMdllSet->pulserAmpl;
		m_mdllSet.pulserPos = pMdllSet->pulserPos;
		m_pCmdPacket->cmd = SETDLLPULSER;
		m_pCmdPacket->data[0] = m_mdllSet.pulserOn;
		m_pCmdPacket->data[1] = m_mdllSet.pulserAmpl;
		m_pCmdPacket->data[2] = m_mdllSet.pulserPos;
		theApp->sendBuffer(m_pCmdPacket->deviceId, 3);
	}
	    
// counter settings
	change = false;
	qDebug("counter 2: %u %u, new setting: %u %u", m_mdllSet.counterCell[2][0], m_mdllSet.counterCell[2][1],
							pMdllSet->counterCell[2][0], pMdllSet->counterCell[2][1]);
	if (m_mdllSet.counterCell[0][0] != pMdllSet->counterCell[0][0])
		change = true;
	if (m_mdllSet.counterCell[1][0] != pMdllSet->counterCell[1][0])
		change = true;
	if (m_mdllSet.counterCell[2][0] != pMdllSet->counterCell[2][0])
		change = true;
	if (m_mdllSet.counterCell[0][1] != pMdllSet->counterCell[0][1])
		change = true;
	if (m_mdllSet.counterCell[1][1] != pMdllSet->counterCell[1][1])
		change = true;
	if(m_mdllSet.counterCell[2][1] != pMdllSet->counterCell[2][1])
		change = true;
	if (change || dontCheck)
	{
		m_mdllSet.counterCell[0][0] = pMdllSet->counterCell[0][0];
		m_mdllSet.counterCell[1][0] = pMdllSet->counterCell[1][0];
		m_mdllSet.counterCell[2][0] = pMdllSet->counterCell[2][0];
		m_mdllSet.counterCell[0][1] = pMdllSet->counterCell[0][1];
		m_mdllSet.counterCell[1][1] = pMdllSet->counterCell[1][1];
		m_mdllSet.counterCell[2][1] = pMdllSet->counterCell[2][1];
		setCountercells();
	}
// auxtimer settings
	if (m_mdllSet.auxTimer[0] != pMdllSet->auxTimer[0])
		change = true;
	if (m_mdllSet.auxTimer[1] != pMdllSet->auxTimer[1])
		change = true;
	if (m_mdllSet.auxTimer[2] != pMdllSet->auxTimer[2])
		change = true;
	if (m_mdllSet.auxTimer[3] != pMdllSet->auxTimer[3])
		change = true;
	if (change || dontCheck)
	{
		m_mdllSet.auxTimer[0] = pMdllSet->auxTimer[0];
		m_mdllSet.auxTimer[1] = pMdllSet->auxTimer[1];
		m_mdllSet.auxTimer[2] = pMdllSet->auxTimer[2];
		m_mdllSet.auxTimer[3] = pMdllSet->auxTimer[3];
		setAuxtimers();
	}
// parameter settings
	if (m_mdllSet.paramSource[0] != pMdllSet->paramSource[0])
		change = true;
	if (m_mdllSet.paramSource[1] != pMdllSet->paramSource[1])
		change = true;
	if (m_mdllSet.paramSource[2] != pMdllSet->paramSource[2])
		change = true;
	if (m_mdllSet.paramSource[3] != pMdllSet->paramSource[3])
		change = true;
	if (change || dontCheck)
	{
		m_mdllSet.paramSource[0] = pMdllSet->paramSource[0];
		m_mdllSet.paramSource[1] = pMdllSet->paramSource[1];
		m_mdllSet.paramSource[2] = pMdllSet->paramSource[2];
		m_mdllSet.paramSource[3] = pMdllSet->paramSource[3];
		setParams();
	}
}

/*!
    \fn void Mdll::setPacket(PMDP_PACKET packet)
 */
void Mdll::setPacket(PMDP_PACKET packet)
{
	m_pCmdPacket = packet;
}

/*!
    \fn Mdll::setThreshold(void);
 */
void Mdll::setThreshold(void)
{
	m_pCmdPacket->cmd = SETDLLTHRESHS;
	m_pCmdPacket->data[0] = m_mdllSet.threshX;
	m_pCmdPacket->data[1] = m_mdllSet.threshY;
	m_pCmdPacket->data[2] = m_mdllSet.threshA;
        theApp->sendBuffer(0, 3);
}


/*!
    \fn Mdll::setSpectrum()
 */
void Mdll::setSpectrum()
{
	m_pCmdPacket->cmd = SETDLLSPECTRUM;
	m_pCmdPacket->data[0] = m_mdllSet.shiftX;
	m_pCmdPacket->data[1] = m_mdllSet.shiftY;
	m_pCmdPacket->data[2] = m_mdllSet.scaleX;
	m_pCmdPacket->data[3] = m_mdllSet.scaleY;
        theApp->sendBuffer(0, 4);
}


/*!
    \fn Mdll::setHistogram(void)
 */
void Mdll::setHistogram(void)
{
/*	m_pCmdPacket->cmd = SETDLLHIST;
	m_pCmdPacket->data[0] = m_mdllSet.previewHistsize;
	m_pCmdPacket->data[1] = m_mdllSet.previewHistrate;
	m_pCmdPacket->data[2] = m_mdllSet.histSize;
	m_pCmdPacket->data[3] = m_mdllSet.histType;
	theApp->sendBuffer(4);
*/}


/*!
    \fn Mdll::setMode(void)
 */
void Mdll::setMode(void)
{
/*	m_pCmdPacket->cmd = SETDLLMODE;
	m_pCmdPacket->data[0] = m_mdllSet.mode;
	theApp->sendBuffer(1);
*/}


/*!
    \fn Mdll::setSlide(void)
 */
void Mdll::setSlide(void)
{
/*	m_pCmdPacket->cmd = SETDLLSLSC;
	m_pCmdPacket->data[0] = m_mdllSet.slscOff;
	theApp->sendBuffer(1);
*/}


/*!
    \fn Mdll::setDatareg(void)
 */
void Mdll::setDatareg(void)
{
	m_pCmdPacket->cmd = SETDLLTESTREG;
	m_pCmdPacket->data[0] = m_mdllSet.datareg;
        theApp->sendBuffer(0, 1);
}


/*!
    \fn Mdll::setAcqset(void)
 */
void Mdll::setAcqset(void)
{
	m_pCmdPacket->cmd = SETDLLACQSET;
	m_pCmdPacket->data[0] = m_mdllSet.eventLimit0;
	m_pCmdPacket->data[1] = m_mdllSet.eventLimit1;
	m_pCmdPacket->data[2] = m_mdllSet.tsumXlo;
	m_pCmdPacket->data[3] = m_mdllSet.tsumXhi;
	m_pCmdPacket->data[4] = m_mdllSet.tsumYlo;
	m_pCmdPacket->data[5] = m_mdllSet.tsumYhi;
        theApp->sendBuffer(0, 6);
}


/*!
    \fn Mdll::setEnergy(void)
 */
void Mdll::setEnergy(void)
{
	m_pCmdPacket->cmd = SETDLLENERGY;
	m_pCmdPacket->data[0] = m_mdllSet.energyLow;
	m_pCmdPacket->data[1] = m_mdllSet.energyHi;
	m_pCmdPacket->data[3] = m_mdllSet.eScaleX;
	m_pCmdPacket->data[4] = m_mdllSet.eScaleY;
        theApp->sendBuffer(0, 4);
}

/*!
    \fn Mdll::setCountercells(void)
 */
void Mdll::setCountercells(void)
{
        quint8 i;
        m_pCmdPacket->cmd = SETCELL;
        for(i=0;i<3;i++){
            m_pCmdPacket->data[0] = i;
            m_pCmdPacket->data[1] = m_mdllSet.counterCell[i][0];
            m_pCmdPacket->data[2] = m_mdllSet.counterCell[i][1];
            theApp->sendBuffer(0, 3);
        }
}

/*!
    \fn Mdll::setAuxtimers(void)
 */
void Mdll::setAuxtimers(void)
{
        quint8 i;
        m_pCmdPacket->cmd = SETAUXTIMER;
        for(i=0;i<4;i++){
            m_pCmdPacket->data[0] = i;
            m_pCmdPacket->data[1] = m_mdllSet.auxTimer[i];
            theApp->sendBuffer(0, 2);
        }
}

/*!
    \fn Mdll::setParams(void)
 */
void Mdll::setParams(void)
{
        quint8 i;
        m_pCmdPacket->cmd = SETPARAM;
        for(i=0;i<4;i++){
            m_pCmdPacket->data[0] = i;
            m_pCmdPacket->data[1] = m_mdllSet.paramSource[i];
            theApp->sendBuffer(0, 2);
        }
}


/*!
    \fn Mdll::setTiming(void)
 */
void Mdll::setTiming(void)
{
    m_pCmdPacket->cmd = SETTIMING;
    if(m_mdllSet.master)
        m_pCmdPacket->data[0] = 1;
    else
        m_pCmdPacket->data[0] = 0;
    if(m_mdllSet.terminate)
        m_pCmdPacket->data[1] = 1;
    else
        m_pCmdPacket->data[1] = 0;
    theApp->sendBuffer(0, 2);
}


/*!
    \fn Mdll::initMdll(void)
 */
void Mdll::initMdll(void)
{
    setTiming();
    setSpectrum();
    setCountercells();
    setAuxtimers();
    setAcqset();
    setDatareg();
    setEnergy();
    setThreshold();
    setParams();
//    setHistogram();
//    setMode();
//    setSlide();
//    setSpectrum();
}

/*!
    \fn mdll::serialize(QFile * fi)
 */
void Mdll::serialize(QFile * fi)
{
        quint8 c;
        QTextStream t( fi );        // use a text stream

        t << "[MDLL]";
        t << '\r' << '\n';
        t << "id = " << 0;
        t << '\r' << '\n';
#warning TODO
#if 0
        t << "ipAddress = " << theApp->netDev->getOwnAddress(0);
#endif
        t << '\r' << '\n';
        t << "master = ";
        if(master)
                t << 1;
        else
                t << 0;
        t << '\r' << '\n';
        t << "terminate = ";
        if(terminate)
                t << 1;
        else
                t << 0;
        t << '\r' << '\n';
        for(c=0;c<3;c++){
                t << "counterCell"<< c << " = " << m_mdllSet.counterCell[c][0] << " " << m_mdllSet.counterCell[c][1];
                t << '\r' << '\n';
        }
        for(c=0;c<4;c++){
                t << "auxTimer"<< c << " = " << m_mdllSet.auxTimer[c];
                t << '\r' << '\n';
        }
        t << '\r' << '\n';
        t << "threshX = " << m_mdllSet.threshX;
        t << '\r' << '\n';
        t << "threshY = " << m_mdllSet.threshY;
        t << '\r' << '\n';
        t << "threshA = " << m_mdllSet.threshA;
        t << '\r' << '\n';
        t << "shiftX = " << m_mdllSet.shiftX;
        t << '\r' << '\n';
        t << "shiftY = " << m_mdllSet.shiftY;
        t << '\r' << '\n';
        t << "scaleX = " << m_mdllSet.scaleX;
        t << '\r' << '\n';
        t << "scaleY = " << m_mdllSet.scaleY;
        t << '\r' << '\n';
        t << "tsumXlo = " << m_mdllSet.tsumXlo;
        t << '\r' << '\n';
        t << "tsumXhi = " << m_mdllSet.tsumXhi;
        t << '\r' << '\n';
        t << "tsumYlo = " << m_mdllSet.tsumYlo;
        t << '\r' << '\n';
        t << "tsumYhi = " << m_mdllSet.tsumYhi;
        t << '\r' << '\n';
        t << "energyLo = " << m_mdllSet.energyLow;
        t << '\r' << '\n';
        t << "energyHi = " << m_mdllSet.energyHi;
        t << '\r' << '\n';
}
