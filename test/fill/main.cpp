#include "../../lib/histogram.cpp"

#include <iostream>
#include <QTime>

#include "hist.h"

const int events = 100000000;

int main(int, char **)
{
	QTime		t;

	t.start();
	Spectrum 	s;
	std::cerr << "init of s took " << t.elapsed() << " ms" << std::endl;

	t.restart();
	Histogram	h1;
	std::cerr << "init of h1 took " << t.elapsed() << " ms" << std::endl;
	t.restart();
	Histogram	h2;
	std::cerr << "init of h2 took " << t.elapsed() << " ms" << std::endl;

	t.restart();
	hist		h3;
	std::cerr << "init of h3 took " << t.elapsed() << " ms" << std::endl;

	const quint32	hsize = 128 * 960;

	t.restart();
	for (int i = 0; i < events; ++i)
		s.incVal(i % 960);
	int elapsed = t.elapsed();
	std::cerr << "inserting of " << events << " into s took " << elapsed << " ms" << std::endl;
	std::cerr << "events/s : " << events / elapsed * 1000 << std::endl;
	std::cerr << s.getTotalCounts() << " are in s" << std::endl;

	t.restart();
	for (int i = 0; i < events; ++i)
		h1.incVal(i % 960, i % 128);
	elapsed = t.elapsed();
	std::cerr << "inserting of " << events << " into h1 took " << elapsed << " ms" << std::endl;
	std::cerr << "events/s : " << events / elapsed * 1000 << std::endl;
	std::cerr << h1.getTotalCounts() << " are in h1" << std::endl;

	t.restart();
	for (int i = 0; i < events; ++i)
		h2.incVal(i % 960, i % 128);
	elapsed = t.elapsed();
	std::cerr << "inserting of " << events << " into h2 took " << elapsed << " ms" << std::endl;
	std::cerr << "events/s : " << events / elapsed * 1000 << std::endl;
	std::cerr << h2.getTotalCounts() << " are in h2" << std::endl;

	t.restart();
	for (int i = 0; i < events; ++i)
		h3.incVal(i % 960, i % 128);
	elapsed = t.elapsed();
	std::cerr << "inserting of " << events << " into h3 took " << elapsed << " ms" << std::endl;
	std::cerr << "events/s : " << events / elapsed * 1000 << std::endl;
	std::cerr << h3.getTotalCounts() << " are in h3" << std::endl;
	return 0;
}
