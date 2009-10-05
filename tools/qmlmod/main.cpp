#include <QCoreApplication>
#include <QTimer>
#include <QDebug>

#include <iostream>
#include <mcpd8.h>
#include <mesydaq2.h>
#include <mdefines.h>

int main(int argc, char **argv)
{
	DEBUGLEVEL = FATAL;

	QCoreApplication app(argc, argv);

	MCPD8 *m = new MCPD8(0, NULL, "192.168.168.122");
	qDebug() << QObject::tr("MCPD : %1").arg(m->version());

	for (int i = 0; i < 8; ++i)
		qDebug() << QObject::tr("module %1 : %2 %3").arg(i + 1).arg(m->getMpsdId(i)).arg(m->version(i));

	QTimer::singleShot(50, &app, SLOT(quit()));

	app.exec();

	delete m;

	return 0;
}
