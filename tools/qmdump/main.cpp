/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2020 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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

#include <QCoreApplication>
#include <QTimer>
#include <QDebug>
#include <QStringList>

#include <iostream>
#include <mcpd8.h>
#include <mesydaq2.h>
#include <mdefines.h>
#include <qmlogging.h>

void readListfile(const QString &, bool = false, bool = false);

void version(void)
{
	qDebug() << "version : " << VERSION;
}

void help(const QString &program)
{
	qDebug() << program << ": [-v] [-h] [-p] [-t] [-d] list_mode_file";
	qDebug() << "       -p - print neutron positions";
	qDebug() << "       -t - print event timestamp";
	qDebug() << "       -d - increase debug level";
	version();
}

int main(int argc, char **argv)
{
	QCoreApplication app(argc, argv);

	int debug = ERROR; /* NOTICE */;

	// qInstallMsgHandler(messageToFile);

	QStringList 	args = app.arguments();
	QString		fileName;
	bool		fPrintPos(false);
	bool		fPrintTime(false);

	if (args.size() > 1)
	{
		for (int i = 1; i < args.size(); ++i)
			if (args.at(i) == "-h")
			{
				help(argv[0]);
				return 1;
			}
			else if (args.at(i) == "-v")
			{
				version();
				return 1;
			}
			else if (args.at(i) == "-p")
				fPrintPos = true;
			else if (args.at(i) == "-t")
				fPrintTime = true;
		        else if (args.at(i) == "-d")
				debug += 1;
			else
				fileName = args.at(i);
	}
	if (fileName.isEmpty())
	{
		help(argv[0]);
		return 1;
	}

// 	startLogging("", "");
	DEBUGLEVEL = debug;

	MSG_ERROR << QObject::tr("Read file : %1 ").arg(fileName);

	readListfile(fileName, fPrintPos, fPrintTime);

	QTimer::singleShot(50, &app, SLOT(quit()));

	app.exec();

	return 0;
}
