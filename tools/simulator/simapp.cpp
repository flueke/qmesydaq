/***************************************************************************
 *   Copyright (C) 2013-2014 by Lutz Rossa <rossa@helmholtz-berlin.de>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

// simulate one or more MCPD-8 with MPSD-8
// usage: simulator --mcpd=...
#include <QTimerEvent>
#include <QDateTime>
#include <QTimer>
#include <QStringList>

#include <iostream>
#include <cmath>
#include <ctime>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#define CLOCK_MONOTONIC 1
int clock_gettime(int /* clockid_t*/ clk_id, struct timespec *tp)
{
	clock_serv_t cclock;
	mach_timespec_t mts;
	host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
	clock_get_time(cclock, &mts);
	mach_port_deallocate(mach_task_self(), cclock);
	tp->tv_sec = mts.tv_sec;
	tp->tv_nsec = mts.tv_nsec;
}
#endif

#include "utils.h"
#include "simapp.h"
#include "simmcpd8.h"
#include "logging.h"
#include "mdefines.h"

#include <algorithm>    // std::random_shuffle

const char *g_szShortUsage = "[[--mdll=127.0.0.2:0] | [--mstd=127.0.0.2:0] | [--mcpd=127.0.0.2:0] [--mcpd=127.0.0.3:1] ... [--width=64] [--height=960] [--v4]]"
			     " [--interval=20] [--ppe=5] [--stop=0]";
const char *g_szLongUsage =
                "  --mdll=<bind-ip>:<id>\n"
                "               bind a MDLL with ID to this IP address (id 0..255)\n"
		"  --mcpd=<bind-ip>:<id>[:type]\n"
		"               bind a MCPD (max 64) with ID to this IP address (id 0..255)\n"
                "               'type'  is optional and could be 'old', 'new', or 'sadc'\n"
		"  --mstd=<bind-ip>:<id>\n"
		"               bind a MCPD with an MSTD-16 module\n"
		"  --width=<n>  each MCPD with <n> channels (1..64, default 64)\n"
		"  --height=<n> spectrum height with <n> bins (8..1024, default 960)\n"
		"  --v4         generate a \"round\" detector like HZB-V4/SANS\n"
		"  --interval=<n>\n"
		"               packet generator interval in ms (1..1000, default 20)\n"
		"  --ppe=<n>    data packets per timer event (default 5)\n"
		"  --fast       do faster (less random) simulation\n"
		"  --stop=<n>   generate max. n data packages\n";

// print special packet with this number
//define PRINTPACKET 1

#if defined(PRINTPACKET) && PRINTPACKET>0
static int             m_iPrintPacket    = PRINTPACKET; // print special packet
#else
#undef PRINTPACKET
#endif

//   ADET with
//   - one main hill and
//   - two small one's
//   near the lower border in the left half of the area
struct point {
	double x_min,
	       x_max;  // 0 <= x_min < x_max <= 1
	double y_min,
	       y_max;  // 0 <= y_min < y_max <= 1
	double x_exp,
	       y_exp;  // exp < 0
	double height; // height >= 2
} allPoints[] = {
	{ 0.0,  1.0,  0.0, 1.0,  -5.0, -5.0, 20.0},   // main
	{ 0.0,  0.25, 0.0, 0.25, -9.0, -9.0, 15.0},   // 1st small
	{ 0.25, 0.5,  0.0, 0.25, -9.0, -9.0, 15.0},   // 2nd small
};

// V4 shape: 3 different lengths of detector tubes
// full length, 3/4 length and 1/2 length inside a round vacuum tube
struct scale {
	double src_start, src_end, intensity;
} v4Scale[] = {
	{ 0.25,  0.75,  0.5  },
	{ 0.125, 0.875, 0.75 },
	{ 0.0,   1.0,   1.0  },
	{ 0.0,   1.0,   1.0  },
	{ 0.125, 0.875, 0.75 },
	{ 0.25,  0.75,  0.5  },
};

/////////////////////////////////////////////////////////////////////////////
// void SimApp::ComputeSpectrum(void)
//
// generate spectrum for simulation
void SimApp::ComputeSpectrum(void)
{
	int iWidth = m_apMCPD8.size() * m_wSpectrumWidth;
	m_abySpectrum.clear();
	m_abySpectrum.resize(iWidth * m_wSpectrumHeight);
	if (iWidth == 1)
	{
		for (int i = 0; i < (int)m_wSpectrumHeight; ++i)
			m_abySpectrum[i] = 1 + (int)20.0 * exp(-10.0 * pow(2.0 * (i + 1) / m_wSpectrumHeight - 1, 2.0));
	}
	else
	{
		int xp,
		    yp;
		QVector<quint8> abyTmp;

		abyTmp.clear();
		abyTmp.resize(iWidth * m_wSpectrumHeight);
		for (yp = 0; yp < iWidth; ++yp)
		{
			for (xp = 0; xp < (int)m_wSpectrumHeight; ++xp)
			{
				int l = yp * m_wSpectrumHeight + xp;
				double x = ((double)xp) / m_wSpectrumHeight;
				double y = ((double)yp) / iWidth;
				double value = 1.0;
				for (unsigned int k = 0; k < sizeof(allPoints) / sizeof(allPoints[0]); ++k)
				{
					struct point *p = &allPoints[k];
					value += p->height * exp(p->y_exp * pow(2.0 * (y - p->y_min) / (p->y_max - p->y_min) - 1.0, 2.0))
							   * exp(p->x_exp * pow(2.0 * (x - p->x_min) / (p->x_max - p->x_min) - 1.0, 2.0));
				}
				abyTmp[l] = (int)value;
			}
		}

		if (m_bV4)
		{
			for (yp = 0; yp < iWidth; ++yp)
			{
				int y = yp * m_wSpectrumHeight;
				struct scale *pScale = &v4Scale[(yp * sizeof(v4Scale) / sizeof(v4Scale[0])) / iWidth];
				for (xp = 0; xp < (int)m_wSpectrumHeight; ++xp)
				{
					int x = m_wSpectrumHeight * (pScale->src_start + xp * (pScale->src_end - pScale->src_start) / m_wSpectrumHeight);
					m_abySpectrum[y + xp] = quint8(abyTmp[y + x] * pScale->intensity);
				}
			}
		}
		else
			m_abySpectrum = abyTmp;
	}
}

