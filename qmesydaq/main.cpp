/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009 by Jens Kr�ger <jens.krueger@frm2.tum.de>          *
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

#include <iostream>

#include <QSplashScreen>
#include <QPlastiqueStyle>
#include <QDebug>
#include <QSettings>

#if USE_TACO
#	include "TACOLoop.h"
#endif
#if USE_CARESS
#	include "CARESSLoop.h"
#endif

#include "LoopObject.h"
#include "QMesydaqDetectorInterface.h"
#include "MultipleLoopApplication.h"
#include "mainwindow.h"

int main(int argc, char **argv)
{
    	MultipleLoopApplication app(argc, argv);

        QStringList argList = app.arguments();

        LoopObject *loop = NULL;
	int i;

#if USE_TACO
	for (i = 0; i < argList.size(); ++i)
	{
		if (argList[i].startsWith("-n") && (++i < argList.size()))
			setenv("NETHOST",  argList[i].toStdString().c_str(), 1); 
	}
        if (!getenv("NETHOST"))
	{
		qDebug() << "Environment variable \"NETHOST\" is not set";
		qDebug() << "You may set it explicitly in the command shell";
		qDebug() << "or by using command line option -n 'nethost.domain'";
	}
	else 
		loop = new TACOLoop;
#endif
#if USE_CARESS
	loop = new CARESSLoop(argList);
#endif
        app.setStyle(new QPlastiqueStyle());

        QPixmap pixmap(":/images/mesytec.jpg");

        QSplashScreen splash(pixmap);
        splash.show();
        app.processEvents();

        app.setOrganizationName("MesyTec");
        app.setOrganizationDomain("mesytec.com");
        app.setApplicationName("Mesydaq2");

        Mesydaq2MainWindow mainWin;

	if (loop)
	{
        	app.setLoopObject(loop);
        	app.setQtInterface(new QMesyDAQDetectorInterface);
        	app.setLoopEventReceiver(mainWin.centralWidget());
		QObject::connect(loop, SIGNAL(terminated()), mainWin.centralWidget(), SLOT(quitContinue()));
	}

        mainWin.show();

	for (i=argList.count()-1; i>=0; --i)
	{
	  if (argList[i]=="-f" || argList[i]=="--file" || argList[i]=="--config")
	  {
	    // load this configuration file
	    mainWin.doLoadConfiguration(argList[i+1]);
	    break;
	  }
	  if (argList[i]=="-nf" || argList[i]=="--nofile" || argList[i]=="--noconfig")
	  {
	    // load no configuration
	    i=0;
	    break;
	  }
	}
	if (i<0)
	{
	  // load last configuration file
	  QSettings settings("MesyTec", "QMesyDAQ");
	  mainWin.doLoadConfiguration(settings.value("lastconfigfile", "mesycfg.mcfg").toString());
	}

        app.processEvents();
        splash.finish(&mainWin);

	return app.exec();
}
