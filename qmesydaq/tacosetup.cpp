/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2013 by Jens Krüger <jens.krueger@frm2.tum.de>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Module Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Module Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Module Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <QSettings>
#include <QApplication>

#include "tacosetup.h"
#include "logging.h"

/*!
    constructor

    \param parent
 */
TACOSetup::TACOSetup(QWidget *parent)
	 : QDialog(parent)
{
	setupUi(this);

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
	settings.beginGroup("TACO");
	personalName->setText(settings.value("personal", "srv0").toString());
	detectorName->setText(settings.value("detector", "test/qmesydaq/det").toString());
	timerName->setText(settings.value("timer", "test/qmesydaq/timer").toString());
	eventsName->setText(settings.value("events", "test/qmesydaq/events").toString());
	counter0Name->setText(settings.value("counter0", "test/qmesydaq/counter0").toString());
	counter1Name->setText(settings.value("counter1", "test/qmesydaq/counter1").toString());
	counter2Name->setText(settings.value("counter2", "test/qmesydaq/counter2").toString());
	counter3Name->setText(settings.value("counter3", "test/qmesydaq/counter3").toString());
	settings.endGroup();
}

void TACOSetup::accept(void)
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
	settings.beginGroup("TACO");
	settings.setValue("personal", personalName->text());
	settings.setValue("detector", detectorName->text());
	settings.setValue("timer", timerName->text());
	settings.setValue("events", eventsName->text());
	settings.setValue("counter0", counter0Name->text());
	settings.setValue("counter1", counter1Name->text());
	settings.setValue("counter2", counter2Name->text());
	settings.setValue("counter3", counter3Name->text());
	settings.endGroup();
	QDialog::accept();
}
