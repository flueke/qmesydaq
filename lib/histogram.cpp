/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2020 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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

#include "histogram.h"
#include "spectrum.h"
#include "qmlogging.h"
#include "stdafx.h"
#include <cmath>
#include <algorithm>

/*!
    \fn Histogram::Histogram(const quint16 h, const quint16 w)

    constructor

    \param h number of channels in x direction
    \param w number of channels in y direction
 */
Histogram::Histogram(const quint16 w, const quint16 h)
	: QObject()
	, m_totalCounts(0)
	, m_data(0)
	, m_height(0)
	, m_width(0)
	, m_xSumSpectrum(NULL)
	, m_ySumSpectrum(NULL)
	, m_maximumPos(0)
	, m_autoResize(false)
	, m_minROI(0)
	, m_maxROI(0)
{
	m_xSumSpectrum = new Spectrum();
	m_ySumSpectrum = new Spectrum();
	resize(w, h);
	clear();
}

Histogram::Histogram(const Histogram &src)
	: QObject()
	, m_totalCounts(0)
	, m_data(0)
	, m_height(0)
	, m_width(0)
	, m_xSumSpectrum(NULL)
	, m_ySumSpectrum(NULL)
	, m_maximumPos(0)
	, m_autoResize(false)
	, m_minROI(0)
	, m_maxROI(0)
{
	*this = src;
}

Histogram& Histogram::operator=(const Histogram &src)
{
	m_totalCounts  = src.m_totalCounts;
	m_data1        = src.m_data1;
	if (m_xSumSpectrum)
	{
		delete m_xSumSpectrum;
		m_xSumSpectrum = NULL;
	}
	if (m_ySumSpectrum)
	{
		delete m_ySumSpectrum;
		m_ySumSpectrum = NULL;
	}
	for (int i = 0; i < m_width; ++i)
	{
		if (m_data[i])
			delete m_data[i];
		m_data[i] = NULL;
	}
	m_data.resize(src.m_width);
	for (int i = 0; i < src.m_width; ++i)
	{
		Spectrum *pSrc = src.m_data[i];
		if (pSrc != NULL)
			m_data[i] = new Spectrum(*pSrc);
		else
			m_data[i] = new Spectrum();
	}
	m_dataKeys     = src.m_dataKeys;
	m_height       = src.m_height;
	m_width        = src.m_width;
	m_xSumSpectrum = new Spectrum(*src.m_xSumSpectrum);
	m_ySumSpectrum = new Spectrum(*src.m_ySumSpectrum);
	m_maximumPos   = src.m_maximumPos;
	m_autoResize   = src.m_autoResize;
	m_minROI       = src.m_minROI;
	m_maxROI       = src.m_maxROI;
	return *this;
}

//! destructor
Histogram::~Histogram()
{
	for (int i = 0; i < m_width; ++i)
	{
		if (m_data[i])
			delete m_data[i];
		m_data[i] = NULL;
	}
	m_data.resize(0);
	if (m_xSumSpectrum)
		delete m_xSumSpectrum;
	m_xSumSpectrum = NULL;
	if (m_ySumSpectrum)
		delete m_ySumSpectrum;
	m_ySumSpectrum = NULL;
}

/*!
	\fn Histogram::max(void) const

    \return the maximum of the whole histogram
*/
quint64 Histogram::max(void) const
{
        if (m_maximumPos < m_width)
		return m_height ? m_data[m_maximumPos]->max() : 0;
	return 0;
}

/*!
    \fn Histogram::value(const quint16 x, const quint16 y) const

	gives the counts of the cell x,y.

    \param x ordinate of the histogram
    \param y abscissa of the histogram
    \return 0 rief the cell does not exist, otherwise the counts
 */
quint64 Histogram::value(const quint16 x, const quint16 y) const
{
	if (x < m_width)
		return m_data[x]->value(y);
	return 0;
}

bool Histogram::checkX(const quint16 x)
{
	if (x < m_width)
		return true;
	if (m_autoResize)
		setWidth(x + 1);
	else
		return false;
	return x < m_width;
}

