/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2013 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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

static const quint16  	sep0 = 0x0000;
static const quint16  	sep5 = 0x5555;    
static const quint16  	sepA = 0xAAAA;
static const quint16  	sepF = 0xFFFF;

char buffer[1500];

extern void fireData(const DATA_PACKET &dataPacket);

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

void readListfile(const QString &readfilename)
{
	QDataStream datStream;
	QTextStream textStream;
	QFile datfile;
	QString str;

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
			fireData(dataBuf);
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
}
