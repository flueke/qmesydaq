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

/*!
 *   \fn void GeneralSetup::selectStandardHistogramFileFormat()
 *
 *   callback to select standard histogram file format
 */
void GeneralSetup::selectStandardHistogramFileFormat()
{
	m_meas->setHistogramFileFormat(Measurement::StandardFormat);
}

/*!
 *   \fn void GeneralSetup::selectSimpleHistogramFileFormat()
 *
 *   callback to select simple histogram file format
 */
void GeneralSetup::selectSimpleHistogramFileFormat()
{
	m_meas->setHistogramFileFormat(Measurement::SimpleFormat);
}
