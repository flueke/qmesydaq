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
#include <QCoreApplication>
#include <QTimer>

#include "mdefines.h"
#include "mpsdpulser.h"
#include "mesydaq2.h"
#include "pulsertest.h"

#include "logging.h"

/*!
    constructor

    \param mesy
    \param parent
 */
MPSDPulser::MPSDPulser(Mesydaq2 *mesy, QWidget *parent)
	: QDialog(parent)
	, m_theApp(mesy)
	, m_enabled(false)
{
	setupUi(this);

	devid->setMCPDList(m_theApp->mcpdId());

	QList<int> modules = m_theApp->mpsdId(devid->value());
	module->setModuleList(modules);
	module->setDisabled(modules.empty());
	pulserGroupBox->setDisabled(modules.empty());

	display();
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
	QPoint pos = settings.value("PulserDialog/pos", QPoint(0, 0)).toPoint();
	if (pos != QPoint(0, 0))
		move(pos);
	connect(automaticPulserTest, SIGNAL(clicked(bool)), this, SLOT(pulserTestSlot(bool)));
}

void MPSDPulser::closeEvent(QCloseEvent *)
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
	settings.setValue("PulserDialog/pos", pos());
}

/*!
    \fn void MPSDPulser::amplitudeChanged(int)

    callback to handle the change of the amplitude settings
*/
void MPSDPulser::amplitudeChanged(int amp)
{
	MSG_DEBUG << amp;
	if (pulserButton->isChecked())
		updatePulser();
}

/*!
    \fn void MPSDPulser::setPulser(int)

    callback to handle the change of the pulser on/off
*/
void MPSDPulser::setPulser(int onoff)
{
	MSG_DEBUG << onoff;
	updatePulser();
}

/*!
    \fn void MPSDPulser::setPulserPosition(bool)

    callback to handle the change of the pulser position
*/
void MPSDPulser::setPulserPosition(bool onoff)
{
	MSG_DEBUG << onoff;
	if (pulserButton->isChecked())
		updatePulser();
}

/*!
    \fn void MPSDPulser::setChannel(int)

    callback to handle the change of the pulser position
*/
void MPSDPulser::setChannel(int chan)
{
	MSG_DEBUG << chan;
	if (pulserButton->isChecked())
		updatePulser();
}

/*!
    \fn void MPSDPulser::updatePulser()

    callback to handle the pulser settings
*/
void MPSDPulser::updatePulser()
{
	if (!m_enabled)
		return;
	MSG_INFO << "U P D A T E P U L S E R";
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
	int modType = m_theApp->getModuleId(id, mod);

	if (modType == TYPE_MSTD16)
	{
		// RIGHT is for the 'even' channels (0, 2, 4, ...) and LEFT for the 'odd' channels (1, 3, 5, ... )
		pos = (chan % 2) ? LEFT : RIGHT;
		chan /= 2;
	}
	else
	{
		if (pulsLeft->isChecked())
			pos = LEFT;
		else if (pulsRight->isChecked())
			pos = RIGHT;
		else if (pulsMid->isChecked())
			pos = MIDDLE;
	}

	bool pulse = pulserButton->isChecked();
	if (pulse)
		const_cast<QPalette &>(pulserButton->palette()).setColor(QPalette::ButtonText, QColor(Qt::red));
	else
		const_cast<QPalette &>(pulserButton->palette()).setColor(QPalette::ButtonText, QColor(Qt::black));
	MSG_INFO << tr("MPSDPulser::updatePulser : id = %1, mod = %2, chan = %3, pos = %4, amp = %5, on = %6").arg(id).arg(mod).arg(chan).arg(pos).arg(ampl).arg(pulse);
	m_theApp->setPulser(id, mod, chan, pos, ampl, pulse);
}

/*!
    \fn void MPSDPulser::setModule(int)

    callback to handle the id setting of the module

    \param id
 */
void MPSDPulser::setModule(int id)
{
	module->setValue(id);
}

/*!
    \fn void MPSDPulser::setMCPD(int)

    callback to display the found module configuration

    \param id
 */
void MPSDPulser::setMCPD(int id)
{
	if (id != -1)
		devid->setValue(id);
	id = devid->value();
	QList<int> modules = m_theApp->mpsdId(id);
	module->setModuleList(modules);
	module->setDisabled(modules.empty());
	pulserGroupBox->setDisabled(modules.empty());
	display();
}

