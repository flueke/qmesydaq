#include <QCoreApplication>
#include <QTimer>
#include <QDebug>
#include <QStringList>

#include <iostream>
#include <mcpd8.h>
#include <mesydaq2.h>
#include <mdefines.h>

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
				qDebug() << argv[0] << ": [-m] [-t] [ipadress [default=192.168.168.121]] [module id [default=0]]";
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

	MCPD8 *m = new MCPD8(id, NULL, ip);
	qDebug() << QObject::tr("%2 : MCPD : %1 (id=%3)").arg(m->version()).arg(m->ip()).arg(id);
	qDebug() << QObject::tr("Set master %1, terminate : %2").arg(master).arg(term);

	m->setTimingSetup(master, term);

	QTimer::singleShot(50, &app, SLOT(quit()));

	app.exec();

	delete m;

	return 0;
}
