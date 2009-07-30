/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann   *
 *   g.montermann@mesytec.com   *
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
#include "controlinterface.h"
#include "mesydaq2.h"
#include "mdefines.h"
#include "mainwidget.h"
#include "measurement.h"

ControlInterface::ControlInterface(QObject *parent)
	: QObject(parent)
	, m_asyncTaskPending(false)
	, m_caressTaskPending(false)
	, m_caressTaskNum(0)
{
	m_theApp = (Mesydaq2*) parent;
	m_transferBuffer = new(ulong[512*960]);
}

ControlInterface::~ControlInterface()
{
}


/*!
    \fn controlInterface::caressTask()
 */
void ControlInterface::caressTask()
{
	quint8 devMap[5] = {0, M1CT, TCT, M2CT, EVCT};
	
	QString pstring, str;
	pstring.sprintf("caress: ");
	
	if(m_caressDevice == HISTO)
	{
    		pstring.append("histogram ");
	}
	else
	{
   		str.sprintf("counter %d (mesydaq #%d) ", m_caressDevice, devMap[m_caressDevice]);
   		pstring.append(str);
	}
	
	switch(m_caressTaskNum)
	{
   		case CAR_INIT:
    			pstring.append("init.");
			if(m_caressDevice == HISTO)
			{
#warning TODO 			m_theApp->meas->setCarHistSize(m_caressHeight, m_caressWidth);
				m_caressHistoSize = m_caressHeight * m_caressWidth;
			}
			else
#warning TODO			m_theApp->meas->clearCounter(devMap[m_caressDevice]);
#warning TODO 		m_theApp->mainWin->update();
			break;
    	
		case CAR_RELEASE:
			pstring.append("release.");
			break;
    	
		case CAR_START:
   			if(m_caressDevice == m_caressMaster)
			{
   				// only start on master start!
   				m_asyncTaskPending = true;
   				m_theApp->start();
				pstring.append("start master.");
			}
			else
				// just notice, don't start yet...
				pstring.append("start.");
			break;

		case CAR_STOP:
   			if(m_caressDevice == m_caressMaster)
			{
   				// only if daq not already stopped (e.g. by preset)
   				if(m_theApp->isDaq() == RUNNING)
				{
					// only stop on master start!
					m_asyncTaskPending = true;
					m_theApp->stop();
   				}
   				else
   					m_caressTaskPending = false;
				pstring.append("stop master.");
			}
			else
				// just notice, don't stop yet...
				pstring.append("stop.");
			break;
    	
		case CAR_DRIVE:
			pstring.append("drive ");
			break;
    	
		case CAR_LOAD:
			if(m_caressDevice == HISTO)
			{
				pstring.append("load.");
#warning TODO			m_theApp->clearAllHist();
			}
			else
			{
				if(m_caressSubTaskNum == CAR_SLAVE)
				{
					pstring.append("load slave");
				}
				if(m_caressSubTaskNum == CAR_MASTER)
				{
#warning TODO				m_theApp->meas->setPreset(devMap[m_caressDevice], m_caressPreset, true); 
					pstring.append("load master preset: ");
					str.sprintf("%ld", m_caressPreset);
					pstring.append(str);
#warning TODO 				m_theApp->mainWin->updatePresets();
					m_caressMaster = m_caressDevice; 
				}
				if(m_caressSubTaskNum == CAR_RESET)
				{
#warning TODO				m_theApp->meas->clearCounter(devMap[m_caressDevice]); 
					pstring.append("(reset / load)");
#warning TODO 				m_theApp->mainWin->updatePresets();
				}
			}
			break;
    	
		case CAR_LOADBLOCK:
			pstring.append("loadblock.");
			break;
    	
		case CAR_READ:
			pstring.append("read.");
			break;
    	
		case CAR_READBLOCKP:
			pstring.append("readblock param.");
			break;
    	
		case CAR_READBLOCKM:
			ulong start, stop, rows;
			start = stop = rows = 0;
			if(m_caressDevice == HISTO)
			{
				// check first channel
				start = m_caressStartChannel - 1;
				start -= start % 960;
				start = start / 960;
				// check last channel
				stop = m_caressEndChannel - 1;
				stop -= stop % 960;
				stop = stop / 960;
				rows = stop - start;
				if(rows == 0)
					rows = 1;
			}
			// now copy requested amount of data into transfer buffer
			for(quint8 c = 0; c < rows; c++)
			{		
#warning TODO			m_theApp->copyData(c+start, &m_transferBuffer[c*960]);
			}
			pstring.append("readblock module start: ");
			str.sprintf("%ld",m_caressStartChannel);
			pstring.append(str);
			str.sprintf(" / %ld",start);
			pstring.append(str);
			pstring.append(" stop: ");
			str.sprintf("%ld",m_caressEndChannel);
			pstring.append(str);
			str.sprintf(" / %ld",stop);
			pstring.append(str);
			str.sprintf(" rows: %ld",rows);
			pstring.append(str);
			break;
    	
		default:
			pstring.sprintf("ERROR: invalid task number!");
			break;
	}
	if(m_caressTaskNum != CAR_READ)
		m_theApp->protocol(pstring, 1);
//	if(m_caressTaskNum != CAR_START && m_caressTaskNum != CAR_STOP)
	m_caressTaskPending = false;
}

/*!
    \fn controlInterface::isActive(void)
 */
bool ControlInterface::isActive(void)
{
	return m_caressTaskPending;
}


/*!
    \fn controlInterface::completeCar()
 */
void ControlInterface::completeCar()
{
	m_caressTaskPending = false;
	m_asyncTaskPending = false;
	m_caressTaskNum = 0;
}

