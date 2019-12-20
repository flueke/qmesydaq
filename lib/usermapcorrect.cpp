/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009 by Jens Krüger <jens.krueger@frm2.tum.de>          *
 *   Copyright (C) 2011-2019 by Lutz Rossa <rossa@helmholtz-berlin.de>     *
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

#include "editormemory.h"
#include "logging.h"

UserMapCorrection::UserMapCorrection()
{
}

UserMapCorrection::UserMapCorrection(const QSize &size, const QString &fName)
	: MapCorrection(size, MapCorrection::OrientationUp, MapCorrection::CorrectSourcePixel)
	, m_pEditorMemory(NULL)
{
	loadCorrectionFile(fName);
}

UserMapCorrection::UserMapCorrection(const QSize &size, enum Orientation iOrientation, enum CorrectionType iCorrection)
	: MapCorrection(size, iOrientation, iCorrection)
	, m_pEditorMemory(NULL)
{
	m_pEditorMemory = new EditorMemory(NULL, size.width());
}

UserMapCorrection::~UserMapCorrection()
{
	if (m_pEditorMemory != NULL)
		delete m_pEditorMemory;
}

bool UserMapCorrection::loadCorrectionFile(const QString &fName)
{
	if (fName.isEmpty())
		return false;
	if (fName.indexOf(".mcal") > 0)
	{
		setMESFData(NULL);
		return loadCalFile(fName);
	}
	else if (fName.indexOf(".txt") > 0)
	{
		setMESFData(NULL);
		return loadLUTFile(fName);
	}
	else if (fName.endsWith(".mesf"))
		return loadMESFFile(fName);
	else
		return false;
}

bool UserMapCorrection::saveCorrectionFile(QString fName)
{
	if (m_pEditorMemory == NULL)
		return false;
	if (fName.isEmpty())
		fName = m_pEditorMemory->getPath();
	return m_pEditorMemory->saveToPath(fName);
}

bool UserMapCorrection::loadCalFile(const QString &fName)
{
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

		initialize(iSrcWidth, iSrcHeight, OrientationDownRev, CorrectSourcePixel);

		tmp = t.readLine();
// 3rd line contains detector limits (min max)
		list = tmp.split(QRegExp("\\s+"));
		if (list.size() > 1)
		{
			qreal 	min = list[0].toUInt(),
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
//				MSG_ERROR << tr("%1 %2 %3").arg(index).arg(min).arg(max);
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
//							MSG_ERROR << QPoint(iStartX, k) << arg(nullPt) << tr(" 0.0");
							if (!map(QPoint(iStartX, k), nullPt, 0.0))
							{
								MSG_FATAL << tr("failed");
							}
						}
						else
						{
//							MSG_ERROR << QPoint(iStartX, k) << arg(nullPt) << tr(" 1.0");
			  				if (!map(QPoint(iStartX, k), pt, 1.0))
							{
								MSG_FATAL << tr("failed");
							}
						}
				}
			}
		}
		return true;
	}
	return false;
}

bool UserMapCorrection::loadLUTFile(const QString &fName)
{
	MSG_NOTICE << tr("load correction file '%1'").arg(fName);
	QFile f;
	f.setFileName(fName);
	if (f.open(QIODevice::ReadOnly))
	{
		int iDstHeight(0);
		int iDstWidth(-1);
		int iSrcHeight(0);
		int iSrcWidth(0);

		QTextStream t(&f);
		QString tmp;
		for (int i = 0; !t.atEnd(); ++i)
		{
			tmp = t.readLine();
			QStringList list = tmp.split(QRegExp("\\s+"));
			switch (i)
			{
				case 0 :
					if (list.at(0) != "mesydaq")
						return false;
					break;
				case 1 :
					if (list.at(0) != "position")
						return false;
					break;
				case 2 :
					if (list.at(0) != "bin")
						return false;
					iSrcWidth = iDstWidth = (list.size() - 1) / 2;
					break;
				default:
					iSrcHeight = (i - 2);
					for (int j = 1; j < list.size() - 1; j += 2)
					{
						int y = list.at(j).toInt();
						if (y > iDstHeight)
							iDstHeight = y;
					}
					break;
			}
		}
		if (iDstHeight >= 125 && iDstHeight < 128) // Special case for the SANS-1 file where max = 126 but 128 bins are needed
			iDstHeight = 128;
		else if (iDstHeight == 246) // Special case for the SPODI instrument where 240 have to be mapped to
			iDstHeight = 255;   // 255 bins
		else if (iDstHeight)
			++iDstHeight;
		t.seek(0);
#if 0
		do
		{
			tmp = t.readLine();
			MSG_ERROR << tmp;
		}while (tmp.startsWith("#"));
#endif
		initialize(iSrcWidth, iSrcHeight, OrientationDownRev, CorrectSourcePixel);

		setMappedRect(QRect(0, 0, iDstWidth, iDstHeight));

		int row;
		QPoint nullPt(0, 0);

		for (int i = 0; !t.atEnd(); ++i)
		{
			tmp = t.readLine();
			QStringList list = tmp.split(QRegExp("\\s+"));
			switch (i)
			{
				case 0 :
					if (list.at(0) != "mesydaq")
						return false;
					break;
				case 1 :
					if (list.at(0) != "position")
						return false;
					break;
				case 2 :
					if (list.at(0) != "bin")
						return false;
					break;
				default:
					row = list.at(0).toInt();
					int col = 0;
					for (int j = 1; j < list.size() - 1; j += 2, ++col)
					{
						int y = list.at(j).toInt() - 1;
						if (y < 0)
						{
							if (!map(QPoint(col, row), nullPt, 0.0))
							{
								MSG_FATAL << QPoint(col, row) << tr("%1 NULL failed").arg(y);
							}
						}
						else
						{
							float correction(list.at(j + 1).toFloat());
			  				if (!map(QPoint(col, row), QPoint(col, y), correction))
							{
								MSG_FATAL << tr("failed");
							}
						}
					}
					break;
			}
		}
		return true;
	}
	return false;
}

