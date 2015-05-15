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
#include "mdefines.h"
#include "modulesetup.h"
#include "mesydaq2.h"

#include "logging.h"

/*!
 *  constructor
 *
 *  \param mesy
 *  \param parent
 */
ModuleSetup::ModuleSetup(Mesydaq2 *mesy, QWidget *parent)
	: QDialog(parent)
	, m_theApp(mesy)
{
	setupUi(this);

	m_label[0] = labelChannel_1;
	m_label[1] = labelChannel_2;
	m_label[2] = labelChannel_3;
	m_label[3] = labelChannel_4;
	m_label[4] = labelChannel_5;
	m_label[5] = labelChannel_6;
	m_label[6] = labelChannel_7;
	m_label[7] = labelChannel_8;
	m_label[8] = labelChannel_9;
	m_label[9] = labelChannel_10;
	m_label[10] = labelChannel_11;
	m_label[11] = labelChannel_12;
	m_label[12] = labelChannel_13;
	m_label[13] = labelChannel_14;
	m_label[14] = labelChannel_15;
	m_label[15] = labelChannel_16;

	m_histogram[0] = checkChannel1Histogram;
	m_histogram[1] = checkChannel2Histogram;
	m_histogram[2] = checkChannel3Histogram;
	m_histogram[3] = checkChannel4Histogram;
	m_histogram[4] = checkChannel5Histogram;
	m_histogram[5] = checkChannel6Histogram;
	m_histogram[6] = checkChannel7Histogram;
	m_histogram[7] = checkChannel8Histogram;
	m_histogram[8] = checkChannel9Histogram;
	m_histogram[9] = checkChannel10Histogram;
	m_histogram[10] = checkChannel11Histogram;
	m_histogram[11] = checkChannel12Histogram;
	m_histogram[12] = checkChannel13Histogram;
	m_histogram[13] = checkChannel14Histogram;
	m_histogram[14] = checkChannel15Histogram;
	m_histogram[15] = checkChannel16Histogram;

	m_active[0] = checkChannel1Use;
	m_active[1] = checkChannel2Use;
	m_active[2] = checkChannel3Use;
	m_active[3] = checkChannel4Use;
	m_active[4] = checkChannel5Use;
	m_active[5] = checkChannel6Use;
	m_active[6] = checkChannel7Use;
	m_active[7] = checkChannel8Use;
	m_active[8] = checkChannel9Use;
	m_active[9] = checkChannel10Use;
	m_active[10] = checkChannel11Use;
	m_active[11] = checkChannel12Use;
	m_active[12] = checkChannel13Use;
	m_active[13] = checkChannel14Use;
	m_active[14] = checkChannel15Use;
	m_active[15] = checkChannel16Use;

	QObject::connect(checkChannel1Histogram, SIGNAL(toggled(bool)), this, SLOT(setHistogram1(bool)));
	QObject::connect(checkChannel2Histogram, SIGNAL(toggled(bool)), this, SLOT(setHistogram2(bool)));
	QObject::connect(checkChannel3Histogram, SIGNAL(toggled(bool)), this, SLOT(setHistogram3(bool)));
	QObject::connect(checkChannel4Histogram, SIGNAL(toggled(bool)), this, SLOT(setHistogram4(bool)));
	QObject::connect(checkChannel5Histogram, SIGNAL(toggled(bool)), this, SLOT(setHistogram5(bool)));
	QObject::connect(checkChannel6Histogram, SIGNAL(toggled(bool)), this, SLOT(setHistogram6(bool)));
	QObject::connect(checkChannel7Histogram, SIGNAL(toggled(bool)), this, SLOT(setHistogram7(bool)));
	QObject::connect(checkChannel8Histogram, SIGNAL(toggled(bool)), this, SLOT(setHistogram8(bool)));
	QObject::connect(checkChannel9Histogram, SIGNAL(toggled(bool)), this, SLOT(setHistogram9(bool)));
	QObject::connect(checkChannel10Histogram, SIGNAL(toggled(bool)), this, SLOT(setHistogram10(bool)));
	QObject::connect(checkChannel11Histogram, SIGNAL(toggled(bool)), this, SLOT(setHistogram11(bool)));
	QObject::connect(checkChannel12Histogram, SIGNAL(toggled(bool)), this, SLOT(setHistogram12(bool)));
	QObject::connect(checkChannel13Histogram, SIGNAL(toggled(bool)), this, SLOT(setHistogram13(bool)));
	QObject::connect(checkChannel14Histogram, SIGNAL(toggled(bool)), this, SLOT(setHistogram14(bool)));
	QObject::connect(checkChannel15Histogram, SIGNAL(toggled(bool)), this, SLOT(setHistogram15(bool)));
	QObject::connect(checkChannel16Histogram, SIGNAL(toggled(bool)), this, SLOT(setHistogram16(bool)));

	QObject::connect(checkChannel1Use, SIGNAL(toggled(bool)), this, SLOT(setActive1(bool)));
	QObject::connect(checkChannel2Use, SIGNAL(toggled(bool)), this, SLOT(setActive2(bool)));
	QObject::connect(checkChannel3Use, SIGNAL(toggled(bool)), this, SLOT(setActive3(bool)));
	QObject::connect(checkChannel4Use, SIGNAL(toggled(bool)), this, SLOT(setActive4(bool)));
	QObject::connect(checkChannel5Use, SIGNAL(toggled(bool)), this, SLOT(setActive5(bool)));
	QObject::connect(checkChannel6Use, SIGNAL(toggled(bool)), this, SLOT(setActive6(bool)));
	QObject::connect(checkChannel7Use, SIGNAL(toggled(bool)), this, SLOT(setActive7(bool)));
	QObject::connect(checkChannel8Use, SIGNAL(toggled(bool)), this, SLOT(setActive8(bool)));
	QObject::connect(checkChannel9Use, SIGNAL(toggled(bool)), this, SLOT(setActive9(bool)));
	QObject::connect(checkChannel10Use, SIGNAL(toggled(bool)), this, SLOT(setActive10(bool)));
	QObject::connect(checkChannel11Use, SIGNAL(toggled(bool)), this, SLOT(setActive11(bool)));
	QObject::connect(checkChannel12Use, SIGNAL(toggled(bool)), this, SLOT(setActive12(bool)));
	QObject::connect(checkChannel13Use, SIGNAL(toggled(bool)), this, SLOT(setActive13(bool)));
	QObject::connect(checkChannel14Use, SIGNAL(toggled(bool)), this, SLOT(setActive14(bool)));
	QObject::connect(checkChannel15Use, SIGNAL(toggled(bool)), this, SLOT(setActive15(bool)));
	QObject::connect(checkChannel16Use, SIGNAL(toggled(bool)), this, SLOT(setActive16(bool)));

	channelLabel->setHidden(comgain->isChecked());
	channel->setHidden(comgain->isChecked());

	QList<int> mcpdList = m_theApp->mcpdId();
	bool noModule = mcpdList.isEmpty();
	tabWidget->setDisabled(noModule);

	devid->setMCPDList(mcpdList);
	QList<int> modules = m_theApp->mpsdId(devid->value());
	module->setModuleList(modules);
	module->setDisabled(modules.empty());
	histogramGroupBox->setDisabled(modules.empty());

	for (int i = 0; i < 16; ++i)
	{
		m_histogram[i]->setChecked(m_theApp->histogram(devid->value(), module->value(), i));
		m_active[i]->setChecked(m_theApp->active(devid->value(), module->value(), i));
	}

	channel->setModuleList(m_theApp->channelId(devid->value(), module->value()));

	displaySlot();
}

