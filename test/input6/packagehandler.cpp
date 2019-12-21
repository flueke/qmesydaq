#include <iostream>
#include <qmlogging.h>
#include "mapcorrect.h"
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
	: nevents(0)
{
	for (int i = 0; i < 2; ++i)
		h[i] = new Histogram();
	for (int i = 0; i < 12; ++i)
		s[i] = new Spectrum();
	MapCorrection *posHistMapCorrection = new LinearMapCorrection(QSize(128, 960), QSize(128, 960));
	mh = new MappedHistogram(posHistMapCorrection);
	int iDstX(-1);
	int iDstY(-1);
	float fCorrection(0.0);
	posHistMapCorrection->getMap(0, 0, iDstX, iDstY, fCorrection);
	for (int i = 0; i < 2; ++i)
		_h[i] = new hist(960, 960);
	std::cerr << fCorrection << std::endl;
}

void PackageHandler::clear()
{
	for (int i = 0; i < 2; ++i)
		h[i]->clear();
	for (int i = 0; i < 12; ++i)
		s[i]->clear();
	mh->clear();
	nevents = 0;
}

quint64 PackageHandler::events()
{
	return nevents;
}

void PackageHandler::analyzeBuffer1(QSharedDataPointer<SD_PACKET> pPacket, void *)
{
	const MDP_PACKET &dp = pPacket.constData()->mdp;
	quint16 n = dp.bufferLength;
	quint64 headerTime = (quint64(dp.time[2]) << 32) | (dp.time[1] << 16) | dp.time[0];
}

void PackageHandler::analyzeBuffer2(QSharedDataPointer<SD_PACKET> pPacket, void *)
{
	const MDP_PACKET &dp = pPacket.constData()->mdp;
	quint64 headerTime = (quint64(dp.time[2]) << 32) | (dp.time[1] << 16) | dp.time[0];
	quint16 events = (dp.bufferLength - dp.headerLength) / 3;

	nevents += events;
	for (int i = 0; i < events; ++i)
	{
		int k = i * 3;
		quint64 event = (quint64(dp.data[k + 2]) << 32) | dp.data[k + 1] << 16 | dp.data[k];
	}
}

