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
#include "modulemasterpage.h"
#include "mcpd8.h"

/*!
    constructor

    \param parent parent widget
 */
ModuleMasterPage::ModuleMasterPage(QWidget *parent)
	: QWizardPage(parent)
{
	setupUi(this);

	registerField("master", masterCheckBox);
	registerField("terminate", terminateCheckBox);
	registerField("externsync", externsyncCheckBox);

	connect(masterCheckBox, SIGNAL(toggled(bool)), this, SLOT(valueChanged(bool)));
	setFinalPage(true);
}

/*!
    \fn void ModuleMasterPage::valueChanged(bool val)

    callback for setting the master/slave
    if val is true it sets automatically the termination of the bus to true

    \param val indicates the master
 */
void ModuleMasterPage::valueChanged(bool val)
{
	if (val)
		terminateCheckBox->setChecked(true);
	else
		externsyncCheckBox->setChecked(false);
}
