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
#include "mdefines.h"
#include "modulesetup.h"
#include "mesydaq2.h"

#include "logging.h"

/*!
    constructor

    \param mesy
    \param parent
 */
ModuleSetup::ModuleSetup(Mesydaq2 *mesy, QWidget *parent)
	: QDialog(parent)
	, m_theApp(mesy)
{
    setupUi(this);

    devid->setMCPDList(m_theApp->mcpdId());
    module->setModuleList(m_theApp->mpsdId(devid->value()));

    checkChannel1Use->setChecked(m_theApp->active(devid->value(), module->value(), 0));
    checkChannel1Histogram->setChecked(m_theApp->histogram(devid->value(), module->value(), 0));

    checkChannel2Use->setChecked(m_theApp->active(devid->value(), module->value(), 1));
    checkChannel2Histogram->setChecked(m_theApp->histogram(devid->value(), module->value(), 1));

    checkChannel3Use->setChecked(m_theApp->active(devid->value(), module->value(), 2));
    checkChannel3Histogram->setChecked(m_theApp->histogram(devid->value(), module->value(), 2));

    checkChannel4Use->setChecked(m_theApp->active(devid->value(), module->value(), 3));
    checkChannel4Histogram->setChecked(m_theApp->histogram(devid->value(), module->value(), 3));

    checkChannel5Use->setChecked(m_theApp->active(devid->value(), module->value(), 4));
    checkChannel5Histogram->setChecked(m_theApp->histogram(devid->value(), module->value(), 4));

    checkChannel6Use->setChecked(m_theApp->active(devid->value(), module->value(), 5));
    checkChannel6Histogram->setChecked(m_theApp->histogram(devid->value(), module->value(), 5));

    checkChannel7Use->setChecked(m_theApp->active(devid->value(), module->value(), 6));
    checkChannel7Histogram->setChecked(m_theApp->histogram(devid->value(), module->value(), 6));

    checkChannel8Use->setChecked(m_theApp->active(devid->value(), module->value(), 7));
    checkChannel8Histogram->setChecked(m_theApp->histogram(devid->value(), module->value(), 7));

    channelLabel->setHidden(comgain->isChecked());
    channel->setHidden(comgain->isChecked());

    displaySlot();
}

/*!
    \fn void ModuleSetup::setPulserSlot()

    callback to handle the pulser settings
*/
void ModuleSetup::setPulserSlot()
{
    if (tabWidget->tabText(tabWidget->currentIndex()) != tr("Pulser"))
       return;
    bool ok;
    quint16 id = (quint16) devid->value();
    quint16 mod = module->value();
    quint16 chan = pulsChan->value();

    quint8 ampl;
    if(pulsampRadio1->isChecked())
        ampl = (quint8) pulsAmp1->text().toInt(&ok);
    else
        ampl = (quint8) pulsAmp2->text().toInt(&ok);

    quint16 pos = MIDDLE;
    int modType = m_theApp->getModuleId(mod, id);
    
    if (pulsLeft->isChecked())
        pos = (modType != TYPE_MSTD16) ? LEFT : RIGHT;
    else if (pulsRight->isChecked())
        pos = (modType != TYPE_MSTD16) ? RIGHT : LEFT;
    else if (pulsMid->isChecked())
        pos = MIDDLE;

    bool pulse = pulserButton->isChecked();
    if (pulse)
        const_cast<QPalette &>(pulserButton->palette()).setColor(QPalette::ButtonText, QColor(Qt::red));
    else
        const_cast<QPalette &>(pulserButton->palette()).setColor(QPalette::ButtonText, QColor(Qt::black));

    m_theApp->setPulser(id, mod, chan, pos, ampl, pulse);
}

/*!
    \fn void ModuleSetup::setGainSlot()

    callback to handle the gain settings

*/
void ModuleSetup::setGainSlot()
{
    bool	ok;
    quint16 	chan = comgain->isChecked() ? 8 : channel->text().toUInt(&ok, 0),
    		id = (quint16) devid->value(),
    		addr = module->value();
    float 	gainval = gain->text().toFloat(&ok);

    m_theApp->setGain(id, addr, chan, gainval);
}

/*!
    \fn void ModuleSetup::setThresholdSlot()

    callback to handle the threshold settings

*/
void ModuleSetup::setThresholdSlot()
{
    bool 	ok;
    quint16 	id = (quint16) devid->value();
    quint16 	addr = module->value();
    quint16 	thresh = threshold->text().toUInt(&ok, 0);

    m_theApp->setThreshold(id, addr, thresh);
}

/*!
    \fn void ModuleSetup::readRegisterSlot()

    callback to read from a MPSD register
    //! \todo display read values
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
    \fn void ModuleSetup::writeRegisterSlot()

    callback to write into a MPSD register
 */
void ModuleSetup::writeRegisterSlot()
{
    bool ok;
    quint16 id = (quint16) devid->value();
    quint16 addr = module->value();
    quint16 reg = registerSelect->value();
    quint16 val = registerValue->text().toUInt(&ok, 0);

    m_theApp->writePeriReg(id, addr, reg, val);
}

