#include <QCoreApplication>
#include <QTimer>
#include <QDebug>

#include <iostream>
#include <mcpd8.h>
#include <detector.h>
#include <mdefines.h>

int main(int argc, char **argv)
{
	QString ip[] = {"192.168.168.121", "192.168.168.122", };	
	quint16 port[] = {54321, 54321, };
	QString sourceIP[] = {"192.168.168.5", "192.168.168.5", };

	DEBUGLEVEL = NOTICE;

	QCoreApplication app(argc, argv);

	Detector *detector = new Detector();
		
	for (int i = 0; i < 2; i++)
		detector->addMCPD(i, ip[i], port[i], sourceIP[i]);

	detector->stop();

	detector->setTimingSetup(0, true, true, false);
	detector->setGain(0, 0, 8, float(1.0));
	detector->setThreshold(0, 0, quint16(20));
	detector->setPulser(0, 0, 0, MIDDLE, 50, true);

	detector->setTimingSetup(1, false, true, false);
	detector->setGain(1, 7, 8, float(1.0));
	detector->setThreshold(1, 7, quint16(20));
	detector->setPulser(1, 7, 0, MIDDLE, 50, true);

	detector->acqListfile(true);
	detector->setListfilename("test.mdat");

	detector->start();
	QTimer::singleShot(500, detector, SLOT(stop()));

	QTimer::singleShot(500, &app, SLOT(quit()));

	app.exec();

	delete detector;

	return 0;
}