bool Histogram::checkY(const quint16 y)
{
	if (y < m_height)
		return true;
	if (m_autoResize)
		setHeight(y + 1);
	else
		return false;
	return y < m_height;
}

void Histogram::calcMaximumPosition(const quint16 x)
{
	if (x < m_height)
		if (m_data[x]->max() > m_data[m_maximumPos]->max())
			m_maximumPos = x;
}

/**
    \fn Histogram::incVal(const quint16 x, const quint16 y)

    increment value by 1 in cell[x, y]. If the cell does
    not exist it will be created.

    \param x ordinate of the histogram
    \param y abscissa of the histogram
    \return true if it was ok otherwise false
 */
bool Histogram::incVal(const quint16 x, const quint16 y)
{
	if (!checkX(x) || !checkY(y))
		return false;
// total counts of histogram (like monitor ??)
	m_totalCounts++;
// sum spectrum of all x and y
	m_ySumSpectrum->incVal(y);
	m_xSumSpectrum->incVal(x);
	m_data[x]->incVal(y);
	calcMaximumPosition(x);
	return true;
}

/**
    \fn Histogram::setValue(const quint16 x, const quint16 y, const quint64 val)

    set the event value in cell[x, y]. If the cell does
    not exist it will be created.

    \param x ordinate of the histogram
    \param y abscissa of the histogram
    \param val events
    \return true if it was ok otherwise false
 */
bool Histogram::setValue(const quint16 x, const quint16 y, const quint64 val)
{
	if (!checkX(x) || !checkY(y))
		return false;
// total counts of histogram (like monitor ??)
	m_totalCounts -= m_data[x]->value(y);
	m_totalCounts += val;
// sum spectrum of all x and y
	m_xSumSpectrum->setValue(x, val);
	m_ySumSpectrum->setValue(y, val);
	m_data[x]->setValue(y, val);
	calcMaximumPosition(x);
	return true;
}

/**
    \fn Histogram::addValue(const quint16 x, const quint16 y, const quint64 val)

    set the event value in cell[x, y]. If the cell does
    not exist it will be created.

    \param x ordinate of the histogram
    \param y abscissa of the histogram
    \param val events
    \return true if it was ok otherwise false
 */
bool Histogram::addValue(const quint16 x, const quint16 y, const quint64 val)
{
	if (!checkX(x) || !checkY(y))
		return false;
// total counts of histogram (like monitor ??)
	m_totalCounts += val;
// sum spectrum of all x and y
	m_xSumSpectrum->addValue(x, val);
	m_ySumSpectrum->addValue(y, val);
	m_data[x]->addValue(y, val);
	calcMaximumPosition(x);
	return true;
}

/*!
    \fn Histogram::clear(void);

    clears the complete histogram
 */
void Histogram::clear(void)
{
	for (int i = 0; i < m_width; ++i)
	{
		Spectrum *value = m_data[i];
		Q_ASSERT_X(value != NULL, "Histogram::clear", "one of the spectra is NULL");
		if (value != NULL)
			value->clear();
	}
	m_xSumSpectrum->clear();
	m_ySumSpectrum->clear();
	m_totalCounts = 0;
	m_maximumPos = 0;
}

/*!
    \fn Histogram::getTotalCounts(void) const

    \return the sum of all counts
 */
quint64 Histogram::getTotalCounts(void) const
{
	return m_totalCounts;
}

/*!
    \fn quint64 Histogram::getCounts(const QRect &region) const

    \param region region of interest
    \return the number of events in the region
 */
quint64 Histogram::getCounts(const QRect &region) const
{
	MSG_DEBUG << "getCounts(" << region << ")";
	quint64 tmp(0);
	int h = region.y() + region.height();
	int w = region.x() + region.width();

	for (int y = region.y(); y < h; ++y)
		for (int x = region.x(); x < w; ++x)
			tmp += value(x, y);
	return tmp;
}

