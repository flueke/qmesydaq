/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009 by Jens Kr�ger <jens.krueger@frm2.tum.de>          *
 *   Copyright (C) 2011-2012 by Lutz Rossa <rossa@helmholtz-berlin.de>     *
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

////////////////////////////////////////////////////////////////////////////
// $$HeadURL$$
//
// last change:
// $$Author$$
// $$Date$$
// revision $$Rev$$
////////////////////////////////////////////////////////////////////////////

#ifndef __MAPCORRECT_H__EA8A6E38_8A00_4C54_861E_106BE233A7D9__
#define __MAPCORRECT_H__EA8A6E38_8A00_4C54_861E_106BE233A7D9__


#include "libqmesydaq_global.h"
#include "histogram.h"

class LIBQMESYDAQ_EXPORT MappedHistogram;

/**
 * \short this object represents histogram mapping and correction data
 *
 * \author Lutz Rossa <rossa@helmholtz-berlin.de>
 */
class LIBQMESYDAQ_EXPORT MapCorrection : public QObject
{
    Q_OBJECT
    Q_ENUMS(Orientation)

    Q_PROPERTY(bool m_bNoMapping READ isNoMap)

public:
    //! orientation of histogram
    enum Orientation {
      OrientationUp = 0,  //!< channel --> X [left=0 ... right], bin --> Y [bottom=0 ... top]
      OrientationDown,    //!< channel --> X [right=0 ... left], bin --> Y [top=0 ... bottom]
      OrientationLeft,    //!< channel --> Y [bottom=0 ... top], bin --> X [left=0 ... right]
      OrientationRight,   //!< channel --> Y [top=0 ... bottom], bin --> X [right=0 ... left]
      OrientationUpRev,   //!< channel --> X [left=0 ... right], bin --> Y [top=0 ... bottom]
      OrientationDownRev, //!< channel --> X [right=0 ... left], bin --> Y [bottom=0 ... top]
      OrientationLeftRev, //!< channel --> Y [bottom=0 ... top], bin --> X [right=0 ... left]
      OrientationRightRev //!< channel --> Y [top=0 ... bottom], bin --> X [left=0 ... right]
    };

    //! select which pixel should be hold a correction factor: source or mapped pixel
    enum CorrectionType {
      CorrectSourcePixel = 0, //!< use correction factor before position mapping
      CorrectMappedPixel      //!< use correction factor after position mapping
    };

    //! default constructor
    MapCorrection()
      : QObject()
      , m_bNoMapping(false)
      , m_iOrientation(MapCorrection::OrientationUp)
      , m_iCorrection(MapCorrection::CorrectSourcePixel)
    {
    }

    MapCorrection(const MapCorrection& src);

    MapCorrection& operator=(const MapCorrection& src);

    /*!
        constructor

        \param size
        \param iOrientation
        \param iCorrection
     */
    MapCorrection(const QSize &size, enum Orientation iOrientation, enum CorrectionType iCorrection)
        : QObject()
    {
        initialize(size.width(), size.height(), iOrientation, iCorrection);
    }

    //! destructor
    ~MapCorrection()
    {
    }

    //! \return whether no mapping
    bool isNoMap() const { return m_bNoMapping; }

    bool isValid() const;

    void setNoMap();

    void initialize(int iWidth, int iHeight, enum Orientation iOrientation, enum CorrectionType iCorrection);

    /*!
        initialize mapping

        \param size
        \param iOrientation
        \param iCorrection
     */
    void initialize(const QSize& size, enum Orientation iOrientation, enum CorrectionType iCorrection)
    {
        initialize(size.width(),size.height(),iOrientation,iCorrection);
    }

    void setMappedRect(const QRect& mapRect);

    bool map(const QPoint& src, const QPoint& dst, float dblCorrection);

    bool map(const QRect& src, const QPoint& dst, float dblCorrection);

    //! \return the correction map
    const QRect& getMapRect() const { return m_mapRect; }

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
    bool getMap(int iSrcX, int iSrcY, int &iDstX, int &iDstY, float &dblCorrection) const
    {
    QPoint 	s(iSrcX, iSrcY),
        d;
        bool r = getMap(s,d,dblCorrection);
        iDstX = d.x();
        iDstY = d.y();
        return r;
    }

    void mirrorVertical();

    void mirrorHorizontal();

    void rotateLeft();

    void rotateRight();

    //! \return an empty mapping
    static MapCorrection noMap()
    {
        MapCorrection r;
        r.m_bNoMapping = true;
        return r;
    }

private:
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

class LIBQMESYDAQ_EXPORT LinearMapCorrection : public MapCorrection
{
public:
    LinearMapCorrection()
        : MapCorrection()
    {
    }

    LinearMapCorrection(const QSize &srcSize, const QSize &destSize)
        : MapCorrection(srcSize, MapCorrection::OrientationUp, MapCorrection::CorrectSourcePixel)
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
};

#endif /* __MAPCORRECT_H__EA8A6E38_8A00_4C54_861E_106BE233A7D9__ */
