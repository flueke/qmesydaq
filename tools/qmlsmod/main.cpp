#include <QCoreApplication>
#include <QTimer>
#include <QDebug>
#include <QStringList>

#include <iostream>
#include <mcpd8.h>
#include <mesydaq2.h>
#include <mdefines.h>

void version(void)
{
	qDebug() << "version : " << VERSION;
}

void help(const QString &program)
{
	qDebug() << program << ": [-h] [-v] [ipadress [default=192.168.168.121]] [module id [default=0]]";
	version();
}

int main(int argc, char **argv)
{
	DEBUGLEVEL = FATAL;

	QCoreApplication app(argc, argv);

	QString ip = "192.168.168.121";
	int	id = 0;

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
	qDebug() << QObject::tr("%2 : MCPD : %1 (id=%3)").arg(m->version()).arg(m->ip()).arg(id);

	for (int i = 0; i < 8; ++i)
		qDebug() << QObject::tr("module %1 (%4): %2 %3").arg(i + 1).arg(m->getModuleId(i)).arg(m->version(i), 0, 'f', 2).arg(m->getModuleType(i));

	QTimer::singleShot(50, &app, SLOT(quit()));

	app.exec();

	delete m;

	return 0;
}
