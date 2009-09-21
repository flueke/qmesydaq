/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009 by Jens Krüger <jens.krueger@frm2.tum.de>          *
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

#include "mesydaqobject.h"

#include "mdefines.h"

#include <QVector>

#include <QMap>

class QFile;

/**
 * \short represents a single spectrum 
 * \author Jens Krüger <jens.krueger@frm2.tum.de>
 */
class Spectrum : public MesydaqObject
{
public:
	/**
	 * constructor
	 *
	 * \param bins number of points in the spectrum
	 */
	Spectrum(quint16 bins = LINBINS);

	//! destructor
	~Spectrum();

	/**
	 * add a event add position bin
	 *
	 * \param bin position inside the spectrum to increment
	 */
	bool incVal(quint16 bin);

	//! clear the spectrum
	void clear();

	//! \return the maximum value of the spectrum
	quint64 max() {return m_data.size() ? m_data[m_maximumPos] : 0;}

	//! \return the first position of the maximum value of the spectrum
	quint16 maxpos() {return m_maximumPos;}

	//! \return sum of counts in the spectrum
	quint16 getTotalCounts() {return m_totalCounts;}

	/**
	 * calculates the mean value and standard deviation of the mean value
	 * 
	 * \param s standard deviation of the mean value
	 * \return mean value
	 */
	float mean(float &s);

//	quint64 operator[](quint16 index) {return m_data[index];}
	
	/**
	 * gives the counts at the position index
	 *
	 * \param index position inside the spectrum for the required counts
	 * \return the number of neutrons
	 */
	quint64 value(quint16 index) {return m_data[index];}

	//! \return the lenght of the spectrum
	quint16	width() {return m_data.size();}

	/**
	 * sets the size of the spectrum to the desired size, if the size
	 * will be increased the new values will be set to zero, if the size
	 * will be decreased the values with indices above the new size will
	 * be discarded.
	 *
	 * \param  bins new size of the spectrum
	 */
	void resize(quint16 bins) {m_data.resize(bins);}

private:
	QVector<quint64>	m_data;

	quint64			m_maximumPos;

	quint64			m_meanCount;

	quint64			m_totalCounts;

	//! implicit ring buffer due to the change of 256 -> 0
	quint8 			m_meanPos;

	//! last events
	QVector<quint16>	m_floatingMean; 
};


/**
 * \short represents a histogram
 * This histogram object will automatically increase its size, if
 * the incVal method gives values which are not exist.
 * \author Gregor Montermann <g.montermann@mesytec.com>
 */
class Histogram : public MesydaqObject
{
Q_OBJECT
public:
	/**
	 * constructor
	 *
	 * \param channels number of channels (i.e. number of tubes)
	 * \param bins number of bins (inside a tube)
	 */
	Histogram(quint16 channels = CHANNELS, quint16 bins = LINBINS);

	~Histogram();
    
	/**
	 * increment value by 1 in cell[chan, bin]. If the cell does
	 * not exist it will be created.
	 *
	 * \param chan number of the spectrum
	 * \param bin number of the bin in the spectrum
	 * \return true if it was ok otherwise false
	 */
	bool incVal(quint16 chan, quint16 bin);

	//! clears the complete histogram
	void clear(void);

	/**
	 * clears the spectrum channel
	 *
	 * \param channel number of the spectrum to be cleared
	 */
	void clear(quint16 channel);

	//! returns the sum of all counts
	quint64 getTotalCounts(void);

	//! \return the pointer to the spectrum of the tube channel
	Spectrum *spectrum(quint16 channel);

	//! \return the pointer to the sum spectrum
	Spectrum *spectrum() {return &m_sumSpectrum;}

	//! \return the maximum of the spectrum of the tube channel
	quint64 max(quint16 channel);

	//! \return the maximum of the whole histogram
	quint64 max(); 
	
	//! \return the number of the first tube containing the maximum value
	quint16 maxpos() {return m_maximumPos;}

	//! \return the number of the bin in the tube channel
	quint16 maxpos(quint16 channel);

	/**
	 * gives the mean value and the standard deviation of the last events
	 *
	 * \param mean mean value
	 * \param sigma standard deviation
	 */
	void getMean(float &mean, float &sigma);

	/**
	 * gives the mean value and the standard deviation of the last events in the tube chan
	 *
	 * \param chan the number of the tube
	 * \param mean mean value
	 * \param sigma standard deviation
	 */
	void getMean(quint16 chan, float &mean, float &sigma);

	/**
	 * sets the width of each cell
	 *
	 * \param width 
	 */
	void setWidth(quint8 width);

	/**
	 * writes the histogram to the opened file with a comment.
	 *
	 * \param f file pointer to the opened file
	 * \param comment comment for the histogram
	 * \return true in case of success else false
	 */
	bool writeHistogram(QFile *f, const QString comment = "");

	/**
	 * gives the counts of the cell x,y. 
	 *
	 * \param x number of the bin
	 * \param y number of the tube
	 * \return 0 rief the cell does not exist, otherwise the counts
	 */
	quint64 value(quint16 x, quint16 y);

	//! \return number of tubes
	quint16	height(); 

private:
	quint64 			m_totalCounts;

	quint8 				m_twidth;

	QMap<quint16, Spectrum*>	m_data;

	Spectrum			m_sumSpectrum;

	quint16				m_maximumPos;
};

#endif


