/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2011-2020 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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
#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <cmath>
//This Header file is not using pre compiled headers.
//The include of the stdafx is necessary for the round function,
//which is implemented inside the stdafx.cpp.
#include "stdafx.h"

class TubeRange
{
public:
	TubeRange();

	TubeRange(qreal min, qreal max);

	TubeRange(qreal min);

	TubeRange(const TubeRange &tr);

	void setMax(const qreal max);

	qreal start(void) const;

	qreal height(void) const;

private:
	qreal	m_min;

	qreal	m_max;
};

class TubeCorrection
{
public:
	TubeCorrection();

	TubeCorrection(const TubeRange &detRange, const TubeRange &tubeRange);

	TubeCorrection(const TubeCorrection &tc);

	quint32 calibrate(const qint32 pos);

private:
	qreal	m_calibScale;

	qint32	m_shift;

	quint32	m_detStart;

	quint32 m_detEnd;
};

#endif
