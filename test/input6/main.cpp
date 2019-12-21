#include <QCoreApplication>
#include <QTimer>
#include <QElapsedTimer>
#include <QDebug>
#include <QHostInfo>
#include <QDateTime>

#include <iostream>
#include <networkdevice.h>
#include <mdefines.h>
#include <qmlogging.h>

#include "packageprovider.h"
#include "packagehandler.h"

void analyzeDataBuffer(QSharedDataPointer<SD_PACKET>);

static unsigned int packets = 0;
static unsigned long events = 0;
int lastDataBufnum = -1;

QElapsedTimer eTimer;

void analyzeBuffer(QSharedDataPointer<SD_PACKET> pPacket, void *pParam)
{
	if (!packets++)
	{
		eTimer.start();
	}
	((PackageHandler *)pParam)->analyzeBuffer6(pPacket, NULL);
}

/**
 * Reads data package from `fast_sender` tool and analyzes the data buffer
 * including histogramming
 */
int main(int argc, char **argv)
{
	QCoreApplication app(argc, argv);

	startLogging("", "");

	DEBUGLEVEL = NOTICE;

	PackageHandler ph;

	eTimer.invalidate();

	int meastime = 1000;

	if (argc > 1)
		meastime = strtol(argv[1], NULL, 10) * 1000;

	NetworkDevice	*nd = NetworkDevice::create(54321);

	MSG_ERROR << QHostInfo::fromName("taco6.taco.frm2").addresses().first();

	if (nd->connect_handler(QHostAddress(QHostInfo::fromName("taco6.taco.frm2").addresses().first()), 0, &analyzeBuffer, &ph))
	{
		QTimer::singleShot(meastime, &app, SLOT(quit()));
		MSG_NOTICE << QDateTime::currentDateTime().toString();
	}
	else
	{
		MSG_ERROR << "Could not install handler.";
		QTimer::singleShot(1, &app, SLOT(quit()));
	}

	app.exec();

	NetworkDevice::destroy(nd);

	events = ph.events();

	if (events == 0 && packets > 0)
		events = 232 * packets;
	MSG_NOTICE << "Got " << packets << " packets with " << events << " events.";
	if (eTimer.isValid())
		MSG_NOTICE << "Event rate: " << (events / (eTimer.elapsed() / 1000.)) << " Ev/s";

	MSG_NOTICE << QDateTime::currentDateTime().toString();
	return 0;
}
