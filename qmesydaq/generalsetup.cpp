/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2011 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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
#include "mesydaq2.h"

/*!
    constructor

    \param mesy
    \param parent
 */
GeneralSetup::GeneralSetup(Mesydaq2 *mesy, QWidget *parent)
	 : QDialog(parent)
{
    setupUi(this);
    configfilepath->setText(mesy->getConfigfilepath());
    histfilepath->setText(mesy->getHistfilepath());
    listfilepath->setText(mesy->getListfilepath());
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

