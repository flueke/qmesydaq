/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009 by Jens Kr�ger <jens.krueger@frm2.tum.de>          *
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

#include <QDebug>
#include <QStringList>
#include "mapcorrect.h"

#ifdef DEBUGBUILD
#define DBG0(fmt) do { char now_s[64]; QDateTime now_q(QDateTime::currentDateTime()); \
  snprintf(now_s,ARRAY_SIZE(now_s),"%04d/%02d/%02d %02d:%02d:%02d.%03d",now_q.date().year(),now_q.date().month(), \
	   now_q.date().day(),now_q.time().hour(),now_q.time().minute(),now_q.time().second(),now_q.time().msec()); \
  now_s[ARRAY_SIZE(now_s)-1]='\0'; qDebug("[%s] %s(%d): " fmt,now_s,__FILE__,__LINE__); } while (0)
#define DBG(fmt,args...) do { char now_s[64]; QDateTime now_q(QDateTime::currentDateTime()); \
  snprintf(now_s,ARRAY_SIZE(now_s),"%04d/%02d/%02d %02d:%02d:%02d.%03d",now_q.date().year(),now_q.date().month(), \
	   now_q.date().day(),now_q.time().hour(),now_q.time().minute(),now_q.time().second(),now_q.time().msec()); \
  now_s[ARRAY_SIZE(now_s)-1]='\0'; qDebug("[%s] %s(%d): " fmt,now_s,__FILE__,__LINE__,args); } while (0)
#define CRITICAL0(fmt) do { char now_s[64]; QDateTime now_q(QDateTime::currentDateTime()); \
  snprintf(now_s,ARRAY_SIZE(now_s),"%04d/%02d/%02d %02d:%02d:%02d.%03d",now_q.date().year(),now_q.date().month(), \
	   now_q.date().day(),now_q.time().hour(),now_q.time().minute(),now_q.time().second(),now_q.time().msec()); \
  now_s[ARRAY_SIZE(now_s)-1]='\0'; qCritical("[%s] %s(%d): " fmt,now_s,__FILE__,__LINE__); } while (0)
#define CRITICAL(fmt,args...) do { char now_s[64]; QDateTime now_q(QDateTime::currentDateTime()); \
  snprintf(now_s,ARRAY_SIZE(now_s),"%04d/%02d/%02d %02d:%02d:%02d.%03d",now_q.date().year(),now_q.date().month(), \
	   now_q.date().day(),now_q.time().hour(),now_q.time().minute(),now_q.time().second(),now_q.time().msec()); \
  now_s[ARRAY_SIZE(now_s)-1]='\0'; qCritical("[%s] %s(%d): " fmt,now_s,__FILE__,__LINE__,args); } while (0)
#else
#define DBG0(fmt)             do {} while (0)
#define DBG(fmt,args...)      do {} while (0)
#define CRITICAL0(fmt)        do {} while (0)
#define CRITICAL(fmt,args...) do {} while (0)
#endif

/***************************************************************************
 * histogram mapping and correction data
 ***************************************************************************/
// copy constructor
MapCorrection::MapCorrection(const MapCorrection& src)
  : MesydaqObject(), m_bNoMapping(src.m_bNoMapping), m_iOrientation(src.m_iOrientation)
  , m_iCorrection(src.m_iCorrection), m_rect(src.m_rect), m_mapRect(src.m_mapRect)
  , m_aptMap(src.m_aptMap), m_afCorrection(src.m_afCorrection)
{
}

// copy operator=
MapCorrection& MapCorrection::operator=(const MapCorrection& src)
{
  m_bNoMapping=src.m_bNoMapping;
  m_iOrientation=src.m_iOrientation;
  m_iCorrection=src.m_iCorrection;
  m_rect=src.m_rect;
  m_mapRect=src.m_mapRect;
  m_aptMap=src.m_aptMap;
  m_afCorrection=src.m_afCorrection;
  return *this;
}

// check, if mapping was initialized
bool MapCorrection::isValid() const
{
  int iCount;
  if (m_bNoMapping) return true;
  if (!m_rect.isValid() || !m_mapRect.isValid() || m_aptMap.isEmpty()) return false;
  iCount=m_rect.width()*m_rect.height();
  if (iCount!=m_aptMap.count()) return false;
  switch (m_iCorrection)
  {
    case MapCorrection::CorrectSourcePixel: break;
    case MapCorrection::CorrectMappedPixel: iCount=m_mapRect.width()*m_mapRect.height(); break;
    default: return false;
  }
  if (iCount!=m_afCorrection.count()) return false;
  return true;
}

