#include <iostream>

#include "packagehandler.h"

struct event
{
	bool trigger;
	quint16	amplitude;
	quint16 x;
	quint16 y;
	quint64	time;
}EVENT;

PackageHandler::PackageHandler()
{
}

void PackageHandler::clear()
{
	for (int i = 0; i < 2; ++i)
		h[i].clear();
	for (int i = 0; i < 12; ++i)
		s[i].clear();
}

void PackageHandler::analyzeBuffer1(const DATA_PACKET &dp)
{
	quint16 n = dp.bufferLength;
	quint64 headerTime = (quint64(dp.time[2]) << 32) | (dp.time[1] << 16) | dp.time[0];
}

void PackageHandler::analyzeBuffer2(const DATA_PACKET &dp)
{
	quint64 headerTime = (quint64(dp.time[2]) << 32) | (dp.time[1] << 16) | dp.time[0];
	quint16 nevents = (dp.bufferLength - dp.headerLength) / 3;

	for (int i = 0; i < nevents; ++i)
	{
		int k = i * 3;
		quint64 event = (quint64(dp.data[k + 2]) << 32) | dp.data[k + 1] << 16 | dp.data[k];
	}
}

void PackageHandler::analyzeBuffer3(const DATA_PACKET &dp)
{
	quint64	headerTime = (quint64(dp.time[2]) << 32) | (dp.time[1] << 16) | dp.time[0];
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

void PackageHandler::analyzeBuffer4(const DATA_PACKET &dp)
{
	quint64	headerTime = (quint64(dp.time[2]) << 32) | (dp.time[1] << 16) | dp.time[0];
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

void PackageHandler::analyzeBuffer5(const DATA_PACKET &dp)
{
	quint64	headerTime = (quint64(dp.time[2]) << 32) | (dp.time[1] << 16) | dp.time[0];
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

void PackageHandler::analyzeBuffer6(const DATA_PACKET &dp)
{
	quint64	headerTime = (quint64(dp.time[2]) << 32) | (dp.time[1] << 16) | dp.time[0];
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
		for (int l = 0; l < 2; ++l)
			s[l].incVal(ev.x);
	}
}
