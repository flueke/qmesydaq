/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009 by Jens Kr�ger <jens.krueger@frm2.tum.de>          *
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
#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "mdefines.h"

#include <QObject>
#include <QVector>
#include <QHash>
#include <QRect>

#include "libqmesydaq_global.h"

class QFile;

/**
 * \short represents a single spectrum 
 *
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class LIBQMESYDAQ_EXPORT Spectrum : public QObject
{
	Q_OBJECT

	//! defines whether the spectrum should be automatically resized or not
        //! if the data points not included in the spectrum
	Q_PROPERTY(bool m_autoResize READ autoResize WRITE setAutoResize)

public:
	Spectrum(const quint16 bins = LINBINS);

	~Spectrum();

	bool incVal(const quint16 bin);

	bool setValue(const quint16 bin, const quint64 val);

	bool addValue(const quint16 bin, const quint64 val);

	void clear(void);

	//! \return the maximum value of the spectrum
	quint64 max() {return m_width ? m_data[m_maximumPos] : 0;}

	//! \return the first position of the maximum value of the spectrum
	quint16 maxpos(void) const {return m_maximumPos;}

	//! \return sum of counts in the spectrum
	quint64 getTotalCounts(void) const {return m_totalCounts;}

	float mean(float &s);

//	quint64 operator[](quint16 index) {return m_data[index];}
	
	/**
	 * gives the counts at the position index
	 *
	 * \param index position inside the spectrum for the required counts
	 * \return the number of neutrons
	 */
	quint64 value(const quint16 index);

	/*!
	   \return the width of the histogram
	*/
	quint16	width(void) const
	{
		return m_width;
	}

	void setWidth(const quint16 w);

	/**
	 * sets the size of the spectrum to the desired size, if the size
	 * will be increased the new values will be set to zero, if the size
	 * will be decreased the values with indices above the new size will
	 * be discarded.
	 *
	 * \param  bins new size of the spectrum
	 */
	void resize(const quint16 bins) 
	{
		if (bins == m_width)
			return;
		m_data = (quint64 *)realloc(m_data, bins * sizeof(quint64));
		if (bins > m_width)
		{
			memset(m_data + m_width, '\0', (bins - m_width) * sizeof(quint64));
		}
		m_width = bins;
	}

	/*!
    	 * The histogram will be formatted
	 *
	 * \return formatted histogram as string
	 */
	QString format(void);

	//! \return auto resizing of the spectrum
	bool autoResize(void) const {return m_autoResize;}

	/**
             sets the autoresizing capability of the spectrum
             \param resize
         */
        void setAutoResize(const bool resize) {m_autoResize = resize;}

private:
	void calcFloatingMean(const quint16 bin);

	void calcMaximumPosition(const quint16 bin);

        /**
            checks whether the bin is inside the histogram or not.
            If the autoresize is set and the bin isn't inside the size
	    of the histogram will be resized to the bin number.
            
	    \param bin requested bin
         */
	bool checkBin(const quint16 bin);

private:
	/* QVector< */quint64 /*> */	*m_data;

	quint64			m_maximumPos;

	quint64			m_meanCount;

	quint64			m_totalCounts;

	//! implicit ring buffer due to the change of 256 -> 0
	quint8 			m_meanPos;

	//! last events
	quint16			m_floatingMean[256]; 

	bool			m_autoResize;

	quint16			m_width;
};


/**
 * \short represents a histogram
 *
 * This histogram object will automatically increase its size, if
 * the incVal method gives values which are not exist.
 *
 * \author Gregor Montermann <g.montermann@mesytec.com>
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class LIBQMESYDAQ_EXPORT Histogram : public QObject
{
	Q_OBJECT

	//! defines whether the histogram should be automatically resized or not
        //! if the data points not included in the histogram
	Q_PROPERTY(bool m_autoResize READ autoResize WRITE setAutoResize)
public:
	Histogram(const quint16 channels = CHANNELS, const quint16 bins = LINBINS);

	~Histogram();

	bool incVal(const quint16 chan, const quint16 bin);

	bool setValue(const quint16 chan, const quint16 bin, const quint64 val);

	bool addValue(const quint16 chan, const quint16 bin, const quint64 val);

	void clear(void);

	void clear(const quint16 channel);

	quint64 getTotalCounts(void) const;

	quint64 getCounts(const QRect &r) const;

	Spectrum *spectrum(const quint16 channel);

	//! \return the sum spectrum
	Spectrum *spectrum(void) {return &m_sumSpectrum;}

	quint64 max(const quint16 channel) const;

	quint64 max(void) const; 
	
	//! \return the number of the first tube containing the maximum value
	quint16 maxpos(void) const {return m_maximumPos;}

	quint16 maxpos(const quint16 channel) const;

	void getMean(float &mean, float &sigma);

	void getMean(const quint16 chan, float &mean, float &sigma);

	QString format(void);

	quint64 value(const quint16 x, const quint16 y) const;

	quint16	height(void) const; 

	void setHeight(const quint16 h);

	//! /return the width of the histogram
	quint16 width(void) const {return m_width;}

	void setWidth(const quint16);

	void resize(const quint16 w, const quint16 h);

	//! \return auto resizing of the spectrum
	bool autoResize(void) const {return m_autoResize;}

	/**
             sets the autoresizing capability of the spectrum
             \param resize
         */
        void setAutoResize(const bool resize); 

	//! returns the minimum value in currently set ROI
	quint64	minROI(void) const;

	//! returns the minimum value in currently set ROI
	quint64 maxROI(void) const;

	void calcMinMaxInROI(const QRectF &);

private:
	/**
	 * Calculates the maximum position of a tube spectrum
	 * 
         * \param chan number of the tube
	 */
	void calcMaximumPosition(const quint16 chan);

        /**
            checks whether the channel chan is inside the histogram or not.
            If the autoresize is set and the channel isn't inside the size
	    of the histogram will be resized to the channel number.
            
	    \param chan requested channel
         */
	bool checkChannel(const quint16 chan);

        /**
            checks whether the bin is inside the histogram or not.
            If the autoresize is set and the bin isn't inside the size
	    of the histogram will be resized to the bin number.
            
	    \param bin requested bin
         */
	bool checkBin(const quint16 bin);

private:
	//! total number of counts inside the histogram
	quint64 			m_totalCounts;

	//! tube spectra list
	QHash<QPoint, int>		m_data1;

	//! all spectra
	Spectrum			**m_data;

	//! the list of all keys for the spectra
	QList<quint16>			m_dataKeys;

	quint16				m_height;

	//! number of tubes
	quint16				m_width;

	//! sum spectrum
	Spectrum			m_sumSpectrum;

	//! number of the tube containing the histogram maximum
	quint16				m_maximumPos;

	//! automatic resize of the histogram allowed or not
	bool				m_autoResize;

	//! minimum value in ROI
	quint64				m_minROI;

	//! maximum value in ROI
	quint64				m_maxROI;
};

#endif