/*!
 *  \fn void ModuleSetup::setGainSlot()
 *
 *  callback to handle the gain settings
 */
void ModuleSetup::setGainSlot()
{
	bool	ok;
	quint16	id = (quint16) devid->value();
	quint16	addr = module->value();
	quint16	modType = m_theApp->getModuleId(id, addr);
	quint16	chan = comgain->isChecked() ? (modType == TYPE_MSTD16 ? 16 : 8) : channel->text().toUInt(&ok, 0);
	float 	gainval = gain->value();
#if 0
	m_theApp->setGain(id, addr, chan, gainval);
#else
	m_theApp->setGain(id, addr, chan, quint8(gainval));
#endif
}

/*!
 *   \fn void ModuleSetup::setThresholdSlot()
 *
 *  callback to handle the threshold settings
 */
void ModuleSetup::setThresholdSlot()
{
	quint16 id = (quint16) devid->value();
	quint16 addr = module->value();
	quint16 thresh = threshold->value();

	m_theApp->setThreshold(id, addr, thresh);
}

/*!
 *  \fn void ModuleSetup::readRegisterSlot()
 *
 *  callback to read from a MPSD register
 *  //! \todo display read values
 */
void ModuleSetup::readRegisterSlot()
{
#if defined(_MSC_VER)
#	pragma message("TODO display read values")
#else
#	warning TODO display read values
#endif
	quint16 id = (quint16) devid->value();
	quint16 addr = module->value();
	quint16 reg = registerSelect->value();

	m_theApp->readPeriReg(id, addr, reg);
}

