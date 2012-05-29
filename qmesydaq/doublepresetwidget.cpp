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

#include "doublepresetwidget.h"

/*!
    constructor
 */
DoublePresetWidget::DoublePresetWidget(QWidget *parent)
	: QWidget(parent)
{
    setupUi(this);
    setLabel("");
    setPresetValue(0.0);
}

/*!
   \fn void DoublePresetWidget::setLabel(const QString &label)

   \param label
 */
void DoublePresetWidget::setLabel(const QString &text)
{
    label->setText(text);
    presetButton->setToolTip(tr("activate preset for '%1'").arg(text));
    preset->setToolTip(tr("preset value for '%1'").arg(text));
    resetButton->setToolTip(tr("reset the preset for '%1'").arg(text));
}

/*!
    \fn quint64 DoublePresetWidget::presetValue(void)

    \return the current selected preset value
 */
double DoublePresetWidget::presetValue(void)
{
    return preset->value();
}

/*!
    \fn void DoublePresetWidget::setValue(const double val)

    sets the value

    \param val new value 
 */
void DoublePresetWidget::setValue(const double val)
{
    currentValue->setText(QString::number(val));
}

/*!
    \fn void DoublePresetWidget::setPresetValue(const double val)

    sets the preset value

    \param val new value of the preset
 */
void DoublePresetWidget::setPresetValue(const double val)
{
    preset->setValue(val);
}

/*!
    \fn void DoublePresetWidget::setChecked(const bool val)

    callback 

    \param val
 */
void DoublePresetWidget::setChecked(const bool val)
{
    presetButton->setChecked(val);
    preset->setEnabled(val);
//    preset->setHidden(!val);
//    resetButton->setHidden(!val);
}

/*!
    \fn bool DoublePresetWidget::isChecked(void)

    \return whether the checkbox is checked
 */
bool DoublePresetWidget::isChecked(void)
{
    return presetButton->isChecked();
}

/*!
    \fn void DoublePresetWidget::presetCheckClicked(bool val)

    callback for clicking the preset checkbox

    \param val
 */
void DoublePresetWidget::presetCheckClicked(bool val)
{
	setChecked(val);
	emit presetClicked(val);
}

/*!
    \fn void DoublePresetWidget::resetButtonClicked(bool val)

    callback for clicking the preset checkbox

    \param val
 */
void DoublePresetWidget::resetButtonClicked()
{
	setPresetValue(0);
	emit resetClicked();
}

