/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009 by Jens Krüger <jens.krueger@frm2.tum.de>          *
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

#include "mapcorrect.h"

/***************************************************************************
 * histogram mapping and correction data
 ***************************************************************************/
/*!
    copy constructor

    \param src source mapping
 */
MapCorrection::MapCorrection(const MapCorrection& src)
    : QObject()
    , m_bNoMapping(src.m_bNoMapping)
    , m_iOrientation(src.m_iOrientation)
    , m_iCorrection(src.m_iCorrection)
    , m_rect(src.m_rect), m_mapRect(src.m_mapRect)
    , m_aptMap(src.m_aptMap), m_afCorrection(src.m_afCorrection)
{
}

/*!
    copy operator=

    \param src source mapping
 */
MapCorrection& MapCorrection::operator=(const MapCorrection& src)
{
    m_bNoMapping = src.m_bNoMapping;
    m_iOrientation = src.m_iOrientation;
    m_iCorrection = src.m_iCorrection;
    m_rect = src.m_rect;
    m_mapRect = src.m_mapRect;
    m_aptMap = src.m_aptMap;
    m_afCorrection = src.m_afCorrection;
    return *this;
}

//! \return true if mapping was initialized
bool MapCorrection::isValid() const
{
    if (m_bNoMapping)
        return true;
    if (!m_rect.isValid() || !m_mapRect.isValid() || m_aptMap.isEmpty())
        return false;

    int iCount = m_rect.width() * m_rect.height();

    if (iCount != m_aptMap.count())
        return false;
    switch (m_iCorrection)
    {
        case MapCorrection::CorrectSourcePixel:
            break;
        case MapCorrection::CorrectMappedPixel:
            iCount = m_mapRect.width() * m_mapRect.height();
            break;
        default:
            return false;
    }
    return (iCount == m_afCorrection.count());
}

/*!
    clear mapping
 */
void MapCorrection::setNoMap()
{
    m_bNoMapping = true;
    m_iOrientation = MapCorrection::OrientationUp;
    m_iCorrection = MapCorrection::CorrectSourcePixel;
    m_rect.setRect(0, 0, 0, 0);
    m_mapRect.setRect(0, 0, 0, 0);
    m_aptMap.clear();
    m_afCorrection.clear();
}

/*!
    initialize mapping

    \param iSrcWidth    maximum width of source positions
    \param iSrcHeight   maximum height of source positions
    \param iOrientation orientation of source data to mapped data
    \param iCorrection  where should the correction factors be applied
 */
void MapCorrection::initialize(int iSrcWidth, int iSrcHeight, MapCorrection::Orientation iOrientation, MapCorrection::CorrectionType iCorrection)
{
    int iCount = iSrcWidth * iSrcHeight;

    m_bNoMapping = false;
    m_afCorrection.clear();
    switch (iOrientation)
    {
        case MapCorrection::OrientationUp:
        case MapCorrection::OrientationDown:
        case MapCorrection::OrientationLeft:
        case MapCorrection::OrientationRight:
        case MapCorrection::OrientationUpRev:
        case MapCorrection::OrientationDownRev:
        case MapCorrection::OrientationLeftRev:
        case MapCorrection::OrientationRightRev:
            break;
        default:
            return;
    }

    switch (iCorrection)
    {
        case MapCorrection::CorrectSourcePixel:
            m_afCorrection.resize(iCount);
            break;
        case MapCorrection::CorrectMappedPixel:
            break;
        default:
            return;
    }

    QPoint nowhere(-1, -1);
    m_rect.setWidth(iSrcWidth);
    m_rect.setHeight(iSrcHeight);
    m_aptMap.clear();
    m_aptMap.resize(iCount);
    for (int i = 0; i < iCount; ++i)
        m_aptMap[i] = nowhere;
    for (int i = 0; i < m_afCorrection.count(); ++i)
        m_afCorrection[i] = 1.0;
    m_iOrientation = iOrientation;
    m_iCorrection = iCorrection;
}

