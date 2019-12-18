/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2016 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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
#include <qwt_color_map.h>
#include <qwt_scale_map.h>
#include "mesydaqdata.h"

#include "logging.h"

/*!
    constructor
 */
MesydaqSpectrumData::MesydaqSpectrumData()
	: QwtData()
{
}

QwtData *MesydaqSpectrumData::copy() const
{
	MesydaqSpectrumData *tmp = new MesydaqSpectrumData();
	tmp->setData((Spectrum *)&m_spectrum);
	return tmp;
}

size_t MesydaqSpectrumData::size() const
{
	return m_spectrum.width() + 1;
}

double MesydaqSpectrumData::x(size_t i) const
{
	return i;
}

double MesydaqSpectrumData::y(size_t i) const
{
	double tmp = 0.0;
// Doubles the last point to get a horizontal line too at the right end
	if (i < m_spectrum.width())
		tmp = m_spectrum.value(i);
	else if (i == m_spectrum.width())
		tmp = m_spectrum.value(m_spectrum.width() - 1);
	if (tmp == 0.0)
		tmp = 0.1;
	return tmp;
}

void MesydaqSpectrumData::setData(Spectrum *data)
{
	m_spectrum = *data;
}

quint32 MesydaqSpectrumData::max(void)
{
	return m_spectrum.max();
}

QwtDoubleRect MesydaqSpectrumData::boundingRect() const
{
	QwtDoubleRect rect = QwtData::boundingRect();
	if (rect.isEmpty())
		 rect = QwtDoubleRect(0.0, 0.1, 1.0, 1.0);
	return rect;
}

MesydaqHistogramData::MesydaqHistogramData()
	: QwtRasterData()
	, m_range(QwtDoubleInterval(0, 0))
{
}

QwtRasterData *MesydaqHistogramData::copy() const
{
	MesydaqHistogramData *tmp = new MesydaqHistogramData();
	tmp->setData((Histogram *)&m_histogram);
	tmp->setRange(m_range);
	return tmp;
}

void MesydaqHistogramData::setRange(const QwtDoubleInterval &r)
{
	m_range = r;
}

QwtDoubleInterval MesydaqHistogramData::range() const
{
	double _max = double(m_histogram.maxROI());
	double _min = double(m_histogram.minROI());
	if (m_range.isNull() && (_min != _max))
			return QwtDoubleInterval(_min, _max);
	return QwtDoubleInterval(0.0, 1.0);
}

double MesydaqHistogramData::value(double x, double y) const
{
	return m_histogram.value(quint16(x), quint16(y));
}

void MesydaqHistogramData::setData(Histogram *data)
{
	m_histogram = *data;
	setBoundingRect(QRectF(0.0, 0.0, data ? data->width() : 0.0, data ? data->height() : 0.0));
}

/*!
    \fn void MesydaqHistogramData::initRaster(const QRectF &, const QSize &)

    initialize the raster data

 */
void MesydaqHistogramData::initRaster(const QRectF &, const QSize &)
{
}

/*!
    \fn QSize MesydaqHistogramData::rasterHint(const QRectF &r) const

    \note set Qwt library

    \param r

    \return the size of the raster data
 */
QSize MesydaqHistogramData::rasterHint(const QRectF &r) const
{
	return QwtRasterData::rasterHint(r);
}
	
/*!
     \fn QImage MesydaqPlotSpectrogram::renderImage(const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &area) const

     creates the image to display

    \param xMap
    \param yMap
    \param area
 
    \return image of the rendered spectrogram
 */
QImage MesydaqPlotSpectrogram::renderImage(const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &area)	const
{
	if (area.isEmpty())
		return QImage();
	QRect rect = transform(xMap, yMap, area);

	QwtRasterData &d_data = const_cast<QwtRasterData &>(data());
	if (d_data.boundingRect().isEmpty())
		return QImage();
	
//	QRect dataRect = transform(xMap, yMap, d_data.boundingRect());

	QwtColorMap::Format format = colorMap().format();
	
	QImage image(rect.size(), format == QwtColorMap::RGB ? QImage::Format_ARGB32 : QImage::Format_Indexed8);
	
	const QwtDoubleInterval intensityRange = d_data.range();
	if (!intensityRange.isValid())
		return image;

	d_data.initRaster(area, rect.size());
	if (format == QwtColorMap::RGB)
	{
		for (int x = rect.left(); x <= rect.right(); ++x)
		{
			const double tx = xMap.invTransform(x);

			double ty = 0;
			double val = 0;
			QRgb rgb = colorMap().rgb(intensityRange, val);
			for (int y = rect.top(); y <= rect.bottom(); ++y)
			{
				const double t = yMap.invTransform(y);
				if (int(t) != ty)
				{
					ty = int(t);
					val = d_data.value(tx, ty);
					rgb = colorMap().rgb(intensityRange, val);
				}	
				image.setPixel(x - rect.left(), y - rect.top(), rgb);
			}
		}	
	}
	return image;	
//	return QwtPlotSpectrogram::renderImage(xMap, yMap, area);
}
