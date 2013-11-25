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

#include <QFileDialog>

#include "mdefines.h"
#include "mdllpulser.h"
#include "mesydaq2.h"

/*!
    constructor

    \param mesy
    \param parent
 */
MdllPulser::MdllPulser(Mesydaq2 *mesy, QWidget *parent)
	: QDialog(parent)
	, m_theApp(mesy)
{
	setupUi(this);

	QList<int> mcpdList = m_theApp->mcpdId();
	devid->setMCPDList(mcpdList);
	display(devid->value());
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
	QPoint pos = settings.value("PulserDialog/pos", QPoint(0, 0)).toPoint();
	if (pos != QPoint(0, 0))
		move(pos);
}

void MdllPulser::closeEvent(QCloseEvent *)
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
	settings.setValue("PulserDialog/pos", pos());
}

/*!
    \fn void MdllSetup::setMCPD(int)

    sets the MCPD

    \param id
 */
void MdllPulser::setMCPD(int /* id */)
{
	display();
}

/*!
    \fn void MdllSetup::updatePulser()

    set pulser
 */
void MdllPulser::updatePulser()
{
	quint8 amp = pulsamp->currentIndex();
	quint8 pos = pulspos->currentIndex();
	m_theApp->setPulser(0, 0, 0, pos, amp, pulserBox->isChecked());
}

/*!
    \fn void MdllSetup::display()

    callback to display the setup

    \param id
 */
void MdllPulser::display(int id)
{
// pulser:  on/off
	pulserBox->setChecked(m_theApp->getMdllPulser(id, 0));
// channel
	pulspos->setCurrentIndex((int)m_theApp->getMdllPulser(id, 2));
// amplitude
	pulsamp->setCurrentIndex((int)m_theApp->getMdllPulser(id, 1));
}

/*!
    \fn void MdllSetup::setPulserAmplitude()

    callback to set the pulser amplitude

    \param id
 */
void MdllPulser::setPulserAmplitude(int)
{
	if (pulserBox->isChecked())
		updatePulser();
}

/*!
    \fn void MdllSetup::setPulserPosition()

    callback to set the pulser position

    \param id
 */
void MdllPulser::setPulserPosition(int)
{
	if (pulserBox->isChecked())
		updatePulser();
}

/*!
    \fn void MPSDPulser::setPulser(bool)

    callback to handle the change of the pulser on/off
*/
void MdllPulser::setPulser(bool)
{
	updatePulser();
}
