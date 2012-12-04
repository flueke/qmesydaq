#include "histomapping.h"

HistoMapping::HistoMapping() 
	: QObject()
	, hMap(NULL)
	, lMap(NULL)
	, hist(NULL)
	, mHist(NULL)
	, m2Hist(NULL)
{
}

HistoMapping::~HistoMapping()
{
	delete m2Hist;
	delete mHist;
	delete hist;
	delete lMap;
	delete hMap;
}

void HistoMapping::initTestCase()
{
	for (quint16 i = 0; i < 16; ++i)
		tubes << i;
	for (quint16 i = 24; i < 40; ++i)
		tubes << i;

	hMap = new HistogramMapCorrection(32, tubes);
	lMap = new LinearMapCorrection(QSize(32, 32), QSize(8, 8));

	hist = new Histogram(40, 32);

	hist->setValue( 0,  0,  1);
	hist->setValue( 0, 10, 10);
	hist->setValue(10, 10, 20);
	hist->setValue(15, 10, 25);
	hist->setValue(24, 10, 34);
	hist->setValue(32, 10, 42);
	hist->setValue(34, 10, 44);
	hist->setValue(38, 10, 48);
	hist->setValue(39, 10, 49);

	mHist = new MappedHistogram(hMap, hist);

	m2Hist = new MappedHistogram(lMap, mHist);
}

void HistoMapping::TubesTest()
{
	QVERIFY(tubes.length() == 32);
	QVERIFY(tubes == QList<quint16>() << 0 << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 
			<< 10 << 11 << 12 << 13 << 14 << 15 << 24 << 25 << 26 << 27 << 28 << 29 
			<< 30 << 31 << 32 << 33 << 34 << 35 << 36 << 37 << 38 << 39);
}

void HistoMapping::HistoryMapTest_data()
{
	QTest::addColumn<QPoint>("source");
	QTest::addColumn<bool>("mapResult");
     	QTest::addColumn<QPoint>("histogram");
	QTest::addColumn<quint64>("value");

	QTest::newRow("source1")  << QPoint( 0,  0) << true  << QPoint( 0,  0) << quint64(1);
	QTest::newRow("source2")  << QPoint( 0, 10) << true  << QPoint( 0, 10) << quint64(10);
	QTest::newRow("source3")  << QPoint(10, 10) << true  << QPoint(10, 10) << quint64(20);
	QTest::newRow("source4")  << QPoint(15, 10) << true  << QPoint(15, 10) << quint64(25);

	QTest::newRow("source5")  << QPoint(16, 10) << false << QPoint(16, 10) << quint64(0);
	QTest::newRow("source6")  << QPoint(23, 10) << false << QPoint(23, 10) << quint64(0);

	QTest::newRow("source7")  << QPoint(24, 10) <<  true << QPoint(16, 10) << quint64(34);
	QTest::newRow("source8")  << QPoint(32, 10) <<  true << QPoint(24, 10) << quint64(42);
	QTest::newRow("source9")  << QPoint(34, 10) <<  true << QPoint(26, 10) << quint64(44);
	QTest::newRow("source10") << QPoint(38, 10) <<  true << QPoint(30, 10) << quint64(48);
	QTest::newRow("source11") << QPoint(39, 10) <<  true << QPoint(31, 10) << quint64(49);

	QTest::newRow("source12") << QPoint(40, 10) << false << QPoint(40, 10) << quint64(0);
}

void HistoMapping::HistoryMapTest()
{
	QFETCH(QPoint, source);
	QFETCH(bool, mapResult);
	QFETCH(QPoint, histogram);
	QFETCH(quint64, value);

	QPoint dest;
	float  corr;
	bool   r = hMap->getMap(source, dest, corr);

	QCOMPARE(r , mapResult);
	if (r)
	{
		QCOMPARE(dest, histogram);
		QCOMPARE(mHist->value(dest.x(), dest.y()), value);
	}
}

void HistoMapping::LinearMapTest_data()
{
	QTest::addColumn<QPoint>("source");
	QTest::addColumn<bool>("mapResult");
     	QTest::addColumn<QPoint>("histogram");
	QTest::addColumn<quint64>("value");

	QTest::newRow("source_1")  << QPoint(0, 0)   << true << QPoint(0, 0) << quint64(1);
	QTest::newRow("source_2")  << QPoint(0, 10)  << true << QPoint(0, 2) << quint64(10);
	QTest::newRow("source_3")  << QPoint(10, 10) << true << QPoint(2, 2) << quint64(20);
	QTest::newRow("source_4")  << QPoint(15, 10) << true << QPoint(3, 2) << quint64(25);
	QTest::newRow("source_7")  << QPoint(16, 10) << true << QPoint(4, 2) << quint64(34);
	QTest::newRow("source_8")  << QPoint(24, 10) << true << QPoint(6, 2) << quint64(86);
	QTest::newRow("source_9")  << QPoint(26, 10) << true << QPoint(6, 2) << quint64(86);
	QTest::newRow("source_10") << QPoint(30, 10) << true << QPoint(7, 2) << quint64(97);
	QTest::newRow("source_11") << QPoint(31, 10) << true << QPoint(7, 2) << quint64(97);
}

void HistoMapping::LinearMapTest()
{
	QFETCH(QPoint, source);
	QFETCH(bool, mapResult);
	QFETCH(QPoint, histogram);
	QFETCH(quint64, value);

	QPoint dest;
	float  corr;
	bool   r = lMap->getMap(source, dest, corr);

	QCOMPARE(r , mapResult);
	if (r)
	{
		QCOMPARE(dest, histogram);
		QCOMPARE(m2Hist->value(dest.x(), dest.y()), value);
	}
}

void HistoMapping::cleanupTestCase()
{
}
