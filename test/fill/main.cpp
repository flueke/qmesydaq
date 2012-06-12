#include "../../lib/histogram.cpp"

#include <iostream>
#include <QTime>

const int events = 10000000;

int main(int, char **)
{
	QTime		t;

	t.start();
	Spectrum 	s;

	std::cerr << "init of s tooks " << t.elapsed() << " ms" << std::endl;

	t.restart();
	Histogram	h1;
	std::cerr << "init of h1 tooks " << t.elapsed() << " ms" << std::endl;
	t.restart();
	Histogram	h2;
	std::cerr << "init of h2 tooks " << t.elapsed() << " ms" << std::endl;

	const quint32	hsize = 128 * 960;

	std::cerr << "fill " << events << " into a spectrum" << std::endl;

	t.restart();
	for (int i = 0; i < events; ++i)
	{
		s.incVal(i % 960);
		h1.incVal(i % 128, i % 960);
		h2.incVal(i % 128, i % 960);
	}
	std::cerr << "adding of " << events << " events tooks " << t.elapsed() << " ms" << std::endl;
	return 0;
}
