#include <QCoreApplication>
#include <QTimer>
#include <QElapsedTimer>
#include <QDebug>
#include <QHostInfo>
#include <QDateTime>

#include <iostream>
#include <detector.h>
#include <mdefines.h>
#include <qmlogging.h>



int main(int argc, char **argv)
{
	int meastime(10);
	QElapsedTimer eTimer;


	QCoreApplication app(argc, argv);

	if (argc > 1)
		meastime = strtol(argv[1], NULL, 10);

	startLogging("", "");
	DEBUGLEVEL = FATAL;

	eTimer.invalidate();

	MSG_FATAL << QHostInfo::fromName("taco6.taco.frm2").addresses().first();


	Detector *detector = new Detector();

	detector->addMCPD(1, "172.25.2.6", 54321);
	MSG_FATAL << "Lost " << detector->missedData() << " packets.";

	qDebug() << DEBUGLEVEL;

	if (detector)
	{
		QTimer::singleShot(meastime * 1000, &app, SLOT(quit()));
		eTimer.start();
		// detector->start();
		qDebug() << QDateTime::currentDateTime().toString();
	}
	else
	{
		std::cerr << "Could not install handler" << std::endl;
		QTimer::singleShot(1, &app, SLOT(quit()));
	}

	app.exec();

	quint64 packets = detector->receivedData();
	quint64 events = 243 * packets;

	MSG_FATAL << "Got " << packets << " packets with " << events << " events.";
	if (eTimer.isValid())
		MSG_FATAL << "Event rate: " << (events / (eTimer.elapsed() / 1000.)) << " Ev/s";
	MSG_FATAL << "Lost " << detector->missedData() << " packets.";

	delete detector;

	return 0;
}
