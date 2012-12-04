#include <QTest>
#include <QDebug>

#include "mdefines.h"

#include "histmap.h"
#include "mappedhistogram.h"

class HistoMapping : public QObject
{
	Q_OBJECT
public:
	HistoMapping();

	~HistoMapping();

private slots:

	void initTestCase();

	void TubesTest();

	void HistoryMapTest_data();

	void HistoryMapTest();

	void LinearMapTest_data();

	void LinearMapTest();

	void cleanupTestCase();

private:

	HistogramMapCorrection	*hMap;
	LinearMapCorrection	*lMap;
	Histogram		*hist;
	MappedHistogram		*mHist;
	MappedHistogram		*m2Hist;

	QList<quint16>		tubes;
};
