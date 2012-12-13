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
#include "modulewizard.h"

/*!
    constructor
 */
ModuleWizard::ModuleWizard(const QString &ip, const quint16 id, QWidget *parent)
	: QWizard(parent)
{
    setupUi(this);
    wizardPage1->initialize(ip, id); 
}

/*!
    \fn void ModuleWizard::accept(void)

    the end of the wizard is called

    At the moment do nothing.
 */
void ModuleWizard::accept(void)
{
    QDialog::accept();
}

/*!
    \fn QString ModuleWizard::ip(void)

    \return the IP address set in the IP input field
 */
QString ModuleWizard::ip(void)
{
    return field("ipaddress").toString();
}

/*!
    \fn quint16 ModuleWizard::id(void)

    \return the value of the module id input field
 */
quint16 ModuleWizard::id(void)
{  
    return field("moduleid").toUInt();
}

/*!
    \fn bool ModuleWizard::master(void)

    \return the checkstate of the master checkbox
 */
bool ModuleWizard::master(void)
{  
    return field("master").toBool();
}

/*!
    \fn bool ModuleWizard::terminate(void)

    \return the checkstate of the terminate checkbox
 */
bool ModuleWizard::terminate(void)
{  
    return field("terminate").toBool();
}

/*!
    \fn bool ModuleWizard::externsync(void)

    \return the checkstate of the extern sync checkbox
 */
bool ModuleWizard::externsync(void)
{
    return field("externsync").toBool();
}
