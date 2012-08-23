/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2012 by Jens Kr�ger <jens.krueger@frm2.tum.de>     *
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

#ifndef _USERMAPCORRECT_H_
#define _USERMAPCORRECT_H_

#include "mapcorrect.h"
#include "calibration.h"

#include <QHash>

/**
 * \short this object represents user defined histogram mapping and correction data
 *
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class LIBQMESYDAQ_EXPORT UserMapCorrection : public MapCorrection
{
public:
	//! default constructor
	UserMapCorrection()
		: MapCorrection()
	{
	}

	/**
	 * constructor
	 *
	 * \param fName file name
	 */
	UserMapCorrection(const QString &fName);

	/**
	 * loads a correction file
         * Depending on the extensions it tries to use different types of reading
	 * If the extension is mcal it uses the loadCalFile otherwise the loadLUTFile
	 *
	 * \param fName file name
	 * \return true if the reading was successful otherwise false
	 */
	bool loadCorrectionFile(const QString &fName);

private:
	bool loadCalFile(const QString &fName);

	bool loadLUTFile(const QString &fName);

private:
	TubeRange		m_detector;

	QHash<int, TubeRange>	m_tube;
};

#endif 
