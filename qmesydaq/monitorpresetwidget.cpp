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
#include "monitorpresetwidget.h"

#include "logging.h"

/*!
    constructor
 */
MonitorPresetWidget::MonitorPresetWidget(QWidget *parent)
	: QWidget(parent)
{
	setupUi(this);
	setLabel("");
	setPresetValue(0);
	setConfigureMode(false);
}

/*!
   \fn void MonitorPresetWidget::setLabel(const QString &label)

   \param label
 */
void MonitorPresetWidget::setLabel(const QString &text)
{
	label->setText(text);
	presetButton->setToolTip(tr("activate preset for '%1'").arg(text));
	preset->setToolTip(tr("preset value for '%1'").arg(text));
	resetButton->setToolTip(tr("reset the preset for '%1'").arg(text));
}

/*!
    \fn quint64 MonitorPresetWidget::presetValue(void)

    \return the current selected preset value
 */
quint64 MonitorPresetWidget::presetValue(void)
{
	return preset->value();
}

/*!
    \fn void MonitorPresetWidget::setRate(const quint64 val)

    sets the current rate

    \param val current rate
 */
void MonitorPresetWidget::setRate(const quint64 val)
{
	rate->setText(QString::number(val));
}

/*!
    \fn void MonitorPresetWidget::setValue(const quint64 val)

    sets the current value

    \param val current value
 */
void MonitorPresetWidget::setValue(const quint64 val)
{
	currentValue->setText(QString::number(val));
}

/*!
    \fn void MonitorPresetWidget::setPresetValue(const quint64 val)

    sets the preset value

    \param val new value of the preset
 */
void MonitorPresetWidget::setPresetValue(const quint64 val)
{
	preset->setValue(val);
}

/*!
    \fn void MonitorPresetWidget::setChecked(const bool val)

    callback

    \param val
 */
void MonitorPresetWidget::setChecked(const bool val)
{
	presetButton->setChecked(val);
	preset->setEnabled(val);
}

/*!
    \fn bool MonitorPresetWidget::isChecked(void)

    \return whether the checkbox is checked
 */
bool MonitorPresetWidget::isChecked(void)
{
	return presetButton->isChecked();
}

/*!
    \fn void MonitorPresetWidget::presetCheckClicked(bool val)

    callback for clicking the preset checkbox

    \param val
 */
void MonitorPresetWidget::presetCheckClicked(bool val)
{
	setChecked(val);
	emit presetClicked(val);
}

/*!
    \fn void MonitorPresetWidget::resetButtonClicked(void)

    callback for clicking the preset checkbox
 */
void MonitorPresetWidget::resetButtonClicked(void)
{
	setPresetValue(0);
	emit resetClicked();
}

void MonitorPresetWidget::mcpdChanged(int)
{
	emitMappingChanged();
}

void MonitorPresetWidget::inputChanged(int)
{
	emitMappingChanged();
}

void MonitorPresetWidget::emitMappingChanged(void)
{
	if (inputComboBox->currentIndex() >= 0)
	{
		int i = objectName().mid(7, 1).toInt() - 1; // objectName assumed as monitor[0-9]Preset
		MSG_DEBUG << tr("emitting %1 : %2 %3 %4").arg(objectName()).arg(mcpdSpinBox->value()).arg(inputComboBox->currentIndex() - 1).arg(i);
		emit mappingChanged(mcpdSpinBox->value(), inputComboBox->currentIndex() - 1, i); // -1 since the index 0 is for 'not used'
	}
}

void MonitorPresetWidget::setConfigureMode(const bool config)
{
	inputComboBox->setEnabled(config);
	mcpdSpinBox->setEnabled(config);
}

void MonitorPresetWidget::configureMapping(const int mcpd, const int input)
{
	mcpdSpinBox->setValue(mcpd);
	inputComboBox->setCurrentIndex(input + 1); // +1 since the index 0 is for 'not used'
}

void MonitorPresetWidget::setMCPDList(QList<int> modules)
{
	mcpdSpinBox->setMCPDList(modules);
}
