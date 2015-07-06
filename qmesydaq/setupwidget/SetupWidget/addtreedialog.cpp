/***************************************************************************
 *   Copyright (C) 2013-2015 by Stefan Rainow <stefan.rainow@frm2.tum.de>  *
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

#include "addtreedialog.h"

#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QWidget>

AddTreeDialog::AddTreeDialog(QWidget *parent)
   : QDialog(parent)
{
    createObjects();
    createLayout();

    topLevelItemComboBox->setCurrentIndex(1);
    subMCPDLabel->setEnabled(false);
    subMCPDSpinBox->setEnabled(false);
    amountSpinBox->setValue(1);
    subMCPDSpinBox->setValue(2);
    subMPSDSpinBox->setValue(2);
    subChannelSpinBox->setValue(2);


    connect(buttonBox, SIGNAL(accepted()),
            this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()),
            this, SLOT(reject()));
    connect(topLevelItemComboBox, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(setActiveState(QString)));
    connect(fullTreeCheckBox, SIGNAL(toggled(bool)),
            this,SLOT(setActiveState(bool)));
}

void AddTreeDialog::createObjects()
{
    topLevelLabel = new QLabel("Top Level");
    amountLabel= new QLabel("Amount");
    subMCPDLabel = new QLabel("Sub MCPDs");
    subMPSDLabel = new QLabel("Sub MPSDs");
    subChannelLabel = new QLabel("Sub Channels");
    topLevelItemComboBox = new QComboBox;
    topLevelItemComboBox->addItem("PC");
    topLevelItemComboBox->addItem("MCPD");
    topLevelItemComboBox->addItem("MPSD");
    topLevelItemComboBox->addItem("Channel");
    amountSpinBox = new QSpinBox;
    amountSpinBox->setMaximum(16);
    subMCPDSpinBox = new QSpinBox;
    subMCPDSpinBox->setMaximum(16);
    subMPSDSpinBox = new QSpinBox;
    subMPSDSpinBox->setMaximum(8);
    subChannelSpinBox= new QSpinBox;
    subChannelSpinBox->setMaximum(8);
    fullTreeCheckBox = new QCheckBox("Add Full Tree");
    fullTreeCheckBox->setChecked(true);
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);


}

void AddTreeDialog::createLayout()
{
    QHBoxLayout *buttonBoxLayout = new QHBoxLayout;
    buttonBoxLayout->addStretch();
    buttonBoxLayout->addWidget(buttonBox);

    QGridLayout *subLayout = new QGridLayout;
    subLayout->addWidget(subMCPDLabel,0,0);
    subLayout->addWidget(subMCPDSpinBox,0,1);
    subLayout->addWidget(subMPSDLabel,1,0);
    subLayout->addWidget(subMPSDSpinBox,1,1);
    subLayout->addWidget(subChannelLabel,2,0);
    subLayout->addWidget(subChannelSpinBox,2,1);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(topLevelLabel,0,0);
    mainLayout->addWidget(topLevelItemComboBox,0,1);
    mainLayout->addWidget(amountLabel,1,0);
    mainLayout->addWidget(amountSpinBox,1,1);
    mainLayout->addWidget(fullTreeCheckBox,2,1);
    mainLayout->addLayout(subLayout,3,0,1,2);
    mainLayout->addLayout(buttonBoxLayout,99,0,1,2);

    setLayout(mainLayout);
}

void AddTreeDialog::setActiveState(bool active)
{
    if (!active)
    {
        subChannelLabel->setEnabled(false);
        subChannelSpinBox->setEnabled(false);
        subMPSDLabel->setEnabled(false);
        subMPSDSpinBox->setEnabled(false);
        subMCPDLabel->setEnabled(false);
        subMCPDSpinBox->setEnabled(false);
    }
    else
    {
        setActiveState(topLevelItemComboBox->currentText());
    }
}

void AddTreeDialog::setActiveState(QString selection)
{
    if (selection=="Channel")
    {
        fullTreeCheckBox->setChecked(false);
        fullTreeCheckBox->setEnabled(false);
        subChannelLabel->setEnabled(false);
        subChannelSpinBox->setEnabled(false);
        subMPSDLabel->setEnabled(false);
        subMPSDSpinBox->setEnabled(false);
        subMCPDLabel->setEnabled(false);
        subMCPDSpinBox->setEnabled(false);

    }
    else if(selection == "MPSD")
    {
        fullTreeCheckBox->setEnabled(true);
        subChannelLabel->setEnabled(true);
        subChannelSpinBox->setEnabled(true);
        subMPSDLabel->setEnabled(false);
        subMPSDSpinBox->setEnabled(false);
        subMCPDLabel->setEnabled(false);
        subMCPDSpinBox->setEnabled(false);
    }
    else if(selection == "MCPD")
    {
        fullTreeCheckBox->setEnabled(true);
        subChannelLabel->setEnabled(true);
        subChannelSpinBox->setEnabled(true);
        subMPSDLabel->setEnabled(true);
        subMPSDSpinBox->setEnabled(true);
        subMCPDLabel->setEnabled(false);
        subMCPDSpinBox->setEnabled(false);
    }
    else
    {
        fullTreeCheckBox->setEnabled(true);
        subChannelLabel->setEnabled(true);
        subChannelSpinBox->setEnabled(true);
        subMPSDLabel->setEnabled(true);
        subMPSDSpinBox->setEnabled(true);
        subMCPDLabel->setEnabled(true);
        subMCPDSpinBox->setEnabled(true);
    }
}

int AddTreeDialog::getTopLevelType()
{
    return topLevelItemComboBox->currentIndex()+1;
}

//int AddTreeDialog::setTopLevelType()
//{

//}
