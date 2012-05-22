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
	Spectrum(quint16 bins = LINBINS);

	~Spectrum();

	bool incVal(quint16 bin);

	bool setValue(quint16 bin, quint64 val);

	bool addValue(quint16 bin, quint64 val);

	void clear();

	//! \return the maximum value of the spectrum
	quint64 max() {return m_data.size() ? m_data[m_maximumPos] : 0;}

	//! \return the first position of the maximum value of the spectrum
	quint16 maxpos() {return m_maximumPos;}

	//! \return sum of counts in the spectrum
	quint64 getTotalCounts() {return m_totalCounts;}

	float mean(float &s);

//	quint64 operator[](quint16 index) {return m_data[index];}
	
	/**
	 * gives the counts at the position index
	 *
	 * \param index position inside the spectrum for the required counts
	 * \return the number of neutrons
	 */
	quint64 value(quint16 index);

	/*!
	   \fn quin16 Histogram::width(void)

	   \return the width of the histogram
	*/
	quint16	width()
	{
		return m_width;
	}

	void setWidth(quint16);

	/**
	 * sets the size of the spectrum to the desired size, if the size
	 * will be increased the new values will be set to zero, if the size
	 * will be decreased the values with indices above the new size will
	 * be discarded.
	 *
	 * \param  bins new size of the spectrum
	 */
	void resize(quint16 bins) {m_data.resize(bins);}

	QString format(void);

	//! \return auto resizing of the spectrum
	bool autoResize(void) {return m_autoResize;}

	/**
             sets the autoresizing capability of the spectrum
             \param resize
         */
        void setAutoResize(bool resize) {m_autoResize = resize;}

private:
	void calcFloatingMean(quint16 bin);

	void calcMaximumPosition(quint16 bin);

private:
	QVector<quint64>	m_data;

	quint64			m_maximumPos;

	quint64			m_meanCount;

	quint64			m_totalCounts;

	//! implicit ring buffer due to the change of 256 -> 0
	quint8 			m_meanPos;

	//! last events
	QVector<quint16>	m_floatingMean; 

	bool			m_autoResize;

	quint32			m_width;
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
	Histogram(quint16 channels = CHANNELS, quint16 bins = LINBINS);

	~Histogram();

	bool incVal(quint16 chan, quint16 bin);

	bool setValue(quint16 chan, quint16 bin, quint64 val);

	bool addValue(quint16 chan, quint16 bin, quint64 val);

	void clear(void);

	void clear(quint16 channel);

	quint64 getTotalCounts(void);

	quint64 getCounts(QRect &r);

	Spectrum *spectrum(quint16 channel);

	//! \return the sum spectrum
	Spectrum *spectrum() {return &m_sumSpectrum;}

	quint64 max(quint16 channel);

	quint64 max(); 
	
	//! \return the number of the first tube containing the maximum value
	quint16 maxpos() {return m_maximumPos;}

	quint16 maxpos(quint16 channel);

	void getMean(float &mean, float &sigma);

	void getMean(quint16 chan, float &mean, float &sigma);

	QString format(void);

	quint64 value(quint16 x, quint16 y);

	quint16	height(); 

	void setHeight(quint16 h);

	quint16 width() {return m_width;}

	void setWidth(quint16);

	void resize(quint16 w, quint16 h);

	//! \return auto resizing of the spectrum
	bool autoResize(void) {return m_autoResize;}

	/**
             sets the autoresizing capability of the spectrum
             \param resize
         */
        void setAutoResize(bool resize); 

private:
	void calcMaximumPosition(quint16 chan);

	bool checkChannel(quint16 chan);

	bool checkBin(quint16 bin);

private:
	quint64 			m_totalCounts;

	QHash<QPoint, int>		m_data1;

	QHash<quint16, Spectrum*>	m_data;

	QList<quint16>			m_dataKeys;

	Spectrum			m_sumSpectrum;

	quint16				m_maximumPos;

	bool				m_autoResize;

	quint16				m_width;
};

#endif