/*!
    set region of mapped data

    \param mapRect rectangle of mapped data
 */
void MapCorrection::setMappedRect(const QRect &mapRect)
{
    m_mapRect = mapRect;
    if (!m_bNoMapping && m_iCorrection == MapCorrection::CorrectMappedPixel)
    {
        int iCount = mapRect.width() * mapRect.height();
        m_afCorrection.clear();
        m_afCorrection.resize(iCount);
        for (int i = 0; i < iCount; ++i)
            m_afCorrection[i] = 1.0;
    }
}

/*!
    store a single mapping

    \param src
    \param dst
    \param fCorrection

    \return true if mapping was successful
 */
bool MapCorrection::map(const QPoint &src, const QPoint &dst, float fCorrection)
{
    m_bNoMapping = false;
    if (!m_rect.contains(src) || !m_mapRect.contains(dst))
        return false;

    int iPos = src.y() * m_rect.width() + src.x();
    QPoint *p = &m_aptMap[iPos];
    p->setX(dst.x());
    p->setY(dst.y());
    if (m_iCorrection == MapCorrection::CorrectMappedPixel)
    {
        iPos = (dst.y() - m_mapRect.top()) * m_mapRect.width() + (dst.x() -  m_mapRect.left());
        if (iPos < 0 || iPos >= m_afCorrection.count())
            return false;
    }
    m_afCorrection[iPos]=fCorrection;
    return true;
}

/*!
    store a region mapping

    \param src
    \param dst
    \param fCorrection

    \return true if mapping was successful
 */
bool MapCorrection::map(const QRect& src, const QPoint& dst, float fCorrection)
{
    m_bNoMapping = false;
    if (!src.isValid() || !m_rect.contains(src) || !m_mapRect.contains(dst))
        return false;

    for (int y = src.top(); y <= src.bottom(); ++y)
    {
        int iStart = y * m_rect.width();
        bool bLastY=(y==src.bottom());
        for (int x = src.left(); x <= src.right(); ++x)
        {
            QPoint *p = &m_aptMap[iStart+x];
            p->setX(dst.x());
            p->setY(dst.y());
            if (m_iCorrection == MapCorrection::CorrectSourcePixel)
                m_afCorrection[iStart + x] = fCorrection;
        }
    }
    if (m_iCorrection == MapCorrection::CorrectMappedPixel)
    {
        int iPos = (dst.y() - m_mapRect.top()) * m_mapRect.width() + (dst.x() -  m_mapRect.left());
        if (iPos < 0 || iPos >= m_afCorrection.count())
            return false;
        m_afCorrection[iPos] = fCorrection;
    }
    return true;
}

/*!
    read mapping

    \param src
    \param dst
    \param fCorrection

    \return true if mapping was successful
 */