/////////////////////////////////////////////////////////////////////////////
// void SimApp::GeneratePoints(void)
//
// generate index array of points
void SimApp::GeneratePoints(void)
{
	if (m_aiPoints.size() < 1)
	{
		std::vector<int> aiPoints;
		for (int i = 0; i < m_abySpectrum.size(); ++i)
			for (int j = m_abySpectrum[i]; j > 0; --j)
				aiPoints.push_back(i);

		if (m_iFastPoint >= 0)
		{
			std::srand(unsigned(std::time(0)));
			// once and only randomisation
			std::random_shuffle(aiPoints.begin(), aiPoints.end());
		}
		m_aiPoints = QVector<int>::fromStdVector(aiPoints); // zero time copy
	}
	if (m_iFastPoint >= m_aiPoints.size())
		m_iFastPoint = 0;
}

/////////////////////////////////////////////////////////////////////////////
// quint64 SimApp::GetClock(void)
//
// read clock with more precision
quint64 SimApp::GetClock(void)
{
	struct timespec tsNow;
	quint64 qwNow;
	clock_gettime(CLOCK_MONOTONIC, &tsNow);
	qwNow = tsNow.tv_sec;
	qwNow *= 10000000ULL;
	qwNow += tsNow.tv_nsec / 100;
	return qwNow;
}

/////////////////////////////////////////////////////////////////////////////
// void logmsg(SimMCPD8 *pMCPD8, const QString& szLine)
//
// print a log message
void logmsg(SimMCPD8 *pMCPD8, const QString &szLine)
{
	QString sPrefix;
	if (pMCPD8 != NULL)
		sPrefix.sprintf("(%d) ", pMCPD8->id());
	qDebug().nospace() << QString("1 %1%2").arg(sPrefix).arg(szLine).toLocal8Bit().constData();
}

/////////////////////////////////////////////////////////////////////////////
// void logmsg(SimMCPD8 *pMCPD8, const char *szFormat, ...)
//
// print a log message
void logmsg(SimMCPD8 *pMCPD8, const char *szFormat, ...)
{
	QString szLine;
	va_list args;
	va_start(args, szFormat);
	szLine.vsprintf(szFormat, args);
	va_end(args);
	logmsg(pMCPD8, szLine);
}

/////////////////////////////////////////////////////////////////////////////
// void SimApp::StartStop(SimMCPD8 *pMCPD8, bool bDAQ, const char *szReason)
//
// handle start/stop commands
void SimApp::StartStop(SimMCPD8 *pMCPD8, bool bDAQ, const char *szReason)
{
	logmsg(pMCPD8, "%s", szReason);
	if (m_bDAQ && !bDAQ)	// first stop
		logmsg(NULL, "send %d packets with %d events", m_dwPackets, m_dwSendEvents);
	m_bDAQ = bDAQ;
	m_dwPackets = 0;
	m_dwSendEvents = 0;
	if (m_bDAQ && !m_iTimer)
		m_iTimer = startTimer(m_wTimerInterval);
	else if (!m_bDAQ && m_iTimer)
	{
		killTimer(m_iTimer);
		m_iTimer = 0;
	}
}