/*!
    \fn quint64 Histogram::getCounts(const QRectF &region) const

    Returns the number of events in the region given by the parameter region. Since the
    given region contains floating point numbers but the histogram has only discrete steps
    in x and y direction a cutting must be performed. The resulting counts are coming from
    all entries lying completely inside the region. If the region is smaller than 1 pixel
    in the histogram the only pixel value is taken.

    \param region region of interest
    \return the number of events in the region
 */
quint64 Histogram::getCounts(const QRectF &r) const
{
	MSG_DEBUG << "getCounts(" << r << ")";
	int 	x = ceil(r.x()),   	// to ensure, that we always on the left edge
		y = ceil(r.y()),   	// or bottom edge of the full pixel
		w = floor(r.x() + r.width()) - x,	// This should be the right edge
		h = floor(r.y() + r.height()) - y;	// or top edge of the pixel
	if (w < 1)		// we are inside one pixel
	{
		w = 1;
		if (r.width() > 1 || ceil(r.x()) == floor(r.x() + r.width()))
		{
			if (fabs(r.x() - x) > fabs(r.x() + r.width() - x))	// take the pixel with the bigger part
				x -= 1;
		}
		else
			x -= 1;
	}
	if (h < 1)		// we are inside one pixel
	{
		h = 1;
		if (r.height() > 1 || ceil(r.y()) == floor(r.y() + r.height()))
		{
			if (fabs(r.y() - y) > fabs(r.x() + r.height() - y))	// take the pixel with the bigger part
				y -= 1;
		}
		else
			y -= 1;
	}
	return getCounts(QRect(x, y, w, h));
}

/*!
    \fn Histogram::spectrum(const quint16 x)

    \param x ordinate
    \return the spectrum of the ordinate
 */
Spectrum *Histogram::spectrum(const quint16 x)
{
	if (x != 0xFFFF && x < m_height && m_data[x])
		return m_data[x];
	return NULL;
}

/*!
    \fn Histogram::max(const quint16 x) const

    \param x ordinate
    \return the maximum of spectrum of the x'th channel
 */
quint64 Histogram::max(const quint16 x) const
{
	if (x < m_height && m_data[x])
		return m_data[x]->max();
	return 0;
}

/*!
    \fn Histogram::maxpos(const quint16 x) const

    \param x ordinate
    \return the number of the y in the x'th
 */
quint16 Histogram::maxpos(const quint16 x) const
{
	if (x < m_height && m_data[x])
		return m_data[x]->maxpos();
	return 0;
}

/*!
    \fn Histogram::getMean(const quint16 x, float &m, float &s)

    gives the mean value and the standard deviation of the last events in the
    spectrum at ordinate x

    \param x ordinate
    \param m mean value
    \param s standard deviation
 */
void Histogram::getMean(const quint16 x, float &m, float &s)
{
	if (x < m_width && m_data[x])
		m = m_data[x]->mean(s);
	else
		m = s = 0.0;
}

/*!
    \fn Histogram::getMean(float &m, float &s)

    gives the mean value and the standard deviation of the last events

    \param m mean value
    \param s standard deviation
 */
void Histogram::getMean(float &m, float &s)
{
	m = m_xSumSpectrum->mean(s);
}

/*!
    \fn	quint16 Histogram::height(void) const

    \return number of tubes
*/
quint16	Histogram::height(void) const
{
	return m_height;
}

/*!
    \fn void Histogram::setWidth(const quint16 w)

    sets the width of each cell

    \param w new witdh of histogram (or number of tubes)
 */
void Histogram::setWidth(const quint16 w)
{
	if (w == m_width)
		return;
	if (w > m_width)
	{
		m_data.resize(w);
		for (int i = m_width; i < w; ++i)
			m_data[i] = new Spectrum(m_height);
	}
	else
	{
		for (int i = m_width - 1; i >= w; --i)
		{
			if (m_data[i])
				delete m_data[i];
			m_data[i] = NULL;
		}
		m_data.resize(w);
	}
	m_xSumSpectrum->setWidth(w);
	m_maximumPos = w - 1;
	m_width = w;
}

