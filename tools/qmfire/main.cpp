#include <QCoreApplication>
#include <QTimer>
#include <QDebug>
#include <QStringList>

#include <QUdpSocket>

#include <cstdlib>

#include "structures.h"
#include "mdefines.h"
#include "logging.h"

void version(void)
{
	qDebug() << "version : " << VERSION;
}

void help(const QString &program)
{
	qDebug() << program << ": [-h] [-v] [ipadress [default=192.168.168.5]] [module id [default=0]]";
	version();
}

quint64 createEvent(void)
{
	quint64 ret(0);

	long r = random();
// generate module id from the last three bits
	ret |= quint64(r & 0x7) << 44;	
// generate slot id from the bits 3 to 5
	ret |= quint64((r & 0x38) >> 3) << 39;
// generate amplitude from the bits 0..9 of the high word
	r = random();
	ret |= quint64((r >> 16) & 0x3F) << 29;
// generate position from the bits 0..9 of the low word
	ret |= quint64(r & 0x3F) << 19;
// timestamp
	return ret;
}

int main(int argc, char **argv)
{
	DEBUGLEVEL = FATAL;

	QCoreApplication app(argc, argv);

	QString ip = "192.168.168.5";
	int	id = 0;

	QStringList args = app.arguments();
	if (args.size() > 1)
	{
		for (int i = 1; i < args.size(); ++i)
			if (args.at(i) == "-h")
			{
				help(argv[0]);
				return 1;
			}
			else if (args.at(i) == "-v")
			{
				version();
				return 1;
			}
			else if (args.at(i).count('.'))	// may be ip address
				ip = args.at(i);
			else
				id = args.at(i).toInt();
	}

	DATA_PACKET	dataPacket;

	qDebug() << "sizeof(dataPacket) " << sizeof(dataPacket);

	dataPacket.bufferLength = 750;
	dataPacket.bufferType = 0;
	dataPacket.headerLength = 21;

	dataPacket.runID = 1;
	dataPacket.deviceId = 0;
	dataPacket.deviceStatus = 0b11;

	for (int i = 0; i < 3; ++i)
		dataPacket.time[i] = 0;

	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 3; ++j)
			dataPacket.param[i][j] = 0;

	QUdpSocket *socket = new QUdpSocket();

//	socket->connectToHost(ip, 54321);

	for (int j = 0; j < 100000; ++j)
	{
		dataPacket.bufferNumber = j;
		for (int i = 0; i < 729; i += 3)
		{
			quint64 e = createEvent();
			dataPacket.data[i] = e & 0xFFFF;
			dataPacket.data[i + 1] = (e >> 16) & 0xFFFF;
			dataPacket.data[i + 2] = (e >> 32) & 0xFFFF;
		}	
		socket->writeDatagram(reinterpret_cast<const char *>(&dataPacket), sizeof(dataPacket), QHostAddress(ip), 54321);
		usleep(10);
	}

//	QTimer::singleShot(50, &app, SLOT(quit()));

//	app.exec();

	return 0;
}
