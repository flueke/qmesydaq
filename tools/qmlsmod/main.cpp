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

#include <QCoreApplication>
#include <QTimer>
#include <QDebug>
#include <QStringList>
#include <QTextStream>

#include <iostream>
#include "mcpd8.h"
#include "mesydaq2.h"
#include "mdefines.h"

#include "logging.h"

void version(void)
{
	qDebug() << "version : " << VERSION;
}

void help(const QString &program)
{
	qDebug() << program << ": [-h] [-v] [ipadress [default=192.168.168.121]] [module id [default=0]]";
	version();
}

const char *g_szShortUsage = "[-v]";

const char *g_szLongUsage = "  -v print the version number";

QString capToString(int cap)
{
	QStringList ret;
	QTextStream cout(stderr);
#if !defined(_MSC_VER) || _MSC_VER > 1700
	if (!(cap & 0b111))
		return "unknown";
	if (cap & 0b100)
		ret << "TPA";
	if (cap & 0b010)
		ret << "TP";
	if (cap & 0b001)
		ret << "P";
#else
	if (!(cap & 0x7))
		return "unknown";
	if (cap & 0x4)
		ret << "TPA";
	if (cap & 0x2)
		ret << "TP";
	if (cap & 0x1)
		ret << "P";
#endif
	return ret.join("|");
}

QString modeToString(int mode)
{
	switch (mode)
	{
		case TPA:
			return "TPA";
		case TP:
			return "TP";
		case P:
			return "P";
		default:
			return "unknown";
	}
}

int main(int argc, char **argv)
{
	DEBUGLEVEL = FATAL;

	QCoreApplication app(argc, argv);

	QString ip = "192.168.168.121";
	int	id = 0;

        startLogging(g_szShortUsage, g_szShortUsage);
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
			else if (args.at(i).count('.'))	// may be ip address
				ip = args.at(i);
			else
				id = args.at(i).toInt();
	}

	MCPD8 *m = new MCPD8(id, ip);

	QTextStream cout(stderr);

	cout << QObject::tr("%2 : MCPD : %1 (FPGA: %6, id=%3), cap: %4: TX mode: %5\n").arg(m->version()).arg(m->ip()).arg(id)
					.arg(capToString(m->capabilities(false))).arg(modeToString(m->getTxMode())).arg(m->fpgaVersion());

	for (quint8 i = 0; i < 8; ++i)
		cout << QObject::tr("module %1 (%4): id: %2, version: %3, capabilities: %5, mode: %6\n").arg(i + 1).arg(m->getModuleId(i)).
			arg(m->version(i), 0, 'f', 2).arg(m->getModuleType(i)).arg(capToString(m->capabilities(i))).arg(modeToString(m->getTxMode(i)));

	QTimer::singleShot(50, &app, SLOT(quit()));

	app.exec();

	delete m;

	return 0;
}
