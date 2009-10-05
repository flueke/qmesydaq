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

	MCPD8 *m = new MCPD8(0, NULL, "192.168.168.122");
	qDebug() << QObject::tr("module 2 : %1").arg(m->version());
//	qDebug() << QObject::tr("module 2 : %1").arg(m->version());
//	qDebug() << QObject::tr("module 2 : %1").arg(m->version());

	m->setProtocol("192.168.168.121");

#if 0
# not really working
	m[0]->readRegister(102);
	m[0]->readRegister(103);
#endif
	QTimer::singleShot(50, &app, SLOT(quit()));

	app.exec();

	delete m;

	return 0;
}
