/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2014 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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

#include <QString>
#include <QDataStream>
#include <QTextStream>
#include <QFile>
#include <QDebug>
#include <QCoreApplication>

#include "structures.h"
#include "mdefines.h"
#include "logging.h"

#include "histogram.h"

#include <algorithm>

static quint64 	g_triggers(0),
		g_neutrons(0);

Histogram	*m_posHist;
Histogram	*m_ampHist;
Spectrum	*m_diffractogram;
Spectrum	*m_tubeSpectrum;

void analyzeBuffer(const DATA_PACKET &pd)
{
	quint64 tim;
	quint16 mod = pd.deviceId;
	quint64 headertime = pd.time[0] + (quint64(pd.time[1]) << 16) + (quint64(pd.time[2]) << 32);
//	setCurrentTime(headertime / 10000); // headertime is in 100ns steps
	quint16 runID(pd.runID);

	MSG_DEBUG << QObject::tr("# : %3, mod : %1, runID : %2").arg(mod).arg(runID).arg(pd.bufferNumber);

	if(pd.bufferType < 0x0003)
	{
		quint16 neutrons(0);
		quint16 triggers(0);
		quint16	monitorTriggers(0);
		quint16	ttlTriggers(0);
		quint16	adcTriggers(0);
		quint16 counterTriggers(0);
// extract parameter values:
		QChar c('0');
		quint32 datalen = (pd.bufferLength - pd.headerLength) / 3;

		MSG_DEBUG << QObject::tr("# : %1 has %2 events").arg(pd.bufferNumber).arg(datalen);
//
// status IDLE is for replaying files
// 
		for(quint32 counter = 0, i = 0; i < datalen; ++i, counter += 3)
		{
			tim = pd.data[counter + 1] & 0x7;
			tim <<= 16;
			tim += pd.data[counter];
			tim += headertime;
// id stands for the trigId and modId depending on the package type
			quint8 id = (pd.data[counter + 2] >> 12) & 0x7;
// not neutron event (counter, chopper, ...)
//			m_counter[TIMERID]->setTime(tim / 10000);
			if((pd.data[counter + 2] & TRIGGEREVENTTYPE))
			{
				triggers++;
				quint8 dataId = (pd.data[counter + 2] >> 8) & 0x0F;
//				ulong data = ((pd.data[counter + 2] & 0xFF) << 13) + ((pd.data[counter + 1] >> 3) & 0x7FFF);
//				quint16 time = (quint16)tim;
				switch(dataId)
				{
					case MON1ID :
					case MON2ID :
					case MON3ID :
					case MON4ID :
						++monitorTriggers;
//						++(*m_counter[dataId]);
//						MSG_DEBUG << QObject::tr("counter %1 : (%3 - %4)%2 : %5").arg(dataId).arg(m_counter[dataId]->value()).arg(i).arg(triggers).arg(data), DEBUG);
						break;
					case TTL1ID :
					case TTL2ID :
						++ttlTriggers;
//						++(*m_counter[dataId]);
//						MSG_DEBUG << QObject::tr("counter %1 : (%3 - %4)%2 : %5")
//							.arg(dataId).arg(m_counter[dataId]->value()).arg(i).arg(triggers).arg(data);
						break;
					case ADC1ID :
					case ADC2ID :
						++adcTriggers;
//						++(*m_counter[dataId]);
//						MSG_DEBUG << QObject::tr("counter %1 : (%3 - %4)%2 : %5")
//							.arg(dataId).arg(m_counter[dataId]->value()).arg(i).arg(triggers).arg(data);
						break;
					default:
						++counterTriggers;
//						MSG_DEBUG << QObject::tr("counter %1 : %2").arg(dataId).arg(i);
						break;
				}
			}
// neutron event:
			else
			{
				neutrons++;
				quint8 slotId = (pd.data[counter + 2] >> 7) & 0x1F;
				quint8 modChan = (id << 3) + slotId;
				quint8 chan = modChan + (mod << 6);
				quint16 amp = ((pd.data[counter+2] & 0x7F) << 3) + ((pd.data[counter+1] >> 13) & 0x7),
					pos = (pd.data[counter+1] >> 3) & 0x3FF;

				if(pd.bufferType == 0x0002)
				{
//
// in MDLL, data format is different:
// The position inside the PSD is used as y direction 
// Therefore the x position of the MDLL has to be used as channel
// the y position as position and the channel value is the amplitude value
// y position (10 bit) is at MPSD "Amplitude" data
// amplitude (8 bit) is at MPSD "chan" data
//
					quint16 val = chan;
					chan = pos;
					pos = amp;
					amp = val;
				}
//
// old MPSD-8 are running in 8-bit mode and the data are stored left in the ten bits
//
#if 0
				if (m_mesydaq->getMpsdId(mod, id) == TYPE_MPSD8OLD)
				{
					amp >>= 2;
					pos >>= 2;
				}
#endif
// BUG in firmware, every first neutron event seems to be "buggy" or virtual
// Only on newer modules with a distinct CPLD firmware
// BUG is reported
				if (neutrons == 1 && modChan == 0 && pos == 0 && amp == 0)
				{
					MSG_DEBUG << QObject::tr("GHOST EVENT: SlotID %1 Mod %2").arg(slotId).arg(id);
					continue;
				}
#if 0
				++(*m_counter[EVID]);
#endif
				if (m_posHist)
					m_posHist->incVal(chan, pos);
				if (m_ampHist)
					m_ampHist->incVal(chan, amp);
				if (m_diffractogram)
					m_diffractogram->incVal(chan);
#if 0
				if (m_posHistCorrected)
					m_posHistCorrected->incVal(chan, pos);
#endif
#if 1
//				if (m_mesydaq->getMpsdId(mod, id) == TYPE_MSTD16)
				{
//					MSG_DEBUG << QObject::tr("MSTD-16 event : chan : %1 : pos : %2 : id : %3").arg(chan).arg(pos).arg(id);
					chan <<= 1;
					chan += (pos >> 9) & 0x1;
					amp &= 0x1FF;
//					MSG_DEBUG << QObject::tr("Put this event into channel : %1").arg(chan);
					if (m_tubeSpectrum)
						m_tubeSpectrum->incVal(chan);
//					MSG_DEBUG << QObject::tr("Value of this channel : %1").arg(m_tubeSpectrum->value(chan)), INFO);
				}
#endif
			}
		}
		MSG_DEBUG << QObject::tr("# : %1 has %2 trigger events and %3 neutrons").arg(pd.bufferNumber).arg(triggers).arg(neutrons);
		MSG_DEBUG << QObject::tr("# : %1 Triggers : monitor %2, TTL %3, ADC %4, counter %5").arg(pd.bufferNumber)
					.arg(monitorTriggers).arg(ttlTriggers).arg(adcTriggers).arg(counterTriggers);
		g_triggers += triggers;
		g_neutrons += neutrons;
#if 0
		m_triggers += triggers;
#endif
		quint64 param[4];
		for(int i = 0; i < 4; i++)
		{
			quint64 var = 0;
			for(int j = 0; j < 3; j++)
			{
				var <<= 16;
				var |= pd.param[i][2 - j];
			}
			param[i] = var;
#if 0
			quint64 tmp = m_counter[i]->value();
			if (var)
			{
// only differences > 1 should be logged
				if ((tmp > var && tmp > (var + 1))|| (tmp < var && (tmp + 1) < var))
				{
					MSG_DEBUG << QObject::tr("%4 counter %1 : is %3 <-> should be %2").arg(i).arg(m_counter[i]->value()).arg(var).arg(m_packages));
					setCounter(i, var);
				}
			}
#endif
		}		
		MSG_DEBUG << QObject::tr("# : %1 params: %2 %3 %4 %5").arg(pd.bufferNumber).arg(param[0]).arg(param[1]).arg(param[2]).arg(param[3]);
	}
	else
		MSG_DEBUG << QObject::tr("buffer type : %1").arg(pd.bufferType);
}

