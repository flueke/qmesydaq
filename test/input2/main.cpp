#include <QCoreApplication>
#include <QTimer>
#include <QElapsedTimer>
#include <QHostInfo>
#include <QDateTime>
#include <QStringList>

#include <unistd.h>
#include <mcpd8.h>
#include <mdefines.h>
#include <logging.h>


int main(int argc, char **argv)
{
	QElapsedTimer eTimer;
	QString host("192.168.168.121");
	const quint16 port(54321);
	int meastime = 600;

	eTimer.invalidate();

	QCoreApplication app(argc, argv);

	QStringList args = app.arguments();

	if (args.size() > 1)
		host = args.at(1);

	if (args.size() > 2)
		meastime = args.at(2).toInt();

	// NetworkDevice	*nd = NetworkDevice::create(54321);
	// nd->connect_handler(QHostAddress(QHostInfo::fromName("taco6.ictrl.frm2").addresses().first()), 0, &analyzeBuffer, NULL))

	startLogging("", "");
	DEBUGLEVEL = ERROR;

	MSG_ERROR << QHostInfo::fromName("taco6.ictrl.frm2").addresses().first();

	MCPD8 *mcpd = new MCPD8(1, host, port);
	MSG_FATAL << "Lost " << mcpd->missedData() << " packets.";

	qDebug() << DEBUGLEVEL;

	if (mcpd)
	{
		MSG_ERROR << "Stop data stream";
		mcpd->stop();
		sleep(1);
		MSG_ERROR << "Start data acquisition for: " << meastime << " s";
		mcpd->start();
		QTimer::singleShot(meastime * 1000, &app, SLOT(quit()));
		eTimer.start();
		MSG_INFO << QDateTime::currentDateTime().toString();
	}
	else
	{
		MSG_ERROR << "Could not install handler";
		QTimer::singleShot(1, &app, SLOT(quit()));
	}

	app.exec();

	mcpd->stop();

	quint64 packets = mcpd->receivedData();
	quint64 events = 232 * packets;
	MSG_FATAL << "Got " << packets << " packets with " << events << " events.";
	if (eTimer.isValid())
	{
		float etime = eTimer.elapsed();
		MSG_FATAL << "Results for " << meastime << " s";
		MSG_FATAL << "Elapsed time: " << etime << " s";
		MSG_FATAL << "Packet rate: " << (packets / etime) << " packets/s";
		MSG_FATAL << "Event rate: " << (events / etime) << " Ev/s";
	}
	MSG_FATAL << "Lost " << mcpd->missedData() << " packets.";

	MSG_ERROR << QDateTime::currentDateTime().toString();
	delete mcpd;
	return 0;
}
