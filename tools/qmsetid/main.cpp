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

	QString toIP = "192.168.168.123";
	quint16	id = 0;

	if (argc > 1)
		id = QString(argv[1]).toUInt();
	if (argc > 2)
	{
		toIP = argv[1];
		id = QString(argv[2]).toUInt();
	}

	MCPD8 *m = new MCPD8(0, NULL, toIP);

	qDebug() << QObject::tr("module 2 : %1").arg(m->version());

	m->setId(id);

	QTimer::singleShot(50, &app, SLOT(quit()));

	app.exec();

	delete m;

	return 0;
}
