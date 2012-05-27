#include <QString>
#include <QDataStream>
#include <QTextStream>
#include <QFile>
#include <QDebug>
#include <QCoreApplication>

#include "structures.h"
#include "mdefines.h"

static const quint16  	sep0 = 0x0000;
static const quint16  	sep5 = 0x5555;    
static const quint16  	sepA = 0xAAAA;
static const quint16  	sepF = 0xFFFF;

void analyzeBuffer(DATA_PACKET pd)
{
	quint16 time;
	ulong 	data;
	quint16 neutrons = 0;
	quint16 triggers = 0;
	quint64 tim;
	quint16 mod = pd.deviceId;
	quint64 headertime = pd.time[0] + (quint64(pd.time[1]) << 16) + (quint64(pd.time[2]) << 32);
//	setCurrentTime(headertime / 10000); // headertime is in 100ns steps
	quint16 runID = pd.runID;

	qDebug() << QObject::tr("# : %3, mod : %1, runID : %2").arg(mod).arg(runID).arg(pd.bufferNumber);

	if(pd.bufferType < 0x0002)
	{
// extract parameter values:
		QChar c('0');
		quint32 datalen = (pd.bufferLength - pd.headerLength) / 3;

		qDebug() << QObject::tr("# : %1 has %2 events").arg(pd.bufferNumber).arg(datalen);

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
				data = ((pd.data[counter + 2] & 0xFF) << 13) + ((pd.data[counter + 1] >> 3) & 0x7FFF);
				time = (quint16)tim;
				switch(dataId)
				{
					case MON1ID :
					case MON2ID :
					case MON3ID :
					case MON4ID :
//						++(*m_counter[dataId]);
//						qDebug() << QObject::tr("counter %1 : (%3 - %4)%2 : %5").arg(dataId).arg(m_counter[dataId]->value()).arg(i).arg(triggers).arg(data), DEBUG);
						break;
					case TTL1ID :
					case TTL2ID :
//						++(*m_counter[dataId]);
//						qDebug() << QObject::tr("counter %1 : (%3 - %4)%2 : %5")
//							.arg(dataId).arg(m_counter[dataId]->value()).arg(i).arg(triggers).arg(data);
						break;
					case ADC1ID :
					case ADC2ID :
//						++(*m_counter[dataId]);
//						qDebug() << QObject::tr("counter %1 : (%3 - %4)%2 : %5")
//							.arg(dataId).arg(m_counter[dataId]->value()).arg(i).arg(triggers).arg(data);
						break;
					default:
						qDebug() << QObject::tr("counter %1 : %2").arg(dataId).arg(i);
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
					qDebug() << QObject::tr("GHOST EVENT: SlotID %1 Mod %2").arg(slotId).arg(id);
					continue;
				}
#if 0
				++(*m_counter[EVID]);
				if (m_posHist)
					m_posHist->incVal(chan, pos);
				if (m_ampHist)
					m_ampHist->incVal(chan, amp);
				if (m_diffractogram)
					m_diffractogram->incVal(chan);
				if (m_posHistCorrected)
					m_posHistCorrected->incVal(chan, pos);
#endif
#if 0
				if (m_mesydaq->getMpsdId(mod, id) == TYPE_MSTD16)
				{
//					qDebug() << QObject::tr("MSTD-16 event : chan : %1 : pos : %2 : id : %3").arg(chan).arg(pos).arg(id);
					chan <<= 1;
					chan += (pos >> 9) & 0x1;
					amp &= 0x1FF;
//					if (pos >= 480)
//						++chan;
					qDebug() << QObject::tr("Put this event into channel : %1").arg(chan);
//					m_tubeSpectrum->incVal(chan);
//					qDebug() << QObject::tr("Value of this channel : %1").arg(m_tubeSpectrum->value(chan)), INFO);
				}
//				else if (m_tubeSpectrum)
//					m_tubeSpectrum->incVal(chan);
#endif
			}
		}
		qDebug() << QObject::tr("# : %1 has %2 trigger events and %3 neutrons").arg(pd.bufferNumber).arg(triggers).arg(neutrons);
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
					qDebug() << QObject::tr("%4 counter %1 : is %3 <-> should be %2").arg(i).arg(m_counter[i]->value()).arg(var).arg(m_packages));
					setCounter(i, var);
				}
			}
