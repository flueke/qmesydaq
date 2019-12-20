/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2019 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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
#ifndef CARESS_SETUP_H
#define CARESS_SETUP_H

#include <QDialog>
#include "ui_caresssetup.h"

/*!
    \class CARESSSetup

    \short This class handles the setup dialog for setting up the CARESS remote interface

    \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class CARESSSetup : public QDialog, public Ui_CARESSSetup
{
	Q_OBJECT
public:
	CARESSSetup(QWidget * = 0);

	virtual ~CARESSSetup(){}

public slots:
	virtual void accept(void);
};
#endif