void MapCorrection::setNoMap()
{
  m_bNoMapping=true;
  m_iOrientation=MapCorrection::OrientationUp;
  m_iCorrection=MapCorrection::CorrectSourcePixel;
  m_rect.setRect(0,0,0,0);
  m_mapRect.setRect(0,0,0,0);
  m_aptMap.clear();
  m_afCorrection.clear();
}

// initialize mapping
void MapCorrection::initialize(int iSrcWidth, int iSrcHeight, MapCorrection::Orientation iOrientation, MapCorrection::CorrectionType iCorrection)
{
  int i,iCount=iSrcWidth*iSrcHeight;
  m_bNoMapping=false;
  m_afCorrection.clear();
  switch (iOrientation)
  {
    case MapCorrection::OrientationUp:
    case MapCorrection::OrientationDown:
    case MapCorrection::OrientationLeft:
    case MapCorrection::OrientationRight:
      break;
    default: return;
  }

  switch (iCorrection)
  {
    case MapCorrection::CorrectSourcePixel:
      m_afCorrection.resize(iCount);
      break;
    case MapCorrection::CorrectMappedPixel:
      break;
    default: return;
  }

  MapCorrection::point nowhere={-1,-1};
  m_rect.setWidth(iSrcWidth);
  m_rect.setHeight(iSrcHeight);
  m_aptMap.clear();
  m_aptMap.resize(iCount);
  for (i=0; i<iCount; ++i)
    m_aptMap[i]=nowhere;
  for (i=0; i<m_afCorrection.count(); ++i)
    m_afCorrection[i]=1.0;
  m_iOrientation=iOrientation;
  m_iCorrection=iCorrection;
}

// set region of mapped data
void MapCorrection::setMappedRect(const QRect& mapRect)
{
  m_bNoMapping=false;
  m_mapRect=mapRect;
  if (m_iCorrection==MapCorrection::CorrectMappedPixel)
  {
    int iCount=mapRect.width()*mapRect.height();
    m_afCorrection.clear();
    m_afCorrection.resize(iCount);
    for (int i=0; i<iCount; ++i)
      m_afCorrection[i]=1.0;
  }
}

// store a single mapping
bool MapCorrection::map(const QPoint& src, const QPoint& dst, float fCorrection)
{
  m_bNoMapping=false;
  if (!m_rect.contains(src) || !m_mapRect.contains(dst))
    return false;

  int iPos=src.y()*m_rect.width()+src.x();
  MapCorrection::point* p=&m_aptMap[iPos];
  p->x=dst.x();
  p->y=dst.y();
  if (m_iCorrection==MapCorrection::CorrectMappedPixel)
  {
    iPos=dst.y()*m_mapRect.width()+dst.y();
    if (iPos<0 || iPos>=m_afCorrection.count())
      return false;
  }
  m_afCorrection[iPos]=fCorrection;
  return true;
}

// store a region mapping
bool MapCorrection::map(const QRect& src, const QPoint& dst, float fCorrection)
{
  m_bNoMapping=false;
  if (!src.isValid() || !m_rect.contains(src) || !m_mapRect.contains(dst))
    return false;

  for (int y=src.top(); y<=src.bottom(); ++y)
  {
    int iStart=y*m_rect.width();
    for (int x=src.left(); x<=src.right(); ++x)
    {
      MapCorrection::point* p=&m_aptMap[iStart+x];
      p->x=dst.x();
      p->y=dst.y();
      if (m_iCorrection==MapCorrection::CorrectSourcePixel)
	m_afCorrection[iStart+x]=fCorrection;
    }
  }
  if (m_iCorrection==MapCorrection::CorrectMappedPixel)
  {
    int iPos=dst.y()*m_mapRect.width()+dst.y();
    if (iPos<0 || iPos>=m_afCorrection.count())
      return false;
    m_afCorrection[iPos]=fCorrection;
  }
  return true;
}

// read mapping
bool MapCorrection::getMap(const QPoint& src, QPoint& dst, float& fCorrection) const
{
  if (m_bNoMapping)
  {
    dst=src;
    fCorrection=1.0;
    return true;
  }
  if (!m_rect.contains(src))
    return false;
  int iPos=src.y()*m_rect.width()+src.x();
  const MapCorrection::point* p=&m_aptMap[iPos];
  dst.setX(p->x);
  dst.setY(p->y);
  if (!m_mapRect.contains(dst))
    return false;
  if (m_iCorrection==MapCorrection::CorrectMappedPixel)
  {
    iPos=dst.y()*m_mapRect.width()+dst.x();
    if (iPos<0 || iPos>=m_afCorrection.count())
      return false;
  }
  fCorrection=m_afCorrection[iPos];
  return true;
}

