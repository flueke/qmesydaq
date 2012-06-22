/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2011 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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

#include <QFileDialog>

#include "mdefines.h"
#include "mdllsetup.h"
#include "mesydaq2.h"

/*!
    constructor

    \param mesy
    \param parent
 */
MdllSetup::MdllSetup(Mesydaq2 *mesy, QWidget *parent)
	: QDialog(parent)
	, m_theApp(mesy)
{
    setupUi(this);

    devid->setMCPDList(m_theApp->mcpdId());
    displaySlot(devid->value());
}

/*!
    \fn void MdllSetup::setMCPD(int)

    sets the MCPD

    \param id
 */
void MdllSetup::setMCPD(int id)
{
#if 0
    devid->setValue(id);
//  devid->setMCPDList(m_theApp->mcpdId());
    id = devid->value();
    module->setModuleList(m_theApp->mpsdId(id));
    int mid = module->value();

    checkChannel1Use->setChecked(m_theApp->active(id, mid, 0));
    checkChannel1Histogram->setChecked(m_theApp->histogram(id, mid, 0));
#endif
}

/*!
    \fn void MdllSetup::setPulserSlot()

    set pulser 
 */
void MdllSetup::setPulserSlot()
{
    quint8 amp = pulsamp->currentIndex();
    quint8 pos = pulspos->currentIndex();
    if(pulserBox->isChecked())
        m_theApp->setPulser(0, 0, 0, pos, amp, true);
    else
        m_theApp->setPulser(0, 0, 0, pos, amp, false);
}


/*!
    \fn void MdllSetup::setTimingSlot()

    callback to set the timing window
 */
void MdllSetup::setTimingSlot()
{
    quint16 xlo = txlo->value();
    quint16 xhi = txhi->value();
    quint16 ylo = tylo->value();
    quint16 yhi = tyhi->value();
    m_theApp->setMdllTimingWindow(0, xlo, xhi, ylo, yhi);
}

/*!
    \fn void MdllSetup::setSpectrumSlot()

    callback to set the spectrum
 */
void MdllSetup::setSpectrumSlot()
{
    quint8 shX = shiftX->value();
    quint8 shY = shiftY->value();
    quint8 scX = scaleX->value();
    quint8 scY = scaleY->value();
    m_theApp->setMdllSpectrum(0, shX, shY, scX, scY);
}

/*!
    \fn void MdllSetup::setThresholdsSlot()

    callback to set the thresholds
 */
void MdllSetup::setThresholdsSlot()
{
    quint8 tX = threshX->value();
    quint8 tY = threshY->value();
    quint8 tA = threshA->value();
    m_theApp->setMdllThresholds(0, tX, tY, tA);
}

/*!
    \fn void MdllSetup::setDatasetSlot()

    callback to set the data set
 */
void MdllSetup::setDatasetSlot()
{
    if(posButton->isChecked())
        m_theApp->setMdllDataset(0, 0);
    else
        m_theApp->setMdllDataset(0, 1);
}

/*!
    \fn void MdllSetup::setEnergySlot()

    callback to set the energy window
 */
void MdllSetup::setEnergySlot()
{
    quint8 eL = elo->value();
    quint8 eH = ehi->value();
    m_theApp->setMdllEnergyWindow(0, eL, eH);
}

/*!
    \fn void MdllSetup::displaySlot()

    callback to display the setup

    \param id
 */
void MdllSetup::displaySlot(int id)
{
    // dataset:
    if(m_theApp->getMdllDataset(id)){
        posButton->setChecked(false);
        timingButton->setChecked(true);
    }
    else{
        posButton->setChecked(true);
        timingButton->setChecked(false);
    }

    // timing window
    txlo->setValue(m_theApp->getMdllTimingWindow(id, 0));
    txhi->setValue(m_theApp->getMdllTimingWindow(id, 1));
    tylo->setValue(m_theApp->getMdllTimingWindow(id, 2));
    tyhi->setValue(m_theApp->getMdllTimingWindow(id, 3));

    // energy window
    elo->setValue(m_theApp->getMdllEnergyWindow(id, 0));
    ehi->setValue(m_theApp->getMdllEnergyWindow(id, 1));

    // spectrum parameters
    shiftX->setValue(m_theApp->getMdllSpectrum(id, 0));
    shiftY->setValue(m_theApp->getMdllSpectrum(id, 1));
    scaleX->setValue(m_theApp->getMdllSpectrum(id, 2));
    scaleY->setValue(m_theApp->getMdllSpectrum(id, 3));

    // CFD thresholds
    threshX->setValue(m_theApp->getMdllThresholds(id, 0));
    threshY->setValue(m_theApp->getMdllThresholds(id, 1));
    threshA->setValue(m_theApp->getMdllThresholds(id, 2));

    // pulser:  on/off
    if(m_theApp->getMdllPulser(id, 0))
    {
        pulserBox->setChecked(true);
    }
    else
    {
        pulserBox->setChecked(false);
    }
// channel
    pulspos->setCurrentIndex((int)m_theApp->getMdllPulser(id, 2));
// amplitude
    // channel
    pulsamp->setCurrentIndex((int)m_theApp->getMdllPulser(id, 1));
}
