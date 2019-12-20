/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2019 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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

#include "mapcorrect.h"

LinearMapCorrection::LinearMapCorrection()
	: MapCorrection()
{
}

LinearMapCorrection::LinearMapCorrection(const QSize &srcSize, const QSize &destSize, const enum Orientation iOrientation)
	: MapCorrection(srcSize, iOrientation, MapCorrection::CorrectSourcePixel)
{
	int iDstHeight(destSize.height());
	int iSrcHeight(srcSize.height());
	int iDstWidth(destSize.width());
	int iSrcWidth(srcSize.width());

	setMappedRect(QRect(0, 0, iDstWidth, iDstHeight));

	for (int i = 0; i < iDstHeight; ++i)
	{
		int iStartY = (iSrcHeight * i) / iDstHeight;
		int iEndY   = (iSrcHeight * (i + 1)) / iDstHeight;
		for(int k = iStartY; k < iEndY; ++k)
			for (int j = 0; j < iDstWidth; ++j)
			{
				int iStartX = (iSrcWidth * j) / iDstWidth;
				int iEndX   = (iSrcWidth * (j + 1)) / iDstWidth;
				QPoint pt(j, i);
				for (; iStartX < iEndX; ++iStartX)
					map(QPoint(iStartX, k), pt, 1.0);
			}
	}
}
