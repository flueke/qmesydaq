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

#ifndef __MAPCORRECTPARSER_H__8C42E71B_F6B6_49E9_B2B3_A53982C9165D__
#define __MAPCORRECTPARSER_H__8C42E71B_F6B6_49E9_B2B3_A53982C9165D__

#include "mapcorrect.h"

namespace CaressHelper
{
	bool zlib_zip(QByteArray abyIn, QByteArray &abyOut, bool bGzipHeader=true);

	bool zlib_unzip(QByteArray abyIn, QByteArray &abyOut);
}

/*!
  \brief base class for different mapping parsers
  \author Lutz Rossa <rossa@helmholtz-berlin.de>
 */
class CaressMapCorrection
{
public:
	CaressMapCorrection() {}
	virtual ~CaressMapCorrection() {}

	virtual MapCorrection parseCaressMapCorrection(const char* pMapping, int iLength, int iSrcWidth, int iSrcHeight, int iDstWidth, int iDstHeight);
	virtual MapCorrection parseCaressMapCorrection(const QString& mapping, int iSrcWidth, int iSrcHeight, int iDstWidth, int iDstHeight) = 0;

	enum CaressMapType { CARESSMAP_EXED, CARESSMAP_V4_LISTMODE, CARESSMAP_V4_CARESS };
};

/*!
  \brief default mapping parser (useable by EXED, VSANS)
  \author Lutz Rossa <rossa@helmholtz-berlin.de>
 */
class CaressMapCorrectionDefault : public CaressMapCorrection
{
public:
	CaressMapCorrectionDefault() {}

	/*!
	  \brief intermediate storage of mapping of source data for later use of MapCorrection
	  \class mapstorage
	  \author Lutz Rossa <rossa@helmholtz-berlin.de>
	 */
	class mapstorage {
	public:
		mapstorage() : m_iTube(-1), m_fCorrection(1.0), m_iSkipFirst(0), m_iFirstBin(0) {}
		mapstorage(const mapstorage& src) : m_iTube(src.m_iTube), m_fCorrection(src.m_fCorrection),
			m_iSkipFirst(src.m_iSkipFirst), m_iFirstBin(src.m_iFirstBin), m_abyMapData(src.m_abyMapData) {}
		mapstorage& operator=(const mapstorage& src)
		{ m_iTube=src.m_iTube; m_fCorrection=src.m_fCorrection; m_iSkipFirst=src.m_iSkipFirst;
			m_iFirstBin=src.m_iFirstBin; m_abyMapData=src.m_abyMapData; return *this; }

		int m_iTube;
		float m_fCorrection;
		int m_iSkipFirst;
		int m_iFirstBin;
		QByteArray m_abyMapData;
	};

	virtual MapCorrection parseCaressMapCorrection(const QString& mapping, int iSrcWidth, int iSrcHeight, int iDstWidth, int iDstHeight);
};

/*!
  \brief mapping parser for V4 instrument
  \author Lutz Rossa <rossa@helmholtz-berlin.de>
 */
class CaressMapCorrectionV4 : public CaressMapCorrection
{
public:
	CaressMapCorrectionV4(bool bCaressMap=false) : m_bCaressMap(bCaressMap) {}

	virtual MapCorrection parseCaressMapCorrection(const QString& mapping, int iSrcWidth, int iSrcHeight, int iDstWidth, int iDstHeight);

	bool m_bCaressMap;
};

#endif /*__MAPCORRECTPARSER_H__8C42E71B_F6B6_49E9_B2B3_A53982C9165D__*/
