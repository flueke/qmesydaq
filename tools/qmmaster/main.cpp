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
#include <detector.h>
#include <mdefines.h>

void version(void)
{
	qDebug() << "version : " << VERSION;
}

void help(const QString &program)
{
	qDebug() << program << ": [-v] [-h] [-m] [-t] [ipadress [default=192.168.168.121]] [module id [default=0]]";
	version();
}

int main(int argc, char **argv)
{
	DEBUGLEVEL = FATAL;

	QCoreApplication app(argc, argv);

	QString ip = "192.168.168.121";
	int	id = 0;
	bool	master = false;
	bool	term = false;

	QStringList args = app.arguments();
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
			else if (args.at(i) == "-m")
				master = true;
			else if (args.at(i) == "-t")
				term = true;
			else if (args.at(i).count('.'))	// may be ip address
				ip = args.at(i);
			else
				id = args.at(i).toInt();
	}

	MCPD8 *m = new MCPD8(id, ip);
	qDebug() << QObject::tr("%2 : MCPD : %1 (id=%3)").arg(m->version()).arg(m->ip()).arg(id);
	qDebug() << QObject::tr("Set master %1, terminate : %2").arg(master).arg(term);

	m->setTimingSetup(master, term, false);

	QTimer::singleShot(50, &app, SLOT(quit()));

	app.exec();

	delete m;

	return 0;
}