/*!
    \fn void ModuleSetup::setModeSlot(bool mode)

    callback to handle the settings of the amplitude/position mode

    \param mode
*/
void ModuleSetup::setModeSlot(bool mode)
{
    m_theApp->setMode(devid->value(), module->value(), mode);
}

/*!
    \fn void ModuleSetup::setModule(int)

    callback to handle the id setting of the module

    \param id
 */
void ModuleSetup::setModule(int id)
{
    module->setValue(id);
}

/*!
    \fn void ModuleSetup::setMCPD(int)

    callback to display the found module configuration

    \param id
 */
void ModuleSetup::setMCPD(int id)
{
    devid->setValue(id);
//  devid->setMCPDList(m_theApp->mcpdId());
    id = devid->value();
    module->setModuleList(m_theApp->mpsdId(id));
    int mid = module->value();

    checkChannel1Use->setChecked(m_theApp->active(id, mid, 0));
    checkChannel1Histogram->setChecked(m_theApp->histogram(id, mid, 0));

    checkChannel2Use->setChecked(m_theApp->active(id, mid, 1));
    checkChannel2Histogram->setChecked(m_theApp->histogram(id, mid, 1));

    checkChannel3Use->setChecked(m_theApp->active(id, mid, 2));
    checkChannel3Histogram->setChecked(m_theApp->histogram(id, mid, 2));

    checkChannel4Use->setChecked(m_theApp->active(id, mid, 3));
    checkChannel4Histogram->setChecked(m_theApp->histogram(id, mid, 3));

    checkChannel5Use->setChecked(m_theApp->active(id, mid, 4));
    checkChannel5Histogram->setChecked(m_theApp->histogram(id, mid, 4));

    checkChannel6Use->setChecked(m_theApp->active(id, mid, 5));
    checkChannel6Histogram->setChecked(m_theApp->histogram(id, mid, 5));

    checkChannel7Use->setChecked(m_theApp->active(id, mid, 6));
    checkChannel7Histogram->setChecked(m_theApp->histogram(id, mid, 6));

    checkChannel8Use->setChecked(m_theApp->active(id, mid, 7));
    checkChannel8Histogram->setChecked(m_theApp->histogram(id, mid, 7));
}

/*!
    \fn void ModuleSetup::displayMCPDSlot(int id)
    
    callback to handle the change of the MCPD ID by the user

    \param id new MCPD ID
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
    displaySlot();
}

/*!
    \fn void ModuleSetup::displayMPSDSlot(int id)

    callback to handle the change of the MPSD by the user

    \param id new module id
 */
void ModuleSetup::displayMPSDSlot(int id)
{
    if (id < 0)
       id = 0;
    displaySlot();
}

/*!
    \fn void ModuleSetup::displaySlot()

    callback to display the settings of the module
 */
void ModuleSetup::displaySlot()
{
    quint8 chan = comgain->isChecked() ? 8 : channel->value();

    quint8 mod = devid->value();
    quint8 id = module->value();
// gain:
    gain->setText(tr("%1").arg(double(m_theApp->getGain(mod, id, chan)), 4, 'f', 2));

// threshold:
    threshold->setText(tr("%1").arg(m_theApp->getThreshold(mod, id)));

// pulser:  on/off
    if(m_theApp->isPulserOn(mod, id))
    {
        pulserButton->setChecked(true);
        const_cast<QPalette &>(pulserButton->palette()).setColor(QPalette::ButtonText, QColor(Qt::red));
    }
    else
    {
        pulserButton->setChecked(false);
        const_cast<QPalette &>(pulserButton->palette()).setColor(QPalette::ButtonText, QColor(Qt::black));
    }
// channel
    pulsChan->setValue((int)m_theApp->getPulsChan(mod, id));
// amplitude
#if 0
    QString dstr;
    dstr.sprintf("%3d", m_theApp->getPulsAmp(mod, id));
    pulsAmp->setValue((int)m_theApp->getPulsAmp(mod, id));
#endif
    int modType = m_theApp->getModuleId(mod, id);
    pulsMid->setVisible(modType != TYPE_MSTD16);
    qDebug() << mod << " " << id << " modType " << modType;

// position
    qDebug() << "pulser Pos " << m_theApp->getPulsPos(mod, id);
    switch(m_theApp->getPulsPos(mod, id))
    {
    	case LEFT:
        	modType != TYPE_MSTD16 ? pulsLeft->setChecked(true) : pulsRight->setChecked(true);
        	break;
    	case RIGHT:
        	modType != TYPE_MSTD16 ? pulsRight->setChecked(true) : pulsLeft->setChecked(true);
        	break;
    	case MIDDLE:
        	pulsMid->setChecked(true);
        	break;
    }
// mode
    if (m_theApp->getMode(mod, id))
        amp->setChecked(true);
}
