#include "../../lib/histogram.cpp"

int main(int, char **)
{
	Spectrum 	s;

	Histogram	h1;
	Histogram	h2;

	const quint32	hsize = 128 * 960;

	for (int i = 0; i < 10000000; ++i)
	{
		s.incVal(i % hsize);
		h1.incVal(i % 128, i % 960);
		h2.incVal(i % 128, i % 960);
	}
	return 0;
}