static const quint16  	sep0 = 0x0000;
static const quint16  	sep5 = 0x5555;    
static const quint16  	sepA = 0xAAAA;
static const quint16  	sepF = 0xFFFF;

char buffer[1500];

bool getNextBlock(QDataStream &datStream, DATA_PACKET &dataBuf)
{
	const QChar c('0');
	quint16 sep1, sep2, sep3, sep4;

	datStream >> sep1 >> sep2 >> sep3 >> sep4;
// check for closing signature:
// closing separator: sepF sepA sep5 sep0
	bool ok = !((sep1 == sepF) && (sep2 == sepA) && (sep3 == sep5) && (sep4 == sep0));
	if (ok)
	{
//		memset(&dataBuf, 0, sizeof(dataBuf));
		dataBuf.bufferLength = sep1;
		dataBuf.bufferType = sep2;
		dataBuf.headerLength = sep3;
		dataBuf.bufferNumber = sep4;
		ok = (dataBuf.bufferLength < 730);
		if (ok)
		{
			int buflen = (dataBuf.bufferLength - 4) * sizeof(quint16); 
			char *pD = (char *)&dataBuf.runID;
			ok = datStream.readRawData(pD, buflen) == buflen;
			if (ok)
				for (int i = 0; i < buflen; i += 2)
				{
					char tmp = pD[i];
					pD[i] = pD[i + 1];
					pD[i + 1] = tmp;
				}
			else
				MSG_ERROR << "corrupted file";
			datStream >> sep1 >> sep2 >> sep3 >> sep4;
			// block separator : sep0 sepF sep5 sepA
			ok = ((sep1 == sep0) && (sep2 == sepF) && (sep3 == sep5) && (sep4 == sepA));
		}
		else
		{
			MSG_DEBUG << QObject::tr("erroneous length: %1 - aborting").arg(dataBuf.bufferLength);
			datStream >> sep1 >> sep2 >> sep3 >> sep4;
			MSG_DEBUG << QObject::tr("Separator: %1 %2 %3 %4").arg(sep1, 2, 16, c).arg(sep2, 2, 16, c).arg(sep3, 2, 16, c).arg(sep4, 2, 16, c);
		}
	}
	else
	{
			MSG_DEBUG << QObject::tr("EOF reached");
	}
	return ok;
}

