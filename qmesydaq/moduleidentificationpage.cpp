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
#include <QMouseEvent>
#include <QAbstractButton>

#include "moduleidentificationpage.h"
#include "mcpd8.h"

/*!
    constructor
 */
ModuleIdentificationPage::ModuleIdentificationPage(QWidget *parent)
	: QWizardPage(parent)
{
    setupUi(this);
    registerField("ipaddress*", moduleIPInput);
    registerField("moduleid*", moduleIDInput); 
    initialize();

    connect(moduleIPInput, SIGNAL(textChanged(const QString &)), this, SLOT(valueChanged()));
    connect(moduleIPInput, SIGNAL(textEdited(const QString &)), this, SLOT(valueChanged()));
    connect(moduleIDInput, SIGNAL(valueChanged(int)), this, SLOT(valueChanged()));
}

/*!
    \fn void ModuleIdentificationPage::initialize(const QString &ip, const quint16 id)
 
    initializes the input fields for IP address and module ID

    \param ip IP address 
    \param id Module ID
 */
void ModuleIdentificationPage::initialize(const QString &ip, const quint16 id)
{
    moduleIPInput->setText(ip);
    moduleIDInput->setValue(id);
}

/*!
    \fn bool ModuleIdentificationPage::isComplete() const

    checks for completeness of input. In our case it tries to connect to the MCPD and
    tries to read the connected modules. If found a MCPD it will return true.
   
    \return true in case of found a MCPD otherwise false
 */
bool ModuleIdentificationPage::isComplete() const
{
     MCPD8 *mcpd = new MCPD8(moduleIDInput->value(), moduleIPInput->text());

     bool ret = mcpd->version() != 0;
     delete mcpd;
     return ret;
}

/*!
    \fn void ModuleIdentificationPage::valueChanged()

    sets the Next button true if a value was changed
 */
void ModuleIdentificationPage::valueChanged()
{
        enum QWizard::WizardButton bt = QWizard::NextButton;
	if (isFinalPage())
		bt = QWizard::FinishButton;
	wizard()->button(bt)->setEnabled(false);
	wizard()->button(bt)->setEnabled(isComplete());
}