SimApp::SimApp(int &argc, char **argv)
	: QCoreApplication(argc,argv)
	, m_wSpectrumWidth(64)
	, m_wSpectrumStart(0)
	, m_wSpectrumHeight(960)
	, m_dwStopPacket(0)
	, m_wTimerInterval(20)
	, m_wTimerPackets(5)
	, m_bV4(false)
	, m_bDAQ(false)
	, m_wRunId(1)
	, m_qwMasterOffset(GetClock())
	, m_dwPackets(0)
	, m_qwLoopCount(0ULL)
	, m_iFastPoint(-1)
	, m_dwSendEvents(0)
	, m_iTimer(0)
	, m_bMdll(false)
	, m_bMstd(false)
	, m_wMpsdType(TYPE_MPSD8)
{
	bool bWidth(false);
	int i;

	startLogging(g_szShortUsage, g_szLongUsage);
	for (i = 1; i < argc; ++i)
	{
		QString szArg(argv[i]);
		if (szArg.length() < 1)
			continue;
		if (szArg[0] != '-')
			continue;
		while (szArg[0] == '-')
			szArg.remove(0, 1);
		if (szArg.indexOf("fast", Qt::CaseInsensitive) == 0)
			m_iFastPoint = 0;
		else if (szArg.indexOf("stop", Qt::CaseInsensitive) == 0)
		{
			int j = szArg.indexOf('=') + 1;
			if (j < 2)
			{
				qDebug() << "invalid argument: " << szArg;
				break;
			}
			long long l = szArg.mid(j).toLongLong();
			if (l < 1 || l > 0x7FFFFFFFLL)
			{
				qDebug() << "invalid stop counter: " << l;
				break;
			}
			m_dwStopPacket = l;
		}
		else if (szArg.indexOf("interval", Qt::CaseInsensitive) == 0 ||
				 szArg.indexOf("timer", Qt::CaseInsensitive) == 0)
		{
			int j = szArg.indexOf('=') + 1;
			if (j < 2)
			{
				qDebug() << "invalid argument: " << szArg;
				break;
			}
			int l = szArg.mid(j).toInt();
			if (l < 1 || l > 1000)
			{
				qDebug() << "invalid interval: " << l;
				break;
			}
			m_wTimerInterval = l;
		}
		else if (szArg.indexOf("ppe", Qt::CaseInsensitive) == 0)
		{
			int j = szArg.indexOf('=') + 1;
			if (j < 2)
			{
				qDebug() << "invalid argument: " << szArg;
				break;
			}
			int l = szArg.mid(j).toInt();
			if (l < 1)
			{
				qDebug() << "invalid number of packets per timer event: " << l;
				break;
			}
			m_wTimerPackets = l;
		}
		else if (szArg.indexOf("mcpd", Qt::CaseInsensitive) == 0)
		{
			if (m_bMdll)
				usage(argv[0]);
			int j = szArg.indexOf('=') + 1;
			int id = m_apMCPD8.size();
			if (j < 2)
			{
				qDebug() << "invalid argument: " << szArg;
				break;
			}
			szArg.remove(0, j);
			QStringList arglist = szArg.split(":");
			qDebug() << arglist;
			if (arglist.size() > 1)
			{
				id = arglist[1].toInt();
				if (id < 0 || id > 255)
				{
					qDebug() << "invalid id";
					break;
				}
				if (arglist.size() > 2)
				{
					if (arglist[2] == "old")
						m_wMpsdType = TYPE_MPSD8OLD;
					else if (arglist[2] == "new")
						m_wMpsdType = TYPE_MPSD8P;
					else if (arglist[2] == "sadc")
						m_wMpsdType = TYPE_MPSD8SADC;
					qDebug() << arglist[2] << m_wMpsdType;
				}
			}
			SimMCPD8 *pMcpd = new SimMCPD8(id, QHostAddress(arglist[0]));
			if (pMcpd == NULL)
			{
				qDebug() << "cannot create MCPD with " << szArg << ":" << id;
				break;
			}
			m_apMCPD8.append(pMcpd);
			QObject::connect(pMcpd, SIGNAL(CmdPacket(MDP_PACKET*, SimMCPD8*, QHostAddress&, quint16&)), this,
					SLOT(NewCmdPacket(MDP_PACKET*, SimMCPD8*, QHostAddress&, quint16&)), Qt::DirectConnection);
			qDebug() << "created " << pMcpd->ip() << ":" << id;
		}
		else if (szArg.indexOf("mstd", Qt::CaseInsensitive) == 0)
		{
			if (m_bMdll)
				usage(argv[0]);
			int j = szArg.indexOf('=') + 1;
			int id = m_apMCPD8.size();
			if (j < 2)
			{
				qDebug() << "invalid argument: " << szArg;
				break;
			}
			szArg.remove(0, j);
			j = szArg.indexOf(':');
			if (j > 0)
			{
				id = szArg.mid(j + 1).toInt();
				szArg.remove(j, szArg.size() - j);
				if (id < 0 || id > 255)
				{
					qDebug() << "invalid id";
					break;
				}
			}
			SimMCPD8 *pMcpd = new SimMCPD8(id, QHostAddress(szArg));
			if (pMcpd == NULL)
			{
				qDebug() << "cannot create MCPD with " << szArg << ":" << id;
				break;
			}
			m_apMCPD8.append(pMcpd);
			QObject::connect(pMcpd, SIGNAL(CmdPacket(MDP_PACKET*, SimMCPD8*, QHostAddress&, quint16&)), this,
					SLOT(NewCmdPacket(MDP_PACKET*, SimMCPD8*, QHostAddress&, quint16&)), Qt::DirectConnection);
			qDebug() << "created " << pMcpd->ip() << ":" << id;
			m_bMstd = true;
		}
		else if (szArg.indexOf("mdll", Qt::CaseInsensitive) == 0)
		{
			if (m_apMCPD8.size())
				usage(argv[0]);
			m_bMdll = true;
			m_wSpectrumWidth = 960;
			int j = szArg.indexOf('=') + 1;
			int id = 0;
			if (j < 2)
			{
				qDebug() << "invalid argument: " << szArg;
				break;
			}
			szArg.remove(0, j);
			j = szArg.indexOf(':');
			if (j > 0)
			{
				id = szArg.mid(j + 1).toInt();
				szArg.remove(j, szArg.size() - j);
				if (id < 0 || id > 255)
				{
					qDebug() << "invalid id";
					break;
				}
			}
			SimMCPD8 *pMcpd = new SimMCPD8(id, QHostAddress(szArg));
			if (pMcpd == NULL)
			{
				qDebug() << "cannot create MCPD with " << szArg << ":" << id;
				break;
			}
			m_apMCPD8.append(pMcpd);
			QObject::connect(pMcpd, SIGNAL(CmdPacket(MDP_PACKET*, SimMCPD8*, QHostAddress&, quint16&)), this,
					SLOT(NewCmdPacket(MDP_PACKET*, SimMCPD8*, QHostAddress&, quint16&)), Qt::DirectConnection);
			qDebug() << "created " << pMcpd->ip() << ":" << id;
		}
		else if (szArg.indexOf("width", Qt::CaseInsensitive) == 0)
		{
			if (m_bMdll)
				usage(argv[0]);
			int j = szArg.indexOf('=') + 1;
			if (j < 2)
			{
				qDebug() << "invalid argument: " << szArg;
				break;
			}
			QString arg = szArg.mid(j);
			QStringList argList = arg.split('-');
			if (argList.size() > 1)
			{
				int l = argList[0].toInt();
				if (l < 1 || l > 64)
				{
					qDebug() << "invalid start of spectrum: " << l;
					break;
				}
				m_wSpectrumStart = l - 1;
			}
			int l = argList[argList.size() - 1].toInt();
			if (l < 0 || l > 64)
			{
				qDebug() << "invalid width: " << l;
				break;
			}
			bWidth = true;
			m_wSpectrumWidth = l;
		}
		else if (szArg.indexOf("height", Qt::CaseInsensitive) == 0)
		{
			if (m_bMdll)
				usage(argv[0]);
			int j = szArg.indexOf('=') + 1;
			if (j < 2)
			{
				qDebug() << "invalid argument: " << szArg;
				break;
			}
			int l = szArg.mid(j).toInt();
			if (l < 1 || l > 1024)
			{
				qDebug() << "invalid height: " << l;
				break;
			}
			m_wSpectrumHeight = l;
		}
		else if (szArg.indexOf("v4", Qt::CaseInsensitive) == 0)
		{
			if (m_bMdll)
				usage(argv[0]);
			m_bV4 = true;
		}
	}
	if (i < argc)
	{
		usage(argv[0]);
		return;
	}

	if (m_apMCPD8.size() < 1)
	{
		SimMCPD8 *pMcpd = new SimMCPD8(0, QHostAddress::LocalHost);
		if (pMcpd != NULL)
		{
			m_apMCPD8.append(pMcpd);
			QObject::connect(pMcpd, SIGNAL(CmdPacket(MDP_PACKET*, SimMCPD8*, QHostAddress&, quint16&)), this,
					SLOT(NewCmdPacket(MDP_PACKET*, SimMCPD8*, QHostAddress&, quint16&)), Qt::DirectConnection);
			qDebug() << "created " << pMcpd->ip() << ":" << pMcpd->id();
		}
		else
		{
			qDebug() << "cannot create a default MCPD";
			QTimer::singleShot(50, this, SLOT(quit()));
			return;
		}
	}

	if (m_bV4 && !bWidth)
		m_wSpectrumWidth = 56;

	QString szText;
	if (m_bMdll)
		szText.sprintf("created a MDLL (width=%u, height=%u) ", m_wSpectrumWidth, m_wSpectrumHeight);
	else if (m_bMstd)
		szText.sprintf("created %d MCPD-8 each with %d MSTD-16 (%u channels)",
			m_apMCPD8.size(), (m_wSpectrumWidth - m_wSpectrumStart + 7) >> 3, 2 * (m_wSpectrumWidth - m_wSpectrumStart));
	else if (m_wMpsdType == TYPE_MPSD8SADC)
		szText.sprintf("created %d MCPD-8 each with %d MPSD-8 DNS type (width=%u height=%u)",
			m_apMCPD8.size(), (m_wSpectrumWidth - m_wSpectrumStart + 7) >> 3, (m_wSpectrumWidth - m_wSpectrumStart), m_wSpectrumHeight);
	else if (m_wMpsdType == TYPE_MPSD8)
		szText.sprintf("created %d MCPD-8 each with %d MPSD-8 (width=%u height=%u)",
			m_apMCPD8.size(), (m_wSpectrumWidth - m_wSpectrumStart + 7) >> 3, (m_wSpectrumWidth - m_wSpectrumStart), m_wSpectrumHeight);
	else if (m_wMpsdType == TYPE_MPSD8OLD)
		szText.sprintf("created %d MCPD-8 each with %d MPSD-8 SPODI type (width=%u height=%u)",
			m_apMCPD8.size(), (m_wSpectrumWidth - m_wSpectrumStart + 7) >> 3, (m_wSpectrumWidth - m_wSpectrumStart), m_wSpectrumHeight);
	else if (m_wMpsdType == TYPE_MPSD8P)
		szText.sprintf("created %d MCPD-8 each with %d MPSD-8+ (width=%u height=%u)",
			m_apMCPD8.size(), (m_wSpectrumWidth - m_wSpectrumStart + 7) >> 3, (m_wSpectrumWidth - m_wSpectrumStart), m_wSpectrumHeight);
	if (m_bV4)
		szText.append(QString().sprintf("round shape with %d different lengths", (int)((sizeof(v4Scale) / sizeof(v4Scale[0]) + 1) / 2)));
	else if (!m_bMstd)
		szText += "rectangular shape";
	if (m_iFastPoint >= 0)
		szText += ", faster simulation";
	qDebug() << szText;

	ComputeSpectrum();
	qDebug() << "Spectrum computed";
	m_aiPoints.clear();
	GeneratePoints();
	qDebug() << "Ready";
}

