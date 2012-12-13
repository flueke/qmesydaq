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
#ifndef MODULE_WIZARD_H
#define MODULE_WIZARD_H

#include <QWizard>
#include "ui_modulewizard.h"

class QMouseEvent;

/*!
    \class ModuleWizard

    \short This class handles the wizard for adding a MCPD to the setup

    \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class ModuleWizard : public QWizard, public Ui_ModuleWizard
{
	Q_OBJECT
public:
	ModuleWizard(const QString & = "192.168.168.121", const quint16 = 0, QWidget * = 0);
	
	QString ip(void);

	quint16 id(void);

	bool master(void);

	bool terminate(void);

	bool externsync(void);

protected:
	void accept();
};
#endif