/*!
    \fn void MPSDPulser::displayMCPDSlot(int id)

    callback to handle the change of the MCPD ID by the user

    \param id new MCPD ID
 */
void MPSDPulser::displayMCPDSlot(int id)
{
	if (id < 0)
		id = devid->value();

	QList<int> modList;
	for (int i = 0; i < 8; ++i)
		if (m_theApp->getModuleId(id, i))
			modList << i;
	module->setModuleList(modList);
	module->setDisabled(modList.empty());
	pulserGroupBox->setDisabled(modList.empty());
	display();
}

/*!
    \fn void MPSDPulser::displayMPSDSlot(int id)

    callback to handle the change of the MPSD by the user

    \param id new module id
 */
void MPSDPulser::displayMPSDSlot(int id)
{
	if (id < 0)
		id = 0;
	display();
}

/*!
    \fn void MPSDPulser::display()

    callback to display the settings of the module
 */
void MPSDPulser::display()
{
	quint8 mod = devid->value();
	quint8 id = module->value();

	int modType = m_theApp->getModuleId(mod, id);
	MSG_DEBUG << mod << " " << id << " modType " << modType;
	positionGroup->setVisible(modType != TYPE_MSTD16);
	pulsChan->setMaximum(modType == TYPE_MSTD16 ? 15 : 7);

// pulser:  on/off
	bool pulser = m_theApp->isPulserOn(mod, id);
	MSG_DEBUG << mod << " " << id << " pulser Chan " << m_theApp->getPulsChan(mod, id);
	pulserButton->setChecked(pulser);

// position
	MSG_DEBUG << mod << " " << id << " pulser Pos " << m_theApp->getPulsPos(mod, id);
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

	m_enabled = false;
	if(pulser)
	{
		const_cast<QPalette &>(pulserButton->palette()).setColor(QPalette::ButtonText, QColor(Qt::red));
// active pulser channel
		pulsChan->setValue((int)m_theApp->getPulsChan(mod, id));
	}
	else
		const_cast<QPalette &>(pulserButton->palette()).setColor(QPalette::ButtonText, QColor(Qt::black));
// amplitude
	MSG_DEBUG << mod << " " << id << " pulser Amp " << m_theApp->getPulsAmp(mod, id);
	if(pulsampRadio1->isChecked())
		pulsAmp1->setValue((int)m_theApp->getPulsAmp(mod, id));
	else
		pulsAmp2->setValue((int)m_theApp->getPulsAmp(mod, id));
	m_enabled = true;
}

void MPSDPulser::pulserTestSlot(bool onoff)
{
	MSG_ERROR << "pulser test : " << onoff;
	if (onoff)
		emit clear();
	emit pulserTest(onoff);
	if (onoff)
	{
		m_pulses = PulserTest::sequence(m_theApp);
		m_it = m_pulses.begin();
		QTimer::singleShot(0, this, SLOT(nextStep()));
	}
	else
		m_it = m_pulses.end();
}

void MPSDPulser::nextStep()
{
	if (m_it != m_pulses.end())
	{
		pulserButton->setCheckState(Qt::Unchecked);
		devid->setValue(m_it->mod);
		module->setValue(m_it->addr);
		pulsChan->setValue(m_it->channel);
		pulsampRadio1->setChecked(true);
		pulsAmp1->setValue(m_it->amp);
		switch (m_it->position)
		{
			case LEFT:
				pulsLeft->setChecked(true);
				break;
			case MIDDLE:
				pulsMid->setChecked(true);
				break;
			case RIGHT:
				pulsRight->setChecked(true);
				break;
		}
		pulserButton->setCheckState(Qt::Checked);
		QTimer::singleShot(m_it->onTime, this, SLOT(pulserOff()));
		++m_it;
	}
	else if (automaticPulserTest->isChecked())
		automaticPulserTest->animateClick();
}

void MPSDPulser::pulserOn()
{
}

void MPSDPulser::pulserOff()
{
	pulserButton->setCheckState(Qt::Unchecked);
	QTimer::singleShot(0, this, SLOT(nextStep()));
}
