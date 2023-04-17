/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009 by Jens Krüger <jens.krueger@frm2.tum.de>          *
 *   Copyright (C) 2011-2020 by Lutz Rossa <rossa@helmholtz-berlin.de>     *
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

#ifndef __MAPCORRECT_H__EA8A6E38_8A00_4C54_861E_106BE233A7D9__
#define __MAPCORRECT_H__EA8A6E38_8A00_4C54_861E_106BE233A7D9__

#include "libqmesydaq_global.h"
#include "histogram.h"

class MappedHistogram;

/**
 * \short this object represents histogram mapping and correction data
 *
 * \author Lutz Rossa <rossa@helmholtz-berlin.de>
 */
class LIBQMESYDAQ_EXPORT MapCorrection : public QObject
{
	Q_OBJECT
	Q_ENUMS(Orientation)
	Q_ENUMS(CorrectionType);

	Q_PROPERTY(bool m_bNoMapping READ isNoMap)

public:
	/**
	 * orientation of histogram
	 *
	 * OrientatationUp means the origin at 0, 0 is in the left bottom corner
	 */
	enum Orientation {
		OrientationUp = 0,  //!< channel --> X [left=0 ... right], bin --> Y [bottom=0 ... top]
		OrientationDown,    //!< rotation 180 deg channel --> X [right=0 ... left], bin --> Y [top=0 ... bottom]
		OrientationLeft,    //!< rotation 90 deg ccw channel --> Y [bottom=0 ... top], bin --> X [left=0 ... right]
		OrientationRight,   //!< rotation 90 deg cw channel --> Y [top=0 ... bottom], bin --> X [right=0 ... left]
		OrientationUpRev,   //!< mirror channel --> X [left=0 ... right], bin --> Y [top=0 ... bottom]
		OrientationDownRev, //!< rotation 180 deg and mirror channel --> X [right=0 ... left], bin --> Y [bottom=0 ... top]
		OrientationLeftRev, //!< rotation 90 deg ccw and mirror channel --> Y [bottom=0 ... top], bin --> X [right=0 ... left]
		OrientationRightRev //!< rotation 90 deg cw and mirror channel --> Y [top=0 ... bottom], bin --> X [left=0 ... right]
	};

	//! select which pixel should be hold a correction factor: source or mapped pixel
	enum CorrectionType {
		CorrectSourcePixel = 0, //!< use correction factor before position mapping
		CorrectMappedPixel      //!< use correction factor after position mapping
	};

	//! default constructor
	MapCorrection();

	MapCorrection(const MapCorrection& src);

	MapCorrection& operator=(const MapCorrection& src);

	/*!
		constructor

		\param size
		\param iOrientation
		\param iCorrection
	 */
	MapCorrection(const QSize &size, enum Orientation iOrientation, enum CorrectionType iCorrection);

	//! destructor
	virtual ~MapCorrection();

	//! \return whether no mapping
	bool isNoMap() const;

	bool isValid() const;

	void setNoMap();

	void initialize(int iWidth, int iHeight, enum Orientation iOrientation, enum CorrectionType iCorrection);

	/*!
		initialize mapping

		\param size
		\param iOrientation
		\param iCorrection
	 */
	void initialize(const QSize& size, enum Orientation iOrientation, enum CorrectionType iCorrection);

	void setMappedRect(const QRect& mapRect);

	bool map(const QPoint& src, const QPoint& dst, float dblCorrection);

	bool map(const QRect& src, const QPoint& dst, float dblCorrection);

	//! \return the correction map
	const QRect& getMapRect() const;

	//! \return the source mapping size
	QSize getSourceSize() const;

	bool getMap(const QPoint& src, QPoint& dst, float& dblCorrection) const;

	/*!
		read mapping

		\param iSrcX
		\param iSrcY
		\param iDstX
		\param iDstY
		\param dblCorrection

		\return true if mapping was successful
	*/
	bool getMap(int iSrcX, int iSrcY, int &iDstX, int &iDstY, float &dblCorrection) const;

	void mirrorVertical();

	void mirrorHorizontal();

	void rotateLeft();

	void rotateRight();

	//! \return an empty mapping
	static MapCorrection noMap();

	void setOrientation(Orientation x);

	//! \return the orientation of the mapping
	enum Orientation orientation(void);

protected:
	//! do not apply mapping
	bool m_bNoMapping;

	//! orientation
	enum Orientation m_iOrientation;

	//! correction type
	enum CorrectionType m_iCorrection;

	//! size of the original array
	QRect m_rect;

	//! size and position of the mapped array
	QRect m_mapRect;

	//! mapping information
	QVector<QPoint> m_aptMap;

	//! intensity correction
	QVector<float> m_afCorrection;
};

/**
 * \short this object represents simple linear histogram mapping and correction data
 *
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class LIBQMESYDAQ_EXPORT LinearMapCorrection : public MapCorrection
{
public:
	//! default constructor
	LinearMapCorrection();

	/**
	 * constructor
	 *
	 * \param srcSize
	 * \param destSize
	 * \param iOrientation
	 */
	LinearMapCorrection(const QSize &srcSize, const QSize &destSize, const enum Orientation iOrientation = MapCorrection::OrientationUp);
};

#endif /* __MAPCORRECT_H__EA8A6E38_8A00_4C54_861E_106BE233A7D9__ */
