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
public:
  //! orientation of histogram
  // OrientationUp:       channel --> X [left=0 ... right], bin --> Y [botton=0 ... top]
  // OrientationUpRev:    channel --> X [left=0 ... right], bin --> Y [top=0 ... bottom]
  // OrientationDown:     channel --> X [right=0 ... left], bin --> Y [top=0 ... bottom]
  // OrientationDownRev:  channel --> X [right=0 ... left], bin --> Y [botton=0 ... top]
  // OrientationLeft:     channel --> Y [bottom=0 ... top], bin --> X [left=0 ... right]
  // OrientationLeftRev:  channel --> Y [bottom=0 ... top], bin --> X [right=0 ... left]
  // OrientationRight:    channel --> Y [top=0 ... bottom], bin --> X [right=0 ... left]
  // OrientationRightRev: channel --> Y [top=0 ... bottom], bin --> X [left=0 ... right]
  // this is used at class MappedHistogram, but stored here for convenience
  enum Orientation { OrientationUp=0, OrientationDown, OrientationLeft, OrientationRight,
		     OrientationUpRev, OrientationDownRev, OrientationLeftRev, OrientationRightRev };

  //! select which pixel should be hold a correction factor: source or mapped pixel
  enum CorrectionType { CorrectSourcePixel=0, CorrectMappedPixel };

  struct point // 4 bytes per pixel
  {
    quint16 x;
    quint16 y;
  };

  MapCorrection() : m_bNoMapping(false), m_iOrientation(MapCorrection::OrientationUp), m_iCorrection(MapCorrection::CorrectSourcePixel) {}
  MapCorrection(const MapCorrection& src);
  MapCorrection& operator=(const MapCorrection& src);
  MapCorrection(const QSize& size, enum Orientation iOrientation, enum CorrectionType iCorrection)
    { initialize(size.width(),size.height(),iOrientation,iCorrection); }
  ~MapCorrection() {}

  bool isNoMap() const { return m_bNoMapping; }
  bool isValid() const;

  void setNoMap();
  void initialize(int iWidth, int iHeight, enum Orientation iOrientation, enum CorrectionType iCorrection);
  inline void initialize(const QSize& size, enum Orientation iOrientation, enum CorrectionType iCorrection)
    { initialize(size.width(),size.height(),iOrientation,iCorrection); }
  void setMappedRect(const QRect& mapRect);

  bool map(const QPoint& src, const QPoint& dst, float dblCorrection);
  bool map(const QRect& src, const QPoint& dst, float dblCorrection);

  const QRect& getMapRect() const { return m_mapRect; }
  bool getMap(const QPoint& src, QPoint& dst, float& dblCorrection) const;
  inline bool getMap(int iSrcX, int iSrcY, int& iDstX, int& iDstY, float& dblCorrection) const
    { QPoint s(iSrcX,iSrcY),d; bool r=getMap(s,d,dblCorrection); iDstX=d.x(); iDstY=d.y(); return r; }

  void mirrorVertical();
  void mirrorHorizontal();
  void rotateLeft();
  void rotateRight();

  static MapCorrection noMap() { MapCorrection r; r.m_bNoMapping=true; return r; }

protected:
  bool                m_bNoMapping;
  enum Orientation    m_iOrientation;
  enum CorrectionType m_iCorrection;
  QRect               m_rect;
  QRect               m_mapRect;
  QVector<point>      m_aptMap;
  QVector<float>      m_afCorrection;
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
  MappedHistogram(MapCorrection* pCorrection, Histogram* pHistogram = NULL);
  MappedHistogram(const MappedHistogram& src);
  MappedHistogram& operator=(const MappedHistogram& src);
  ~MappedHistogram() {}

  void setMapCorrection(MapCorrection* pMapCorrection, Histogram* pSrc = NULL);
  void setHistogram(Histogram* pSrc) { setMapCorrection(NULL,pSrc); }

  //! \return sum of all events
  quint64 getTotalCounts(void) { return m_ullTotalCounts; }
  quint64 getCorrectedTotalCounts(void);

  //! \return the maximum value of this histogram
  quint64 max() { return m_iMaxPos>=0 && m_iMaxPos<m_adblData.count() ? ((quint64)m_adblData[m_iMaxPos]) : 0ULL; }

  //! \return the number of the first tube containing the maximum value
  int maxpos() { return m_iMaxPos; }

  //! \return the counts at a specific position
  quint64 value(quint16 x, quint16 y);
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

protected:
  quint16         m_iWidth;
  quint16         m_iHeight;
  MapCorrection*  m_pMapCorrection;

  QVector<double> m_adblData;
  quint64         m_ullTotalCounts;
  double          m_dblTotalCounts;
  int             m_iMaxPos;
/*
protected:
  void maporientation(int iSrcX, int iSrcY, int& iDstX, int& iDstY);
  QPoint maporientation(const QPoint& src);
*/
};

#endif /* __MAPCORRECT_H__EA8A6E38_8A00_4C54_861E_106BE233A7D9__ */