/*!
 *  \fn void ModuleSetup::writeRegisterSlot()
 *
 *  callback to write into a MPSD register
 */
void ModuleSetup::writeRegisterSlot()
{
	bool	ok;
	quint16 id = (quint16) devid->value();
	quint16 addr = module->value();
	quint16 reg = registerSelect->value();
	quint16 val = registerValue->text().toUInt(&ok, 0);

	m_theApp->writePeriReg(id, addr, reg, val);
}

/*!
 *  \fn void ModuleSetup::setModeSlot()
 *
 *  callback to handle the settings of the amplitude/position mode
 *
 *  \param mode
 */
void ModuleSetup::setModeSlot()
{
	int mod = module->value();
	if (comAmp->isChecked())
		mod = 8;
	m_theApp->setMode(devid->value(), mod, ampMode->isChecked());
}

/*!
 *  \fn void ModuleSetup::setModule(int)
 *
 *  callback to handle the id setting of the module
 *
 *  \param id
 */
void ModuleSetup::setModule(int id)
{
	module->setValue(id);
}

/*!
 *  \fn void ModuleSetup::setMCPD(int)
 *
 *  callback to display the found module configuration
 *
 *  \param id
 */
void ModuleSetup::setMCPD(int id)
{
	devid->setValue(id);
//	devid->setMCPDList(m_theApp->mcpdId());
	id = devid->value();
	QList<int> modules = m_theApp->mpsdId(id);
	module->setModuleList(modules);
	module->setDisabled(modules.empty());
	histogramGroupBox->setDisabled(modules.empty());
	int mid = module->value();

	for (int i = 0; i < 8; ++i)
	{
		m_active[i]->setChecked(m_theApp->active(id, mid, i));
		m_histogram[i]->setChecked(m_theApp->histogram(id, mid, i));
	}
	if (m_theApp->getModuleId(id, mid) == TYPE_MSTD16)
		for (int i = 8; i < 16; ++i)
		{
			m_active[i]->setChecked(m_theApp->active(id, mid, i));
			m_histogram[i]->setChecked(m_theApp->histogram(id, mid, i));
		}

	displayMCPDSlot();
}

/*!
 *  \fn void ModuleSetup::displayMCPDSlot(int id)
 *
 *  callback to handle the change of the MCPD ID by the user
 *
 *  \param id new MCPD ID
 */
void ModuleSetup::displayMCPDSlot(int id)
{
	if (id < 0)
		id = devid->value();

	QList<int> modList;
	for (int i = 0; i < 8; ++i)
		if (m_theApp->getModuleId(id, i))
			modList << i;
	module->setModuleList(modList);
	module->setDisabled(modList.empty());
	histogramGroupBox->setDisabled(modList.empty());
	displaySlot();
}

