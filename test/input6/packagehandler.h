#include <QObject>

#include "structures.h"
#include "histogram.h"
#include "mappedhistogram.h"

#include "hist.h"


class PackageHandler : public QObject
{
	Q_OBJECT

	Histogram	*h[2];
	MappedHistogram *mh;
	Spectrum	*s[12];
	hist		*_h[2];
	quint64		nevents;

public:
	PackageHandler();

	void clear(void);

	quint64 events();

public slots:
	void analyzeBuffer1(QSharedDataPointer<SD_PACKET>, void *);

	void analyzeBuffer2(QSharedDataPointer<SD_PACKET>, void *);

	void analyzeBuffer3(QSharedDataPointer<SD_PACKET>, void *);

	void analyzeBuffer4(QSharedDataPointer<SD_PACKET>, void *);

	void analyzeBuffer5(QSharedDataPointer<SD_PACKET>, void *);

	void analyzeBuffer6(QSharedDataPointer<SD_PACKET>, void *);

	void analyzeBuffer7(QSharedDataPointer<SD_PACKET>, void *);

	void analyzeBuffer8(QSharedDataPointer<SD_PACKET>, void *);

	void analyzeBuffer9(QSharedDataPointer<SD_PACKET>, void *);

	void analyzeBuffer10(QSharedDataPointer<SD_PACKET>, void *);

	void analyzeBuffer11(QSharedDataPointer<SD_PACKET>, void *);

	void analyzeBuffer12(QSharedDataPointer<SD_PACKET>, void *);
};
