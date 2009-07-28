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

#include <QObject>

#include "mdefines.h"

class QFile;

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

private:
	quint64		*m_data;

	quint16		m_bins;

	quint64		m_maximumPos;

	quint64		m_meanCount;

	quint64		m_meanPos;

	quint64		m_totalCounts;

	quint16		m_floatingMean[255];
};


/**
	@author Gregor Montermann <g.montermann@mesytec.com>
*/
class Histogram : public QObject
{
Q_OBJECT
public:
	Histogram(quint16 channels = CHANNELS, quint16 bins = LINBINS, QObject *parent = 0);

	~Histogram();
    
	bool incVal(quint16 chan, quint16 bin, quint64 time);

	void clear(void);

	void clear(quint16 channel);

	ulong getTotalCounts(void);

	void copyLine(quint16 channel, ulong *pLineBuffer);

	ulong max(quint16 channel);

	quint16 maxpos(quint16 channel);

	void getMean(quint16 chan, float* vals);

	void getMean(quint16 chan, float &mean, float &sigma);

	void setWidth(quint8 width);

	bool writeHistogram(QFile* f);

protected:
	ulong 		m_lastTime;

	ulong 		m_totalCounts;

	quint8 		m_twidth;

	Spectrum  	**m_data;

	quint16		m_channels;

	Spectrum	*m_sumSpectrum;

	Spectrum	*m_timeSpectrum;

	quint16		m_maximumPos;

//	quint64 	m_chanCounts[CHANNELS+1];

//	quint32 	m_floatingMean[CHANNELS+3][255];

//	quint8 		m_meanCount[CHANNELS+3];

//	quint8 		m_meanPos[CHANNELS+3];
};

#endif


