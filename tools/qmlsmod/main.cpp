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

	MCPD8 *m = new MCPD8(id, NULL, ip);

	QTextStream cout(stderr);
	cout << QObject::tr("%2 : MCPD : %1 (id=%3), cap: %4: TX mode: %5\n").arg(m->version()).arg(m->ip()).arg(id).arg(m->capabilities()).arg(m->getTxMode());

	for (int i = 0; i < 8; ++i)
		cout << QObject::tr("module %1 (%4): id: %2, version: %3, capabilities: %5, mode: %6\n").arg(i + 1).arg(m->getModuleId(i)).
			arg(m->version(i), 0, 'f', 2).arg(m->getModuleType(i)).arg(m->capabilities(i)).arg(m->getTxMode(i));

	QTimer::singleShot(50, &app, SLOT(quit()));

	app.exec();

	delete m;

	return 0;
}
