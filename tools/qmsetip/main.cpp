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
	DEBUGLEVEL = FATAL /* NOTICE */;

	QCoreApplication app(argc, argv);

	QString fromIP = "192.168.168.121";
	QString toIP = "192.168.168.122";
	int 	id = 0;
	qint16	ipCount = 0;
	
	QStringList args = app.arguments();
	if (args.size() > 1)
	{
		for (int i = 1; i < args.size(); ++i)
			if (args.at(i) == "-h")
			{
				qDebug() << argv[0] << ": [from ipadress [default=192.168.168.121]] [to ipadress [default=192.168.168.122]] [module id [default=0]]";
				return 1;
			}
			else if (args.at(i).count('.'))	// may be ip address
			{
				if (!ipCount)
					fromIP = args.at(i);
				else
					toIP = args.at(i);
				++ipCount;
			}
			else
				id = args.at(i).toInt();
	}

	if (ipCount == 1)
	{
		toIP = fromIP;
		fromIP = "192.168.168.121";
	}

	MCPD8 *m = new MCPD8(id, NULL, fromIP);

	qDebug() << QObject::tr("%2 module : %1 (id=%3)").arg(m->version()).arg(m->ip()).arg(id);
	qDebug() << QObject::tr("new IP address : %1").arg(toIP);

	m->setProtocol(toIP);

	QTimer::singleShot(50, &app, SLOT(quit()));

	app.exec();

	delete m;

	return 0;
}
