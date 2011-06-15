#include <QCoreApplication>
#include <QTimer>
#include <QDebug>

#include <iostream>
#include <mcpd8.h>
#include <mesydaq2.h>
#include <mdefines.h>

int main(int argc, char **argv)
{
	DEBUGLEVEL = NOTICE;

	QCoreApplication app(argc, argv);

	QString fromIP = "192.168.168.121";
	QString toIP = "192.168.168.122";
	int 	id = 0;

	if (argc > 1)
	{
		if (QString(argv[1]) == "-h")
		{
			qDebug() << argv[0] << ": [from ipadress [default=192.168.168.121]] [to ipadress [default=192.168.168.122]] [module id [default=0]]";
			return 1;
		}
		toIP = argv[1];
	}
	if (argc > 2)
	{
		toIP = argv[2];
		fromIP = argv[1];
	}
	if (argc > 3)
		id = QString(argv[3]).toInt();

	MCPD8 *m = new MCPD8(id, NULL, fromIP);

	qDebug() << QObject::tr("module 2 : %1").arg(m->version());

	m->setProtocol(toIP);

	QTimer::singleShot(50, &app, SLOT(quit()));

	app.exec();

	delete m;

	return 0;
}
