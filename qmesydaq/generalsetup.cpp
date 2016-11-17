/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2014 by Jens Krüger <jens.krueger@frm2.tum.de>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QFileDialog>

#include "generalsetup.h"
#include "measurement.h"
#include "logging.h"

/*!
    constructor

    \param meas
    \param parent
 */
GeneralSetup::GeneralSetup(Measurement *meas, QWidget *parent)
	: QDialog(parent)
	, m_meas(meas)
{
	setupUi(this);
	configfilepath->setText(m_meas->getConfigfilepath());
	histfilepath->setText(m_meas->getHistfilepath());
	listfilepath->setText(m_meas->getListfilepath());
	runId->setValue(m_meas->runId());
	runIdAuto->setChecked(m_meas->getAutoIncRunId());
	writeProtect->setChecked(m_meas->getWriteProtection());
	switch (m_meas->getHistogramFileFormat())
	{
		case Measurement::SimpleFormat:
			simpleHistogramFileFormat->setChecked(true);
			break;
		default:
		case Measurement::StandardFormat:
			standardHistogramFileFormat->setChecked(true);
	}
	histogramFileformatButtonGroup->setId(standardHistogramFileFormat, Measurement::StandardFormat);
	histogramFileformatButtonGroup->setId(simpleHistogramFileFormat, Measurement::SimpleFormat);
	switch (m_meas->getPsdArrangement())
	{
		default:
		case Measurement::Square:
			tube2D->setChecked(true);
			break;
		case Measurement::Line:
			tube1D->setChecked(true);
			break;
	}
	arrangementButtonGroup->setId(tube2D, Measurement::Square);
	arrangementButtonGroup->setId(tube1D, Measurement::Line);
}

/*!
    \fn void GeneralSetup::selectConfigpathSlot()

    callback to set the path for the config data files
*/
void GeneralSetup::selectConfigpathSlot()
{
	QString name = QFileDialog::getExistingDirectory((QWidget *)this, tr("Select Config File Path..."), configfilepath->text());
	if(!name.isEmpty())
		configfilepath->setText(name);
}

/*!
    \fn GeneralSetup::selectListpathSlot()

    callback to set the path for the list mode data files
*/
void GeneralSetup::selectListpathSlot()
{
	QString name = QFileDialog::getExistingDirectory((QWidget *)this, tr("Select List File Path..."), listfilepath->text());
	if(!name.isEmpty())
		listfilepath->setText(name);
}

/*!
    \fn GeneralSetup::selectHistpathSlot()

    callback to set the path for the histogram data files
*/
void GeneralSetup::selectHistpathSlot()
{
	QString name = QFileDialog::getExistingDirectory((QWidget *)this, tr("Select Histogram File Path..."), histfilepath->text());
	if(!name.isEmpty())
		histfilepath->setText(name);
}

/*!
    \fn void GeneralSetup::setRunIdSlot()

    callback to set the run ID on the module
 */
void GeneralSetup::setRunIdSlot()
{
	m_meas->setRunId(runId->value());
}

/*!
 *   \fn GeneralSetup::selectAutoIncRunIdSlot
 *
 *   callback to select auto increment for run id
 */
void GeneralSetup::selectAutoIncRunIdSlot()
{
	m_meas->setAutoIncRunId(runIdAuto->isChecked());
}

/*!
 *   \fn GeneralSetup::selectWriteProtectSlot
 *
 *   callback to select write protection for closed files
 */
void GeneralSetup::selectWriteProtectSlot()
{
	m_meas->setWriteProtection(writeProtect->isChecked());
}

int GeneralSetup::getArrangement(void) const
{
	return arrangementButtonGroup->checkedId();
}

int GeneralSetup::getHistogramFileFormat(void) const
{
	return histogramFileformatButtonGroup->checkedId();
}

void GeneralSetup::setLastRunId(const quint32 val)
{
	runId->setValue(val);
}

QString GeneralSetup::configFilePath(void)
{
	return configfilepath->text();
}

QString GeneralSetup::listFilePath(void)
{
	return listfilepath->text();
}

QString GeneralSetup::histFilePath(void)
{
	return histfilepath->text();
}

quint32 GeneralSetup::lastRunId(void)
{
	return runId->value();
}

bool GeneralSetup::getAutoIncRunId(void)
{
	return runIdAuto->isChecked();
}

bool GeneralSetup::getWriteProtection(void)
{
	return writeProtect->isChecked();
}

int GeneralSetup::getTriggerId(void)
{
	return triggerInput->currentIndex() - 1;
}

void GeneralSetup::setTriggerId(const int id)
{
	if (id < 0)
		triggerInput->setCurrentIndex(0);
	else if (id < 6)
		triggerInput->setCurrentIndex(id + 1);
	else
		triggerInput->setCurrentIndex(0);
}
