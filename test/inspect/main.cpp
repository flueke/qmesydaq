#include "../../lib/structures.h"
#include "../../lib/histogram.cpp"

#include <iostream>
#include <QTime>

void results(std::string s, int packets, int elapsed)
{
	if (!elapsed)
		elapsed = 1;
	std::cerr << s << " inspecting of " << packets * 243 << " events took " << elapsed << " ms" << std::endl;
	std::cerr << "MEvents/s : " << packets * 243 / elapsed / 1000. << std::endl;
	// std::cerr << number << " are in " << s << std::endl;
}

struct event
{
	bool trigger;
	quint16	amplitude;
	quint16 x;
	quint16 y;
	quint64	time;
}EVENT;

int main(int, char **)
{
	QTime		t;
	const int packets = 400000;
	struct DATA_PACKET dp;
	quint64		n(0);

	dp.bufferLength = 750;
	dp.bufferType = 1;
	dp.headerLength = 21;
	dp.bufferNumber = 1;
	dp.runID = 1;
	dp.deviceStatus = 0;
	dp.deviceId = 0;
	for (int i = 0; i < 3; ++i)
		dp.time[0] = 0;
	// dp.param[4][3];
	// dp.data[750];

	t.start();
	for (int i = 0; i < packets; ++i)
		n += dp.bufferLength;
	results("bufferLength", packets, t.elapsed());

	std::cerr << n << std::endl;

	quint64 headerTime(0);
	n = 0;
	t.restart();
	for (int i = 0; i < packets; ++i)
	{
		n += dp.bufferLength;
		headerTime = (quint64(dp.time[2]) << 32) | (dp.time[1] << 16) | dp.time[0];
	}
	results("headerTime", packets, t.elapsed());

	n = 0;
	t.restart();
	for (int i = 0; i < packets; ++i)
	{
		n += dp.bufferLength;
		headerTime = (quint64(dp.time[2]) << 32) | (dp.time[1] << 16) | dp.time[0];
		quint16 nevents = (dp.bufferLength - dp.headerLength) / 3;

		for (int j = 0; j < nevents; ++j)
		{
			int k = j * 3;
			quint64 event = (quint64(dp.data[k + 2]) << 32) | dp.data[k + 1] << 16 | dp.data[k];
		}
	}
	results("events", packets, t.elapsed());

	n = 0;
	t.restart();
	for (int i = 0; i < packets; ++i)
	{
		n += dp.bufferLength;
		headerTime = (quint64(dp.time[2]) << 32) | (dp.time[1] << 16) | dp.time[0];
		quint16 nevents = (dp.bufferLength - dp.headerLength) / 3;

		for (int j = 0; j < nevents; ++j)
		{
			int k = j * 3;
			quint64 evt = (quint64(dp.data[k + 2]) << 32) | dp.data[k + 1] << 16 | dp.data[k];
			struct event ev;
			ev.trigger = evt & (0x800000);
			ev.amplitude = (evt >> 39) & 0xFF;
			ev.x = (evt >> 19) & 0x2FF;
			ev.y = (evt >> 29) & 0x2FF;
			ev.time = headerTime + (evt & 0x3FFFF);
		}
	}
	results("inspect events", packets, t.elapsed());

	Histogram	h[3];
	Spectrum s[12];
	n = 0;
	t.restart();
	for (int i = 0; i < packets; ++i)
	{
		n += dp.bufferLength;
		headerTime = (quint64(dp.time[2]) << 32) | (dp.time[1] << 16) | dp.time[0];
		quint16 nevents = (dp.bufferLength - dp.headerLength) / 3;

		for (int j = 0; j < nevents; ++j)
		{
			int k = j * 3;
			quint64 evt = (quint64(dp.data[k + 2]) << 32) | dp.data[k + 1] << 16 | dp.data[k];
			struct event ev;
			ev.trigger = evt & (0x800000);
			ev.amplitude = (evt >> 39) & 0xFF;
			ev.x = (evt >> 19) & 0x2FF;
			ev.y = (evt >> 29) & 0x2FF;
			ev.time = headerTime + (evt & 0x3FFFF);
			h[0].incVal(ev.x, ev.y);
			s[0].incVal(ev.x);
		}
	}
	results("histogram/spectrum events", packets, t.elapsed());

	n = 0;
	t.restart();
	for (int i = 0; i < packets; ++i)
	{
		n += dp.bufferLength;
		headerTime = (quint64(dp.time[2]) << 32) | (dp.time[1] << 16) | dp.time[0];
		quint16 nevents = (dp.bufferLength - dp.headerLength) / 3;

		for (int j = 0; j < nevents; ++j)
		{
			int k = j * 3;
			quint64 evt = (quint64(dp.data[k + 2]) << 32) | dp.data[k + 1] << 16 | dp.data[k];
			struct event ev;
			ev.trigger = evt & (0x800000);
			ev.amplitude = (evt >> 39) & 0xFF;
			ev.x = (evt >> 19) & 0x2FF;
			ev.y = (evt >> 29) & 0x2FF;
			ev.time = headerTime + (evt & 0x3FFFF);
			for (int l = 0; l < 3; ++l)
				h[l].incVal(ev.x, ev.y);
		}
	}
	results("multiple histogram events", packets, t.elapsed());

	n = 0;
	t.restart();
	for (int i = 0; i < packets; ++i)
	{
		n += dp.bufferLength;
		headerTime = (quint64(dp.time[2]) << 32) | (dp.time[1] << 16) | dp.time[0];
		quint16 nevents = (dp.bufferLength - dp.headerLength) / 3;

		for (int j = 0; j < nevents; ++j)
		{
			int k = j * 3;
			quint64 evt = (quint64(dp.data[k + 2]) << 32) | dp.data[k + 1] << 16 | dp.data[k];
			struct event ev;
			ev.trigger = evt & (0x800000);
			ev.amplitude = (evt >> 39) & 0xFF;
			ev.x = (evt >> 19) & 0x2FF;
			ev.y = (evt >> 29) & 0x2FF;
			ev.time = headerTime + (evt & 0x3FFFF);
			for (int l = 0; l < 3; ++l)
				h[l].incVal(ev.x, ev.y);
			for (int l = 0; l < 12; ++l)
				s[l].incVal(ev.x);
		}
	}
	results("histogram and spectrum events", packets, t.elapsed());

#if 0
	t.restart();
	for (int i = 0; i < events; ++i)
		h2.incVal(i % 960, i % 128);
	results("h2", events, t.elapsed(), h2.getTotalCounts());

	t.restart();
	for (int i = 0; i < events; ++i)
		h3.incVal(i % 960, i % 128);
	results("h3", events, t.elapsed(), h3.getTotalCounts());
#endif
	return 0;
}
