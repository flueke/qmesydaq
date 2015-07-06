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

#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include <QDialog>

class QLabel;
class QLineEdit;
class QTextEdit;
class QDialogButtonBox;
class QGridLayout;


/** @brief The edit name and tooltip dialog.
  *
  * This dialog popps up, when the user creates or doubleclicks an item.
  */

class InputDialog : public QDialog
{
    Q_OBJECT

public:
    /** @brief The dialog's constructor.
      *
      * @param name The item's name, to be inserted as default value in the name
      * line edit.
      * @param toolTip The item's tooltip, to be inserted as default value in the
      * tooltip text edit.
      */
    InputDialog(QString name, QString toolTip, QWidget *parent=NULL);
    /** @brief Returns the newName variable*/
    QString getNewName()
        {return newName;}
    /** @brief Returns the newToolTip variable*/
    QString getNewToolTip()
        {return newToolTip;}

public slots:
    /** @brief Changes the newName variable according to the text inserted into
      * the name line edit.
      */
    void changeNewName(QString name);
    /** @brief Changes the newToolTip variable according to the text inserted
      * into the tooltip text edit.
      */
    void changeNewToolTip();

protected:
    void createObjects();
    void createLayout();
    QGridLayout *mainLayout;
    QLabel *name;
    QLabel *toolTip;
    QLineEdit *nameEdit;
    QTextEdit *toolTipEdit;

    QString newName;
    QString newToolTip;

private:
    QDialogButtonBox *buttonBox;

};

#endif // INPUTDIALOG_H
