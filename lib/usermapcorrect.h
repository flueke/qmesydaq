/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2020 by Jens Krüger <jens.krueger@frm2.tum.de>     *
 *   Copyright (C) 2011-2020 by Lutz Rossa <rossa@helmholtz-berlin.de>     *
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
#include "libqmesydaq_export.h"

#include <QHash>

class LIBQMESYDAQ_EXPORT EditorMemory;

/**
 * \short this object represents user defined histogram mapping and correction data
 *
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class LIBQMESYDAQ_EXPORT UserMapCorrection : public MapCorrection
{
	//! unaccessible default constructor
	UserMapCorrection();

public:
	/**
	 * constructor
	 *
	 * \param size
	 * \param fName file name
	 */
	UserMapCorrection(const QSize &size, const QString &fName);
	/*!
		constructor

		\param size
		\param iOrientation
		\param iCorrection
	 */
	UserMapCorrection(const QSize &size, enum Orientation iOrientation, enum CorrectionType iCorrection);

	//! \brief destructor
	virtual ~UserMapCorrection();

	/**
	 * loads a correction file
	 * Depending on the extensions it tries to use different types of reading
	 * extensions: mcal -> loadCalFile, mesf -> loadMESFFile, txt -> loadLUTFile
	 *
	 * \param fName file name
	 * \return true if the reading was successful otherwise false
	 */
	bool loadCorrectionFile(const QString &fName);

	/**
	 * save a correction file
	 * implemented for MESF files only
	 * \param fName filename
	 * \return true if the writing was successful
	 */
	bool saveCorrectionFile(QString fName);

	//! \brief get access to histogram mapping editor data
	EditorMemory* getMESFData() const { return m_pEditorMemory; }

	//! \brief set new histogram mapping editor data pointer
	void setMESFData(EditorMemory *pEM);

	//! \brief set mapping from histogram mapping editor
	bool setMapCorrection(EditorMemory &EM);

private:
	bool loadCalFile(const QString &fName);

	bool loadLUTFile(const QString &fName);

	bool loadMESFFile(const QString &fName);

private:
	TubeRange		m_detector;

	QHash<int, TubeRange>	m_tube;

	EditorMemory*	m_pEditorMemory;
};

#endif
