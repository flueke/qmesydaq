/***************************************************************************
 *   Copyright (C) 2013 by Lutz Rossa <rossa@helmholtz-berlin.de>          *
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
const char *g_szShortUsage = "[--mcpd=127.0.0.2:0] [--mcpd=127.0.0.3:1] ... [--width=64] [--height=960] [--interval=20] [--v4]";
const char *g_szLongUsage =
		"  --mcpd=<bind-ip>:<id>\n"
		"               bind a MCPD (max 64) with ID to this IP address (id 0..255)\n"
		"  --width=<n>  each MCPD with <n> channels (1..64, default 64)\n"
		"  --height=<n> spectrum height with <n> bins (8..1024, default 960)\n"
		"  --interval=<n>\n"
		"               packet generator interval in ms (1..1000, default 20)\n"
		"  --v4         generate a \"round\" detector like HZB-V4/SANS\n";

#include "main.h"
#include "simmcpd8.h"
#include <QDateTime>
#include <cmath>
#include <ctime>
#include "logging.h"

// print special packet with this number
//define PRINTPACKET 1

/////////////////////////////////////////////////////////////////////////////
// class declaraction

/////////////////////////////////////////////////////////////////////////////
// global variables
static quint16            g_wSpectrumWidth  = 64;    // default: MCPD has 64 channels (8 full MPSDs)
static quint16            g_wSpectrumHeight = 960;   // spectrum height
static quint16            g_dwStopPacket    = 0;     // stop after this number of packets
static quint16            g_wTimerInterval  = 20;    // simulation timer
static bool               g_bV4             = false; // simulate "round" detector of HZB instrument V4/SANS

static QVector<SimMCPD8*> g_apMCPD8;
static bool               g_bDAQ            = false; // simulator is running
static quint16            g_wRunId          = 1;     // header run#
static quint64            g_qwMasterOffset  = 0ULL;  // start time
static quint32            g_dwPackets       = 0;     // packet counter
static quint64            g_qwLoopCount     = 0ULL;  // loop counter for full simulation cycles
static QVector<quint8>    g_abySpectrum;             // output spectrum
static QVector<int>       g_aiPoints;                // point to simulate

#if defined(PRINTPACKET) && PRINTPACKET>0
static int             g_iPrintPacket    = PRINTPACKET; // print special packet
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
// static void ComputeSpectrum(void)
//
// generate spectrum for simulation
static void ComputeSpectrum(void)
{
	int iWidth = g_apMCPD8.size() * g_wSpectrumWidth;
	g_abySpectrum.clear();
	g_abySpectrum.resize(iWidth * g_wSpectrumHeight);
	if (iWidth < 2)
	{
		for (int i = 0; i < (int)g_wSpectrumHeight; ++i)
			g_abySpectrum[i] = 1 + (int)20.0 * exp(-10.0 * pow(2.0 * (i + 1) / g_wSpectrumHeight - 1, 2.0));
	}
	else
	{
		int xp,
		    yp;
		QVector<quint8> abyTmp;

		abyTmp.clear();
		abyTmp.resize(iWidth * g_wSpectrumHeight);
		for (yp = 0; yp < iWidth; ++yp)
		{
			for (xp = 0; xp < (int)g_wSpectrumHeight; ++xp)
			{
				int l = yp * g_wSpectrumHeight + xp;
				double x = ((double)xp) / g_wSpectrumHeight;
				double y = ((double)yp) / iWidth;
				double value = 1.0;
				for (unsigned int k = 0; k < sizeof(allPoints)/sizeof(allPoints[0]); ++k) {
					struct point *p = &allPoints[k];
					value += p->height * exp (p->y_exp * pow (2.0 * (y - p->y_min) / (p->y_max - p->y_min) - 1.0,
						2.0)) * exp (p->x_exp * pow (2.0 * (x - p->x_min) / (p->x_max - p->x_min) - 1.0, 2.0));
				}
				abyTmp[l] = (int)value;
			}
		}

		if (g_bV4)
		{
			for (yp = 0; yp < iWidth; ++yp)
			{
				int y = yp * g_wSpectrumHeight;
				struct scale *pScale = &v4Scale[(yp * sizeof(v4Scale) / sizeof(v4Scale[0])) / iWidth];
				for (xp = 0; xp < (int)g_wSpectrumHeight; ++xp)
				{
					int x = g_wSpectrumHeight * (pScale->src_start + xp * (pScale->src_end - pScale->src_start) / g_wSpectrumHeight);
					g_abySpectrum[y + xp] = quint8(abyTmp[y + x] * pScale->intensity);
				}
			}
		}
		else
			g_abySpectrum = abyTmp;
	}
}

/////////////////////////////////////////////////////////////////////////////
// static quint64 GetClock(void)
//
// read clock with more precision
static quint64 GetClock(void)
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
// static void StartStop(SimMCPD8 *pMCPD8, bool bDAQ, const char *szReason)
//
// handle start/stop commands
static void StartStop(SimMCPD8 *pMCPD8, bool bDAQ, const char *szReason)
{
	logmsg(pMCPD8, "%s", szReason);
	g_bDAQ = bDAQ;
	g_dwPackets = 0;
}

/////////////////////////////////////////////////////////////////////////////
// void SimApp::timerEvent(QTimerEvent * event)
//
// generate a data packet for every MCPD
void SimApp::timerEvent(QTimerEvent *)
{
	QVector<struct DATA_PACKET> packets;
	if (g_bDAQ)
	{
		quint64 qwHeaderTime = GetClock()-g_qwMasterOffset;
		unsigned int i,
			     j,
			     k;
		quint16 *p;

		if (g_aiPoints.size() < 1)
		{
			g_aiPoints.clear();
			for (i = 0; i < (unsigned)g_abySpectrum.size(); ++i)
				for (j = g_abySpectrum[i]; j > 0; --j)
					g_aiPoints.append(i);
			++g_qwLoopCount;
		}

#ifdef PRINTPACKET
		bool bPrintPacket(false);
		if (g_iPrintPacket > 0)
			bPrintPacket = ((--g_iPrintPacket) == 0);
#endif
		packets.resize(g_apMCPD8.size());
		for (i = 0; i < (unsigned int)g_apMCPD8.size(); ++i)
		{
			struct DATA_PACKET *pPacket = (struct DATA_PACKET*)(&packets[i]);
			memset(pPacket, 0, sizeof(*pPacket));

			pPacket->bufferLength = 0;
			//pPacket->bufferType = 0x0002; // MDLL data buffer
			pPacket->bufferType   = 0x0000; // data event buffer
			pPacket->headerLength = (sizeof(*pPacket) - sizeof(pPacket->data)) / sizeof(quint16); // header length
			pPacket->bufferNumber = g_apMCPD8[i]->NextBufferNo();
			pPacket->runID        = g_wRunId;
			pPacket->deviceStatus = g_bDAQ ? 0x03 : 0x02; // bit 0: DAQ active, bit 1: SYNC ok
			pPacket->deviceId     = g_apMCPD8[i]->id();
			pPacket->time[0]      = qwHeaderTime & 0xFFFF;
			pPacket->time[1]      = (qwHeaderTime >> 16) & 0xFFFF;
			pPacket->time[2]      = (qwHeaderTime >> 32) & 0xFFFF;

			pPacket->param[2][0]  = g_qwLoopCount & 0xFFFF;
			pPacket->param[2][1]  = (g_qwLoopCount >> 16) & 0xFFFF;
			pPacket->param[2][2]  = (g_qwLoopCount >> 32) & 0xFFFF;
			quint32 dwTime        = QDateTime::currentDateTimeUtc().toTime_t();
			pPacket->param[3][0]  = dwTime & 0xFFFF;
			pPacket->param[3][1]  = (dwTime >> 16) & 0xFFFF;
		}

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

		for (i = 0; i < 480; ++i)
		{
			if (g_aiPoints.size() < 1)
				break;
			j = (j + qrand()) % g_aiPoints.size();
			k = g_aiPoints.at(j);
			g_aiPoints.remove(j);
			if (k >= (unsigned)g_abySpectrum.size())
			{
#if 0
				logmsg(NULL, "j=%u k=%u (max. %d)", j, k, m_abyStartSpectrum.size());
				Q_ASSERT(false);
#endif
				continue;
			}
			unsigned int y = k / g_wSpectrumHeight;
			quint16 mod = y / g_wSpectrumWidth;
			if (mod >= (unsigned int)g_apMCPD8.size())
			{
#if 0
				logmsg(NULL, "j=%u k=%u mod=%u (max. %d)", j, k, mod, g_apMCPD8.size());
				Q_ASSERT(false);
#endif
				continue;
			}
			y %= g_wSpectrumWidth;
			// Q_ASSERT(mod < 2);
			// Q_ASSERT(y < 56);

			struct DATA_PACKET *pPacket = &packets[mod];
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
			p[1] |= (k % g_wSpectrumHeight) << 3; // Position
			p[1] |= 0x0000;                       // Timestamp-HI

			p[0] = i;                             // Timestamp-LO

#ifdef PRINTPACKET
			if (bPrintPacket)
			{
				logmsg(NULL, "i=%u j=%u k=%u mod=%u y=%u (%u/%u) - p[3]=%04x %04x %04x",
					i, j, k, mod, y, k % g_wSpectrumHeight, k / g_wSpectrumHeight, p[0], p[1], p[2]);
			}
#endif
#if 0
			//             TimeStamp-LO
			*p++ = htole16(i&0xFFFF);
			//       Amplitude-LO   Position                         TimeStamp-HI
			*p++ = htole16(0x0000 | ((k % g_wSpectrumHeight) << 3) | (i >> 16));
			//             Type     ModID                    SlotID           Amplitude-HI
			*p++ = htole16(0x0000 | (((y >> 3) & 7) << 12) | ((y & 7) << 4) | 0x0000);
#endif

			if (pPacket->bufferLength >= ((sizeof(pPacket->data) - 6 - 136) / 6))
				break;
		}

		for (i = 0; i < (unsigned int)g_apMCPD8.size(); ++i)
		{
			struct DATA_PACKET *pPacket = (struct DATA_PACKET*)(&packets[i]);
			int iSize(g_aiPoints.size());
			pPacket->param[1][0] = iSize & 0xFFFF;
			pPacket->param[1][1] = (iSize >> 16) & 0xFFFF;
		}

		for (i = 0; i < (unsigned int)packets.size(); ++i)
		{
			struct DATA_PACKET *pPacket = (struct DATA_PACKET*)(&packets[i]);
			pPacket->bufferLength = (sizeof(*pPacket) - sizeof(pPacket->data)) / sizeof(quint16) + (3 * pPacket->bufferLength);
			g_apMCPD8[i]->Send(pPacket);
		}
#ifdef PRINTPACKET
		if (bPrintPacket)
		{
			for (i = 0; i < (unsigned int)packets.size(); ++i)
			{
				struct DATA_PACKET *pPacket = (struct DATA_PACKET*)(&packets[i]);
				logmsg(g_apMCPD8[i], QString(HexDump(QByteArray((const char*)pPacket, pPacket->bufferLength * sizeof(quint16)))));
				p = (quint16*)(&pPacket->data[0]);
				for (j = 0; j < (unsigned int)((pPacket->bufferLength - pPacket->headerLength) / 3); ++j, p += 3)
				{
					logmsg(g_apMCPD8[i], "%u p[3]=%04x %04x %04x type=%u mod=%u slot=%u amp=%u pos=%u time=%u/%u",
						j, p[0], p[1], p[2], p[2] >> 15, (p[2] >> 12) & 0x07, (p[2] >> 4) & 0x1F,
						((p[2] & 0x7F) << 3) + (p[1] >> 13), (p[1] >> 3) & 0x3FF, p[1] & 0x07, p[0]);
				}
			}
		}
#endif

		if (g_dwStopPacket > 0)
			if ((++g_dwPackets) >= g_dwStopPacket)
				StartStop(NULL, false, "STOP due packet counter");
	}
}

void SimApp::NewCmdPacket(struct MDP_PACKET *pPacket, SimMCPD8 *pMCPD8, QHostAddress &sender, quint16 &senderPort)
{
	quint16 wBufferLen = pPacket->bufferLength;
//	logmsg(pMCPD8, QString(HexDump((const char*)pPacket, 2 * pPacket->bufferLength)));

	pPacket->bufferType |= 0x8000; // CMDBUFTYPE
	pPacket->bufferLength = CMDHEADLEN;
	switch (pPacket->cmd)
	{
		case START:
		{
			g_qwMasterOffset = GetClock();
			pMCPD8->SetTarget(sender, senderPort);
			StartStop(pMCPD8, true, "START via network");
			break;
		}
		case CONTINUE:
			StartStop(pMCPD8, true, "CONTINUE via network");
			break;
		case RESET:
			g_qwMasterOffset = GetClock();
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
			g_wRunId = pPacket->data[0];
			logmsg(pMCPD8, "SETRUNID %u", g_wRunId);
			break;
		case SETCLOCK: // set master clock
		{
			quint64 qwNow = GetClock();
			quint64 qwMaster = (((quint64)pPacket->data[2]) << 32) + (((quint64)pPacket->data[1]) << 16) + pPacket->data[0];
			g_qwMasterOffset = qwNow - qwMaster;
			logmsg(pMCPD8, "SETCLOCK %Lu (now=%Lu offset=%Lu)", qwMaster, qwNow, g_qwMasterOffset);
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
		case SETGAIN:
			logmsg(pMCPD8, "SETGAIN addr=%d chan=%d gain=%d", pPacket->data[0], pPacket->data[1], pPacket->data[2]);
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
			pPacket->data[1] = (g_wSpectrumWidth >  8) ? 1 : 0;
			pPacket->data[2] = (g_wSpectrumWidth > 16) ? 1 : 0;
			pPacket->data[3] = (g_wSpectrumWidth > 24) ? 1 : 0;
			pPacket->data[4] = (g_wSpectrumWidth > 32) ? 1 : 0;
			pPacket->data[5] = (g_wSpectrumWidth > 40) ? 1 : 0;
			pPacket->data[6] = (g_wSpectrumWidth > 48) ? 1 : 0;
			pPacket->data[7] = (g_wSpectrumWidth > 56) ? 1 : 0;
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
			if (pPacket->data[1] == 102)
				pPacket->data[0] = 1; // MCPD-8 capabilities (bit0=P, bit1=TP, bit2=TPA)
			else
				pPacket->data[0] = 0;
			logmsg(pMCPD8, "READREGISTER reg=%d val=%d", pPacket->data[0], pPacket->data[1]);
			break;
//		case SETPOTI:
//		case GETPOTI:
		case READID: // read connected devices
			pPacket->bufferLength = CMDHEADLEN + 8;
			// TODO READID: which types are supported
			pPacket->data[0] = TYPE_MPSD8;
			pPacket->data[1] = (g_wSpectrumWidth >  8) ? TYPE_MPSD8 : TYPE_NOMODULE;
			pPacket->data[2] = (g_wSpectrumWidth > 16) ? TYPE_MPSD8 : TYPE_NOMODULE;
			pPacket->data[3] = (g_wSpectrumWidth > 24) ? TYPE_MPSD8 : TYPE_NOMODULE;
			pPacket->data[4] = (g_wSpectrumWidth > 32) ? TYPE_MPSD8 : TYPE_NOMODULE;
			pPacket->data[5] = (g_wSpectrumWidth > 40) ? TYPE_MPSD8 : TYPE_NOMODULE;
			pPacket->data[6] = (g_wSpectrumWidth > 48) ? TYPE_MPSD8 : TYPE_NOMODULE;
			pPacket->data[7] = (g_wSpectrumWidth > 56) ? TYPE_MPSD8 : TYPE_NOMODULE;
			logmsg(pMCPD8, "READID");
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
					pPacket->data[2] = pPacket->data[0] < (g_wSpectrumWidth >> 3) ? 1 : 0; // only P-mode supported
					break;
				case 1: // MPSD-8 mode (1=P, 2=TP, 4=TPA)
					pPacket->data[2] = pPacket->data[0] < (g_wSpectrumWidth >> 3) ? 1 : 0;
					break;
				case 2: // MPSD-8 version (5.06)
					pPacket->data[2] = pPacket->data[0] < (g_wSpectrumWidth >> 3) ? 0x0505 : 0x0000;
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

/////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char *argv[])
{
	SimApp app(argc, argv);
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
		if (szArg.indexOf("mcpd", Qt::CaseInsensitive) == 0)
		{
			int j = szArg.indexOf('=') + 1;
			int id = g_apMCPD8.size();
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
			g_apMCPD8.append(pMcpd);
			QObject::connect(pMcpd, SIGNAL(CmdPacket(MDP_PACKET*, SimMCPD8*, QHostAddress&, quint16&)), &app,
					SLOT(NewCmdPacket(MDP_PACKET*, SimMCPD8*, QHostAddress&, quint16&)), Qt::DirectConnection);
			qDebug() << "created " << pMcpd->ip() << ":" << id;
		}
		else if (szArg.indexOf("width", Qt::CaseInsensitive) == 0)
		{
			int j = szArg.indexOf('=') + 1;
			if (j < 2)
			{
				qDebug() << "invalid argument: " << szArg;
				break;
			}
			int l = szArg.mid(j).toInt();
			if (l < 1 || l > 64)
			{
				qDebug() << "invalid width: " << l;
				break;
			}
			bWidth = true;
			g_wSpectrumWidth = l;
		}
		else if (szArg.indexOf("height", Qt::CaseInsensitive) == 0)
		{
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
			g_wSpectrumHeight = l;
		}
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
			g_dwStopPacket = l;
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
			g_wTimerInterval = l;
		}
		else if (szArg.indexOf("v4", Qt::CaseInsensitive) == 0)
			g_bV4 = true;
	}
	if (i < argc)
	{
		int i = strlen(argv[0]) + 1;
		fprintf(stderr,
				"%s [--mcpd=127.0.0.2:0] [--mcpd=127.0.0.3:1] ...\n"
				"%*c[--width=64] [--height=960] [--interval=20] [--v4]\n"
				"%*c--mcpd=<bind-ip>:<id>   bind a MCPD (max 64) with ID to this IP address (id 0..255)\n"
				"%*c--width=<n>             each MCPD with <n> channels (1..64, default 64)\n"
				"%*c--height=<n>            spectrum height with <n> bins (8..1024, default 960)\n"
				"%*c--interval=<n>          packet generator interval in ms (1..1000, default 20)\n"
				"%*c--v4                    generate a \"round\" detector like HZB-V4/SANS\n",
				argv[0], i, ' ', i, ' ', i, ' ', i, ' ', i, ' ', i, ' ');
		return 1;
	}

	if (g_apMCPD8.size() < 1)
	{
		SimMCPD8 *pMcpd = new SimMCPD8(0, QHostAddress::LocalHost);
		if (pMcpd != NULL)
		{
			g_apMCPD8.append(pMcpd);
			QObject::connect(pMcpd, SIGNAL(CmdPacket(MDP_PACKET*, SimMCPD8*, QHostAddress&, quint16&)), &app,
					SLOT(NewCmdPacket(MDP_PACKET*, SimMCPD8*, QHostAddress&, quint16&)), Qt::DirectConnection);
			qDebug() << "created " << pMcpd->ip() << ":" << pMcpd->id();
		}
		else
		{
			qDebug() << "cannot create a default MCPD";
			return 1;
		}
	}

	if (g_bV4 && !bWidth)
		g_wSpectrumWidth = 56;

	QString szText;
	szText.sprintf("created %d MCPD-8 each with %d MPSD-8 (width=%u height=%u) and ",
		g_apMCPD8.size(), (g_wSpectrumWidth + 7) >> 3, g_wSpectrumWidth, g_wSpectrumHeight);
	if (g_bV4)
		szText.append(QString().sprintf("round shape with %d different lengths", (int)((sizeof(v4Scale) / sizeof(v4Scale[0]) + 1) / 2)));
	else
		szText += "rectangular shape";
	qDebug() << szText;

	ComputeSpectrum();
	g_aiPoints.clear();
	app.startTimer(g_wTimerInterval);

	return app.exec();
}
