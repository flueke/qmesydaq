/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009 by Jens Krüger <jens.krueger@frm2.tum.de>          *
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

#include <QApplication>
#include <QSplashScreen>
#include <QPlastiqueStyle>

#include <QDebug>

#include "mainwindow.h"
#include <cstdlib>

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	app.setApplicationVersion(VERSION);
#if USE_TACO
	QStringList argList = app.arguments();
	for (int i = 0; i < argList.size(); ++i)
	{
		if (argList[i].startsWith("-n") && (++i < argList.size()))
			setenv("NETHOST",  argList[i].toStdString().c_str(), 1); 
	}
	if (!getenv("NETHOST"))
	{
		qDebug() << "Environment variable \"NETHOST\" is not set";
		qDebug() << "You may set it explicitly in the command shell";
		qDebug() << "or by using command line option -n 'nethost.domain'";
		return 1;
	}
#endif
	app.setStyle(new QPlastiqueStyle());

	QPixmap pixmap(":/images/mesytec.jpg");

	QSplashScreen splash(pixmap);
	splash.show();
	app.processEvents();

	app.setOrganizationName("MesyTec");
	app.setOrganizationDomain("mesytec.com");
	app.setApplicationName("Mesydaq2");

	Mesydaq2MainWindow *mainWin = new Mesydaq2MainWindow();
	mainWin->resize(1280, 980);
	mainWin->show();
	app.processEvents();
	splash.finish(mainWin);

	return app.exec();
}

