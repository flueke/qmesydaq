/***************************************************************************
 *   Copyright (C) 2013-2014 by Lutz Rossa <rossa@helmholtz-berlin.de>,    *
 *                    Eric Faustmann <eric.faustmann@helmholtz-berlin.de>, *
 *                    Damian Rhein <damian.rhein@helmholtz-berlin.de>      *
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

#ifndef EDITORMEMORY_H
#define EDITORMEMORY_H
#include "mapcorrect.h"
#include "mappeddetector.h"

/**
	\short Mapping editor memory

	\author Eric Faustmann <eric.faustmann@helmholtz-berlin.de>, Damian Rhein <damian.rhein@helmholtz-berlin.de>, Lutz Rossa <rossa@helmholtz-berlin.de>
*/
class LIBQMESYDAQ_EXPORT EditorMemory
{
public:
	//! constructor
	EditorMemory(QWidget *pParent, int iDefaultCount = 0);

	//! special copy constructor
	EditorMemory(QWidget *pParent, EditorMemory *pSrc);

	//! copy operator
	EditorMemory& operator=(const EditorMemory& src);

	//! returns the number of mapped channels
	int getChannelCount() const { return m_aTubes.size(); }

	//! returns the mapped channel data
	QVector<MappedDetector>& getTubes() { return m_aTubes; }

	//! returns the mapped channel read only data
	const QVector<MappedDetector>& getTubes() const { return m_aTubes; }

	//! returns the configuration file path
	QString getPath() const { return m_sPath; }

	//! sets the configuration file path
	void setPath(QString sPath) { m_sPath=sPath; }

	//! returns the orientation of the histogram
	enum MapCorrection::Orientation getOrientation(void) const { return m_iOrientation; }

	//! sets the orientation of the histogram
	void setOrientation(MapCorrection::Orientation iOrientation) { m_iOrientation = iOrientation; }

	//! resizes the number of mapped channels
	void tubesResize(int iNumTubes) { m_aTubes.resize(iNumTubes); }

	//! reads the "changed" flag
	bool changed() const { return m_bChanged; }

	//! set the "changed" flag
	void setChanged(bool bChanged = true) { m_bChanged = bChanged; }

	//! save to previously defined file
	void save();

	//! save to specified file
	bool saveToPath(QString sPath);

	//! load from previously defined file
	bool load();

	//! load from specified file
	bool loadFromPath(QString sPath);

	//! checks, if content has really changed
	bool needsToSave(int iDefaultCount = 0) const;

	//! checks, if the file was modified and asks the user to save to a file
	void wantToSave(QWidget *pParent, int iDefaultCount = 0);

protected:
	QWidget                         *m_pParent;     //! parent widget
	bool                            m_bChanged;     //! changed flag
	QVector<MappedDetector>         m_aTubes;       //! mapped channel data
	enum MapCorrection::Orientation m_iOrientation; //! orientation of histogram
	QString                         m_sPath;        //! file name

	//! return file name with correct file extension
	static QString toPath(QString sFilename);

	//! checks, if this data is different to the specified
	bool isDifferent(EditorMemory *pOtherEditorMem, QString sOtherPath) const;

	//! take the first number from string list as integer
	static void getNextNumber(QStringList &asParts, int &iNumber);

	//! take the first number from string list as float
	static void getNextNumber(QStringList &asParts, float &fNumber);
};

#endif // EDITORMEMORY_H
