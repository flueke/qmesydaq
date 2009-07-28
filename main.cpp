/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann   *
 *   g.montermann@mesytec.com   *
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

#include "mainwindow.h"

static const char version[] = "0.8";

int main(int argc, char **argv)
{
	
//    KAboutData about("mesydaq2", I18N_NOOP("mesydaq2"), version, description,
//                      KAboutData::License_GPL, "(C) 2008 Gregor Montermann", 0, 0, "g.montermann@mesytec.com");
//    about.addAuthor( "Gregor Montermann", 0, "g.montermann@mesytec.com" );

	QApplication app(argc, argv);
	QPixmap pixmap(":/mesytec.jpg");

	QSplashScreen splash(pixmap);
	splash.show();
	app.processEvents();

	app.setOrganizationName("MesyTec");
	app.setOrganizationDomain("mesytec.com");
	app.setApplicationName("Mesydaq2");

	Mesydaq2MainWindow *mainWin = new Mesydaq2MainWindow();
	mainWin->resize(1280, 980);
	mainWin->show();
	splash.finish(mainWin);

//	mainWin->draw();
	return app.exec();
}

