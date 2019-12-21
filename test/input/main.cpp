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

void analyzeDataBuffer(QSharedDataPointer<SD_PACKET>);

static unsigned int packets = 0;
static unsigned long events = 0;
int lastDataBufnum = -1;

QElapsedTimer eTimer;

void analyzeBuffer(QSharedDataPointer<SD_PACKET> pPacket, void *pParam)
{
	const MDP_PACKET *pMdp = &pPacket.constData()->mdp;
	if (!packets++)
	{
		eTimer.start();
		qDebug() << QDateTime::currentDateTime().toString();
	}
	if (pMdp == NULL) //  || pParam == NULL)
		return;
#if 0
	quint16 bufferType;	//!< the buffer type
	quint16 bufferNumber;	//!< number of the packet
#endif
	if ((pMdp->deviceStatus & 0x8))
	{
	}
	else
	{
	}
	if(pMdp->bufferType & CMDBUFTYPE)
	{
	}
	else
	{
		quint16 diff = pMdp->bufferNumber - lastDataBufnum;
		if(diff > 1 && pMdp->bufferNumber > 0 && lastDataBufnum != -1)
			qDebug() << QObject::tr("Lost %1 data buffers: current: %2, last: %3").arg(diff - 1).arg(pMdp->bufferNumber).arg(lastDataBufnum);
		lastDataBufnum = pMdp->bufferNumber;
	}
	analyzeDataBuffer(pPacket);
}

void analyzeDataBuffer(QSharedDataPointer<SD_PACKET> pPacket)
{
	const DATA_PACKET *dp = &pPacket.constData()->dp;
	quint64 headertime = dp->time[0] + (quint64(dp->time[1]) << 16) + (quint64(dp->time[2]) << 32);
	quint32 datalen = (dp->bufferLength - dp->headerLength) / 3;
	events += (dp->bufferLength - dp->headerLength) / 6;
	quint16 mod = dp->deviceId;
	if(dp->bufferType < 0x0003)
	{
// extract parameter values:
		for(quint8 i = 0; i < 4; i++)
		{
			quint64 var = 0;
			for(quint8 j = 0; j < 3; j++)
			{
				var <<= 16;
				var |= dp->param[i][2 - j];
			}
		}
	}
	for(quint32 i = 0, counter = 0; i < datalen; ++i, counter += 3)
	{
		quint64 tim = dp->data[counter + 1] & 0x7;
		tim <<= 16;
		tim += dp->data[counter];
		tim += headertime;
// id stands for the trigId and modId depending on the package type
		quint8 id = (dp->data[counter + 2] >> 12) & 0x7;
// not neutron event (counter, chopper, ...)
		if((dp->data[counter + 2] & TRIGGEREVENTTYPE))
		{
			quint8 dataId = (dp->data[counter + 2] >> 8) & 0x0F;
			quint8 olddataId = dataId;
			ulong data = ((dp->data[counter + 2] & 0xFF) << 13) + ((dp->data[counter + 1] >> 3) & 0x7FFF);
			if (dataId > ADC2ID)
			{
			}
		}
		else
		{
			quint8 slotId = (dp->data[counter + 2] >> 7) & 0x1F;
			quint8 modChan = (id << 3) + slotId;
			quint16 chan = modChan + (mod << 6);
			quint16 amp = ((dp->data[counter+2] & 0x7F) << 3) + ((dp->data[counter+1] >> 13) & 0x7),
				pos = (dp->data[counter+1] >> 3) & 0x3FF;
		}
	}
}

int main(int argc, char **argv)
{
	eTimer.invalidate();

	QCoreApplication app(argc, argv);

	int meastime = 1000;

	if (argc > 1)
		meastime = strtol(argv[1], NULL, 10) * 1000;

	startLogging("", "");

	DEBUGLEVEL = NOTICE;

	NetworkDevice	*nd = NetworkDevice::create(54321);

	MSG_ERROR << QHostInfo::fromName("taco6.taco.frm2").addresses().first();

	if (nd->connect_handler(QHostAddress(QHostInfo::fromName("taco6.taco.frm2").addresses().first()), 0, &analyzeBuffer, NULL))
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

	MSG_NOTICE << "Got " << packets << " packets with " << events << " events.";
	if (eTimer.isValid())
		MSG_NOTICE << "Event rate: " << (events / (eTimer.elapsed() / 1000.)) << " Ev/s";

	MSG_NOTICE << QDateTime::currentDateTime().toString();
	return 0;
}
