#include <iostream>

#include "structures.h"

#include "packageprovider.h"
#include "packagehandler.h"

void results(std::string s, int packets, int elapsed, bool evt = true)
{
	if (!elapsed)
		elapsed = 1;
	quint64 events = packets * 243;
	if (evt)
		std::cerr << s << " of " << events << " events took " << elapsed << " ms" << std::endl;
	else
		std::cerr << s << " of " << packets << " packets took " << elapsed << " ms" << std::endl;
	std::cerr << events / elapsed / 1000. << " MEvents/s" << std::endl;
	std::cerr << 1. * elapsed / events << " ms/Event" << std::endl << std::endl;
}

int main(int, char **)
{
	int		t;

	quint32 	packets(400000);

	// struct DATA_PACKET orig, copy;

	PackageProvider pp;
	PackageHandler ph;

	QObject::connect(&pp, SIGNAL(bufferReceived(const DATA_PACKET &)),
		         &ph, SLOT(analyzeBuffer1(const DATA_PACKET &)));
	ph.clear();
	t = pp.run();
	results("get header time", packets, t, false);
	QObject::disconnect(&pp, SIGNAL(bufferReceived(const DATA_PACKET &)),
		         &ph, SLOT(analyzeBuffer1(const DATA_PACKET &)));

	QObject::connect(&pp, SIGNAL(bufferReceived(const DATA_PACKET &)),
		         &ph, SLOT(analyzeBuffer2(const DATA_PACKET &)));
	ph.clear();
	t = pp.run();
	results("unpack events", packets, t);
	QObject::disconnect(&pp, SIGNAL(bufferReceived(const DATA_PACKET &)),
		         &ph, SLOT(analyzeBuffer2(const DATA_PACKET &)));

	QObject::connect(&pp, SIGNAL(bufferReceived(const DATA_PACKET &)),
		         &ph, SLOT(analyzeBuffer3(const DATA_PACKET &)));
	ph.clear();
	t = pp.run();
	results("analyze events", packets, t);
	QObject::disconnect(&pp, SIGNAL(bufferReceived(const DATA_PACKET &)),
		            &ph, SLOT(analyzeBuffer3(const DATA_PACKET &)));

	QObject::connect(&pp, SIGNAL(bufferReceived(const DATA_PACKET &)),
		         &ph, SLOT(analyzeBuffer4(const DATA_PACKET &)));
	ph.clear();
	t = pp.run();
	results("histogram events", packets, t);
	QObject::disconnect(&pp, SIGNAL(bufferReceived(const DATA_PACKET &)),
		            &ph, SLOT(analyzeBuffer4(const DATA_PACKET &)));

	QObject::connect(&pp, SIGNAL(bufferReceived(const DATA_PACKET &)),
		         &ph, SLOT(analyzeBuffer5(const DATA_PACKET &)));
	ph.clear();
	t = pp.run();
	results("all histogram events", packets, t);
	QObject::disconnect(&pp, SIGNAL(bufferReceived(const DATA_PACKET &)),
		            &ph, SLOT(analyzeBuffer5(const DATA_PACKET &)));

	QObject::connect(&pp, SIGNAL(bufferReceived(const DATA_PACKET &)),
		         &ph, SLOT(analyzeBuffer6(const DATA_PACKET &)));
	ph.clear();
	t = pp.run();
	results("realistic histogram events", packets, t);
	QObject::disconnect(&pp, SIGNAL(bufferReceived(const DATA_PACKET &)),
		            &ph, SLOT(analyzeBuffer6(const DATA_PACKET &)));

	QObject::connect(&pp, SIGNAL(bufferReceived(const DATA_PACKET &)),
		         &ph, SLOT(analyzeBuffer7(const DATA_PACKET &)));
	ph.clear();
	t = pp.run();
	results("put events into spectrum", packets, t);
	QObject::disconnect(&pp, SIGNAL(bufferReceived(const DATA_PACKET &)),
		            &ph, SLOT(analyzeBuffer7(const DATA_PACKET &)));

	QObject::connect(&pp, SIGNAL(bufferReceived(const DATA_PACKET &)),
		         &ph, SLOT(analyzeBuffer8(const DATA_PACKET &)));
	ph.clear();
	t = pp.run();
	results("put events into histogram", packets, t);
	QObject::disconnect(&pp, SIGNAL(bufferReceived(const DATA_PACKET &)),
		            &ph, SLOT(analyzeBuffer8(const DATA_PACKET &)));

	QObject::connect(&pp, SIGNAL(bufferReceived(const DATA_PACKET &)),
		         &ph, SLOT(analyzeBuffer9(const DATA_PACKET &)));
	ph.clear();
	t = pp.run();
	results("put events into mapped histogram", packets, t);
	QObject::disconnect(&pp, SIGNAL(bufferReceived(const DATA_PACKET &)),
		         &ph, SLOT(analyzeBuffer9(const DATA_PACKET &)));

	QObject::connect(&pp, SIGNAL(bufferReceived(const DATA_PACKET &)),
		         &ph, SLOT(analyzeBuffer10(const DATA_PACKET &)));
	ph.clear();
	t = pp.run();
	results("put events into new histogram impl", packets, t);
	QObject::disconnect(&pp, SIGNAL(bufferReceived(const DATA_PACKET &)),
		         &ph, SLOT(analyzeBuffer10(const DATA_PACKET &)));

	QObject::connect(&pp, SIGNAL(bufferReceived(const DATA_PACKET &)),
		         &ph, SLOT(analyzeBuffer11(const DATA_PACKET &)));
	ph.clear();
	t = pp.run();
	results("put events into 2 new histogram impl", packets, t);
	QObject::disconnect(&pp, SIGNAL(bufferReceived(const DATA_PACKET &)),
		         &ph, SLOT(analyzeBuffer11(const DATA_PACKET &)));
	return 0;
}