/*!
    \fn void Histogram::setHeight(const quint16 h)

    \param h
 */
void Histogram::setHeight(const quint16 h)
{
	for(int i = 0; i < m_width; ++i)
	{
		Spectrum *s = m_data[i];
		Q_ASSERT_X(s != NULL, "Histogram::setWidth", "one of the spectra is NULL");
		s->setWidth(h);
	}
	m_ySumSpectrum->setWidth(h);
	m_height = h;
}

/*!
    \fn void Histogram::resize(const quint16 w, const quint16 h)

    sets the size of histogram

    \param w width of histogram
    \param h height of histogram
 */
void Histogram::resize(const quint16 w, const quint16 h)
{
	setWidth(w);
	setHeight(h);
}

void Histogram::setAutoResize(const bool resize)
{
	m_autoResize = resize;
	for(int i = 0; i < m_width; ++i)
	{
		Spectrum *s = m_data[i];
		Q_ASSERT_X(s != NULL, "Histogram::setAutoResize", "one of the spectra is NULL");
		s->setAutoResize(resize);
	}
}

QString Histogram::formatLine(const int line)
{
	QString t("");
	for (int j = 0; j < width(); j++)
		t += QString("\t%1").arg(value(j, line));
	return t;
}

QString Histogram::format(void)
{
	QString endLine("\r\n");
	QString t("");
        for (int i = 0; i < height(); i++)
                t += formatLine(i) + endLine;
	return t;
}

QString Histogram::format(const QString &headline)
{
	QString endLine("\r\n");
	QString t(headline + endLine);
        for (int i = 0; i < width(); ++i)
                t += QString("\t%1").arg(i);
        t += endLine;
        for (int i = 0; i < height(); i++)
                t += QString("%1").arg(i) + formatLine(i) + endLine;
        t += endLine;
        return t;
}

/**
   \fn void Histogram::calcMinMaxInROI(const QRectF &r)

   calculates the min and max in the ROI

   \param r required ROI
 */
void Histogram::calcMinMaxInROI(const QRectF &r)
{
	m_minROI = max();
	m_maxROI = 0;
// no idea why, but Qwt seems to define height as width and vice versa
	int right = ceil(r.left() + r.width());
	int top = ceil(r.top() + r.height());
	for (int j = floor(r.top()); j <= top; ++j)
		for (int i = floor(r.left()); i <= right; ++i)
		{
			quint64 tmp = value(i, j);
			m_minROI = std::min(m_minROI, tmp);
			m_maxROI = std::max(m_maxROI, tmp);
		}
	if (m_minROI == max())
	{
		m_maxROI = m_minROI = value(round(r.left()), round(r.top()));
	}
}

quint64 Histogram::minROI(void) const
{
	return m_minROI;
}

quint64 Histogram::maxROI(void) const
{
	return m_maxROI;
}

/**
    \fn void Histogram::addSlice(const quint16 n, const quint16 d, const Histogram &h)

   Adds a histogram as a slice to an existing histogram in the following manner:
   Beginning with the number n the histogram h will will be added column by column
   to the histogram. The first column of h will be the column n in the histogram and
   the second column of h will be the (n + d)th column of the histogram, and so on.

   \param n starting column in the histogram
   \param d distance between the columns
   \param h histogram
 */
void Histogram::addSlice(const quint16 n, const quint16 d, const Histogram &h)
{
	quint16 col(n);
	for (int i = 0; i < h.width(); ++i, col += d)
		for (int j = 0; j < h.height(); ++j)
			setValue(col, j, h.value(i, j));
}

Spectrum *Histogram::xSumSpectrum(void)
{
	return m_xSumSpectrum;
}

Spectrum *Histogram::ySumSpectrum(void)
{
	return m_ySumSpectrum;
}

quint16 Histogram::maxpos(void) const
{
	return m_maximumPos;
}

quint16 Histogram::width(void) const
{
	return m_width;
}

bool Histogram::autoResize(void) const
{
	return m_autoResize;
}
