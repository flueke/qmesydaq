#include <QCoreApplication>
#include <QTimer>
#include <QTime>
#include <QDebug>

#include <iostream>

#include "histogram.h"

#include "structures.h"

void results(std::string s, int packets, int elapsed)
{
	if (!elapsed)
		elapsed = 1;
	std::cerr << s << " of " << packets << " took " << elapsed << " ms" << std::endl;
	std::cerr << "s/copy : " << elapsed / 1000. / packets << std::endl;
}

int main(int, char**)
{
	const int	packets = 400000;
	QTime		t;

	struct DATA_PACKET orig, copy;

	orig.bufferLength = 750;
	orig.bufferType = 1;
	orig.headerLength = 21;
	orig.bufferNumber = 1;
	orig.runID = 1;
	orig.deviceStatus = 0;
	orig.deviceId = 0;
	for (int i = 0; i < 3; ++i)
		orig.time[0] = 0;
	// dp.param[4][3];
	// dp.data[750];

	t.start();
	for (int i = 0; i < packets; ++i)
	{
		copy = orig;
	}
	results("package copying", packets, t.elapsed());

	Histogram hOrig, hCopy;

	hOrig.incVal(0, 0);
	hOrig.incVal(959, 0);
	hOrig.incVal(959, 127);
	hOrig.incVal(0, 127);
	t.restart();
	int histograms(10000);
	for (int i = 0; i < histograms; ++i)
	{
		hCopy = hOrig;
	}
	results("histogram copying", histograms, t.elapsed());

#if 0
	std::cerr << copy.runID << std::endl;

	copy.runID = 2;
	std::cerr << orig.runID << " " << copy.runID << std::endl;
#endif

	return 0;
}

