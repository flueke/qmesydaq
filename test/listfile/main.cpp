#include <QCoreApplication>
#include <QTimer>
#include <QDebug>

#include <iostream>
#include <mcpd8.h>
#include <mesydaq2.h>
#include <mdefines.h>

int main(int argc, char **argv)
{
	QString ip[] = {"192.168.168.121", "192.168.168.122", };	
	quint16 port[] = {54321, 54321, };
	QString sourceIP[] = {"192.168.168.5", "192.168.168.5", };

	DEBUGLEVEL = NOTICE;

	QCoreApplication app(argc, argv);

	Mesydaq2 *mesy = new Mesydaq2();
		
	for (int i = 0; i < 2; i++)
		mesy->addMCPD(i, ip[i], port[i], sourceIP[i]);

	mesy->stop();

	mesy->setTimingSetup(0, true, true, false);
	mesy->setGain(0, 0, 8, float(1.0));
	mesy->setThreshold(0, 0, quint16(20));
	mesy->setPulser(0, 0, 0, MIDDLE, 50, true);

	mesy->setTimingSetup(1, false, true, false);
	mesy->setGain(1, 7, 8, float(1.0));
	mesy->setThreshold(1, 7, quint16(20));
	mesy->setPulser(1, 7, 0, MIDDLE, 50, true);

	mesy->acqListfile(true);
	mesy->setListfilename("test.mdat");

	mesy->start();
	QTimer::singleShot(500, mesy, SLOT(stop()));

	QTimer::singleShot(500, &app, SLOT(quit()));

	app.exec();

	delete mesy;

	return 0;
}
