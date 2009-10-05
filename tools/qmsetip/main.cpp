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

	QString fromIP = "192.168.168.122";
	QString toIP = "192.168.168.121";

	if (argc > 1)
		toIP = argv[1];
	if (argc > 2)
	{
		toIP = argv[2];
		fromIP = argv[1];
	}

	MCPD8 *m = new MCPD8(0, NULL, fromIP);

	qDebug() << QObject::tr("module 2 : %1").arg(m->version());

	m->setProtocol(toIP);

	QTimer::singleShot(50, &app, SLOT(quit()));

	app.exec();

	delete m;

	return 0;
}
