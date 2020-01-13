/***************************************************************************
 *   Copyright (C) 2013-2020 by Stefan Rainow <stefan.rainow@frm2.tum.de>  *
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

#include "inputdialog.h"

#include <QGridLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QLabel>

#include <QDebug>

InputDialog::InputDialog(QString name, QString toolTip, QWidget *parent)
    : QDialog(parent)
{

    createObjects();
    createLayout();

    while (toolTip.startsWith("\n"))
        toolTip.remove(0,1);

    toolTipEdit->setText(toolTip);
    nameEdit->setText(name);
    newName = name;
    newToolTip = toolTip;

    connect(nameEdit, SIGNAL(textChanged(QString)),
            this, SLOT(changeNewName(QString)));
    connect(toolTipEdit, SIGNAL(textChanged()),
            this, SLOT(changeNewToolTip()));
    connect(buttonBox, SIGNAL(accepted()),
            this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()),
            this, SLOT(reject()));
}

void InputDialog::createObjects()
{
    name = new QLabel("Name: ",this);
    nameEdit = new QLineEdit;
    nameEdit->setMaxLength(20);
    toolTip = new QLabel("Beschreibung: ",this);
    toolTipEdit = new QTextEdit;
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

}

void InputDialog::createLayout()
{
    mainLayout = new QGridLayout;
    mainLayout->addWidget(name, 0,0);
    mainLayout->addWidget(nameEdit,0,1);
    mainLayout->addWidget(toolTip,1,0);
    mainLayout->addWidget(toolTipEdit,1,1);
    mainLayout->addWidget(buttonBox, 99,0,1,2);

    setLayout(mainLayout);
}

void InputDialog::changeNewName(QString name)
{
    newName = name;
}

void InputDialog::changeNewToolTip()
{
    newToolTip = toolTipEdit->toPlainText();
}
