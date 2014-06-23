/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2013 by Jens Krüger <jens.krueger@frm2.tum.de>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Module Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Module Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Module Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "mdefines.h"
#include "mcpdsetup.h"
#include "mesydaq2.h"
#include "logging.h"

/*!
    constructor

    \param mesy
    \param parent
 */
MCPDSetup::MCPDSetup(Mesydaq2 *mesy, QWidget *parent)
	 : QDialog(parent)
	, m_theApp(mesy)
{
    setupUi(this);

    QList<int> mcpdList = m_theApp->mcpdId();
    bool noModule = mcpdList.isEmpty();
    tabWidget->setDisabled(noModule);

    cellCompare->setDisabled(true);
    connect(cellTrigger, SIGNAL(currentIndexChanged(int)), this, SLOT(cellTriggerChangedSlot(int)));
//  listfilepath->setText(mesy->getListfilepath());

    dataIPAddress->setAddress("192.168.168.005");
    cmdIPAddress->setAddress("192.168.168.005");

    mcpdId->setMCPDList(mcpdList);
    moduleNr->setMaximum(mcpdList.size());
    if (!noModule)
	    moduleNr->setMinimum(1);
    displayAuxTimerSlot(1);
    displayMCPDSlot(-1);
}

/*!
    \fn void MCPDSetup::sendCellSlot()

    callback to send the cell setup
 */
void MCPDSetup::sendCellSlot()
{
    quint16 id = mcpdId->value();
    m_theApp->setCounterCell(id, cellSource->currentIndex(), cellTrigger->currentIndex(), cellCompare->currentIndex());
}

/*!
    \fn void MCPDSetup::sendParamSlot()

    callback to send the parameter setup
 */
void MCPDSetup::sendParamSlot()
{
    qint16 id = mcpdId->value();
    m_theApp->setParamSource(id, param->value(), paramSource->currentIndex());
}

/*!
    \fn void MCPDSetup::sendAuxSlot()

    callback to send the auxiliary timer setup
 */
void MCPDSetup::sendAuxSlot()
{
    bool ok;
    quint16 compare = compareAux->cleanText().toUInt(&ok, 10);
    if (ok)
        m_theApp->setAuxTimer(mcpdId->value(), timer->value() - 1, compare);
}

/*!
    \fn void MCPDSetup::resetTimerSlot()

    callback to reset the master timer clock
 */
void MCPDSetup::resetTimerSlot()
{
    quint16 id = mcpdId->value();
    MSG_NOTICE << tr("reset timer");
    m_theApp->setMasterClock(id, 0LL);
}

/*!
    \fn void MCPDSetup::setTimingSlot()

    callback to send the timing setup
 */
void MCPDSetup::setTimingSlot()
{
    quint16 id = mcpdId->value();
    resetTimer->setEnabled(master->isChecked());
    MSG_NOTICE << tr("set timing");
    m_theApp->setTimingSetup(id, master->isChecked(), terminate->isChecked(), extsync->isChecked());
    if (master->isChecked())
        emit sync(extsync->isChecked());
}

/*!
    \fn void MCPDSetup::setMcpdIdSlot()

    callback to set the MCPD ID
 */
void MCPDSetup::setMcpdIdSlot()
{
    m_theApp->setId(mcpdId->value(), deviceId->value());
}

/*!
    \fn void MCPDSetup::setIpUdpSlot()

    callback to send the networking setup of the MCPD
 */
void MCPDSetup::setIpUdpSlot()
{
    quint16 id =  deviceId->value();
    QString mcpdIP = modifyIp->isChecked() ? mcpdIPAddress->getAddress() : "0.0.0.0",
    cmdIP = !cmdThisPc->isChecked() ? cmdIPAddress->getAddress() : "0.0.0.0",
    dataIP = !dataThisPc->isChecked() ? dataIPAddress->getAddress() : "0.0.0.0";
    quint16 cmdPort = (quint16) cmdUdpPort->value(),
    dataPort = (quint16) dataUdpPort->value();
    m_theApp->setProtocol(id, mcpdIP, dataIP, dataPort, cmdIP, cmdPort);
}

/*!
    \fn void MCPDSetup::displayMCPDSlot(int id)
   
    display the information if the MCPD was changed

    \param id  ID of the MCPD
 */
void MCPDSetup::displayMCPDSlot(int id)
{
// retrieve displayed ID
    if (!m_theApp->numMCPD())
        return;
    if (id < 0)
        id = mcpdId->value();

// store the current termination value it will be change if switch from master to slave
    master->setChecked(m_theApp->isMaster(id));
    if (!master->isChecked())
        terminate->setChecked(m_theApp->isTerminated(id));
    else
        extsync->setChecked(m_theApp->isExtsynced(id));

    displayCounterCellSlot(-1);
    displayParameterSlot(-1);
    displayAuxTimerSlot(-1);
}

/*!
    \fn void MCPDSetup::displayCounterCellSlot(int id)

    display the counter/cell setup

    \param id number of the counter cell
 */
void MCPDSetup::displayCounterCellSlot(int id)
{
// now get and display parameters:
    quint16 values[4];
   
    if (id < 0)
        id = cellSource->currentIndex();
// get cell parameters
    m_theApp->getCounterCell(mcpdId->value(), id, values);
    cellTrigger->setCurrentIndex(values[0]);
    cellCompare->setCurrentIndex(values[1]);
}

/*!    
    \fn void MCPDSetup::displayParameterSlot(int id)

    \param id
 */
void MCPDSetup::displayParameterSlot(int id)
{
    if (id < 0)
        id = param->value();
// get parameter settings
    paramSource->setCurrentIndex(m_theApp->getParamSource(mcpdId->value(), id));
}

/*!
    \fn void MCPDSetup::displayAuxTimerSlot(int id)
    
    \param id
 */
void MCPDSetup::displayAuxTimerSlot(int id)
{
    if (id < 0)
        id = timer->value();
// get timer settings
    compareAux->setValue(m_theApp->getAuxTimer(mcpdId->value(), id - 1));

// get stream setting
//  statusStream->setChecked(m_theApp->myMcpd[id]->getStream());
}

/*!
    \fn void MCPDSetup::cellTriggerChangedSlot(int index)

    callback for the cell trigger change
 */
void MCPDSetup::cellTriggerChangedSlot(int index)
{
    cellCompare->setEnabled(index);
}

/*!
    \fn void MCPDSetup::setSyncSlot(bool master)

    callback for the master change

    \deprecated
 */
void MCPDSetup::setSyncSlot(bool master)
{
    extsync->setEnabled(master);
    if(!master)
        extsync->setChecked(false);
}

/*!
    \fn void MCPDSetup::setModule(int modNr)

    callback for the module number change

    \param modNr number of the module

 */
void MCPDSetup::setModule(int modNr)
{
    QList<int> mcpdList = m_theApp->mcpdId();

    int id = mcpdList[modNr - 1];

    QString mcpdIP,
            dataIP,
            cmdIP;
    quint16 dataPort,
            cmdPort;

    m_theApp->getProtocol(id, mcpdIP, dataIP, dataPort, cmdIP, cmdPort);
    MSG_DEBUG << tr("%1 %2(%3) %4(%5)").arg(mcpdIP).arg(dataIP).arg(dataPort).arg(cmdIP).arg(cmdPort);

    mcpdIPAddress->setAddress(mcpdIP);
    if (!dataIP.isEmpty())
        dataIPAddress->setAddress(dataIP);
    if (!cmdIP.isEmpty())
        cmdIPAddress->setAddress(cmdIP);

    deviceId->setValue(id);
}
