/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2014 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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
#ifndef MSTD_16_H
#define MSTD_16_H

#include "libqmesydaq_global.h"
#include "mpsd8.h"

/**
 * \short representation of MSTD-16 peripheral module 
 *
 * \author Jens Krüger <jens.krueger@frm2.tum.de>
 */
class LIBQMESYDAQ_EXPORT MSTD16 : public MPSD8
{
Q_OBJECT
public:
	MSTD16(quint8 id, QObject *parent = 0);

	//! \return the type of the MPSD as string
	QString getType(void) {return tr("MSTD-16");}

	//! \return the type of the MPSD as number
	virtual int type(void) {return TYPE_MSTD16;}

	virtual void	setPulser(quint8 chan, quint8 pos = 2, quint8 poti = 128, quint8 on = 0, bool preset = false);

	virtual QList<quint16> getHistogramList(void);

//	virtual QList<quint16> getActiveList(void);

protected:

};

#endif