bool MapCorrection::getMap(const QPoint &src, QPoint &dst, float &fCorrection) const
{
    int iPos = 0;
    const QPoint *p;

    if (m_bNoMapping)
        p = &src;
    else
    {
        if (!m_rect.contains(src))
            return false;
        iPos = src.y() * m_rect.width() + src.x();
        p = &m_aptMap[iPos];
    }

    if (m_mapRect.isValid())
    {
        switch (m_iOrientation)
        {
            default: //MapCorrection::OrientationUp: // channel --> X [left=0 ... right], bin --> Y [bottom=0 ... top]
                dst.setX(p->x());
                dst.setY(p->y());
                break;
            case MapCorrection::OrientationDownRev:  // channel --> X [right=0 ... left], bin --> Y [bottom=0 ... top]
                dst.setX(p->x());
                dst.setY(m_mapRect.bottom() - p->y());
                break;
            case MapCorrection::OrientationDown:     // channel --> X [right=0 ... left], bin --> Y [top=0 ... bottom]
                dst.setX(m_mapRect.right() - p->x());
                dst.setY(m_mapRect.bottom() - p->y());
                break;
            case MapCorrection::OrientationUpRev:    // channel --> X [left=0 ... right], bin --> Y [top=0 ... bottom]
                dst.setX(m_mapRect.right() - p->x());
                dst.setY(p->y());
                break;
            case MapCorrection::OrientationLeft:     // channel --> Y [bottom=0 ... top], bin --> X [left=0 ... right]
                dst.setX(p->y());
                dst.setY(p->x());
                break;
            case MapCorrection::OrientationRightRev: // channel --> Y [top=0 ... bottom], bin --> X [left=0 ... right]
                dst.setX(p->y());
                dst.setY(m_mapRect.right() - p->x());
                break;
            case MapCorrection::OrientationRight:    // channel --> Y [top=0 ... bottom], bin --> X [right=0 ... left]
                dst.setX(m_mapRect.bottom() - p->y());
                dst.setY(m_mapRect.right() - p->x());
                break;
            case MapCorrection::OrientationLeftRev:  // channel --> Y [bottom=0 ... top], bin --> X [right=0 ... left]
                dst.setX(m_mapRect.bottom() - p->y());
                dst.setY(p->x());
                break;
        }
    }
    else
        dst = *p;

    if (m_bNoMapping)
    {
        fCorrection = 1.0;
        return true;
    }

    if (!m_mapRect.contains(dst))
        return false;

    if (m_iCorrection == MapCorrection::CorrectMappedPixel)
    {
        iPos = (dst.y() - m_mapRect.top()) * m_mapRect.width() + (dst.x() -  m_mapRect.left());
        if (iPos < 0 || iPos >= m_afCorrection.count())
            return false;
    }
    fCorrection = m_afCorrection[iPos];
    return true;
}

/*!
    vertical mirror mapping data
 */
void MapCorrection::mirrorVertical()
{
    if (m_bNoMapping)
        return;

    int iWidth = m_rect.width();
    int iHeight = m_rect.height() - 1;
    int iMaxHeight = (iHeight + 1) / 2;

    for (int y = 0; y < iMaxHeight; ++y)
    {
        int iYPos1 = iWidth * y;
        int iYPos2 = iWidth * (iHeight - y);
        for (int x = 0; x < iWidth; ++x)
        {
            QPoint pt(m_aptMap[iYPos1 + x]);
            m_aptMap[iYPos1 + x] = m_aptMap[iYPos2 + x];
            m_aptMap[iYPos2 + x] = pt;
            if (m_iCorrection == MapCorrection::CorrectSourcePixel)
            {
                float fCorrection(m_afCorrection[iYPos1 + x]);
                m_afCorrection[iYPos1 + x] = m_afCorrection[iYPos2 + x];
                m_afCorrection[iYPos2 + x] = fCorrection;
            }
        }
    }
}

/*!
    horizontal mirror mapping data
 */
void MapCorrection::mirrorHorizontal()
{
    if (m_bNoMapping)
        return;

    int iWidth = m_rect.width() - 1;
    int iHeight = m_rect.height();
    int iMaxWidth = (iWidth + 1) / 2;

    for (int y = 0; y < iHeight; ++y)
    {
        int iYPos = iWidth * y;
        for (int x = 0; x < iMaxWidth; ++x)
        {
            QPoint pt(m_aptMap[iYPos + x]);
            m_aptMap[iYPos + x] = m_aptMap[iYPos + iWidth - x];
            m_aptMap[iYPos + iWidth - x] = pt;
            if (m_iCorrection == MapCorrection::CorrectSourcePixel)
            {
                float fCorrection(m_afCorrection[iYPos + x]);
                m_afCorrection[iYPos + x] = m_afCorrection[iYPos + iWidth - x];
                m_afCorrection[iYPos + iWidth - x] = fCorrection;
            }
        }
    }
}

