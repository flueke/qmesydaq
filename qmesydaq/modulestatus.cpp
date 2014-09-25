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
#include <QMouseEvent>

#include "modulestatus.h"

/*!
    constructor
 */
ModuleStatus::ModuleStatus(QWidget *parent)
	: QWidget(parent)
{
	setupUi(this);
	setLabel("");
}

/*!
   \fn void ModuleStatus::setLabel(const QString &label)

   \param label
 */
void ModuleStatus::setLabel(const QString &label)
{
	moduleLabel->setText(label);
}

/*!
    \fn void ModuleStatus::update(const QString &type, const float version, const bool online, const bool histogram, const bool active)

    \param type
    \param version
    \param online
    \param histogram
    \param active
 */
void ModuleStatus::update(const QString &type, const float version, const bool online, const bool histogram, const bool active)
{
	moduleType->setText(type);
	moduleVersion->setText(QString("%1").arg(version));
	moduleStatus->setText(online ? tr("online") : tr(""));
	checkHistogramBox->setChecked(histogram);
	checkActiveBox->setVisible(online);
	checkActiveBox->setEnabled(online);
	checkActiveBox->setChecked(online & active);
	m_online = online;
}

/*!
    \fn void ModuleStatus::mouseDoubleClickEvent(QMouseEvent *ev)

    \param ev
 */
void ModuleStatus::mouseDoubleClickEvent(QMouseEvent *ev)
{
	ev->accept();
	if (m_online)
		emit clicked(m_id);
}

/*!
    \fn void ModuleStatus::setId(const uint8 id)

    \param id
 */
void ModuleStatus::setId(const quint8 id)
{
	m_id = id;
	setLabel(tr("%1:").arg(m_id + 1));
}

/*!
    \fn void ModuleStatus::histogramSlot(bool val)

    \param val
 */
void ModuleStatus::histogramSlot(bool val)
{
	checkActiveBox->setVisible(m_online);
	checkActiveBox->setEnabled(m_online);
	emit histogram(m_id, val);
}

/*!
    \fn void ModuleStatus::activeSlot(bool val)

    \param val
 */
void ModuleStatus::activeSlot(bool val)
{
	emit active(m_id, val);
}