// vertical mirror mapping data
void MapCorrection::mirrorVertical()
{
  int iWidth=m_rect.width();
  int iHeight=m_rect.height();
  int iMaxHeight=iHeight/2;

  if (m_bNoMapping) return;
  --iHeight;
  for (int y=0; y<iMaxHeight; ++y)
  {
    int iYPos1=iWidth*y;
    int iYPos2=iWidth*(iHeight-y);
    for (int x=0; x<iWidth; ++x)
    {
      MapCorrection::point pt(m_aptMap[iYPos1+x]);
      m_aptMap[iYPos1+x]=m_aptMap[iYPos2+x];
      m_aptMap[iYPos2+x]=pt;
      if (m_iCorrection==MapCorrection::CorrectSourcePixel)
      {
	float fCorrection(m_afCorrection[iYPos1+x]);
	m_afCorrection[iYPos1+x]=m_afCorrection[iYPos2+x];
	m_afCorrection[iYPos2+x]=fCorrection;
      }
    }
  }
}

// horizontal mirror mapping data
void MapCorrection::mirrorHorizontal()
{
  int iWidth=m_rect.width();
  int iHeight=m_rect.height();
  int iMaxWidth=iWidth/2;

  if (m_bNoMapping) return;
  --iWidth;
  for (int y=0; y<iHeight; ++y)
  {
    int iYPos=iWidth*y;
    for (int x=0; x<iMaxWidth; ++x)
    {
      MapCorrection::point pt(m_aptMap[iYPos+x]);
      m_aptMap[iYPos+x]=m_aptMap[iYPos+iWidth-x];
      m_aptMap[iYPos+iWidth-x]=pt;
      if (m_iCorrection==MapCorrection::CorrectSourcePixel)
      {
	float fCorrection(m_afCorrection[iYPos+x]);
	m_afCorrection[iYPos+x]=m_afCorrection[iYPos+iWidth-x];
	m_afCorrection[iYPos+iWidth-x]=fCorrection;
      }
    }
  }
}

// rotate mapping data counter clockwise
void MapCorrection::rotateLeft()
{
  int iSrcW=m_rect.width();
  int iSrcH=m_rect.height();

  int iMapL=m_mapRect.left();
  int iMapR=m_mapRect.right();
  int iMapW=m_mapRect.width();
  int iMapH=m_mapRect.height();

  bool bRotateCorrection=(m_iCorrection==MapCorrection::CorrectMappedPixel);
  QVector<float> afCorrection;

  if (m_bNoMapping) return;
  if (bRotateCorrection)
    afCorrection.resize(m_afCorrection.count());
  for (int y=0; y<iSrcH; ++y)
  {
    int iYPos=y*iSrcW;
    for (int x=0; x<iSrcW; ++x)
    {
      MapCorrection::point* p=&m_aptMap[iYPos+x];
      MapCorrection::point dst;
      dst.x=p->y;
      dst.y=iMapL+iMapR-p->x;
      if (bRotateCorrection)
	afCorrection[dst.y*iMapW+dst.x]=m_afCorrection[p->y*iMapW+p->x];
      *p=dst;
    }
  }
  m_mapRect.setWidth(iMapH);
  m_mapRect.setHeight(iMapW);
  if (bRotateCorrection)
    m_afCorrection=afCorrection;
}

// rotate mapping data clockwise
void MapCorrection::rotateRight()
{
  int iSrcW=m_rect.width();
  int iSrcH=m_rect.height();

  int iMapT=m_mapRect.top();
  int iMapB=m_mapRect.bottom();
  int iMapW=m_mapRect.width();
  int iMapH=m_mapRect.height();

  bool bRotateCorrection=(m_iCorrection==MapCorrection::CorrectMappedPixel);
  QVector<float> afCorrection;

  if (m_bNoMapping) return;
  if (bRotateCorrection)
    afCorrection.resize(m_afCorrection.count());
  for (int y=0; y<iSrcH; ++y)
  {
    int iYPos=y*iSrcW;
    for (int x=0; x<iSrcW; ++x)
    {
      MapCorrection::point* p=&m_aptMap[iYPos+x];
      MapCorrection::point dst;
      dst.x=iMapT+iMapB-p->y;
      dst.y=p->x;
      if (bRotateCorrection)
	afCorrection[dst.y*iMapW+dst.x]=m_afCorrection[p->y*iMapW+p->x];
      *p=dst;
    }
  }
  m_mapRect.setWidth(iMapH);
  m_mapRect.setHeight(iMapW);
  if (bRotateCorrection)
    m_afCorrection=afCorrection;
}

