/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2011 by Jens Kr�ger <jens.krueger@frm2.tum.de>     *
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

#include "presetwidget.h"

/*!
    constructor
 */
PresetWidget::PresetWidget(QWidget *parent)
	: QWidget(parent)
{
    setupUi(this);
    setLabel("");
    setValue(0);
//  connect(resetButton, SIGNAL(clicked()), this, SLOT(resetButtonClicked()));
//  connect(presetButton, SIGNAL(clicked(bool)), this, SLOT(presetCheckClicked(bool)));
}

/*!
   \fn void PresetWidget::setLabel(const QString &label)

   \param label
 */
void PresetWidget::setLabel(const QString &text)
{
    label->setText(text);
    presetButton->setToolTip(tr("activate preset for '%1'").arg(text));
    preset->setToolTip(tr("preset value for '%1'").arg(text));
    resetButton->setToolTip(tr("reset the preset for '%1'").arg(text));
}

/*!
    \fn quint64 PresetWidget::value(void)

    \return the current selected preset value
 */
quint64 PresetWidget::value(void)
{
    return preset->value();
}

/*!
    \fn void PresetWidget::setValue(const quint64 val)

    sets the preset value

    \param val new value of the preset
 */
void PresetWidget::setValue(const quint64 val)
{
    preset->setValue(val);
}

/*!
    \fn void PresetWidget::setChecked(const bool val)

    callback 

    \param val
 */
void PresetWidget::setChecked(const bool val)
{
    presetButton->setChecked(val);
    preset->setEnabled(val);
//    preset->setHidden(!val);
//    resetButton->setHidden(!val);
}

/*!
    \fn bool PresetWidget::isChecked(void)

    \return whether the checkbox is checked
 */
bool PresetWidget::isChecked(void)
{
    return presetButton->isChecked();
}

/*!
    \fn void PresetWidget::presetCheckClicked(bool val)

    callback for clicking the preset checkbox

    \param val
 */
void PresetWidget::presetCheckClicked(bool val)
{
	setChecked(val);
	emit presetClicked(val);
}

/*!
    \fn void PresetWidget::resetButtonClicked(bool val)

    callback for clicking the preset checkbox

    \param val
 */
void PresetWidget::resetButtonClicked()
{
	setValue(0);
	emit resetClicked();
}