void readListfile(QString readfilename)
{
	QDataStream datStream;
	QTextStream textStream;
	QFile datfile;
	QString str;

	m_posHist = new Histogram(0, 0);
	m_ampHist = new Histogram(0, 0);
	m_diffractogram = new Spectrum(0);
	m_tubeSpectrum = new Spectrum(0);
	m_tubeSpectrum->setAutoResize(true);
    
	datfile.setFileName(readfilename);
	datfile.open(QIODevice::ReadOnly);
	datStream.setDevice(&datfile);
	textStream.setDevice(&datfile);
#if 0
	char buffer[1500];	
	while (!datStream.atEnd())
	{
		datStream.readRawData(buffer, 1500);
		for (int i = 0; i < 1500; i += 2)
		{
			// std::swap takes about 4x more time
			char t = buffer[i];
			buffer[i] = buffer[i + 1];
			buffer[i + 1] = t;
		}
	}
	return;
#endif

	quint32 blocks(0),
		bcount(0);

	g_neutrons = g_triggers = 0;

	qint64	seekPos(0);
	for(;;) 
	{
		str = textStream.readLine();
		seekPos += str.size() + 1;
		MSG_DEBUG << str;
		if (str.startsWith("header length:"))
			break;
	}
	textStream.seek(seekPos);
	MSG_DEBUG << QObject::tr("readListfile : %1").arg(readfilename);

	DATA_PACKET 	dataBuf;
	quint16 sep1, sep2, sep3, sep4;
	datStream >> sep1 >> sep2 >> sep3 >> sep4;

	// header separator: sep0 sep5 sepA sepF
	if ((sep1 == sep0) && (sep2 == sep5) && (sep3 == sepA) && (sep4 == sepF))
		for(; getNextBlock(datStream, dataBuf); ++blocks, ++bcount)
		{
			quint64 tmp = dataBuf.time[0] + (quint64(dataBuf.time[1]) << 16) + (quint64(dataBuf.time[2]) << 32);
			MSG_DEBUG << blocks << ". header time : " << tmp;
			if (!blocks)
			{
				tmp = dataBuf.time[0] + (quint64(dataBuf.time[1]) << 16) + (quint64(dataBuf.time[2]) << 32);
// 				m_counter[TIMERID]->start(tmp / 10000);	// headertime is in 100ns steps
			}
// hand over data buffer for processing
			analyzeBuffer(dataBuf);
// check for next separator:
			if(!(bcount % 1000))
			{
				QCoreApplication::processEvents();
			}
		}	
	else
	{
		QChar c('0');
		MSG_DEBUG << QObject::tr("File structure error ");
		qint64 p = datStream.device()->pos();
		MSG_DEBUG << QObject::tr("at position : %1").arg(p);
		MSG_DEBUG << QObject::tr("Separator: %1 %2 %3 %4").arg(sep1, 2, 16, c).arg(sep2, 2, 16, c).arg(sep3, 2, 16, c).arg(sep4, 2, 16, c);
	}
	datfile.close();
        MSG_DEBUG << "Found " << blocks << " data packages";
	MSG_DEBUG << QObject::tr("%2 trigger events and %3 neutrons").arg(g_triggers).arg(g_neutrons);
	MSG_ERROR << sizeof(DATA_PACKET);

	MSG_ERROR << m_tubeSpectrum->width();

	for (int i = 0; i < m_tubeSpectrum->width(); ++i)
		MSG_ERROR << "tube[" << i << "] = " << m_tubeSpectrum->value(i);
}