/*!
 *  \fn void ModuleSetup::displayMPSDSlot(int id)
 *
 *  callback to handle the change of the MPSD by the user
 *
 *  \param id new module id
 */
void ModuleSetup::displayMPSDSlot(int id)
{
	if (id < 0)
		id = 0;
	channel->setModuleList(m_theApp->channelId(devid->value(), id));
	displaySlot();
}

/*!
 *  \fn void ModuleSetup::displaySlot()
 *
 *  callback to display the settings of the module
 */
void ModuleSetup::displaySlot()
{
	quint8 mod = devid->value();
	quint8 id = module->value();
	int modType = m_theApp->getModuleId(mod, id);
	quint8 chan = comgain->isChecked() ? (modType == TYPE_MSTD16 ? 16 : 8) : channel->value();
	posMode->setEnabled(modType != TYPE_MPSD8P && modType != TYPE_MDLL);
	ampMode->setEnabled(modType != TYPE_MPSD8P && modType != TYPE_MDLL);
//	channel->setMaximum((modType == TYPE_MSTD16 ? 15 : 7));

	for (int i = 8; i < 16; ++i)
	{
		m_label[i]->setVisible(modType == TYPE_MSTD16);
		m_histogram[i]->setVisible(modType == TYPE_MSTD16);
		m_active[i]->setVisible(modType == TYPE_MSTD16);
	}

// gain:
	gain->blockSignals(true);
	gain->setValue(m_theApp->getGain(mod, id, chan));
	gain->blockSignals(false);

// threshold:
	threshold->blockSignals(true);
	threshold->setValue(m_theApp->getThreshold(mod, id));
	threshold->blockSignals(false);

// mode:
	if (m_theApp->getMode(mod, id))
		ampMode->setChecked(true);

// histogram/active:
	int modules = (modType == TYPE_MSTD16) ? 16 : 8;
	for (int i = 0; i < modules; ++i)
	{
		m_histogram[i]->setChecked(m_theApp->active(mod, id, i));
		m_active[i]->setChecked(m_theApp->histogram(mod, id, i));
	}
}

/*!
 *  \fn void ModuleSetup::setHistogram1(bool hist)
 *
 *  callback to handle the histogram settings of channel 1
 *
 *  \param hist
 */
void ModuleSetup::setHistogram1(bool hist)
{
	m_theApp->setHistogram(devid->value(), module->value(), 0, hist);
}

/*!
 *  \fn void ModuleSetup::setActive1(bool act)
 *
 *  callback to handle the activation settings of channel 1
 *
 *  \param act
 */
void ModuleSetup::setActive1(bool act)
{
	m_theApp->setActive(devid->value(), module->value(), 0, act);
}

/*!
 *  \fn void ModuleSetup::setHistogram2(bool hist)
 *
 *  callback to handle the histogram settings of channel 2
 *
 *  \param hist
 */
void ModuleSetup::setHistogram2(bool hist)
{
	m_theApp->setHistogram(devid->value(), module->value(), 1, hist);
}

/*!
 *  \fn void ModuleSetup::setActive2(bool act)
 *
 *  callback to handle the activation settings of channel 2
 *
 *  \param act
 */
void ModuleSetup::setActive2(bool act)
{
	m_theApp->setActive(devid->value(), module->value(), 1, act);
}

/*!
 *  \fn void ModuleSetup::setHistogram3(bool hist)
 *
 *  callback to handle the histogram settings of channel 3
 *
 *  \param hist
 */
void ModuleSetup::setHistogram3(bool hist)
{
	m_theApp->setHistogram(devid->value(), module->value(), 2, hist);
}

/*!
 *  \fn void ModuleSetup::setActive3(bool act)
 *
 *  callback to handle the activation settings of channel 3
 *
 *  \param act
 */
void ModuleSetup::setActive3(bool act)
{
	m_theApp->setActive(devid->value(), module->value(), 2, act);
}

