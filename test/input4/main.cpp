#include <iostream>

#include "structures.h"

#include "packageprovider.h"
#include "packagehandler.h"

void results(std::string s, int packets, int elapsed)
{
	if (!elapsed)
		elapsed = 1;
	std::cerr << s << " inspecting of " << packets * 243 << " events took " << elapsed << " ms" << std::endl;
	std::cerr << "MEvents/s : " << packets * 243 / elapsed / 1000. << std::endl;
	// std::cerr << number << " are in " << s << std::endl;
}

int main(int, char **)
{
	int		t;

	// struct DATA_PACKET orig, copy;

	PackageProvider pp;
	PackageHandler ph;

	QObject::connect(&pp, SIGNAL(bufferReceived(const DATA_PACKET &)),
		         &ph, SLOT(analyzeBuffer1(const DATA_PACKET &)));
	t = pp.run();
	results("package number", 400000, t);
	QObject::disconnect(&pp, SIGNAL(bufferReceived(const DATA_PACKET &)),
		         &ph, SLOT(analyzeBuffer1(const DATA_PACKET &)));

	QObject::connect(&pp, SIGNAL(bufferReceived(const DATA_PACKET &)),
		         &ph, SLOT(analyzeBuffer2(const DATA_PACKET &)));
	t = pp.run();
	results("package time", 400000, t);
	QObject::disconnect(&pp, SIGNAL(bufferReceived(const DATA_PACKET &)),
		         &ph, SLOT(analyzeBuffer2(const DATA_PACKET &)));

	QObject::connect(&pp, SIGNAL(bufferReceived(const DATA_PACKET &)),
		         &ph, SLOT(analyzeBuffer3(const DATA_PACKET &)));
	t = pp.run();
	results("events", 400000, t);
	QObject::disconnect(&pp, SIGNAL(bufferReceived(const DATA_PACKET &)),
		            &ph, SLOT(analyzeBuffer3(const DATA_PACKET &)));

	QObject::connect(&pp, SIGNAL(bufferReceived(const DATA_PACKET &)),
		         &ph, SLOT(analyzeBuffer4(const DATA_PACKET &)));
	t = pp.run();
	results("histogram events", 400000, t);
	QObject::disconnect(&pp, SIGNAL(bufferReceived(const DATA_PACKET &)),
		            &ph, SLOT(analyzeBuffer4(const DATA_PACKET &)));

	QObject::connect(&pp, SIGNAL(bufferReceived(const DATA_PACKET &)),
		         &ph, SLOT(analyzeBuffer5(const DATA_PACKET &)));
	t = pp.run();
	results("all histogram events", 400000, t);
	QObject::disconnect(&pp, SIGNAL(bufferReceived(const DATA_PACKET &)),
		            &ph, SLOT(analyzeBuffer5(const DATA_PACKET &)));
	return 0;
}