void SimApp::usage(const QString &progname)
{
	std::cout << QString(progname).split('/').last().toStdString()
		<< " [-l=<logfile> | --log=<logfile] [-d|--debug|-d=<level>|--debug=<level>]" << std::endl
		<< "         [-nt|--no-timestamp] [-ns|--no-source]" << std::endl;
	if (g_szShortUsage != NULL && g_szShortUsage[0] != '\0')
		std::cout << "         " << g_szShortUsage << std::endl;
	std::cout
		<< "  -h --help    this help text" << std::endl
		<< "  -l --log     write messages to this log file" << std::endl
		<< "  -d=<level> --debug=<level>" << std::endl
		<< "  -d --debug   set logging level: 0=fatal errors only, 1=errors, 2=warnings," << std::endl
		<< "               3=notices (default), 4=info messages, 5=debug messages" << std::endl
		<< "  -nt --no-timestamps" << std::endl
		<< "               do not print timestamps" << std::endl
		<< "  -ns --no-source" << std::endl
		<< "               do not print source file names" << std::endl;
	if (g_szLongUsage != NULL && g_szLongUsage[0] != '\0')
		std::cout << g_szLongUsage << std::endl;
	QTimer::singleShot(50, this, SLOT(quit()));
	return;
}

/////////////////////////////////////////////////////////////////////////////
// void SimApp::timerEvent(QTimerEvent * event)
//
// generate a data packet for every MCPD
void SimApp::timerEvent(QTimerEvent *)
{
	QVector<struct DATA_PACKET> packets;
	quint64 qwHeaderTime = GetClock() - m_qwMasterOffset;
	unsigned int i,
		     j,
		     k;
	quint16 *p;

	if (m_aiPoints.size() < 1 || m_iFastPoint >= m_aiPoints.size())
	{
		GeneratePoints();
		++m_qwLoopCount;
	}

#ifdef PRINTPACKET
	bool bPrintPacket(false);
	if (m_iPrintPacket > 0)
		bPrintPacket = ((--m_iPrintPacket) == 0);
#endif
	packets.resize(m_apMCPD8.size());
	for (i = 0; i < (unsigned int)m_apMCPD8.size(); ++i)
	{
		struct DATA_PACKET *pPacket = &packets[i];
		memset(pPacket, 0, sizeof(*pPacket));

		pPacket->bufferLength = 0;
		if (m_bMdll)
			pPacket->bufferType = 0x0002; // MDLL data buffer
		else
			pPacket->bufferType   = 0x0000; // data event buffer
		pPacket->headerLength = (sizeof(*pPacket) - sizeof(pPacket->data)) / sizeof(quint16); // header length
		pPacket->bufferNumber = m_apMCPD8[i]->NextBufferNo();
		pPacket->runID        = m_wRunId;
		pPacket->deviceStatus = m_bDAQ ? 0x03 : 0x02; // bit 0: DAQ active, bit 1: SYNC ok
		pPacket->deviceId     = m_apMCPD8[i]->id();
		pPacket->time[0]      = qwHeaderTime & 0xFFFF;
		pPacket->time[1]      = (qwHeaderTime >> 16) & 0xFFFF;
		pPacket->time[2]      = (qwHeaderTime >> 32) & 0xFFFF;

		pPacket->param[2][0]  = m_qwLoopCount & 0xFFFF;
		pPacket->param[2][1]  = (m_qwLoopCount >> 16) & 0xFFFF;
		pPacket->param[2][2]  = (m_qwLoopCount >> 32) & 0xFFFF;
#if QT_VERSION >= 0x040700
		quint32 dwTime        = QDateTime::currentDateTimeUtc().toTime_t();
#else
		quint32 dwTime        = QDateTime::currentDateTime().toUTC().toTime_t();
#endif
		pPacket->param[3][0]  = dwTime & 0xFFFF;
		pPacket->param[3][1]  = (dwTime >> 16) & 0xFFFF;
	}

	// add some trigger events
	for (i = 0; i < (unsigned int)packets.size(); ++i)
		for (j = 0; j < 10; ++j)
		{
			// Type(1)  ModID(3)  SlotID(5)  Amplitude(10)  Position(10)  Timestamp(19)
			// | TrigID(3)
			// | |   DataID(4)
			// | |   |              Data(21)
			// | |   |              |                   Timestamp(19)
			// | |   |              |                   |
			// | |  /\   +---------/ \--------++-------/ \---------+
			// |/ \/  \ /                     \/                   \a
			// YiiiSSSS dddddddd dddddddd dddddTTT TTTTTTTT TTTTTTTT
			// 22222222 22222222 11111111 11111111 00000000 00000000
			struct DATA_PACKET *pPacket = &packets[i];
			p = &pPacket->data[3 * pPacket->bufferLength];
			k = qrand() & 0x1F;
			switch (k) // trigger events
			{
				default:
					// additional chances for monitor counters
					if (k != 8 && k != 16 && k != 24 && // 3*MON1
						k != 9 && k != 17 &&        // 2*MON2
						k != 10)                    // 1*MON3
						break;
					k &= 0x07;
					/*no break*/
				case 0: case 1: case 2: case 3: // monitor counter
				case 4: case 5: // rear TTL inputs 1,2
				case 6: case 7: // ADC inputs 1,2
				{
					pPacket->bufferLength++;
					p[2] = 0x8000 + (k << 8);
					k = qrand() & 0x1FFFFF;
					p[2] |= k >> 13;
					p[1] = (k & 0x1FFF) << 3;
					p[0] = 0x0000;
					break;
				}
			}
		}

	// add neutron events
	for (i = 0; i < 480; ++i)
	{
		if (m_aiPoints.size() < 1 || m_iFastPoint >= m_aiPoints.size())
			break;
		if (m_iFastPoint >= 0)
			k = m_aiPoints.at(m_iFastPoint++);
		else
		{
			j = (j + qrand()) % m_aiPoints.size();
			k = m_aiPoints.at(j);
			m_aiPoints.remove(j);
		}
		if (k >= (unsigned)m_abySpectrum.size())
		{
#if 0
			logmsg(NULL, "j=%u k=%u (max. %d)", j, k, m_abyStartSpectrum.size());
			Q_ASSERT(false);
#endif
			continue;
		}
		struct DATA_PACKET *pPacket;
		unsigned int y = k / m_wSpectrumHeight;

		if (m_bMdll)
		{
			pPacket = &packets[0];
			y %= m_wSpectrumWidth;

			p = &pPacket->data[3 * (pPacket->bufferLength++)];
			// Type(1)  ModID(3)  SlotID(5)  Amplitude(10)  Position(10)  Timestamp(19)
			// |    Amplitude(8)
			// |    |         y(10)
			// |    |         |          x(10)
			// |    |         |          |               Timestamp(19)
			// |    |         |          |               |
			// | +-/ \-+  +--/ \--+  +--/ \--+  +-------/ \-------+
			// |/       \/         \/         \/                   \a
			// YAAAAAAA Ayyyyyyy yyyXXXXX XXXXXttt tttttttt tttttttt
			// 22222222 22222222 11111111 11111111 00000000 00000000
			p[2] &= 0x0000;                       // Type
			p[2] |= (0 & 0xff) << 7;              // Amplitude

			p[2] |= (y >> 3) & 0x7F;              // Y-HI
			p[1] |= (y & 7) << 13;                // Y-LO

			p[1] |= (k % m_wSpectrumHeight) << 3; // X

			p[1] |= 0x0000;                       // Timestamp-HI
			p[0] = i;                             // Timestamp-LO
#ifdef PRINTPACKET
			if (bPrintPacket)
			{
				logmsg(NULL, "i=%u j=%u k=%u y=%u x=%u - p[3]=%04x %04x %04x",
					i, j, k, y, k % m_wSpectrumHeight, k / m_wSpectrumHeight, p[0], p[1], p[2]);
			}
#endif
		}
		else
		{
			quint16 mod = y / m_wSpectrumWidth;
			if (mod >= (unsigned int)m_apMCPD8.size())
			{
#if 0
				logmsg(NULL, "j=%u k=%u mod=%u (max. %d)", j, k, mod, m_apMCPD8.size());
				Q_ASSERT(false);
#endif
				continue;
			}
			y %= m_wSpectrumWidth;

			// remove all events with tubes lower than the start tube
			if (y < m_wSpectrumStart)
				continue;
			// Q_ASSERT(mod < 2);
			// Q_ASSERT(y < 56);

			pPacket = &packets[mod];
			p = &pPacket->data[3 * (pPacket->bufferLength++)];

#if 0
			quint16 mod = pd.deviceId;
			quint8 id = (pd.data[counter + 2] >> 12) & 0x7;
			quint8 slotId = (pd.data[counter + 2] >> 7) & 0x1F;
			quint8 modChan = (id << 3) + slotId;
			quint16 chan = modChan + (mod << 6);
			quint16 amp = ((pd.data[counter + 2] & 0x7F) << 3) + ((pd.data[counter + 1] >> 13) & 0x7);
			quint16 pos = (pd.data[counter + 1] >> 3) & 0x3FF;
#endif
			// Type(1)  ModID(3)  SlotID(5)  Amplitude(10)  Position(10)  Timestamp(19)
			// | ModID(3)
			// | |   SlotID(5)
			// | |   |        Amplitude(10)
			// | |   |        |          Position(10)
			// | |   |        |          |               Timestamp(19)
			// | |   /\       |          |               |
			// | |  /  \  +--/ \--+  +--/ \--+  +-------/ \-------+
			// |/ \/    \/         \/         \/                   \a
			// YmmmSSSS Saaaaaaa aaaPPPPP PPPPPttt tttttttt tttttttt
			// 22222222 22222222 11111111 11111111 00000000 00000000
			p[2] |= 0x0000;                       // Type

			p[2] |= ((y >> 3) & 7) << 12;         // ModID
			p[2] |= (y & 7) << 7;                 // SlotID

			p[2] |= 0x0000;                       // Amplitude-HI
			p[1] |= 0x0000;                       // Amplitude-LO

			p[1] |= (k % m_wSpectrumHeight) << 3; // Position

			p[1] |= 0x0000;                       // Timestamp-HI
			p[0] = i;                             // Timestamp-LO

#ifdef PRINTPACKET
			if (bPrintPacket)
			{
				logmsg(NULL, "i=%u j=%u k=%u mod=%u y=%u (%u/%u) - p[3]=%04x %04x %04x",
					i, j, k, mod, y, k % m_wSpectrumHeight, k / m_wSpectrumHeight, p[0], p[1], p[2]);
			}
#endif
#if 0
			//             TimeStamp-LO
			*p++ = htole16(i & 0xFFFF);
			//       Amplitude-LO   Position                         TimeStamp-HI
			*p++ = htole16(0x0000 | ((k % m_wSpectrumHeight) << 3) | (i >> 16));
			//             Type     ModID                    SlotID           Amplitude-HI
			*p++ = htole16(0x0000 | (((y >> 3) & 7) << 12) | ((y & 7) << 4) | 0x0000);
#endif
		}
		if (pPacket->bufferLength >= ((sizeof(pPacket->data) - 6 - 136) / 6))
			break;
	}

	for (i = 0; i < (unsigned int)m_apMCPD8.size(); ++i)
	{
		struct DATA_PACKET *pPacket = &packets[i];
		int iSize(m_aiPoints.size());
		if (m_iFastPoint >= 0)
			iSize -= m_iFastPoint;
		pPacket->param[1][0] = iSize & 0xFFFF;
		pPacket->param[1][1] = (iSize >> 16) & 0xFFFF;
	}

	for (i = 0; i < (unsigned int)packets.size(); ++i)
	{
		struct DATA_PACKET *pPacket = &packets[i];
		pPacket->bufferLength = (sizeof(*pPacket) - sizeof(pPacket->data)) / sizeof(quint16) + (3 * pPacket->bufferLength);
#ifdef PRINTPACKET
		logmsg(m_apMCPD8[i], "Package contains: %d events", (pPacket->bufferLength - pPacket->headerLength) / 3);
		if (bPrintPacket)
		{
			logmsg(m_apMCPD8[i], QString(HexDump(QByteArray((const char*)pPacket, pPacket->bufferLength * sizeof(quint16)))));
			p = (quint16*)(&pPacket->data[0]);
			for (j = 0; j < (unsigned int)((pPacket->bufferLength - pPacket->headerLength) / 3); ++j, p += 3)
			{
				logmsg(m_apMCPD8[i], "%u p[3]=%04x %04x %04x type=%u mod=%u slot=%u amp=%u pos=%u time=%u/%u",
					j, p[0], p[1], p[2], p[2] >> 15, (p[2] >> 12) & 0x07, (p[2] >> 4) & 0x1F,
					((p[2] & 0x7F) << 3) + (p[1] >> 13), (p[1] >> 3) & 0x3FF, p[1] & 0x07, p[0]);
			}
		}
#endif
	}
	for (quint16 k = 0; k < m_wTimerPackets; ++k)
	{
		for (i = 0; i < (unsigned int)packets.size(); ++i)
		{
			m_apMCPD8[i]->Send(&packets[i]);
			m_dwSendEvents += (packets[i].bufferLength - packets[i].headerLength) / 3;
			++m_dwPackets;
			if (m_dwStopPacket > 0 && m_dwPackets >= m_dwStopPacket)
			{
				StartStop(NULL, false, "STOP due packet counter");
				return;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// void SimApp::NewCmdPacket(struct MDP_PACKET *pPacket, SimMCPD8 *pMCPD8,
//                           QHostAddress &sender, quint16 &senderPort)
//
// MCPD8 command packet handler
void SimApp::NewCmdPacket(struct MDP_PACKET *pPacket, SimMCPD8 *pMCPD8, QHostAddress &sender, quint16 &senderPort)
{
	quint16 wBufferLen = pPacket->bufferLength;
//	logmsg(pMCPD8, QString(HexDump((const char*)pPacket, 2 * pPacket->bufferLength)));

	pPacket->bufferType |= 0x8000; // CMDBUFTYPE
	pPacket->bufferLength = CMDHEADLEN;
	quint64 qwHeaderTime = GetClock() - m_qwMasterOffset;
	pPacket->time[0] = qwHeaderTime & 0xFFFF;
	pPacket->time[1] = (qwHeaderTime >> 16) & 0xFFFF;
	pPacket->time[2] = (qwHeaderTime >> 32) & 0xFFFF;
	switch (pPacket->cmd)
	{
		case START:
		{
			// m_qwMasterOffset = GetClock();
			pMCPD8->SetTarget(sender, senderPort);
			StartStop(pMCPD8, true, "START via network");
			break;
		}
		case CONTINUE:
			StartStop(pMCPD8, true, "CONTINUE via network");
			break;
		case RESET:
			m_qwMasterOffset = GetClock();
			StartStop(pMCPD8, false, "RESET via network");
			break;
		case STOP:
			StartStop(pMCPD8, false, "STOP via network");
			break;
		case SETID:
			logmsg(pMCPD8, "SETID %d", pPacket->data[0] & 7);
			pMCPD8->id(pPacket->data[0] & 7);
			break;
		case SETPROTOCOL:
			logmsg(pMCPD8, "SETPROTOCOL ownip=%d.%d.%d.%d datareceiver=%d.%d.%d.%d cmdport=%u dataport=%u cmdreceiver=%d.%d.%d.%d",
			pPacket->data[0], pPacket->data[1], pPacket->data[2], pPacket->data[3],
			pPacket->data[4], pPacket->data[5], pPacket->data[6], pPacket->data[7],
			pPacket->data[8], pPacket->data[9],
			pPacket->data[10], pPacket->data[11], pPacket->data[12], pPacket->data[13]);
			break;
		case SETTIMING: // master, termination, external synchronisation
			logmsg(pMCPD8, "SETTIMING");
			break;
		case SETRUNID:
			m_wRunId = pPacket->data[0];
			logmsg(pMCPD8, "SETRUNID %u", m_wRunId);
			break;
		case SETCLOCK: // set master clock
		{
			quint64 qwNow = GetClock();
			quint64 qwMaster = (((quint64)pPacket->data[2]) << 32) + (((quint64)pPacket->data[1]) << 16) + pPacket->data[0];
			m_qwMasterOffset = qwNow - qwMaster;
			logmsg(pMCPD8, "SETCLOCK %Lu (now=%Lu offset=%Lu)", qwMaster, qwNow, m_qwMasterOffset);
			break;
		}
//		case SETDAC:
//		case SENDSERIAL:
//		case READSERIAL:
//		case SCANPERI:
		case SETCELL:
			logmsg(pMCPD8, "SETCELL source=%d trigger=%d compare=%d", pPacket->data[0], pPacket->data[1], pPacket->data[2]);
			// pPacket->data[0] = source;
			// pPacket->data[1] = trigger;
			// pPacket->data[2] = compare;
			break;
		case SETAUXTIMER:
			logmsg(pMCPD8, "SETAUXTIMER timer=%d value=%d", pPacket->data[0], pPacket->data[1]);
			pPacket->bufferLength = CMDHEADLEN + 2;
			// pPacket->data[0] = tim;
			// pPacket->data[1] = val;
			break;
		case SETPARAM:
			logmsg(pMCPD8, "SETPARAM param=%d source=%d", pPacket->data[0], pPacket->data[1]);
			// pPacket->data[0] = param;
			// pPacket->data[1] = source;
			break;
//		case GETPARAM:
		case SETGAIN_MPSD:
			logmsg(pMCPD8, "SETGAIN_MPSD addr=%d chan=%d gain=%d", pPacket->data[0], pPacket->data[1], pPacket->data[2]);
			pPacket->bufferLength = wBufferLen;
			// pPacket->data[0] = addr;
			// pPacket->data[1] = chan;
			// pPacket->data[2] = gainval;
			break;
		case SETGAIN_MSTD:
			logmsg(pMCPD8, "SETGAIN_MSTD addr=%d chan=%d gain=%d", pPacket->data[0], pPacket->data[1], pPacket->data[2]);
			pPacket->bufferLength = wBufferLen;
			// pPacket->data[0] = addr;
			// pPacket->data[1] = chan;
			// pPacket->data[2] = gainval;
			break;
		case SETTHRESH:
			logmsg(pMCPD8, "SETTHRESH addr=%d threshold=%d", pPacket->data[0], pPacket->data[1]);
			pPacket->bufferLength = CMDHEADLEN + 2;
			// pPacket->data[0] = addr;
			// pPacket->data[1] = thresh;
			break;
		case SETPULSER: // enable pulser
			logmsg(pMCPD8, "SETPULSER addr=%d chan=%d pos=%d amp=%d onoff=%d", pPacket->data[0], pPacket->data[1], pPacket->data[2], pPacket->data[3], pPacket->data[4]);
			pPacket->bufferLength = CMDHEADLEN + 5;
			// TODO handle SETPULSER
			// pPacket->data[0] = addr;
			// pPacket->data[1] = chan;
			// pPacket->data[2] = pos;
			// pPacket->data[3] = amp;
			// pPacket->data[4] = onoff;
			break;
		case SETMODE: // ==0 position mode, !=0 amplifier mode
			logmsg(pMCPD8, "SETMODE addr=%d mode=%d", pPacket->data[0], pPacket->data[1]);
			// TODO handle SETMODE
			switch (pPacket->data[0])
			{
				case 0: // MPSD-8 #0
				case 1: // MPSD-8 #1
				case 2: // MPSD-8 #2
				case 3: // MPSD-8 #3
				case 4: // MPSD-8 #4
				case 5: // MPSD-8 #5
				case 6: // MPSD-8 #6
				case 7: // MPSD-8 #7
				default: // all
					break;
			}
			break;
		case GETCAPABILITIES: // MPSD-8 capabilities
			// TODO handle GETCAPABILITIES: which types are supported
			pPacket->bufferLength = CMDHEADLEN + 8;
			pPacket->data[0] = 1; // MPSD-8 capabilities (bit0=P, bit1=TP, bit2=TPA)
			pPacket->data[1] = (m_wSpectrumWidth >  8) ? 1 : 0;
			pPacket->data[2] = (m_wSpectrumWidth > 16) ? 1 : 0;
			pPacket->data[3] = (m_wSpectrumWidth > 24) ? 1 : 0;
			pPacket->data[4] = (m_wSpectrumWidth > 32) ? 1 : 0;
			pPacket->data[5] = (m_wSpectrumWidth > 40) ? 1 : 0;
			pPacket->data[6] = (m_wSpectrumWidth > 48) ? 1 : 0;
			pPacket->data[7] = (m_wSpectrumWidth > 56) ? 1 : 0;
			logmsg(pMCPD8, "GETCAPABILITIES");
			break;
//		case SETCAPABILITIES:
//		case WRITEFPGA:
//		case READFPGA:
		case WRITEREGISTER:
			logmsg(pMCPD8, "WRITEREGISTER addr=%d reg=%d val=%d", pPacket->data[0], pPacket->data[1], pPacket->data[2]);
			// pPacket->data[0] = addr;
			// pPacket->data[1] = reg;
			// pPacket->data[2] = val;
			break;
		case READREGISTER:
			pPacket->bufferLength = CMDHEADLEN + 1;
			pPacket->data[1] = m_wMpsdType;
			switch (m_wMpsdType)	// MCPD-8 capabilities (bit0=P, bit1=TP, bit2=TPA)
			{
				case TYPE_MPSD8SADC:
					pPacket->data[0] = 1;
					break;
				case TYPE_MPSD8OLD:
					pPacket->data[0] = 0;
					break;
				default:
					pPacket->data[0] = 2;
					break;
			}
			logmsg(pMCPD8, "READREGISTER reg=%d val=%d", pPacket->data[0], pPacket->data[1]);
			break;
//		case SETPOTI:
//		case GETPOTI:
		case READID: // read connected devices
			pPacket->bufferLength = CMDHEADLEN + 8;
			// TODO READID: which types are supported
			if (m_bMdll)
				pPacket->data[0] = TYPE_MDLL;
			else
			{
				pPacket->data[0] = (m_wSpectrumStart <= 0 && m_wSpectrumWidth >  0) ? (m_bMstd ? TYPE_MSTD16 : m_wMpsdType) : TYPE_NOMODULE;
				pPacket->data[1] = (m_wSpectrumStart <= 8 && m_wSpectrumWidth >  8) ? (m_bMstd ? TYPE_MSTD16 : m_wMpsdType) : TYPE_NOMODULE;
				pPacket->data[2] = (m_wSpectrumStart <= 16 && m_wSpectrumWidth > 16) ? (m_bMstd ? TYPE_MSTD16 : m_wMpsdType) : TYPE_NOMODULE;
				pPacket->data[3] = (m_wSpectrumStart <= 24 && m_wSpectrumWidth > 24) ? (m_bMstd ? TYPE_MSTD16 : m_wMpsdType) : TYPE_NOMODULE;
				pPacket->data[4] = (m_wSpectrumStart <= 32 && m_wSpectrumWidth > 32) ? (m_bMstd ? TYPE_MSTD16 : m_wMpsdType) : TYPE_NOMODULE;
				pPacket->data[5] = (m_wSpectrumStart <= 40 && m_wSpectrumWidth > 40) ? (m_bMstd ? TYPE_MSTD16 : m_wMpsdType) : TYPE_NOMODULE;
				pPacket->data[6] = (m_wSpectrumStart <= 48 && m_wSpectrumWidth > 48) ? (m_bMstd ? TYPE_MSTD16 : m_wMpsdType) : TYPE_NOMODULE;
				pPacket->data[7] = (m_wSpectrumStart <= 56 && m_wSpectrumWidth > 56) ? (m_bMstd ? TYPE_MSTD16 : m_wMpsdType) : TYPE_NOMODULE;
			}
			logmsg(pMCPD8, "READID : %u", pPacket->data[0]);
			break;
//		case DATAREQUEST:
//		case QUIET:
		case GETVER: // version 9.2
			pPacket->bufferLength = CMDHEADLEN + 2;
			pPacket->data[0] = 9;
			pPacket->data[1] = 2;
			logmsg(pMCPD8, "GETVER h=0x%04x l=0x%04x", pPacket->data[0], pPacket->data[1]);
			break;
		case READPERIREG: // read MPSD-8 module
			pPacket->bufferLength = CMDHEADLEN + 3;
			switch (pPacket->data[1])
			{
				case 0: // MPSD-8 capabilities
					pPacket->data[2] = pPacket->data[0] < (m_wSpectrumWidth >> 3) ? 1 : 0; // only P-mode supported
					break;
				case 1: // MPSD-8 mode (1=P, 2=TP, 4=TPA)
					pPacket->data[2] = pPacket->data[0] < (m_wSpectrumWidth >> 3) ? 1 : 0;
					break;
				case 2: // MPSD-8 version (5.06)
					pPacket->data[2] = pPacket->data[0] < (m_wSpectrumWidth >> 3) ? 0x0505 : 0x0000;
					break;
				default: // unknown register
					pPacket->data[2] = 0x0000;
					break;
			}
			logmsg(pMCPD8, "READPERIREG addr=%d reg=%d val=%d", pPacket->data[0], pPacket->data[1], pPacket->data[2]);
			break;
		case WRITEPERIREG:
			logmsg(pMCPD8, "WRITEPERIREG addr=%d reg=%d val=%d", pPacket->data[0], pPacket->data[1], pPacket->data[2]);
			break;
		case SETMDLLTHRESHS:
		case SETMDLLSPECTRUM:
//		case SETMDLLMODE:
//		case SETMDLLHIST:
//		case SETMDLLSLSC:
		case SETMDLLPULSER:
		case SETMDLLDATASET:
		case SETMDLLACQSET:
		case SETMDLLEWINDOW:
			logmsg(pMCPD8, "MDLL command 0x%04x", pPacket->cmd);
			pPacket->cmd |= 0x8000; // command not supported
			// pPacket->data[0] = module
			// pPacket->data[1] = register
			// pPacket->data[2] = value
			break;
		default:
			logmsg(pMCPD8, "unknown command 0x%04x", pPacket->cmd);
			pPacket->cmd |= 0x8000; // command not supported
			break;
	}
	pMCPD8->Send(pPacket, sender, senderPort);
}
