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

    QList<int> mcpdList = m_theApp->mcpdId();
    bool noModule = mcpdList.isEmpty();
    tabWidget->setDisabled(noModule);

    devid->setMCPDList(mcpdList);
    QList<int> modules = m_theApp->mpsdId(devid->value());
    module->setModuleList(modules);
    module->setEnabled(!modules.empty());

    checkChannel1Histogram->setChecked(m_theApp->histogram(devid->value(), module->value(), 0));
    checkChannel1Use->setChecked(m_theApp->active(devid->value(), module->value(), 0));
    QObject::connect(checkChannel1Histogram, SIGNAL(toggled(bool)), this, SLOT(setHistogram1(bool)));

    checkChannel2Use->setChecked(m_theApp->active(devid->value(), module->value(), 1));
    checkChannel2Histogram->setChecked(m_theApp->histogram(devid->value(), module->value(), 1));
    QObject::connect(checkChannel2Histogram, SIGNAL(toggled(bool)), this, SLOT(setHistogram2(bool)));

    checkChannel3Use->setChecked(m_theApp->active(devid->value(), module->value(), 2));
    checkChannel3Histogram->setChecked(m_theApp->histogram(devid->value(), module->value(), 2));
    QObject::connect(checkChannel3Histogram, SIGNAL(toggled(bool)), this, SLOT(setHistogram3(bool)));

    checkChannel4Use->setChecked(m_theApp->active(devid->value(), module->value(), 3));
    checkChannel4Histogram->setChecked(m_theApp->histogram(devid->value(), module->value(), 3));
    QObject::connect(checkChannel4Histogram, SIGNAL(toggled(bool)), this, SLOT(setHistogram4(bool)));

    checkChannel5Use->setChecked(m_theApp->active(devid->value(), module->value(), 4));
    checkChannel5Histogram->setChecked(m_theApp->histogram(devid->value(), module->value(), 4));
    QObject::connect(checkChannel5Histogram, SIGNAL(toggled(bool)), this, SLOT(setHistogram5(bool)));

    checkChannel6Use->setChecked(m_theApp->active(devid->value(), module->value(), 5));
    checkChannel6Histogram->setChecked(m_theApp->histogram(devid->value(), module->value(), 5));
    QObject::connect(checkChannel6Histogram, SIGNAL(toggled(bool)), this, SLOT(setHistogram6(bool)));

    checkChannel7Use->setChecked(m_theApp->active(devid->value(), module->value(), 6));
    checkChannel7Histogram->setChecked(m_theApp->histogram(devid->value(), module->value(), 6));
    QObject::connect(checkChannel7Histogram, SIGNAL(toggled(bool)), this, SLOT(setHistogram7(bool)));

    checkChannel8Use->setChecked(m_theApp->active(devid->value(), module->value(), 7));
    checkChannel8Histogram->setChecked(m_theApp->histogram(devid->value(), module->value(), 7));
    QObject::connect(checkChannel8Histogram, SIGNAL(toggled(bool)), this, SLOT(setHistogram8(bool)));

    channelLabel->setHidden(comgain->isChecked());
    channel->setHidden(comgain->isChecked());

    displaySlot();
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
    \fn void ModuleSetup::setModeSlot()

    callback to handle the settings of the amplitude/position mode

    \param mode
*/
void ModuleSetup::setModeSlot()
{
    int mod = module->value();
    if (comAmp->isChecked())
        mod = 8;
    m_theApp->setMode(devid->value(), mod, amp->isChecked());
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
    QList<int> modules = m_theApp->mpsdId(id);
    module->setModuleList(modules);
    module->setEnabled(!modules.empty());
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

    setGainSlot();
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
    module->setEnabled(!modList.empty());
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
    int modType = m_theApp->getModuleId(mod, id);
    qDebug() << mod << " " << id << " modType " << modType;
    Ui_ModuleSetup::pos->setEnabled(modType != TYPE_MPSD8P && modType != TYPE_MDLL);
    Ui_ModuleSetup::amp->setEnabled(modType != TYPE_MPSD8P && modType != TYPE_MDLL);

// gain:
    gain->setText(tr("%1").arg(double(m_theApp->getGain(mod, id, chan)), 4, 'f', 2));

// threshold:
    threshold->setText(tr("%1").arg(m_theApp->getThreshold(mod, id)));

// mode
    if (m_theApp->getMode(mod, id))
        amp->setChecked(true);
}

