/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2014 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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
#ifndef MDLL_SETUP_H
#define MDLL_SETUP_H

#include <QDialog>
#include "ui_mdllsetup.h"

class Detector;

/*!
    \class MdllSetup

    \short This class handles the setup dialog for setting up the MPSD modules

    \author Gregor Montermann <g.montermann@mesytec.com>
 */
class MdllSetup : public QDialog, public Ui_MdllSetup
{
	Q_OBJECT
public:
	MdllSetup(Detector *, QWidget * = 0);

public slots:
	void setMCPD(int);

private slots:
	void displaySlot(int = -1);

	void setTimingSlot();

	void setThresholdsSlot();

	void setEnergySlot();

	void setSpectrumSlot();

	void setDatasetSlot();

private:
	//! The detector object
	Detector	*m_detector;

};
#endif