/***************************************************************************
 * a histogram of mapped and corrected data
 ***************************************************************************/
MappedHistogram::MappedHistogram(MapCorrection* pCorrection, Histogram* pHistogram /*= NULL*/)
  : m_iWidth(0), m_iHeight(0), m_pMapCorrection(NULL), m_ullTotalCounts(0ULL), m_dblTotalCounts(0.0), m_iMaxPos(-1)
{
  setMapCorrection(pCorrection,pHistogram);
}

MappedHistogram::MappedHistogram(const MappedHistogram& src)
  : MesydaqObject(), m_iWidth(src.m_iWidth), m_iHeight(src.m_iHeight)
  , m_pMapCorrection(src.m_pMapCorrection), m_adblData(src.m_adblData)
  , m_ullTotalCounts(src.m_ullTotalCounts), m_dblTotalCounts(src.m_dblTotalCounts)
  , m_iMaxPos(src.m_iMaxPos)
{
}

MappedHistogram& MappedHistogram::operator=(const MappedHistogram& src)
{
  m_iWidth=src.m_iWidth;
  m_iHeight=src.m_iHeight;
  m_pMapCorrection=src.m_pMapCorrection;
  m_adblData=src.m_adblData;
  m_ullTotalCounts=src.m_ullTotalCounts;
  m_dblTotalCounts=src.m_dblTotalCounts;
  m_iMaxPos=src.m_iMaxPos;
  return *this;
}

// OrientationUp:       channel --> X [left=0 ... right], bin --> Y [botton=0 ... top]
// OrientationUpRev:    channel --> X [left=0 ... right], bin --> Y [top=0 ... bottom]
// OrientationDown:     channel --> X [right=0 ... left], bin --> Y [top=0 ... bottom]
// OrientationDownRev:  channel --> X [right=0 ... left], bin --> Y [botton=0 ... top]
// OrientationLeft:     channel --> Y [bottom=0 ... top], bin --> X [left=0 ... right]
// OrientationLeftRev:  channel --> Y [bottom=0 ... top], bin --> X [right=0 ... left]
// OrientationRight:    channel --> Y [top=0 ... bottom], bin --> X [right=0 ... left]
// OrientationRightRev: channel --> Y [top=0 ... bottom], bin --> X [left=0 ... right]
void MappedHistogram::setMapCorrection(MapCorrection* pMapCorrection, Histogram* pSrc /*= NULL*/)
{
  if (pMapCorrection!=NULL && pMapCorrection!=m_pMapCorrection)
  {
    const QRect& mapRect=pMapCorrection->getMapRect();
    m_pMapCorrection=pMapCorrection;
    m_iWidth=mapRect.width();
    m_iHeight=mapRect.height();
  }
  else
  {
    if (pSrc==NULL) return;
    m_iWidth=pSrc->width();
    m_iHeight=pSrc->height();
  }
  m_adblData.resize(m_iWidth*m_iHeight);
  for (int ys=0; ys<pSrc->height(); ++ys)
  {
    for (int xs=0; xs<pSrc->width(); ++xs)
    {
      QPoint src(xs,ys);
      QPoint dst(-1,-1);
      float fCorrection=0.0;
      if (!m_pMapCorrection->getMap(src,dst,fCorrection)) continue;
      int xd=dst.x();
      int yd=dst.y();
      if (xd<0 || yd<0) continue;

      quint64 ull=pSrc->value(xs,ys);
      double dbl=((double)ull)*fCorrection;
      int iPos=yd*m_iWidth+xd;
      m_adblData[iPos]+=dbl;
      m_ullTotalCounts+=ull;
      m_dblTotalCounts+=dbl;
      if (m_iMaxPos<0) m_iMaxPos=iPos; else
	if (m_adblData[iPos]>m_adblData[m_iMaxPos]) m_iMaxPos=iPos;
    }
  }
}

quint64 MappedHistogram::getCorrectedTotalCounts(void)
{
  quint64 r=(quint64)(m_dblTotalCounts+0.5);
  if (!r && m_ullTotalCounts>0) ++r; // single event only
  return r;
}

quint64 MappedHistogram::value(quint16 x, quint16 y)
{
  register int iPos=m_iWidth*y+x;
  if (x<m_iWidth && y<m_iHeight && iPos>=0 && iPos<m_adblData.count())
  {
    double r=m_adblData[iPos];
    if (r>0.0 && r<1.0) r=1.0; // single event only
    return (quint64)(r+0.5);
  }
  return 0ULL;
}

