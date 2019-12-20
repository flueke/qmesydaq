/***************************************************************************
 *   Copyright (C) 2013-2019 by Lutz Rossa <rossa@helmholtz-berlin.de>,    *
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

#include <QFileDialog>
#include <QFile>
#include <QDebug>
#include <QMessageBox>
#include <QDateTime>
#include "editormemory.h"
#include "mappeddetector.h"

EditorMemory::EditorMemory(QWidget *pParent, int iDefaultCount)
	: m_pParent(pParent), m_bChanged(false), m_iOrientation(MapCorrection::OrientationUp)
{
	if (iDefaultCount > 0)
		m_aTubes.resize(iDefaultCount);
}

EditorMemory::EditorMemory(QWidget *pParent, EditorMemory *pSrc)
	: m_pParent(pParent), m_bChanged(false), m_iOrientation(MapCorrection::OrientationUp)
{
	if (pSrc != NULL)
		*this = *pSrc;
	m_bChanged = false;
}

EditorMemory& EditorMemory::operator =(const EditorMemory &src)
{
	if (this != &src)
	{
		m_bChanged = true;
		m_aTubes = src.m_aTubes;
		m_iOrientation = src.m_iOrientation;
		m_sPath = src.m_sPath;
	}
	return *this;
}

void EditorMemory::save()
{
	saveToPath(toPath(QFileDialog::getSaveFileName(m_pParent, QString("Save"), QString(""), QString("Mesydaq mapping editor file(*.mesf)"))));
}

bool EditorMemory::load()
{
	return loadFromPath(toPath(QFileDialog::getOpenFileName(m_pParent, QString("Load"), QString(""), QString("Mesydaq mapping editor file(*.mesf)"))));
}

bool EditorMemory::saveToPath(const QString &sPath)
{
	setPath(sPath);

	QFile file(sPath);
	file.open(QFile::WriteOnly);
	if (!file.isOpen())
		return false;

	QTextStream out(&file);
	out << "# configuration for histogram mapping editor\r\n";
	out << "# saved " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\r\n";
	out << "\r\n";
	out << "# orientation (up=" << MapCorrection::OrientationUp
		<< ", down=" << MapCorrection::OrientationDown
		<< ", left=" << MapCorrection::OrientationLeft
		<< ", right=" << MapCorrection::OrientationRight
		<< ", reverse+" << ((int)MapCorrection::OrientationUpRev) - ((int)MapCorrection::OrientationUp)
		<< ")\r\n";
	out << (int)getOrientation() << "\r\n";
	out << "\r\n";
	out << "# mapping data for " << m_aTubes.size() << " tubes (start in, end in, start out, end out, factor)\r\n";
	for (int i = 0; i < m_aTubes.size(); ++i)
	{
		MappedDetector &md = m_aTubes[i];
		out << md.getStartInput() << '\t' << md.getEndInput() << '\t'
			<< md.getStartOutput() << '\t' << md.getEndOutput() << '\t'
			<< md.getFactor() << "\r\n";
	}

	file.close();
	m_bChanged = false;
	return true;
}

bool EditorMemory::loadFromPath(QString sPath)
{
	if (sPath.isEmpty())
		return false;

	QFile file(sPath);
	if (file.exists())
	{
		setPath(sPath);
		file.open(QFile::ReadOnly);

		QTextStream in(&file);
		m_aTubes.clear();
		bool bOrientationLine(true);
		while (!in.atEnd())
		{
			QString inS = in.readLine();
			inS.remove(QRegExp("^[ \t]+"));
			if (inS.startsWith('#') || inS.startsWith(';') ||
				inS.startsWith("//") || inS.isEmpty())
				continue; // ignore comments and empty lines

			if (bOrientationLine)
			{
				bOrientationLine = false;
				m_iOrientation = (MapCorrection::Orientation)inS.toInt();
			}
			else
			{
				QStringList asParts(inS.split(QRegExp("[ \t#;]+"), QString::SkipEmptyParts));
				int iStartInput, iEndInput, iStartOutput, iEndOutput;
				float fFactor;

				getNextNumber(asParts, iStartInput);
				getNextNumber(asParts, iEndInput);
				getNextNumber(asParts, iStartOutput);
				getNextNumber(asParts, iEndOutput);
				getNextNumber(asParts, fFactor);

				m_aTubes.append(MappedDetector(iStartInput, iEndInput, iStartOutput, iEndOutput, fFactor));
			}
		}

		file.close();
		m_bChanged = false;
		return true;
	}
	return false;
}

bool EditorMemory::isDifferent(EditorMemory *pOtherEditorMem, QString sOtherPath) const
{
	pOtherEditorMem->loadFromPath(sOtherPath);

	if (pOtherEditorMem->getChannelCount() != getChannelCount() ||
		pOtherEditorMem->getOrientation() != getOrientation())
		return true;

	for (int i = 0; i < getChannelCount(); ++i)
	{
		const MappedDetector &md1 = pOtherEditorMem->getTubes()[i];
		const MappedDetector &md2 = getTubes()[i];
		if (md1.getEndInput()    != md2.getEndInput()    ||
			md1.getEndOutput()   != md2.getEndOutput()   ||
			md1.getStartInput()  != md2.getStartInput()  ||
			md1.getStartOutput() != md2.getStartOutput() ||
			md1.getFactor()      != md2.getFactor())
			return true;
	}
	return false;
}

bool EditorMemory::needsToSave(int iDefaultCount) const
{
	if (!m_bChanged)
		return false;
	EditorMemory *pOther = new EditorMemory(m_pParent);
	EditorMemory *pEmpty = new EditorMemory(m_pParent, iDefaultCount);
	bool bResult(isDifferent(pOther, getPath()) && isDifferent(pEmpty, QString::null));
	delete pOther;
	delete pEmpty;
	return bResult;
}

void EditorMemory::wantToSave(QWidget *pParent, int iDefaultCount)
{
	if (needsToSave(iDefaultCount))
		if (QMessageBox::Yes == QMessageBox::question(pParent, "Save Histogramm Editor",
				"The Histogramm Editor has some unsaved changes.\r\nDo you want to save them?",
				QMessageBox::Yes, QMessageBox::No))
			save();
}

QString EditorMemory::toPath(QString sFilename)
{
	if (!sFilename.endsWith(".mesf"))
		sFilename.append(".mesf");
	return sFilename;
}

void EditorMemory::getNextNumber(QStringList &asParts, int &iNumber)
{
	if (asParts.isEmpty())
		return;

	QString sNumber(asParts.takeFirst());
	bool bOK(false);
	if (!sNumber.isEmpty())
		iNumber = sNumber.toInt(&bOK);
	if (!bOK)
		iNumber = -1;
}

void EditorMemory::getNextNumber(QStringList &asParts, float &fNumber)
{
	if (asParts.isEmpty())
		return;

	QString sNumber(asParts.takeFirst());
	bool bOK(false);
	if (!sNumber.isEmpty())
		fNumber = sNumber.toFloat(&bOK);
	if (!bOK || fNumber < 0.0)
		fNumber = 1.0;
}

int EditorMemory::getChannelCount() const
{
	return m_aTubes.size();
}

QVector<MappedDetector>& EditorMemory::getTubes()
{
	return m_aTubes;
}

const QVector<MappedDetector>& EditorMemory::getTubes() const
{
	return m_aTubes;
}

QString EditorMemory::getPath() const
{
	return m_sPath;
}

void EditorMemory::setPath(const QString &sPath)
{
	m_sPath = sPath;
}

enum MapCorrection::Orientation EditorMemory::getOrientation(void) const
{
	return m_iOrientation;
}

void EditorMemory::setOrientation(MapCorrection::Orientation iOrientation)
{
	m_iOrientation = iOrientation;
}

void EditorMemory::tubesResize(int iNumTubes)
{
	m_aTubes.resize(iNumTubes);
}

bool EditorMemory::changed() const
{
	return m_bChanged;
}

void EditorMemory::setChanged(bool bChanged)
{
	m_bChanged = bChanged;
}