void PackageHandler::analyzeBuffer3(QSharedDataPointer<SD_PACKET> pPacket, void *)
{
	const MDP_PACKET &dp = pPacket.constData()->mdp;
	quint64	headerTime = (quint64(dp.time[2]) << 32) | (dp.time[1] << 16) | dp.time[0];
	quint16 events = (dp.bufferLength - dp.headerLength) / 3;

	nevents += events;
	for (int j = 0; j < events; ++j)
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

void PackageHandler::analyzeBuffer4(QSharedDataPointer<SD_PACKET> pPacket, void *)
{
	const MDP_PACKET &dp = pPacket.constData()->mdp;
	quint64	headerTime = (quint64(dp.time[2]) << 32) | (dp.time[1] << 16) | dp.time[0];
	quint16 events = (dp.bufferLength - dp.headerLength) / 3;

	nevents += events;
	for (int j = 0; j < events; ++j)
	{
		int k = j * 3;
		quint64 evt = (quint64(dp.data[k + 2]) << 32) | dp.data[k + 1] << 16 | dp.data[k];
		struct event ev;
		ev.trigger = evt & (0x800000);
		ev.amplitude = (evt >> 39) & 0xFF;
		ev.x = (evt >> 19) & 0x2FF;
		ev.y = (evt >> 29) & 0x2FF;
		ev.time = headerTime + (evt & 0x3FFFF);
		h[0]->incVal(ev.x, ev.y);
		s[0]->incVal(ev.x);
	}
}

void PackageHandler::analyzeBuffer5(QSharedDataPointer<SD_PACKET> pPacket, void *)
{
	const MDP_PACKET &dp = pPacket.constData()->mdp;
	quint64	headerTime = (quint64(dp.time[2]) << 32) | (dp.time[1] << 16) | dp.time[0];
	quint16 events = (dp.bufferLength - dp.headerLength) / 3;

	nevents += events;
	for (int j = 0; j < events; ++j)
	{
		int k = j * 3;
		quint64 evt = (quint64(dp.data[k + 2]) << 32) | dp.data[k + 1] << 16 | dp.data[k];
		struct event ev;
		ev.trigger = evt & (0x800000);
		ev.amplitude = (evt >> 39) & 0xFF;
		ev.x = (evt >> 19) & 0x2FF;
		ev.y = (evt >> 29) & 0x2FF;
		ev.time = headerTime + (evt & 0x3FFFF);

		for (int l = 0; l < 2; ++l)
			h[l]->incVal(ev.x, ev.y);
		mh->incVal(ev.x, ev.y);
		for (int l = 0; l < 12; ++l)
			s[l]->incVal(ev.x);
	}
}

void PackageHandler::analyzeBuffer6(QSharedDataPointer<SD_PACKET> pPacket, void *)
{
	const MDP_PACKET &dp = pPacket.constData()->mdp;
	quint64	headerTime = (quint64(dp.time[2]) << 32) | (dp.time[1] << 16) | dp.time[0];
	quint16 events = (dp.bufferLength - dp.headerLength) / 3;

	nevents += events;
	for (int j = 0; j < events; ++j)
	{
		int k = j * 3;
		quint64 evt = (quint64(dp.data[k + 2]) << 32) | dp.data[k + 1] << 16 | dp.data[k];
		struct event ev;
		ev.trigger = evt & (0x800000);
		ev.amplitude = (evt >> 39) & 0xFF;
		ev.x = (evt >> 19) & 0x2FF;
		ev.y = (evt >> 29) & 0x2FF;
		ev.time = headerTime + (evt & 0x3FFFF);

		for (int l = 0; l < 2; ++l)
			_h[l]->incVal(ev.x, ev.y);
		// mh->incVal(ev.x, ev.y);
		for (int l = 0; l < 1; ++l)
		  	s[l]->incVal(ev.x);
	}
}

void PackageHandler::analyzeBuffer7(QSharedDataPointer<SD_PACKET> pPacket, void *)
{
	const MDP_PACKET &dp = pPacket.constData()->mdp;
	quint64	headerTime = (quint64(dp.time[2]) << 32) | (dp.time[1] << 16) | dp.time[0];
	quint16 events = (dp.bufferLength - dp.headerLength) / 3;

	nevents += events;
	for (int j = 0; j < events; ++j)
	{
		int k = j * 3;
		quint64 evt = (quint64(dp.data[k + 2]) << 32) | dp.data[k + 1] << 16 | dp.data[k];
		struct event ev;
		ev.trigger = evt & (0x800000);
		ev.amplitude = (evt >> 39) & 0xFF;
		ev.x = (evt >> 19) & 0x2FF;
		ev.y = (evt >> 29) & 0x2FF;
		ev.time = headerTime + (evt & 0x3FFFF);
		s[0]->incVal(ev.x);
	}
}

void PackageHandler::analyzeBuffer8(QSharedDataPointer<SD_PACKET> pPacket, void *)
{
	const MDP_PACKET &dp = pPacket.constData()->mdp;
	quint64	headerTime = (quint64(dp.time[2]) << 32) | (dp.time[1] << 16) | dp.time[0];
	quint16 events = (dp.bufferLength - dp.headerLength) / 3;

	nevents += events;
	for (int j = 0; j < events; ++j)
	{
		int k = j * 3;
		quint64 evt = (quint64(dp.data[k + 2]) << 32) | dp.data[k + 1] << 16 | dp.data[k];
		struct event ev;
		ev.trigger = evt & (0x800000);
		ev.amplitude = (evt >> 39) & 0xFF;
		ev.x = (evt >> 19) & 0x2FF;
		ev.y = (evt >> 29) & 0x2FF;
		ev.time = headerTime + (evt & 0x3FFFF);
		h[0]->incVal(ev.x, ev.y);
	}
}

void PackageHandler::analyzeBuffer9(QSharedDataPointer<SD_PACKET> pPacket, void *)
{
	const MDP_PACKET &dp = pPacket.constData()->mdp;
	quint64	headerTime = (quint64(dp.time[2]) << 32) | (dp.time[1] << 16) | dp.time[0];
	quint16 events = (dp.bufferLength - dp.headerLength) / 3;

	nevents += events;
	for (int j = 0; j < events; ++j)
	{
		int k = j * 3;
		quint64 evt = (quint64(dp.data[k + 2]) << 32) | dp.data[k + 1] << 16 | dp.data[k];
		struct event ev;
		ev.trigger = evt & (0x800000);
		ev.amplitude = (evt >> 39) & 0xFF;
		ev.x = (evt >> 19) & 0x2FF;
		ev.y = (evt >> 29) & 0x2FF;
		ev.time = headerTime + (evt & 0x3FFFF);
		mh->incVal(ev.x, ev.y);
	}
}

void PackageHandler::analyzeBuffer10(QSharedDataPointer<SD_PACKET> pPacket, void *)
{
	const MDP_PACKET &dp = pPacket.constData()->mdp;
	quint64	headerTime = (quint64(dp.time[2]) << 32) | (dp.time[1] << 16) | dp.time[0];
	quint16 events = (dp.bufferLength - dp.headerLength) / 3;

	nevents += events;
	for (int j = 0; j < events; ++j)
	{
		int k = j * 3;
		quint64 evt = (quint64(dp.data[k + 2]) << 32) | dp.data[k + 1] << 16 | dp.data[k];
		struct event ev;
		ev.trigger = evt & (0x800000);
		ev.amplitude = (evt >> 39) & 0xFF;
		ev.x = (evt >> 19) & 0x2FF;
		ev.y = (evt >> 29) & 0x2FF;
		ev.time = headerTime + (evt & 0x3FFFF);
		_h[0]->incVal(ev.x, ev.y);
	}
}

void PackageHandler::analyzeBuffer11(QSharedDataPointer<SD_PACKET> pPacket, void *)
{
	const MDP_PACKET &dp = pPacket.constData()->mdp;
	quint64	headerTime = (quint64(dp.time[2]) << 32) | (dp.time[1] << 16) | dp.time[0];
	quint16 events = (dp.bufferLength - dp.headerLength) / 3;

	nevents += events;
	for (int j = 0; j < events; ++j)
	{
		int k = j * 3;
		quint64 evt = (quint64(dp.data[k + 2]) << 32) | dp.data[k + 1] << 16 | dp.data[k];
		struct event ev;
		ev.trigger = evt & (0x800000);
		ev.amplitude = (evt >> 39) & 0xFF;
		ev.x = (evt >> 19) & 0x2FF;
		ev.y = (evt >> 29) & 0x2FF;
		ev.time = headerTime + (evt & 0x3FFFF);
		for (int i = 0; i < 2; ++i)
			_h[i]->incVal(ev.x, ev.y);
	}
}

void PackageHandler::analyzeBuffer12(QSharedDataPointer<SD_PACKET> pPacket, void *)
{
	const MDP_PACKET &dp = pPacket.constData()->mdp;
	quint64	headerTime = (quint64(dp.time[2]) << 32) | (dp.time[1] << 16) | dp.time[0];
	quint16 events = (dp.bufferLength - dp.headerLength) / 3;

	nevents += events;
	for (int j = 0; j < events; ++j)
	{
		int k = j * 3;
		quint64 evt = (quint64(dp.data[k + 2]) << 32) | dp.data[k + 1] << 16 | dp.data[k];
		struct event ev;
		ev.trigger = evt & (0x800000);
		ev.amplitude = (evt >> 39) & 0xFF;
		ev.x = (evt >> 19) & 0x2FF;
		ev.y = (evt >> 29) & 0x2FF;
		ev.time = headerTime + (evt & 0x3FFFF);
		for (int l = 0; l < 12; ++l)
			s[l]->incVal(ev.x);
	}
}
