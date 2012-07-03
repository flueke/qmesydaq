/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009 by Jens Krüger <jens.krueger@frm2.tum.de>          *
 *   Copyright (C) 2011 by Lutz Rossa <rossa@helmholtz-berlin.de>          *
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

#include "usermapcorrect.h"

#include <QFile>
#include <QTextStream>
#include <QStringList>

#include "logging.h"

UserMapCorrection::UserMapCorrection(const QString &fName)
{
	if (fName.isEmpty())
		return;

	QFile f;
	f.setFileName(fName);
	if (f.open(QIODevice::ReadOnly))
	{
		QTextStream t(&f);
		QString tmp;

		do
		{
			tmp = t.readLine();
			MSG_ERROR << tmp;
		}while (tmp.startsWith("#"));
// 1st line contains the target size
		QStringList list = tmp.split(QRegExp("\\s+"));
		int iDstHeight(128);
		int iDstWidth(128);
		if (list.size() > 1)
		{
			iDstWidth = list[0].toUInt();
			iDstHeight = list[1].toUInt();
		}
		tmp = t.readLine();
		list = tmp.split(QRegExp("\\s+"));
// 2nd line constains the source size
		int iSrcHeight(960);
		int iSrcWidth(128);
		if (list.size() > 1)
		{
			iSrcWidth = list[0].toUInt();
			iSrcHeight = list[1].toUInt();
		}

		initialize(iSrcWidth, iSrcHeight, OrientationDown, CorrectSourcePixel);

		tmp = t.readLine();
// 3rd line contains detector limits (min max)
		list = tmp.split(QRegExp("\\s+"));
		if (list.size() > 1)
		{
			qreal min = list[0].toUInt(),
			max = list[1].toUInt();
			if (min < max)
				m_detector = TubeRange(min, max);
		}

		for (int i = 0; !t.atEnd(); ++i)
		{
			tmp = t.readLine();
			list = tmp.split(QRegExp("\\s+"));
#if 0
			if (list.size() > 3 && !list[3].isEmpty())
			{
				for (int j = 0; j < list.size(); ++j)
					if (!i)
						m_tube[i] = TubeRange(list[j].toUInt());
					else
						m_tube[i].setMax(list[j].toUInt());
			}
			else
#endif
			{
				quint32 index = list[0].toUInt();
				qreal min = list[1].toUInt(),
				max = list[2].toUInt();
				m_tube[index] = TubeRange(min, max);
//				MSG_ERROR << index << " " << min << " " << max;
			}
		}
		f.close();

		setMappedRect(QRect(0, 0, iDstWidth, iDstHeight));

  		for (int i = 0; i < iDstHeight; ++i)
		{
			int iStartY = (iSrcHeight * i) / iDstHeight;
			int iEndY   = (iSrcHeight * (i + 1)) / iDstHeight;
			for(int k = iStartY; k < iEndY; ++k)
			{
				for (int j = 0; j < iDstWidth; ++j)
				{
					TubeCorrection corr(m_detector, m_tube.contains(j) ? m_tube[j] : m_detector);
					int chan = corr.calibrate(k);
					int iStartX = (iSrcWidth * j) / iDstWidth;
					int iEndX   = (iSrcWidth * (j + 1)) / iDstWidth;
					QPoint pt(j, i);
					QPoint nullPt(0, 0);
					for (; iStartX < iEndX; ++iStartX)
						if (chan < 0)
						{
//							MSG_ERROR << QPoint(iStartX, k) << nullPt << " 0.0";
							if (!map(QPoint(iStartX, k), nullPt, 0.0))
							{
								MSG_FATAL << "failed";
							}
						}
						else
						{
//							MSG_ERROR << QPoint(iStartX, k) << pt << " 1.0";
			  				if (!map(QPoint(iStartX, k), pt, 1.0))
							{
								MSG_FATAL << "failed";
							}
						}
				}
			}
		}
	}
}
