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
#ifndef MODULE_SETUP_H
#define MODULE_SETUP_H

#include <QDialog>
#include "ui_modulesetup.h"

class Mesydaq2;

/*!
    \class ModuleSetup

    \short This class handles the setup dialog for setting up the MPSD modules

    \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class ModuleSetup : public QDialog, public Ui_ModuleSetup
{
	Q_OBJECT
public:
	ModuleSetup(Mesydaq2 *, QWidget * = 0);

public:
	void setModule(int);

public slots:
	void setMCPD(int);

private slots:
	void setPulserSlot();

	void setGainSlot();

	void setThresholdSlot();

	void readRegisterSlot();

	void writeRegisterSlot();

	void setModeSlot(bool);

	void displayMCPDSlot(int = -1);

	void displayMPSDSlot(int = -1);

	void displaySlot();

	void setHistogram1(bool);
	void setHistogram2(bool);
	void setHistogram3(bool);
	void setHistogram4(bool);
	void setHistogram5(bool);
	void setHistogram6(bool);
	void setHistogram7(bool);
	void setHistogram8(bool);

	void setActive1(bool);
	void setActive2(bool);
	void setActive3(bool);
	void setActive4(bool);
	void setActive5(bool);
	void setActive6(bool);
	void setActive7(bool);
	void setActive8(bool);

private:
	//! The MesyDaq objext
	Mesydaq2	*m_theApp;

};
#endif
