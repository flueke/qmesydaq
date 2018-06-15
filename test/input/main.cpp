#include <QCoreApplication>
#include <QTimer>
#include <QElapsedTimer>
#include <QDebug>

#include <iostream>
#include <networkdevice.h>
#include <mdefines.h>


static int cts = 0;

QElapsedTimer eTimer;

void analyzeBuffer(QSharedDataPointer<SD_PACKET> pPacket, void *pParam)
{
	if (!cts)
		eTimer.start();
	cts++;
}

int main(int argc, char **argv)
{
	eTimer.invalidate();

	DEBUGLEVEL = NOTICE;

	QCoreApplication app(argc, argv);

	NetworkDevice	*nd = NetworkDevice::create(54321);

    	if (nd->connect_handler(QHostAddress("127.0.0.1"), 0, &analyzeBuffer, NULL))
		QTimer::singleShot(60000, &app, SLOT(quit()));
	else
	{
		std::cerr << "Could not install handler" << std::endl;
		QTimer::singleShot(1, &app, SLOT(quit()));
	}

	app.exec();

	NetworkDevice::destroy(nd);

	if (eTimer.isValid())
		std::cout << eTimer.elapsed() << std::endl;
	std::cout << "Got " << cts << " packets" << std::endl;

	return 0;
}
