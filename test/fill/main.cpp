#include "../../lib/histogram.cpp"

#include <iostream>
#include <QTime>

#include "hist.h"

void results(std::string s, int events, int elapsed, long number)
{
	std::cerr << "inserting of " << events << " into " << s << " took " << elapsed << " ms" << std::endl;
	std::cerr << "MEvents/s : " << events / elapsed / 1000 << std::endl;
	std::cerr << number << " are in " << s << std::endl;
}

int main(int, char **)
{
	QTime		t;
	const int events = 100000000;

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
	results("s", events, t.elapsed(), s.getTotalCounts());

	t.restart();
	for (int i = 0; i < events; ++i)
		h1.incVal(i % 960, i % 128);
	results("h1", events, t.elapsed(), h1.getTotalCounts());

	t.restart();
	for (int i = 0; i < events; ++i)
		h2.incVal(i % 960, i % 128);
	results("h2", events, t.elapsed(), h2.getTotalCounts());

	t.restart();
	for (int i = 0; i < events; ++i)
		h3.incVal(i % 960, i % 128);
	results("h3", events, t.elapsed(), h3.getTotalCounts());

	t.restart();
	for (int i = 0; i < events; ++i)
	{
		s.incVal(i % 960);
		h1.incVal(i % 960, i % 128);
		h2.incVal(i % 960, i % 128);
	}
	results("s, h1, h2", events, t.elapsed(), h3.getTotalCounts());

	Histogram h[3];
	Spectrum sp[12];
	t.restart();
	for (int i = 0; i < events; ++i)
	{
		for (int j = 0; j < 12; j++)
			sp[j].incVal(i % 960);
		for (int j = 0; j < 3; j++)
			h[j].incVal(i % 960, i % 128);
	}
	results("all histograms/spectra", events, t.elapsed(), h3.getTotalCounts());
	return 0;
}
