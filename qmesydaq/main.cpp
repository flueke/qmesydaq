/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2013 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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
#include <QSplashScreen>
#include <QPlastiqueStyle>
#include <QSettings>
#include "logging.h"

#if USE_TACO
#	include "TACOLoop.h"
#elif USE_CARESS
#	include "CARESSLoop.h"
#elif USE_TCP
#	include "TCPLoop.h"
#endif

const char* g_szShortUsage =
#if USE_TACO
	"[-n=<nethost>] "
#endif
	"[-f|--file|--config|-nf|--nofile|--noconfig]";
const char* g_szLongUsage =
#if USE_TACO
	"  -n=<nethost> set environment variable NETHOST\n"
#elif USE_CARESS
	"  -n=<name>    use this name in CORBA name service\n"
#endif
	"  -f=<file>\n  --file=<file>\n  --config=<file>\n" \
	"               load configuration from file\n" \
	"  -nf\n  --nofile\r\n  --noconfig   do not load last configuration automatically";

#include "LoopObject.h"
#include "QMesydaqDetectorInterface.h"
#include "MultipleLoopApplication.h"
#include "mainwindow.h"

#if defined(_MSC_VER)
	#include <io.h>
	#include <FCNTL.H>
	#include <Windows.h>
#endif

int main(int argc, char **argv)
{
#if defined(_MSC_VER)
	//Open the Console
	int hCrt;
	FILE *hf;

	AllocConsole();

	hCrt = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
	hf = _fdopen( hCrt, "w" );
	*stdout = *hf;
	setvbuf(stdout, NULL, _IONBF, 0);
#endif

	MultipleLoopApplication app(argc, argv);
	QStringList 		argList = app.arguments();
	LoopObject 		*loop(NULL);
	QString 		szLoadConfiguration(QString::null);

	startLogging(g_szShortUsage, g_szLongUsage);
	for (int i = 1; i < argList.size(); ++i)
	{
		QString szArgument = argList[i], szParameter;
		bool bSeparatedParameter(false);
		if (szArgument.indexOf('=') >= 0)
		{
			int iPos = szArgument.indexOf('=');
			szParameter = szArgument.mid(iPos + 1);
			szArgument.remove(iPos, szArgument.size() - iPos);
		}
		else
		{
			szParameter = argList[i];
			bSeparatedParameter = true;
		}

#if USE_TACO
		if (szArgument == "-n")
			setenv("NETHOST", szParameter.toStdString().c_str(), 1);
		else
#endif
		if (szArgument == "-f" || szArgument == "--file" || szArgument == "--config")
		{
			// load this configuration file
			if (szLoadConfiguration.isEmpty())
				szLoadConfiguration = szParameter;
			if (bSeparatedParameter)
				++i;
		}
		else if (szArgument == "-nf" || szArgument == "--nofile" || szArgument == "--noconfig")
		{
			// load no configuration
			szLoadConfiguration = "";
		}
	}

#if USE_TACO
	if (!getenv("NETHOST"))
	{
		MSG_DEBUG << QObject::tr("Environment variable \"NETHOST\" is not set");
		MSG_DEBUG << QObject::tr("You may set it explicitly in the command shell");
		MSG_DEBUG << QObject::tr("or by using command line option -n='nethost.domain'");
	}
	else
		loop = new TACOLoop;
#elif USE_CARESS
	loop = new CARESSLoop(argList);
#elif USE_TCP
	loop = new TCPLoop;
#endif
	if (loop)
		app.setLoopObject(loop);

	app.setStyle(new QPlastiqueStyle());

	QPixmap pixmap(":/images/mesytec.jpg");

	QSplashScreen splash(pixmap);
	splash.show();
	app.processEvents();

	app.setOrganizationName("MesyTec");
	app.setOrganizationDomain("mesytec.com");
	app.setApplicationName("QMesyDAQ");
	app.setWindowIcon(QIcon(":/images/logo32.png"));

	MainWindow mainWin;

	if (loop)
	{
		app.setQtInterface(new QMesyDAQDetectorInterface);
		app.setLoopEventReceiver(mainWin.centralWidget());
		QObject::connect(loop, SIGNAL(terminated()), mainWin.centralWidget(), SLOT(quitContinue()));
		QObject::connect(loop, SIGNAL(finished()), mainWin.centralWidget(), SLOT(quitContinue()));
	}
	app.processEvents();

	if (szLoadConfiguration.isNull())
	{
		// get last configuration file name
		QSettings settings(QSettings::IniFormat, QSettings::UserScope, app.organizationName(), app.applicationName());
		szLoadConfiguration = settings.value("lastconfigfile", "mesycfg.mcfg").toString();
	}
	app.processEvents();

	// load configuration
	if (!szLoadConfiguration.isEmpty())
		mainWin.doLoadConfiguration(szLoadConfiguration);

	mainWin.updateStatusBar();

	app.processEvents();
	splash.finish(&mainWin);

	mainWin.show();

	return app.exec();
}