/*!
 rotate mapping data counter clockwise
 \verbatim
 example (3*4 block):                | x  y      x  y
                                   --+------    ------
    6 |            6 |             A | 1  2      4  2
    5 | J-K-L      5 |             B | 2  2      4  3
    4 | G-H-I ===> 4 | L-I-F-C     C | 3  2 ===> 4  4
    3 | D-E-F ===> 3 | K-H-E-B     D | 1  3 ===> 3  2
    2 | A-B-C ===> 2 | J-G-D-A     ...
    1 |            1 |             J | 1  5 ===> 1  2
      +---------     +---------    K | 2  5      1  3
    0   1 2 3 4    0   1 2 3 4     L | 3  5      1  4
 \endverbatim
*/
void MapCorrection::rotateLeft()
{
    if (m_bNoMapping)
        return;

    int iSrcW = m_rect.width();
    int iSrcH = m_rect.height();

    int iMapT = m_mapRect.top();
    int iMapL = m_mapRect.left();
    int iMapW = m_mapRect.width();
    int iMapH = m_mapRect.height();

    bool bRotateCorrection = (m_iCorrection == MapCorrection::CorrectMappedPixel);
    QVector<float> afCorrection;

    if (bRotateCorrection)
        afCorrection.resize(m_afCorrection.count());
    for (int y = 0; y < iSrcH; ++y)
    {
        int iYPos = y * iSrcW;
        for (int x = 0; x < iSrcW; ++x)
        {
            QPoint *p = &m_aptMap[iYPos + x];
            QPoint dst;
            dst.setX(iMapL + iMapH-1 + iMapT - p->y());
            dst.setY(iMapT + p->x() - iMapL);
            if (bRotateCorrection)
                afCorrection[(dst.y() - iMapT) * iMapH + (dst.x() - iMapL)] = m_afCorrection[p->y() * iMapW + p->x()];
            *p = dst;
        }
    }
    m_mapRect.setWidth(iMapH);
    m_mapRect.setHeight(iMapW);
    if (bRotateCorrection)
        m_afCorrection=afCorrection;
}

/*!
    rotate mapping data clockwise

 \verbatim
 example (3*4 block):                | x  y      x  y
                                   --+------    ------
    6 |            6 |             A | 1  2      1  4
    5 | J-K-L      5 |             B | 2  2      1  3
    4 | G-H-I ===> 4 | A-D-G-J     C | 3  2 ===> 1  2
    3 | D-E-F ===> 3 | B-E-H-K     D | 1  3 ===> 2  4
    2 | A-B-C ===> 2 | C-F-I-L     ...
    1 |            1 |             J | 1  5 ===> 4  4
      +---------     +---------    K | 2  5      4  3
    0   1 2 3 4    0   1 2 3 4     L | 3  5      4  2
 \endverbatim
*/
void MapCorrection::rotateRight()
{
    if (m_bNoMapping)
        return;

    int iSrcW = m_rect.width();
    int iSrcH = m_rect.height();

    int iMapT = m_mapRect.top();
    int iMapL = m_mapRect.left();
    int iMapW = m_mapRect.width();
    int iMapH = m_mapRect.height();

    bool bRotateCorrection = (m_iCorrection == MapCorrection::CorrectMappedPixel);
    QVector<float> afCorrection;

    if (bRotateCorrection)
        afCorrection.resize(m_afCorrection.count());
    for (int y = 0; y < iSrcH; ++y)
    {
        int iYPos = y * iSrcW;
        for (int x=0; x<iSrcW; ++x)
        {
            QPoint *p = &m_aptMap[iYPos + x];
            QPoint dst;
            dst.setX(iMapL + p->y() - iMapT);
            dst.setY(iMapT + iMapW-1 + iMapL - p->x());
            if (bRotateCorrection)
                afCorrection[(dst.y() - iMapT) * iMapH + (dst.x() - iMapL)] = m_afCorrection[p->y() * iMapW + p->x()];
            *p = dst;
        }
    }
    m_mapRect.setWidth(iMapH);
    m_mapRect.setHeight(iMapW);
    if (bRotateCorrection)
        m_afCorrection = afCorrection;
}
