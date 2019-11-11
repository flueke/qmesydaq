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

public:
	PackageHandler();

	void clear(void);

public slots:
	void analyzeBuffer1(const DATA_PACKET &);

	void analyzeBuffer2(const DATA_PACKET &);

	void analyzeBuffer3(const DATA_PACKET &);

	void analyzeBuffer4(const DATA_PACKET &);

	void analyzeBuffer5(const DATA_PACKET &);

	void analyzeBuffer6(const DATA_PACKET &);

	void analyzeBuffer7(const DATA_PACKET &);

	void analyzeBuffer8(const DATA_PACKET &);

	void analyzeBuffer9(const DATA_PACKET &);

	void analyzeBuffer10(const DATA_PACKET &);

	void analyzeBuffer11(const DATA_PACKET &);
};