bool UserMapCorrection::loadMESFFile(const QString &fName)
{
	if (m_pEditorMemory == NULL)
		m_pEditorMemory = new EditorMemory(NULL, m_rect.width());
	if (!m_pEditorMemory->loadFromPath(fName))
		return false;
	return setMapCorrection(*m_pEditorMemory);
}

void UserMapCorrection::setMESFData(EditorMemory *pEM)
{
	if (m_pEditorMemory == pEM)
		return;
	if (m_pEditorMemory != NULL)
		delete m_pEditorMemory;
	m_pEditorMemory = pEM;
}

bool UserMapCorrection::setMapCorrection(EditorMemory &EM)
{
	int iNonExisting = 0;                      // number of unused tubes
	int iNumberOfTubes = EM.getChannelCount(); // tube count
	int iMinStart = m_rect.height();           // lowest used channel
	int iMaxEnd = 0;                          // highest used channel
	int iFirst = m_rect.width();               // lowest used tube
	int iLast = -1;                            // highest used tube

	m_iOrientation = EM.getOrientation();
	m_aptMap.resize(m_rect.width() * m_rect.height());
	m_afCorrection.resize(m_rect.width() * m_rect.height());
	m_iCorrection = CorrectSourcePixel;
	if (iNumberOfTubes > m_rect.width())
		iNumberOfTubes = m_rect.width();
	for (int iY = 0; iY < iNumberOfTubes; ++iY)
	{
		MappedDetector &md = EM.getTubes()[iY];
		if (md.getEndOutput() >= 0 && iMaxEnd <= md.getEndOutput())
			iMaxEnd = md.getEndOutput() + 1;
		if (md.getStartOutput() >= 0 && iMinStart > md.getStartOutput())
			iMinStart = md.getStartOutput();

		double dblFactor(1.0);
		if (md.getEndInput() != md.getStartInput())
			dblFactor = double(md.getEndOutput() - md.getStartOutput()) / double(md.getEndInput() - md.getStartInput());

		bool bOffline = md.getEndInput() < 0 || md.getEndOutput() < 0 ||
						md.getStartInput() < 0 || md.getStartOutput() < 0 || md.getFactor() <= 0.0;

		for (int iX = 0; iX < m_rect.height(); ++iX)
		{
			QPoint *pDst = &m_aptMap[iY + iX * m_rect.width()];
			float* pFactor = &m_afCorrection[iY + iX * m_rect.width()];
			if (!bOffline && iX >= md.getStartInput() && iX <= md.getEndInput())
			{
				pDst->setX(iY - iNonExisting);
				pDst->setY(int(dblFactor * (iX - md.getStartInput()) + md.getStartOutput() + 0.5));
				*pFactor = md.getFactor();
			}
			else
			{
				pDst->setX(-1);
				pDst->setY(-1);
				*pFactor = 1.0;
			}
		}
		if (bOffline)
			++iNonExisting;
		else
		{
			if (iFirst > iY)
				iFirst = iY;
			if (iLast < iY)
				iLast = iY;
		}
	}
	if (iMinStart >= m_rect.height() || iMaxEnd <= 0 || iMinStart >= iMaxEnd)
		iMinStart = iMaxEnd = 0;
	if (iFirst > iLast)
		iFirst = iLast = 0;

	m_bNoMapping = false;
	m_mapRect = QRect(iFirst, iMinStart, iNumberOfTubes - iNonExisting, iMaxEnd - iMinStart);
	return (iMinStart < iMaxEnd && iFirst < iLast && iNumberOfTubes > iNonExisting);
}
