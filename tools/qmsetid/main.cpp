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
	qDebug() << program << ": [-v] [-h] [ipadress [default=192.168.168.121]] [from module id [default=0]] [module id [default=1]]";
	version();
}

int main(int argc, char **argv)
{
	DEBUGLEVEL = FATAL /* NOTICE */;

	QCoreApplication app(argc, argv);

	QString toIP = "192.168.168.121";
	qint16	id = 1;
	qint16	fromId = 0;
	qint16	idCount = 0;
	
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
				toIP = args.at(i);
			else
			{
				if (!idCount)
					fromId = args.at(i).toUInt();
				else
					id = args.at(i).toInt();
				++idCount;
			}
	}

	if (idCount == 1)
	{
		id = fromId;
		fromId = 0;
	}

	MCPD8 *m = new MCPD8(fromId, NULL, toIP);

	qDebug() << QObject::tr("%2 MCPD : %1 (id=%3)").arg(m->version()).arg(m->ip()).arg(fromId);
	qDebug() << QObject::tr("new id : %1").arg(id);

	m->setId(id);

	QTimer::singleShot(50, &app, SLOT(quit()));

	app.exec();

	delete m;

	return 0;
}
