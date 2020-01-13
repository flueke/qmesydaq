/***************************************************************************
 *   Copyright (C) 2013-2020 by Lutz Rossa <rossa@helmholtz-berlin.de>,    *
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

#include "mappeddetector.h"

MappedDetector::MappedDetector()
{
	clr();
}

MappedDetector::MappedDetector(int iStartInput, int iEndInput, int iStartOutput, int iEndOutput, float fFactor)
{
	set(iStartInput, iEndInput, iStartOutput, iEndOutput, fFactor);
}

MappedDetector::MappedDetector(const MappedDetector &src)
{
	set(src);
}

int MappedDetector::getStartInput() const
{
	return m_iStartInput;
}

int MappedDetector::getEndInput() const
{
	return m_iEndInput;
}

int MappedDetector::getStartOutput() const
{
	return m_iStartOutput;
}

int MappedDetector::getEndOutput() const
{
	return m_iEndOutput;
}

float MappedDetector::getFactor() const
{
	return m_fFactor;
}

void MappedDetector::clr()
{
	set(-1, -1, -1, -1, 0.0);
}

void MappedDetector::operator=(const MappedDetector &src)
{
	set(src);
}

void MappedDetector::set(const MappedDetector &src)
{
	set(src.m_iStartInput, src.m_iEndInput, src.m_iStartOutput, src.m_iEndOutput, src.m_fFactor);
}

void MappedDetector::set(int iStartInput, int iEndInput, int iStartOutput, int iEndOutput, float fFactor)
{
	m_iStartInput  = iStartInput  >= 0 ? iStartInput  : -1;
	m_iEndInput    = iEndInput    >= 0 ? iEndInput    : -1;
	m_iStartOutput = iStartOutput >= 0 ? iStartOutput : -1;
	m_iEndOutput   = iEndOutput   >= 0 ? iEndOutput   : -1;
	m_fFactor      = fFactor     > 0.0 ? fFactor      : 0.0;
}

ListedMappedDetector::ListedMappedDetector()
	: MappedDetector()
	, m_iChannelNr(0)
{
}

ListedMappedDetector::ListedMappedDetector(const MappedDetector &src, int iChannelNr)
	: MappedDetector(src)
	, m_iChannelNr(iChannelNr)
{
}

int ListedMappedDetector::getChannelNumber() const
{
	return m_iChannelNr;
}

void ListedMappedDetector::setChannelNumber(int nr)
{
	m_iChannelNr=nr;
}
