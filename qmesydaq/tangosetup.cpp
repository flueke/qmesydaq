/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2014 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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

#include "tangosetup.h"
#include "qmlogging.h"

/*!
    constructor

    \param parent
 */
TangoSetup::TangoSetup(QWidget *parent)
	 : QDialog(parent)
{
	setupUi(this);

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
	settings.beginGroup("TANGO");
	personalName->setText(settings.value("personal", "qm").toString());
	imageName->setText(settings.value("image", "qm/qmesydaq/image").toString());
	timerName->setText(settings.value("timer", "qm/qmesydaq/timer").toString());
	eventsName->setText(settings.value("events", "qm/qmesydaq/events").toString());
	counter0Name->setText(settings.value("counter0", "qm/qmesydaq/counter0").toString());
	counter1Name->setText(settings.value("counter1", "qm/qmesydaq/counter1").toString());
	counter2Name->setText(settings.value("counter2", "qm/qmesydaq/counter2").toString());
	counter3Name->setText(settings.value("counter3", "qm/qmesydaq/counter3").toString());
	counter4Name->setText(settings.value("counter4", "qm/qmesydaq/counter4").toString());
	counter5Name->setText(settings.value("counter5", "qm/qmesydaq/counter5").toString());
	settings.endGroup();
}

void TangoSetup::accept(void)
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
	settings.beginGroup("TANGO");
	settings.setValue("personal", personalName->text());
	settings.setValue("image", imageName->text());
	settings.setValue("timer", timerName->text());
	settings.setValue("events", eventsName->text());
	settings.setValue("counter0", counter0Name->text());
	settings.setValue("counter1", counter1Name->text());
	settings.setValue("counter2", counter2Name->text());
	settings.setValue("counter3", counter3Name->text());
	settings.setValue("counter4", counter4Name->text());
	settings.setValue("counter5", counter5Name->text());
	settings.endGroup();
	QDialog::accept();
}