/*!
 *  \fn void ModuleSetup::setHistogram4(bool hist)
 *
 *  callback to handle the histogram settings of channel 4
 *
 *  \param hist
 */
void ModuleSetup::setHistogram4(bool hist)
{
	m_theApp->setHistogram(devid->value(), module->value(), 3, hist);
}

/*!
 *  \fn void ModuleSetup::setActive4(bool act)
 *
 *  callback to handle the activation settings of channel 4
 *
 *  \param act
 */
void ModuleSetup::setActive4(bool act)
{
	m_theApp->setActive(devid->value(), module->value(), 3, act);
}

/*!
 *  \fn void ModuleSetup::setHistogram5(bool hist)
 *
 *  callback to handle the histogram settings of channel 5
 *
 *  \param hist
 */
void ModuleSetup::setHistogram5(bool hist)
{
	m_theApp->setHistogram(devid->value(), module->value(), 4, hist);
}

/*!
 *  \fn void ModuleSetup::setActive5(bool act)
 *
 *  callback to handle the activation settings of channel 5
 *
 *  \param act
 */
void ModuleSetup::setActive5(bool act)
{
	m_theApp->setActive(devid->value(), module->value(), 4, act);
}

/*!
 *  \fn void ModuleSetup::setHistogram6(bool hist)
 *
 *  callback to handle the histogram settings of channel 6
 *
 *  \param hist
 */
void ModuleSetup::setHistogram6(bool hist)
{
	m_theApp->setHistogram(devid->value(), module->value(), 5, hist);
}

/*!
 *  \fn void ModuleSetup::setActive6(bool act)
 *
 *  callback to handle the activation settings of channel 6
 *
 *  \param act
 */
void ModuleSetup::setActive6(bool act)
{
	m_theApp->setActive(devid->value(), module->value(), 5, act);
}

/*!
 *  \fn void ModuleSetup::setHistogram7(bool hist)
 *
 *  callback to handle the histogram settings of channel 7
 *
 *  \param hist
 */
void ModuleSetup::setHistogram7(bool hist)
{
	m_theApp->setHistogram(devid->value(), module->value(), 6, hist);
}

/*!
 *  \fn void ModuleSetup::setActive7(bool act)
 *
 *  callback to handle the activation settings of channel 7
 *
 *  \param act
 */
void ModuleSetup::setActive7(bool act)
{
	m_theApp->setActive(devid->value(), module->value(), 6, act);
}

/*!
 *  \fn void ModuleSetup::setHistogram8(bool hist)
 *
 *  callback to handle the histogram settings of channel 8
 *
 *  \param hist
 */
void ModuleSetup::setHistogram8(bool hist)
{
	m_theApp->setHistogram(devid->value(), module->value(), 7, hist);
}

/*!
 *  \fn void ModuleSetup::setActive8(bool act)
 *
 *  callback to handle the activation settings of channel 8
 *
 *  \param act
 */
void ModuleSetup::setActive8(bool act)
{
	m_theApp->setActive(devid->value(), module->value(), 7, act);
}

/*!
 *  \fn void ModuleSetup::setHistogram9(bool hist)
 *
 *  callback to handle the histogram settings of channel 9
 *
 *  \param hist
 */
void ModuleSetup::setHistogram9(bool hist)
{
	m_theApp->setHistogram(devid->value(), module->value(), 8, hist);
}

/*!
 *  \fn void ModuleSetup::setActive9(bool act)
 *
 *  callback to handle the activation settings of channel 9
 *
 *  \param act
 */
void ModuleSetup::setActive9(bool act)
{
	m_theApp->setActive(devid->value(), module->value(), 8, act);
}

/*!
 *  \fn void ModuleSetup::setHistogram10(bool hist)
 *
 *  callback to handle the histogram settings of channel 10
 *
 *  \param hist
 */
void ModuleSetup::setHistogram10(bool hist)
{
	m_theApp->setHistogram(devid->value(), module->value(), 9, hist);
}

