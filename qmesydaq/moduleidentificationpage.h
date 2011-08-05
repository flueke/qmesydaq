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
#ifndef MODULE_IDENTIFICATION_PAGE_H
#define MODULE_IDENTIFICATION_PAGE_H

#include <QWizardPage>
#include "ui_moduleidentificationpage.h"

class QMouseEvent;

class ModuleIdentificationPage : public QWizardPage, public Ui_ModuleIdentificationPage
{
	Q_OBJECT
public:
	ModuleIdentificationPage(QWidget * = 0);

	void initialize(const QString &ip = "192.168.168.121", const quint16 id = 0);

	bool isComplete() const;

private slots:
	void valueChanged();
};
#endif
