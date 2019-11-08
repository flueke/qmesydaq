#include <QObject>

#include "structures.h"
#include "histogram.h"


class PackageHandler : public QObject
{
	Q_OBJECT

	Histogram	h[3];
	Spectrum	s[12];

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
};