/*!
 *  \fn void ModuleSetup::setActive10(bool act)
 *
 *  callback to handle the activation settings of channel 10
 *
 *  \param act
 */
void ModuleSetup::setActive10(bool act)
{
	m_theApp->setActive(devid->value(), module->value(), 9, act);
}

/*!
 *  \fn void ModuleSetup::setHistogram11(bool hist)
 *
 *  callback to handle the histogram settings of channel 11
 *
 *  \param hist
 */
void ModuleSetup::setHistogram11(bool hist)
{
	m_theApp->setHistogram(devid->value(), module->value(), 10, hist);
}

/*!
 *  \fn void ModuleSetup::setActive11(bool act)
 *
 *  callback to handle the activation settings of channel 11
 *
 *  \param act
 */
void ModuleSetup::setActive11(bool act)
{
	m_theApp->setActive(devid->value(), module->value(), 10, act);
}

/*!
 *  \fn void ModuleSetup::setHistogram12(bool hist)
 *
 *  callback to handle the histogram settings of channel 12
 *
 *  \param hist
 */
void ModuleSetup::setHistogram12(bool hist)
{
	m_theApp->setHistogram(devid->value(), module->value(), 11, hist);
}

/*!
 *  \fn void ModuleSetup::setActive12(bool act)
 *
 *  callback to handle the activation settings of channel 12
 *
 *  \param act
 */
void ModuleSetup::setActive12(bool act)
{
	m_theApp->setActive(devid->value(), module->value(), 11, act);
}

/*!
 *  \fn void ModuleSetup::setHistogram13(bool hist)
 *
 *  callback to handle the histogram settings of channel 13
 *
 *  \param hist
 */
void ModuleSetup::setHistogram13(bool hist)
{
	m_theApp->setHistogram(devid->value(), module->value(), 12, hist);
}

/*!
 *  \fn void ModuleSetup::setActive13(bool act)
 *
 *  callback to handle the activation settings of channel 13
 *
 *  \param act
 */
void ModuleSetup::setActive13(bool act)
{
	m_theApp->setActive(devid->value(), module->value(), 12, act);
}

/*!
 *  \fn void ModuleSetup::setHistogram14(bool hist)
 *
 *  callback to handle the histogram settings of channel 14
 *
 *  \param hist
 */
void ModuleSetup::setHistogram14(bool hist)
{
	m_theApp->setHistogram(devid->value(), module->value(), 13, hist);
}

/*!
 *  \fn void ModuleSetup::setActive14(bool act)
 *
 *  callback to handle the activation settings of channel 14
 *
 *  \param act
 */
void ModuleSetup::setActive14(bool act)
{
	m_theApp->setActive(devid->value(), module->value(), 13, act);
}

/*!
 *  \fn void ModuleSetup::setHistogram15(bool hist)
 *
 *  callback to handle the histogram settings of channel 15
 *
 *  \param hist
 */
void ModuleSetup::setHistogram15(bool hist)
{
	m_theApp->setHistogram(devid->value(), module->value(), 14, hist);
}

/*!
 *  \fn void ModuleSetup::setActive15(bool act)
 *
 *  callback to handle the activation settings of channel 15
 *
 *  \param act
 */
void ModuleSetup::setActive15(bool act)
{
	m_theApp->setActive(devid->value(), module->value(), 14, act);
}

/*!
 *  \fn void ModuleSetup::setHistogram16(bool hist)
 *
 *  callback to handle the histogram settings of channel 16
 *
 *  \param hist
 */
void ModuleSetup::setHistogram16(bool hist)
{
	m_theApp->setHistogram(devid->value(), module->value(), 15, hist);
}

/*!
 *  \fn void ModuleSetup::setActive16(bool act)
 *
 *  callback to handle the activation settings of channel 16
 *
 *  \param act
 */
void ModuleSetup::setActive16(bool act)
{
	m_theApp->setActive(devid->value(), module->value(), 15, act);
}