/*!
    \fn void ModuleSetup::setHistogram1(bool hist)

    callback to handle the histogram settings of channel 1

    \param hist
*/
void ModuleSetup::setHistogram1(bool hist)
{
    m_theApp->setHistogram(devid->value(), module->value(), 0, hist);
}

/*!
    \fn void ModuleSetup::setActive1(bool act)

    callback to handle the activation settings of channel 1

    \param act
*/
void ModuleSetup::setActive1(bool act)
{
    m_theApp->setActive(devid->value(), module->value(), 0, act);
}

/*!
    \fn void ModuleSetup::setHistogram2(bool hist)

    callback to handle the histogram settings of channel 2

    \param hist
*/
void ModuleSetup::setHistogram2(bool hist)
{
    m_theApp->setHistogram(devid->value(), module->value(), 1, hist);
}

/*!
    \fn void ModuleSetup::setActive2(bool act)

    callback to handle the activation settings of channel 2

    \param act
*/
void ModuleSetup::setActive2(bool act)
{
    m_theApp->setActive(devid->value(), module->value(), 1, act);
}

/*!
    \fn void ModuleSetup::setHistogram3(bool hist)

    callback to handle the histogram settings of channel 3

    \param hist
*/
void ModuleSetup::setHistogram3(bool hist)
{
    m_theApp->setHistogram(devid->value(), module->value(), 2, hist);
}

/*!
    \fn void ModuleSetup::setActive3(bool act)

    callback to handle the activation settings of channel 3

    \param act
*/
void ModuleSetup::setActive3(bool act)
{
    m_theApp->setActive(devid->value(), module->value(), 2, act);
}

/*!
    \fn void ModuleSetup::setHistogram4(bool hist)

    callback to handle the histogram settings of channel 4

    \param hist
*/
void ModuleSetup::setHistogram4(bool hist)
{
    m_theApp->setHistogram(devid->value(), module->value(), 3, hist);
}

/*!
    \fn void ModuleSetup::setActive4(bool act)

    callback to handle the activation settings of channel 4

    \param act
*/
void ModuleSetup::setActive4(bool act)
{
    m_theApp->setActive(devid->value(), module->value(), 3, act);
}

/*!
    \fn void ModuleSetup::setHistogram5(bool hist)

    callback to handle the histogram settings of channel 5

    \param hist
*/
void ModuleSetup::setHistogram5(bool hist)
{
    m_theApp->setHistogram(devid->value(), module->value(), 4, hist);
}

/*!
    \fn void ModuleSetup::setActive5(bool act)

    callback to handle the activation settings of channel 5

    \param act
*/
void ModuleSetup::setActive5(bool act)
{
    m_theApp->setActive(devid->value(), module->value(), 4, act);
}

/*!
    \fn void ModuleSetup::setHistogram6(bool hist)

    callback to handle the histogram settings of channel 6

    \param hist
*/
void ModuleSetup::setHistogram6(bool hist)
{
    m_theApp->setHistogram(devid->value(), module->value(), 5, hist);
}

/*!
    \fn void ModuleSetup::setActive6(bool act)

    callback to handle the activation settings of channel 6

    \param act
*/
void ModuleSetup::setActive6(bool act)
{
    m_theApp->setActive(devid->value(), module->value(), 5, act);
}

/*!
    \fn void ModuleSetup::setHistogram7(bool hist)

    callback to handle the histogram settings of channel 7

    \param hist
*/
void ModuleSetup::setHistogram7(bool hist)
{
    m_theApp->setHistogram(devid->value(), module->value(), 6, hist);
}

/*!
    \fn void ModuleSetup::setActive7(bool act)

    callback to handle the activation settings of channel 7

    \param act
*/
void ModuleSetup::setActive7(bool act)
{
    m_theApp->setActive(devid->value(), module->value(), 6, act);
}

/*!
    \fn void ModuleSetup::setHistogram8(bool hist)

    callback to handle the histogram settings of channel 8

    \param hist
*/
void ModuleSetup::setHistogram8(bool hist)
{
    m_theApp->setHistogram(devid->value(), module->value(), 7, hist);
}

/*!
    \fn void ModuleSetup::setActive8(bool act)

    callback to handle the activation settings of channel 8

    \param act
*/
void ModuleSetup::setActive8(bool act)
{
    m_theApp->setActive(devid->value(), module->value(), 7, act);
}
