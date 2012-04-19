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
#include "mcpd8.h"
#include "logging.h"

/*!
    constructor
    \param app
    \param parent
 */
MDll::MDll(mesydaq3 *app, QObject *parent)
	: QObject(parent)
	, m_mcpd(NULL)
{
	theApp = (mesydaq3*)app;
	// Mesydaq2::addMCPD(quint16 id, QString ip, quint16 port, QString sourceIP)
 	// m_mcpd = new MCPD8(id, this, ip, port, sourceIP);
 	m_mcpd = new MCPDMDLL(0, this, "192.168.168.121", 54321, "0.0.0.0");

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
#if 0
	m_mdllSet.mode = 0;
	m_mdllSet.previewHistsize = 0;
	m_mdllSet.previewHistrate = 0xFFF;
	m_mdllSet.histSize = 2;
	m_mdllSet.histType = 0;
	m_mdllSet.slscOff = 1;

	m_mdllSet.eventCounter0 = 0;
	m_mdllSet.eventCounter1 = 0;
#endif
	m_mdllSet.datareg = 0;
    
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

MDll::~MDll()
{
}

/*!
    \fn void MDll::getMdllSet(P_MDLL_SETTING pMDllSet)

    \param pMDllSet
 */
void MDll::getMdllSet(P_MDLL_SETTING pMDllSet)
{
	memcpy(pMDllSet, &m_mdllSet, sizeof(MDLL_SETTING));
}

/*!
    \fn void MDll::setAll(P_MDLL_SETTING pMDllSet)

    \param pMDllSet
 */
void MDll::setAll(P_MDLL_SETTING pMDllSet)
{
	memcpy(&m_mdllSet, pMDllSet, sizeof(MDLL_SETTING));
	initMdll();
}


/*!
    \fn void MDll::setMdll(P_MDLL_SETTING pMDllSet, bool dontCheck)

    \param pMDllSet
    \param dontCheck
 */
void MDll::setMdll(P_MDLL_SETTING pMDllSet, bool dontCheck)
{
	MSG_DEBUG << "set MDLL";
	bool change(false);
//	m_pCmdPacket->deviceId = 0;
// check for different command blocks:
    
// MDLL part
// thresholds
	if (m_mdllSet.threshX != pMDllSet->threshX)
    		change = true;
	if (m_mdllSet.threshY != pMDllSet->threshY)
    		change = true;
	if (m_mdllSet.threshA != pMDllSet->threshA)
		change = true;
// new settings?
	if (change || dontCheck)
	{
		m_mcpd->setThreshold(pMDllSet->threshX, pMDllSet->threshY, pMDllSet->threshA);
    	}
// spectrum
	if (m_mdllSet.shiftX != pMDllSet->shiftX)
		change = true;
	if (m_mdllSet.shiftY != pMDllSet->shiftY)
		change = true;
	if (m_mdllSet.scaleX != pMDllSet->scaleX)
		change = true;
	if (m_mdllSet.scaleY != pMDllSet->scaleY)
		change = true;
// testreg (= datareg)
	if (m_mdllSet.datareg != pMDllSet->datareg)
		change = true;
// acq settings
	if (m_mdllSet.eventLimit0 != pMDllSet->eventLimit0)
		change = true;
	if (m_mdllSet.eventLimit1 != pMDllSet->eventLimit1)
		change = true;
	if (m_mdllSet.tsumXlo != pMDllSet->tsumXlo)
		change = true;
	if (m_mdllSet.tsumXhi != pMDllSet->tsumXhi)
		change = true;
	if (m_mdllSet.tsumYlo != pMDllSet->tsumYlo)
		change = true;
	if (m_mdllSet.tsumYhi != pMDllSet->tsumYhi)
		change = true;
// energy window
	if (m_mdllSet.energyLow != pMDllSet->energyLow)
		change = true;
	if (m_mdllSet.energyHi !=pMDllSet->energyHi)
		change = true;

	if (change || dontCheck)
	{
		MSG_DEBUG << "change in MDLL part";
		m_mcpd->setEnergy(pMDllSet->energyLow, pMDllSet->energyHi, pMDllSet->eScaleX, pMDllSet->eScaleY);
		m_mcpd->setThreshold(pMDllSet->threshX, pMDllSet->threshY, pMDllSet->threshA);
		m_mcpd->setSpectrum(pMDllSet->shiftX, pMDllSet->shiftY, pMDllSet->scaleX, pMDllSet->scaleY);
		m_mcpd->setDataReg(pMDllSet->datareg);
		m_mcpd->setAcqset(pMDllSet->eventLimit0 | (pMDllSet->eventLimit1 << 16), 
			pMDllSet->tsumXlo | (pMDllSet->tsumXhi << 16),
			pMDllSet->tsumYlo | (pMDllSet->tsumYhi << 16));
	}
#if 0
// mode
	if (m_mdllSet.mode != pMDllSet->mode)
	{
		m_mcpd->setMode(pMDllSet->mode);
	}

// histogram
	change = true;
	if (m_mdllSet.previewHistsize != pMDllSet->previewHistsize)
		change = true;
	if (m_mdllSet.previewHistrate != pMDllSet->previewHistrate)
    		change = true;
	if (m_mdllSet.histSize != pMDllSet->histSize)
		change = true;
	if (m_mdllSet.histType != pMDllSet->histType)
		change = true;
// new settings?
	if(change)
	{
		m_mcpd->setHistogram(pMDllSet->previewHistsize, pMDllSet->previewHistrate, pMDllSet->histSize, pMDllSet->histType);
	}

// sliding scale
	if (m_mdllSet.slscOff != pMDllSet->slscOff)
	{
		m_mcpd->setSlide(pMDllSet->slscOff);
	}
#endif

// pulser settings
	change = false;
	if (m_mdllSet.pulserOn != pMDllSet->pulserOn)
    		change = true;
	if (m_mdllSet.pulserAmpl != pMDllSet->pulserAmpl)
		change = true;
	if (m_mdllSet.pulserPos != pMDllSet->pulserPos)
		change = true;
	if (change)
	{
		MSG_DEBUG << "change in Pulser part";
		m_mdllSet.pulserOn = pMDllSet->pulserOn;
		m_mdllSet.pulserAmpl = pMDllSet->pulserAmpl;
		m_mdllSet.pulserPos = pMDllSet->pulserPos;
		m_mcpd->setPulser(m_mdllSet.pulserOn, m_mdllSet.pulserAmpl, m_mdllSet.pulserPos); 
//		theApp->sendBuffer(m_pCmdPacket->deviceId, 3);
	}
	    
// counter settings
	change = false;
	MSG_DEBUG << "counter 2: " << m_mdllSet.counterCell[2][0] << ' ' << m_mdllSet.counterCell[2][1] << ", new setting: "
						<< pMDllSet->counterCell[2][0] << ' ' << pMDllSet->counterCell[2][1];
	if (m_mdllSet.counterCell[0][0] != pMDllSet->counterCell[0][0])
		change = true;
	if (m_mdllSet.counterCell[1][0] != pMDllSet->counterCell[1][0])
		change = true;
	if (m_mdllSet.counterCell[2][0] != pMDllSet->counterCell[2][0])
		change = true;
	if (m_mdllSet.counterCell[0][1] != pMDllSet->counterCell[0][1])
		change = true;
	if (m_mdllSet.counterCell[1][1] != pMDllSet->counterCell[1][1])
		change = true;
	if(m_mdllSet.counterCell[2][1] != pMDllSet->counterCell[2][1])
		change = true;
	if (change || dontCheck)
	{
        	for(quint16 i = 0; i < 3; i++)
			m_mcpd->setCounterCell(i, pMDllSet->counterCell[i][0], pMDllSet->counterCell[i][1]);
	}
// auxtimer settings
	if (m_mdllSet.auxTimer[0] != pMDllSet->auxTimer[0])
		change = true;
	if (m_mdllSet.auxTimer[1] != pMDllSet->auxTimer[1])
		change = true;
	if (m_mdllSet.auxTimer[2] != pMDllSet->auxTimer[2])
		change = true;
	if (m_mdllSet.auxTimer[3] != pMDllSet->auxTimer[3])
		change = true;
	if (change || dontCheck)
	{
        	for(quint16 i = 0; i < 4; i++)
			m_mcpd->setAuxTimer(i, pMDllSet->auxTimer[i]);
	}
// parameter settings
	if (m_mdllSet.paramSource[0] != pMDllSet->paramSource[0])
		change = true;
	if (m_mdllSet.paramSource[1] != pMDllSet->paramSource[1])
		change = true;
	if (m_mdllSet.paramSource[2] != pMDllSet->paramSource[2])
		change = true;
	if (m_mdllSet.paramSource[3] != pMDllSet->paramSource[3])
		change = true;
	if (change || dontCheck)
	{
        	for(quint16 i = 0; i < 4; i++)
			m_mcpd->setParamSource(i, pMDllSet->paramSource[i]);
	}
}

/*!
    \fn void MDll::initMdll(void)

    initialize settings
 */
void MDll::initMdll(void)
{
	m_mcpd->setTimingSetup(m_mdllSet.master, m_mdllSet.terminate);
	m_mcpd->setSpectrum(m_mdllSet.shiftX, m_mdllSet.shiftY, m_mdllSet.scaleX, m_mdllSet.scaleY);
        for(quint16 i = 0; i < 3; i++)
		m_mcpd->setCounterCell(i, m_mdllSet.counterCell[i][0], m_mdllSet.counterCell[i][1]);
        for(quint16 i = 0; i < 4; i++)
		m_mcpd->setAuxTimer(i, m_mdllSet.auxTimer[i]);
	m_mcpd->setAcqset(m_mdllSet.eventLimit0 | (m_mdllSet.eventLimit1 << 16), 
			m_mdllSet.tsumXlo | (m_mdllSet.tsumXhi << 16),
			m_mdllSet.tsumYlo | (m_mdllSet.tsumYhi << 16));
	m_mcpd->setDataReg(m_mdllSet.datareg);
	m_mcpd->setEnergy(m_mdllSet.energyLow, m_mdllSet.energyHi, m_mdllSet.eScaleX, m_mdllSet.eScaleY);
	m_mcpd->setThreshold(m_mdllSet.threshX, m_mdllSet.threshY, m_mdllSet.threshA);
        for(quint16 i = 0; i < 4; i++)
		m_mcpd->setParamSource(i, m_mdllSet.paramSource[i]);
	m_mcpd->setHistogram(m_mdllSet.previewHistsize, m_mdllSet.previewHistrate, m_mdllSet.histSize, m_mdllSet.histType);
	m_mcpd->setMode(m_mdllSet.mode);
	m_mcpd->setSlide(m_mdllSet.slscOff);
}

/*!
    \fn void MDll::serialize(QFile * fi)

    \param fi
 */
void MDll::serialize(QFile *fi)
{
        quint8 c;
        QTextStream t( fi );        // use a text stream

        t << "[MDLL]";
        t << '\r' << '\n';
        t << "id = " << 0;
        t << '\r' << '\n';
#if defined(_MSC_VER)
#	pragma message("ToDo")
#else
#	warning TODO
#endif
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
