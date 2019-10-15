#include <QCoreApplication>
#include <QTimer>
#include <QElapsedTimer>
#include <QDebug>
#include <QHostInfo>
#include <QDateTime>

#include <iostream>
#include <mesydaq2.h>
#include <mdefines.h>
#include <logging.h>


QElapsedTimer eTimer;

int main(int argc, char **argv)
{
	eTimer.invalidate();

	QCoreApplication app(argc, argv);

	startLogging("", "");

	qDebug() << QHostInfo::fromName("taco6.taco.frm2").addresses().first();

	DEBUGLEVEL = ERROR;

	qDebug() << DEBUGLEVEL;
	Mesydaq2 *mesy = new Mesydaq2();

	mesy->addMCPD(1, "172.25.2.6", 54321);
	// MSG_FATAL << "Lost " << mcpd->missedData() << " packets.";

	qDebug() << DEBUGLEVEL;

	if (mesy)
	{
		QTimer::singleShot(0.75 * 360 * 1000, &app, SLOT(quit()));
		eTimer.start();
		mesy->start();
		qDebug() << QDateTime::currentDateTime().toString();
	}
	else
	{
		std::cerr << "Could not install handler" << std::endl;
		QTimer::singleShot(1, &app, SLOT(quit()));
	}

	app.exec();

	quint64 events = 0;
	MSG_FATAL << "Got " << mesy->receivedData() << " packets with " << events << " events.";
	if (eTimer.isValid())
		MSG_FATAL << "Event rate: " << (events / (eTimer.elapsed() / 1000.)) << " Ev/s";
	MSG_FATAL << "Lost " << mesy->missedData() << " packets.";

	delete mesy;

	qDebug() << QDateTime::currentDateTime().toString();
	return 0;
}
