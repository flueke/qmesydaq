#include <QCoreApplication>
#include <QTimer>
#include <QDebug>

#include <iostream>
#include <mcpd8.h>
#include <mesydaq2.h>
#include <mdefines.h>

int main(int argc, char **argv)
{
	MCPD8	*m[] = {NULL, NULL};

	DEBUGLEVEL = NOTICE;

	QCoreApplication app(argc, argv);

	Mesydaq2 *mesy(NULL);
		
#if 1
	m[0] = new MCPD8(0, "192.168.168.121", 54321, "192.168.168.5");
	qDebug() << QObject::tr("module 2 : %1").arg(m[0]->version());
	qDebug() << "MCPD capabilities " << m[0]->capabilities(false);
	qDebug() << "MPSD capabilities " << m[0]->capabilities(quint8(1));

	m[1] = new MCPD8(1, "192.168.168.122", 54321, "192.168.168.5");
	qDebug() << QObject::tr("module 1 : %1").arg(m[1]->version());
	qDebug() << "MCPD capabilities " << m[1]->capabilities(false);
	qDebug() << "MPSD capabilities " << m[1]->capabilities(quint8(7));

#if 0
# not really working
	m[0]->readRegister(102);
	m[0]->readRegister(103);
#endif

	float gv(18.00);

	gv = 1.0;
	m[1]->setGain(1, 8, gv);
	m[1]->setThreshold(1, 10);
	m[1]->setPulser(1, 0, MIDDLE, 50, true);

	gv = 1.0;
	m[0]->setGain(0, 1, gv);
	m[0]->setThreshold(0, 20);
	m[0]->setPulser(0, 0, MIDDLE, 50, true);
#else
	mesy = new Mesydaq2();

	qDebug() << QObject::tr("module 1 : %1").arg(mesy->getFirmware(0));
	qDebug() << QObject::tr("module 2 : %1").arg(mesy->getFirmware(1));
#endif

	QTimer::singleShot(50, &app, SLOT(quit()));

	app.exec();

	delete m[0];
	delete m[1];

	delete mesy;

	return 0;
}
