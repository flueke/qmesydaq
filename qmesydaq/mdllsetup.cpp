/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2014 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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
#include "detector.h"

/*!
    constructor

    \param mesy
    \param parent
 */
MdllSetup::MdllSetup(Detector *detector, QWidget *parent)
	: QDialog(parent)
	, m_detector(detector)
{
	setupUi(this);

	QList<int> mcpdList = m_detector->mcpdId();
	bool noModule = mcpdList.isEmpty();
	tabWidget->setDisabled(noModule);

	devid->setMCPDList(mcpdList);
	displaySlot(devid->value());
}

/*!
    \fn void MdllSetup::setMCPD(int)

    sets the MCPD

    \param id
 */
void MdllSetup::setMCPD(int /* id */)
{
#if 0
	devid->setValue(id);
//	devid->setMCPDList(m_detector->mcpdId());
	id = devid->value();
	module->setModuleList(m_detector->mpsdId(id));
	int mid = module->value();

	checkChannel1Use->setChecked(m_detector->active(id, mid, 0));
	checkChannel1Histogram->setChecked(m_detector->histogram(id, mid, 0));
#endif
	displaySlot(devid->value());
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
	m_detector->setMdllTimingWindow(0, xlo, xhi, ylo, yhi);
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
	m_detector->setMdllSpectrum(0, shX, shY, scX, scY);
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
	m_detector->setMdllThresholds(0, tX, tY, tA);
}

/*!
    \fn void MdllSetup::setDatasetSlot()

    callback to set the data set
 */
void MdllSetup::setDatasetSlot()
{
	if(posButton->isChecked())
		m_detector->setMdllDataset(0, 0);
 	else
		m_detector->setMdllDataset(0, 1);
}

/*!
    \fn void MdllSetup::setEnergySlot()

    callback to set the energy window
 */
void MdllSetup::setEnergySlot()
{
	quint8 eL = elo->value();
	quint8 eH = ehi->value();
	m_detector->setMdllEnergyWindow(0, eL, eH);
}

/*!
    \fn void MdllSetup::displaySlot()

    callback to display the setup

    \param id
 */
void MdllSetup::displaySlot(int id)
{
	bool mdll = m_detector->getModuleId(id, 0) == TYPE_MDLL;
	tabWidget->setTabEnabled(tabWidget->indexOf(dataTab), mdll);
	tabWidget->setTabEnabled(tabWidget->indexOf(hardwareTab), mdll);
//	dataTab->setVisible(mdll);
//	hardwareTab->setVisible(mdll);
	if (!mdll)
		return;
// dataset:
	if(m_detector->getMdllDataset(id))
	{
		posButton->setChecked(false);
		timingButton->setChecked(true);
	}
	else
	{
		posButton->setChecked(true);
		timingButton->setChecked(false);
	}

// timing window
	txlo->setValue(m_detector->getMdllTimingWindow(id, 0));
	txhi->setValue(m_detector->getMdllTimingWindow(id, 1));
	tylo->setValue(m_detector->getMdllTimingWindow(id, 2));
	tyhi->setValue(m_detector->getMdllTimingWindow(id, 3));

// energy window
	elo->setValue(m_detector->getMdllEnergyWindow(id, 0));
	ehi->setValue(m_detector->getMdllEnergyWindow(id, 1));

// spectrum parameters
	shiftX->setValue(m_detector->getMdllSpectrum(id, 0));
	shiftY->setValue(m_detector->getMdllSpectrum(id, 1));
	scaleX->setValue(m_detector->getMdllSpectrum(id, 2));
	scaleY->setValue(m_detector->getMdllSpectrum(id, 3));

// CFD thresholds
	threshX->setValue(m_detector->getMdllThresholds(id, 0));
	threshY->setValue(m_detector->getMdllThresholds(id, 1));
	threshA->setValue(m_detector->getMdllThresholds(id, 2));
}
