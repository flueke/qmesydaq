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

#include <QList>
#include <QRect>
#include <QVector>
#include <QPoint>
#include "histogram.h"

class MappedHistogram;

/**
 * \short this object represents histogram mapping and correction data
 *
 * \author Lutz Rossa <rossa@helmholtz-berlin.de>
 */
class MapCorrection : public MesydaqObject
{
    Q_OBJECT
    Q_ENUMS(Orientation)

    Q_PROPERTY(bool m_bNoMapping READ isNoMap)

public:
    //! orientation of histogram
    //!
    //! used at class MappedHistogram, but stored here for convenience)
    //!
    enum Orientation {
      OrientationUp = 0,  //!< channel --> X [left=0 ... right], bin --> Y [botton=0 ... top]
      OrientationDown,    //!< channel --> X [right=0 ... left], bin --> Y [top=0 ... bottom]
      OrientationLeft,    //!< channel --> Y [bottom=0 ... top], bin --> X [left=0 ... right]
      OrientationRight,   //!< channel --> Y [top=0 ... bottom], bin --> X [right=0 ... left]
      OrientationUpRev,   //!< channel --> X [left=0 ... right], bin --> Y [top=0 ... bottom]
      OrientationDownRev, //!< channel --> X [right=0 ... left], bin --> Y [botton=0 ... top]
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
      : MesydaqObject()
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
      : MesydaqObject()
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

/**
 * \short represents a histogram of mapped and corrected data
 *
 * \author Lutz Rossa <rossa@helmholtz-berlin.de>
 */
class MappedHistogram : public MesydaqObject
{
    Q_OBJECT
public:
    MappedHistogram(MapCorrection* pMapCorrection, Histogram* pHistogram = NULL);
    MappedHistogram(const MappedHistogram& src);
    MappedHistogram& operator=(const MappedHistogram& src);

    //! destructor
    ~MappedHistogram() {}

    /*!
	set new histogram and possible new mapping (generate a new mapped copy of the source)

	\param pMapCorrection pointer to to new mapping data (this class stores the reference only)
	\param pSrc           pointer to existing source histogram or NULL
     */
    void setMapCorrection(MapCorrection* pMapCorrection, Histogram* pSrc = NULL);

    /*!
	sets the histogram (generate a mapped copy of the source)

	\param pSrc pointer to existing source histogram
     */
    void setHistogram(Histogram *pSrc) 
    { 
        setMapCorrection(NULL, pSrc); 
    }

    //! \return sum of all events as uncorrected value
    quint64 getTotalCounts(void) { return m_ullTotalCounts; }

    //! \return sum of all events after correction
    quint64 getCorrectedTotalCounts(void);

    //! \return the maximum value of this histogram
    quint64 max() { return m_iMaxPos>=0 && m_iMaxPos<m_adblData.count() ? ((quint64)m_adblData[m_iMaxPos]) : 0; }

    //! \return the number of the first tube containing the maximum value
    int maxpos() { return m_iMaxPos; }

    quint64 value(quint16 x, quint16 y);

    //! \return the mapped value as double
    double floatValue(quint16 x, quint16 y);

    //! \return the height of this histogram
    quint16 height() { return m_iHeight; }

    //! \return the width of this histogram
    quint16 width() { return m_iWidth; }

    //! \param chan hardware input channel
    //! \param bin  position at specified channel
    //! \brief store an event at position
    bool incVal(quint16 chan, quint16 bin);

    //! \brief clear this histogram
    void clear(void);

private:
    //! width of mapped histogram
    quint16 m_iWidth;

    //! height of mapped histogram
    quint16 m_iHeight;

    //! pointer to mapping information
    MapCorrection* m_pMapCorrection;

    //! mapped histogram data
    QVector<double> m_adblData;

    //! uncorrected event counter
    quint64 m_ullTotalCounts;

    //! corrected event counter
    double m_dblTotalCounts;

    //! array position of maximum value
    int m_iMaxPos;

#if 0
    //! remap point using stored orientation
    void maporientation(int iSrcX, int iSrcY, int& iDstX, int& iDstY);

    //! remap point using stored orientation
    QPoint maporientation(const QPoint& src);
#endif
};

#endif /* __MAPCORRECT_H__EA8A6E38_8A00_4C54_861E_106BE233A7D9__ */
