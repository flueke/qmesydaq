#include <QCoreApplication>
#include <QElapsedTimer>
#include <QStringList>
#include <QTimer>

#include <mcpd8.h>
#include <logging.h>
#include <mdefines.h>

#include <histogram.h>


struct event
{
	bool trigger;
	quint16	amplitude;
	quint16 x;
	quint16 y;
	quint64	time;
};

class TestApplication : public QCoreApplication
{
	Q_OBJECT
public:
	TestApplication(int &argc, char **argv)
		: QCoreApplication(argc, argv)
		, host("192.168.168.122")
		, port(54321)
		, meastime(20)
		, id(1)
	{
		QStringList args = arguments();
		qRegisterMetaType<QSharedDataPointer<SD_PACKET> >("QSharedDataPointer<SD_PACKET>");
	
		startLogging("", "");
		DEBUGLEVEL = ERROR;

		if (args.size() > 1)
		{
			host = args.at(1);
			if (host != "192.168.168.122")
				id = 0;
		}

		if (args.size() > 2)
			meastime = args.at(2).toInt();
		mcpd = new MCPD8(id, host, port);
		connect(mcpd, SIGNAL(analyzeDataBuffer(QSharedDataPointer<SD_PACKET>)), this, SLOT(analyzeBuffer(QSharedDataPointer<SD_PACKET>)));
		MSG_FATAL << "Lost " << mcpd->missedData() << " packets.";
	}

	virtual ~TestApplication()
	{
		delete mcpd;
	}

	void start()
	{
		MSG_ERROR << "Start data acquisition for: " << meastime << " s";
		mcpd->start();
		QTimer::singleShot(meastime * 1000, this, SLOT(finish()));
		evtCounter = 0;
		maxX = maxY = 0;
	}

	quint64 receivedData()
	{
		return mcpd->receivedData();
	}

	void result(const QElapsedTimer &eTimer)
	{
		quint64 packets = mcpd->receivedData();
		quint64 events = evtCounter;  // 232 * packets;
		MSG_FATAL << "Got " << packets << " packets with " << events << " events.";
		if (eTimer.isValid())
		{
			float etime = eTimer.elapsed() / 1000.;
			MSG_FATAL << "Results for " << meastime << " s";
			MSG_FATAL << "Elapsed time: " << etime << " s";
			MSG_FATAL << "Packet rate: " << (packets / etime) << " packets/s";
			MSG_FATAL << "Event rate: " << (events / etime) << " Ev/s";
		}
 		MSG_FATAL << "Lost " << mcpd->missedData() << " packets.";
		for (int i = 0; i < 3; ++i)
			MSG_FATAL << "Counts: " << h[i].getTotalCounts();
	}

	void stop()
	{
		MSG_ERROR << "Stop data stream";
		mcpd->stop();
	}

	void clear()
	{
		for (int i = 0; i < 3; ++i)
			h[i].clear();
		for (int i = 0; i < 12; ++i)
			s[i].clear();
	}


public slots:
	void finish()
	{
		MSG_ERROR << "Remaining packets: " << mcpd->queueLength();
		quit();
	}

	void analyzeBuffer(QSharedDataPointer<SD_PACKET> pPacket)
	{
		const DATA_PACKET &dp = pPacket.constData()->dp;
		quint64 headerTime = (quint64(dp.time[2]) << 32) | (dp.time[1] << 16) | dp.time[0];
		quint16 nevents = (dp.bufferLength - dp.headerLength) / 3;

		for (int i = 0; i < nevents; ++i,++evtCounter)
		{
			int k = i * 3;
			quint64 evt = (quint64(dp.data[k + 2]) << 32) | dp.data[k + 1] << 16 | dp.data[k];

			struct event ev;
			ev.trigger = evt & (0x800000);
			ev.amplitude = (evt >> 39) & 0xFF;
			ev.x = (evt >> 19) & 0x2FF;
			ev.y = (evt >> 29) & 0x2FF;
			ev.time = headerTime + (evt & 0x3FFFF);

			if (ev.x > maxX)
				maxX = ev.x;
			if (ev.y > maxY)
				maxY = ev.y;

			for (int l = 0; l < 3; ++l)
				h[l].incVal(ev.y, ev.x);
			for (int l = 0; l < 12; ++l)
				s[l].incVal(ev.x);
		}
		// MSG_FATAL << "Package analysis";
	}

private:
	MCPD8 *mcpd;
	QString host;
	quint16 port;
	int	meastime;
	quint64	evtCounter;
	int	id;

	quint32	maxX;
	quint32 maxY;

	Histogram	h[3];
	Spectrum	s[12];
};