double MappedHistogram::floatValue(quint16 x, quint16 y)
{
  register int iPos=m_iWidth*y+x;
  if (x<m_iWidth && y<m_iHeight && iPos>=0 && iPos<m_adblData.count())
    return m_adblData[iPos];
  return 0.0;
}

// OrientationUp:       channel --> X [left=0 ... right], bin --> Y [botton=0 ... top]
// OrientationUpRev:    channel --> X [left=0 ... right], bin --> Y [top=0 ... bottom]
// OrientationDown:     channel --> X [right=0 ... left], bin --> Y [top=0 ... bottom]
// OrientationDownRev:  channel --> X [right=0 ... left], bin --> Y [botton=0 ... top]
// OrientationLeft:     channel --> Y [bottom=0 ... top], bin --> X [left=0 ... right]
// OrientationLeftRev:  channel --> Y [bottom=0 ... top], bin --> X [right=0 ... left]
// OrientationRight:    channel --> Y [top=0 ... bottom], bin --> X [right=0 ... left]
// OrientationRightRev: channel --> Y [top=0 ... bottom], bin --> X [left=0 ... right]
bool MappedHistogram::incVal(quint16 channel, quint16 bin)
{
  bool bOK=false;
  if (!m_pMapCorrection)
  {
    int iDstX=-1;
    int iDstY=-1;
    float fCorrection=0.0;
    if (m_pMapCorrection->getMap(channel,bin,iDstX,iDstY,fCorrection))
    {
      int iPos=m_iWidth*iDstY+iDstX;
      if (iDstX>=0 && iDstX<m_iWidth && iDstY>=0 && iDstY<m_iHeight && iPos>=0 && iPos<m_adblData.count())
      {
        ++m_ullTotalCounts;
	m_adblData[iPos]+=fCorrection;
	m_dblTotalCounts+=fCorrection;
	if (m_adblData[iPos]>m_adblData[m_iMaxPos])
	  m_iMaxPos=iPos;
	bOK=true;
      }
    }
  }
  return bOK;
}

void MappedHistogram::clear(void)
{
  for (int i=0; i<m_adblData.count(); ++i)
    m_adblData[i]=0.0;
  m_ullTotalCounts=0ULL;
  m_dblTotalCounts=0.0;
  m_iMaxPos=-1;
}
/*
// OrientationUp:       channel --> X [left=0 ... right], bin --> Y [botton=0 ... top]
// OrientationUpRev:    channel --> X [left=0 ... right], bin --> Y [top=0 ... bottom]
// OrientationDown:     channel --> X [right=0 ... left], bin --> Y [top=0 ... bottom]
// OrientationDownRev:  channel --> X [right=0 ... left], bin --> Y [botton=0 ... top]
// OrientationLeft:     channel --> Y [bottom=0 ... top], bin --> X [left=0 ... right]
// OrientationLeftRev:  channel --> Y [bottom=0 ... top], bin --> X [right=0 ... left]
// OrientationRight:    channel --> Y [top=0 ... bottom], bin --> X [right=0 ... left]
// OrientationRightRev: channel --> Y [top=0 ... bottom], bin --> X [left=0 ... right]
void MappedHistogram::maporientation(int iSrcX, int iSrcY, int& iDstX, int& iDstY)
{
}

QPoint MappedHistogram::maporientation(const QPoint& src)
{
  QPoint dst;
  if (!m_pMapCorrection) return src;
  switch (m_pMapCorrection->m_iOrientation) // X
  {
    default: //MapCorrection::OrientationUp:
      return src;
    case MapCorrection::OrientationUpRev:
      dst.setX(src.x());
      break;
    case MapCorrection::OrientationDown:
    case MapCorrection::OrientationDownRev:
      dst.setX(m_iWidth-src.x()-1);
      break;
    case MapCorrection::OrientationLeft:
    case MapCorrection::OrientationRightRev:
      dst.setX(src.y());
      break;
    case MapCorrection::OrientationLeftRev:
    case MapCorrection::OrientationRight:
      dst.setX(m_iHeight-src.y()-1);
      break;
  }
  switch (m_pMapCorrection->m_iOrientation) // Y
  {
    default: return src; // never reached
    case MapCorrection::OrientationUpRev:
    case MapCorrection::OrientationDown:
    case MapCorrection::OrientationDownRev:
    case MapCorrection::OrientationLeft:
    case MapCorrection::OrientationLeftRev:
    case MapCorrection::OrientationRight:
    case MapCorrection::OrientationRightRev:
  }
}
*/