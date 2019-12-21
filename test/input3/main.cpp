#include <QCoreApplication>
#include <QTimer>
#include <QElapsedTimer>
#include <QDebug>
#include <QHostInfo>
#include <QDateTime>

#include <iostream>
#include <mesydaq2.h>
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


	Mesydaq2 *mesy = new Mesydaq2();

	mesy->addMCPD(1, "172.25.2.6", 54321);
	MSG_FATAL << "Lost " << mesy->missedData() << " packets.";

	qDebug() << DEBUGLEVEL;

	if (mesy)
	{
		QTimer::singleShot(meastime * 1000, &app, SLOT(quit()));
		eTimer.start();
		// mesy->start();
		qDebug() << QDateTime::currentDateTime().toString();
	}
	else
	{
		std::cerr << "Could not install handler" << std::endl;
		QTimer::singleShot(1, &app, SLOT(quit()));
	}

	app.exec();

	quint64 packets = mesy->receivedData();
	quint64 events = 243 * packets;

	MSG_FATAL << "Got " << packets << " packets with " << events << " events.";
	if (eTimer.isValid())
		MSG_FATAL << "Event rate: " << (events / (eTimer.elapsed() / 1000.)) << " Ev/s";
	MSG_FATAL << "Lost " << mesy->missedData() << " packets.";

	delete mesy;

	return 0;
}