#endif
		}		
		qDebug() << QObject::tr("# : %1 params: %2 %3 %4 %5").arg(pd.bufferNumber).arg(param[0]).arg(param[1]).arg(param[2]).arg(param[3]);
	}
	else
		qDebug() << QObject::tr("buffer type : %1").arg(pd.bufferType);
}

void readListfile(QString readfilename)
{
	QDataStream datStream;
	QTextStream textStream;
	QFile datfile;
	QString str;
	quint16 sep1, sep2, sep3, sep4;
    
	datfile.setFileName(readfilename);
	datfile.open(QIODevice::ReadOnly);
	datStream.setDevice(&datfile);
	textStream.setDevice(&datfile);

	quint32 blocks(0),
		bcount(0);

	qint64	seekPos(0);
	for(;;) 
	{
		str = textStream.readLine();
		seekPos += str.size() + 1;
		qDebug() << str;
		if (str.startsWith("header length:"))
			break;
	}
	textStream.seek(seekPos);
	datStream >> sep1 >> sep2 >> sep3 >> sep4;

	bool ok = ((sep1 == sep0) && (sep2 == sep5) && (sep3 == sepA) && (sep4 == sepF));
	qDebug() << QObject::tr("readListfile : %1").arg(ok);

	QChar c('0');
	DATA_PACKET 	dataBuf;
	for(; ok; ++blocks, ++bcount)
	{
		datStream >> sep1 >> sep2 >> sep3 >> sep4;
// check for closing signature:
		if((sep1 == sepF) && (sep2 == sepA) && (sep3 == sep5) && (sep4 == sep0))
		{
			qDebug() << QObject::tr("EOF reached after %1 buffers").arg(blocks);
			break;
		}

//		memset(&dataBuf, 0, sizeof(dataBuf));
		dataBuf.bufferLength = sep1;
		dataBuf.bufferType = sep2;
		dataBuf.headerLength = sep3;
		dataBuf.bufferNumber = sep4;
		if(dataBuf.bufferLength > 729)
		{
			qDebug() << QObject::tr("erroneous length: %1 - aborting").arg(dataBuf.bufferLength);
			datStream >> sep1 >> sep2 >> sep3 >> sep4;
			qDebug() << QObject::tr("Separator: %1 %2 %3 %4").arg(sep1, 2, 16, c).arg(sep2, 2, 16, c).arg(sep3, 2, 16, c).arg(sep4, 2, 16, c);
			break;
		}
		quint16 *pD = (quint16 *)&dataBuf.bufferLength;
		for(int i = 4; i < dataBuf.bufferLength; i++)
			datStream >> pD[i];
		quint64 tmp = dataBuf.time[0] + (quint64(dataBuf.time[1]) << 16) + (quint64(dataBuf.time[2]) << 32);
		qDebug() << blocks << ". header time : " << tmp;
		if (blocks == 1)
		{
			quint64 tmp = dataBuf.time[0] + (quint64(dataBuf.time[1]) << 16) + (quint64(dataBuf.time[2]) << 32);
			// m_counter[TIMERID]->start(tmp / 10000);	// headertime is in 100ns steps
		}
		if (blocks)
// hand over data buffer for processing
			analyzeBuffer(dataBuf);
// check for next separator:
//		qint64 p = textStream.device()->pos();
//		qDebug() << QObject::tr("at position : %1 (0x%2)").arg(p).arg(p, 8, 16, c);
		datStream >> sep1 >> sep2 >> sep3 >> sep4;
//		qDebug() << QObject::tr("Separator: %1 %2 %3 %4").arg(sep1, 2, 16, c).arg(sep2, 2, 16, c).arg(sep3, 2, 16, c).arg(sep4, 2, 16, c);
		ok = ((sep1 == sep0) && (sep2 == sepF) && (sep3 == sep5) && (sep4 == sepA));
		if (!ok)
		{
			qDebug() << QObject::tr("File structure error - read aborted after %1 buffers").arg(blocks);
			qint64 p = textStream.device()->pos();
			qDebug() << QObject::tr("at position : %1 (0x%2)").arg(p).arg(p, 8, 16, c);
			qDebug() << QObject::tr("Separator: %1 %2 %3 %4").arg(sep1, 2, 16, c).arg(sep2, 2, 16, c).arg(sep3, 2, 16, c).arg(sep4, 2, 16, c);
		}
		if(!(bcount % 1000))
		{
			QCoreApplication::processEvents();
		}
	}	
	datfile.close();
        qDebug() << "Found " << blocks << " data packages";
}
