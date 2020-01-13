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

#ifndef ADDTREEDIALOG_H
#define ADDTREEDIALOG_H

#include <QInputDialog>
#include <QSpinBox>
#include <QCheckBox>

class QComboBox;
class QCheckBox;
class QLabel;
class QDialogButtonBox;
class QWidget;

class AddTreeDialog : public QDialog
{
    Q_OBJECT
public:
    AddTreeDialog(QWidget *parent = 0);
    int getTopLevelType();
    int getAmount()
    {return amountSpinBox->value();}
    bool addFullTree()
    {return fullTreeCheckBox->isChecked();}
    int getSubMCPDAmount()
    {return subMCPDSpinBox->value();}
    int getSubMPSDAmount()
    {return subMPSDSpinBox->value();}
    int getSubChannelAmount()
    {return subChannelSpinBox->value();}
//    void setTopLevelType(QString type);
//    void setSubMCPDAmount(int amount);
//    void setSubMPSDAmount(int amount);
//    void setSubChannelAmount(int amount);
//    void setTopLevelAmount(int amount);

    
signals:
    
public slots:

private slots:
    void setActiveState(QString selection);
    void setActiveState(bool active);

private:
    void createObjects();
    void createLayout();
    QComboBox *topLevelItemComboBox;
    QSpinBox *amountSpinBox;
    QCheckBox *fullTreeCheckBox;
    QLabel *topLevelLabel;
    QLabel *amountLabel;
    QLabel *subMCPDLabel;
    QLabel *subMPSDLabel;
    QLabel *subChannelLabel;
    QSpinBox *subMCPDSpinBox;
    QSpinBox *subMPSDSpinBox;
    QSpinBox *subChannelSpinBox;
    QDialogButtonBox *buttonBox;
    
};

#endif // ADDTREEDIALOG_H
