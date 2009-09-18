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

class QFile;

/**
	@author Jens Krüger <jens.krueger@frm2.tum.de>
*/
class Spectrum 
{
public:
	Spectrum(quint16 bins = LINBINS);

	~Spectrum();

	bool incVal(quint16 bin);

	void clear();

	quint64 max() {return m_data[m_maximumPos];}

	quint16 maxpos() {return m_maximumPos;}

	quint16 getTotalCounts() {return m_totalCounts;}

	float mean(float &s);

//	quint64 operator[](quint16 index) {return m_data[index];}
	
	quint64 value(quint16 index) {return m_data[index];}

	quint16	width() {return m_data.size();}

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
	@author Gregor Montermann <g.montermann@mesytec.com>
*/
class Histogram : public MesydaqObject
{
Q_OBJECT
public:
	Histogram(quint16 channels = CHANNELS, quint16 bins = LINBINS, QObject *parent = 0);

	~Histogram();
    
	bool incVal(quint16 chan, quint16 bin);

	void clear(void);

	void clear(quint16 channel);

	quint64 getTotalCounts(void);

	Spectrum *spectrum(quint16 channel);

	Spectrum *spectrum() {return &m_sumSpectrum;}

	quint64 max(quint16 channel);

	quint64 max() {return m_data[m_maximumPos]->max();}
	
	quint16 maxpos() {return m_maximumPos;}

	quint16 maxpos(quint16 channel);

	void getMean(float &mean, float &sigma);

	void getMean(quint16 chan, float &mean, float &sigma);

	void setWidth(quint8 width);

	bool writeHistogram(QFile* f, const QString = "");

	quint64 value(quint16 x, quint16 y);

	quint16	height() {return m_channels;}

protected:
	quint64 	m_totalCounts;

	quint8 		m_twidth;

	Spectrum  	**m_data;

	quint16		m_channels;

	Spectrum	m_sumSpectrum;

	quint16		m_maximumPos;
};

#endif


