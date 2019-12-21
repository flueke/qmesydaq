#include <QCoreApplication>
#include <QElapsedTimer>
#include <QStringList>
#include <QTimer>

#include <mesydaq2.h>
#include <qmlogging.h>
#include <mdefines.h>
#include <histogram.h>
#include <measurement.h>


struct event
{
	bool trigger;
	quint16	amplitude;
	quint16 x;
	quint16 y;
	quint64	time;
};

class Mesydaq : public Mesydaq2
{

public:
	virtual quint16 width(void)
	{
		return 128;
	}

	virtual bool active(quint16, quint16, quint16)
	{
		return true;
	}
};

class TestApplication : public QCoreApplication
{
	Q_OBJECT
public:
	TestApplication(int &argc, char **argv)
		: QCoreApplication(argc, argv)
		, host("192.168.168.122")
		, meastime(20)
	{
		QStringList args = arguments();
		qRegisterMetaType<QSharedDataPointer<SD_PACKET> >("QSharedDataPointer<SD_PACKET>");

		startLogging("", "");
		DEBUGLEVEL = ERROR;

		if (args.size() > 1)
			host = args.at(1);

		if (args.size() > 2)
			meastime = args.at(2).toInt();
		mesy = new Mesydaq();
		mesy->addMCPD(1, host);

		meas = new Measurement(mesy);
		meas->resizeHistogram(128, 960);
		Histogram *h = meas->hist(Measurement::PositionHistogram);
		MSG_FATAL << h->width() << "x" << h->height();
		meas->setTimerPreset(1000 * meastime, true);
		MSG_FATAL << "timer " << meas->getTimerPreset();

		MSG_FATAL << "Lost " << mesy->missedData() << " packets.";
	}

	void start()
	{
		MSG_ERROR << "Start data acquisition for: " << meastime << " s";
		meas->start();
		QTimer::singleShot(meastime * 1000 + 1000, this, SLOT(finish()));
	}

	quint64 receivedData()
	{
		return mesy->receivedData();
	}

	void result(const QElapsedTimer &eTimer)
	{
		quint64 packets = mesy->receivedData();
		quint64 events = 232 * packets;
		MSG_FATAL << "Got " << packets << " packets with " << events << " events.";
		if (eTimer.isValid())
		{
			float etime = meas->getMeastime() / 1000.;
			MSG_FATAL << "Results for " << meastime << " s";
			MSG_FATAL << "Elapsed time: " << eTimer.elapsed() / 1000. << " s";
			MSG_FATAL << "Measurement time: " << etime << " s";
			MSG_FATAL << "Packet rate: " << (packets / etime) << " packets/s";
			MSG_FATAL << "Event rate: " << (events / etime) << " Ev/s";
			Histogram *h = meas->hist(Measurement::PositionHistogram);
			MSG_FATAL << "Position histogram : " << h->getTotalCounts();
			MSG_FATAL << h->width() << "x" << h->height();
		}
		MSG_FATAL << "Lost " << mesy->missedData() << " packets.";
#if 0
		for (int i = 0; i < 3; ++i)
			MSG_FATAL << "Counts: " << h[i].getTotalCounts();
#endif
	}

	void stop()
	{
		MSG_ERROR << "Stop data stream";
		meas->stop();
	}

	void clear()
	{
		meas->clearAllHist();
	}

public slots:

	void finish()
	{
		quit();
	}

private:
	Measurement	*meas;
	Mesydaq2	*mesy;
	QString		host;
	int		meastime;
};
