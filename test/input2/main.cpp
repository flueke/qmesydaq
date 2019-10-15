#include <QCoreApplication>
#include <QTimer>
#include <QElapsedTimer>
#include <QHostInfo>
#include <QDateTime>

#include <unistd.h>
#include <iostream>
#include <mcpd8.h>
#include <mdefines.h>
#include <logging.h>


QElapsedTimer eTimer;
const QString host("192.168.168.121");
const quint16 port(54321);

int main(int argc, char **argv)
{
	eTimer.invalidate();

	QCoreApplication app(argc, argv);

	// NetworkDevice	*nd = NetworkDevice::create(54321);
	// nd->connect_handler(QHostAddress(QHostInfo::fromName("taco6.taco.frm2").addresses().first()), 0, &analyzeBuffer, NULL))

	startLogging("", "");
	DEBUGLEVEL = ERROR;

	MSG_ERROR << QHostInfo::fromName("taco6.taco.frm2").addresses().first();

	MCPD8 *mcpd = new MCPD8(1, host, port);
	MSG_FATAL << "Lost " << mcpd->missedData() << " packets.";

	qDebug() << DEBUGLEVEL;

	if (mcpd)
	{
		mcpd->stop();
		sleep(1);
		mcpd->start();
		QTimer::singleShot(0.75 * 360 * 1000, &app, SLOT(quit()));
		eTimer.start();
		MSG_INFO << QDateTime::currentDateTime().toString();
	}
	else
	{
		std::cerr << "Could not install handler" << std::endl;
		QTimer::singleShot(1, &app, SLOT(quit()));
	}

	app.exec();

	mcpd->stop();

	quint64 events = 232 * mcpd->receivedData();
	MSG_FATAL << "Got " << mcpd->receivedData() << " packets with " << events << " events.";
	if (eTimer.isValid())
		MSG_FATAL << "Event rate: " << (events / (eTimer.elapsed() / 1000.)) << " Ev/s";
	MSG_FATAL << "Lost " << mcpd->missedData() << " packets.";

	MSG_ERROR << QDateTime::currentDateTime().toString();
	delete mcpd;
	return 0;
}
