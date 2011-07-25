/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2011 by Jens Kr�ger <jens.krueger@frm2.tum.de>     *
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
#ifndef GENERAL_SETUP_H
#define GENERAL_SETUP_H

#include <QDialog>
#include "ui_generalsetup.h"

class Mesydaq2;

class GeneralSetup : public QDialog, public Ui_GeneralSetup
{
	Q_OBJECT
public:
	GeneralSetup(Mesydaq2 *, QWidget * = 0);

	QString configFilePath(void) {return configfilepath->text();}

	QString listFilePath(void) {return listfilepath->text();}

	QString histFilePath(void) {return histfilepath->text();}

private slots:
	void selectConfigpathSlot();

	void selectHistpathSlot();

	void selectListpathSlot();
};
#endif
