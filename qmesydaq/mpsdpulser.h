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
#ifndef MPSD_PULSER_H
#define MPSD_PULSER_H

#include <QDialog>
#include "ui_mpsdpulser.h"
#include "pulsertest.h"

class Detector;

/*!
    \class ModuleSetup

    \short This class handles the setup dialog for setting up the MPSD modules

    \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class MPSDPulser : public QDialog, public Ui_MPSDPulser
{
	Q_OBJECT
public:
	MPSDPulser(Detector *, QWidget * = 0);

public:
	void setModule(int);

public slots:
	void setMCPD(int = -1);

signals:
	void pulserTest(bool);

	void clear();

protected:
	void closeEvent(QCloseEvent *);

private slots:
	void displayMCPDSlot(int = -1);

	void displayMPSDSlot(int = -1);

	void amplitudeChanged(int);

	void setPulser(int);

	void setPulserPosition(bool);

	void setChannel(int);

	void pulserTestSlot(bool onoff);

	void pulserTestFinished()
	{
		pulserTest(false);
	}

	void nextStep();

	void pulserOn();

	void pulserOff();

private:
	void updatePulser();

	void display();

private:
	//! The detector object
	Detector		*m_detector;

	//! guard to disable the updates of the pulser
	bool			m_enabled;

	//!
	QList<puls>		m_pulses;

	//!
	QList<puls>::iterator	m_it;
};
#endif
