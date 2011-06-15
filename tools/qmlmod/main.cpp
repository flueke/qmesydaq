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

	QString ip = "192.168.168.121";
	int	id = 0;

	if (argc > 1)
	{
		if (QString(argv[1]) == "-h")
		{
			qDebug() << argv[0] << ": [ipadress [default=192.168.168.121]] [module id [default=0]]";
			return 1;
		}
		ip = argv[1];
	}
	if (argc > 2)
		id = QString(argv[2]).toInt();

	MCPD8 *m = new MCPD8(id, NULL, ip);
	qDebug() << QObject::tr("%2 : MCPD : %1").arg(m->version()).arg(ip);

	for (int i = 0; i < 8; ++i)
		qDebug() << QObject::tr("module %1 : %2 %3").arg(i + 1).arg(m->getMpsdId(i)).arg(m->version(i));

	QTimer::singleShot(50, &app, SLOT(quit()));

	app.exec();

	delete m;

	return 0;
}
