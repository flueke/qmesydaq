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

#ifndef MAPPEDDETECTOR_H
#define MAPPEDDETECTOR_H

/**
	\short Data of mapped detector tube without channel

	\author Eric Faustmann <eric.faustmann@helmholtz-berlin.de>
	\author Damian Rhein <damian.rhein@helmholtz-berlin.de>
	\author Lutz Rossa <rossa@helmholtz-berlin.de>
*/
class MappedDetector
{
public:
	MappedDetector() { clr(); }
	MappedDetector(int iStartInput, int iEndInput, int iStartOutput, int iEndOutput, float fFactor)
		{ set(iStartInput, iEndInput, iStartOutput, iEndOutput, fFactor); }
	MappedDetector(const MappedDetector &src) { set(src); }

	inline int   getStartInput()  const { return m_iStartInput; }
	inline int   getEndInput()    const { return m_iEndInput; }
	inline int   getStartOutput() const { return m_iStartOutput; }
	inline int   getEndOutput()   const { return m_iEndOutput; }
	inline float getFactor()      const { return m_fFactor; }

	inline void clr() { set(-1, -1, -1, -1, 0.0); }
	inline void operator=(const MappedDetector &src) { set(src); }

	inline void set(const MappedDetector &src)
		{ set(src.m_iStartInput, src.m_iEndInput, src.m_iStartOutput, src.m_iEndOutput, src.m_fFactor); }

	void set(int iStartInput, int iEndInput, int iStartOutput, int iEndOutput, float fFactor);

private:
	int   m_iStartInput;
	int   m_iEndInput;
	int   m_iStartOutput;
	int   m_iEndOutput;
	float m_fFactor;
};

/**
	\short a single detector tube (map data and channel)

	\author Eric Faustmann <eric.faustmann@helmholtz-berlin.de>, Damian Rhein <damian.rhein@helmholtz-berlin.de>, Lutz Rossa <rossa@helmholtz-berlin.de>
*/
class ListedMappedDetector : public MappedDetector
{
public:
	ListedMappedDetector() : MappedDetector(), m_iChannelNr(0) {}
	ListedMappedDetector(const MappedDetector &src, int iChannelNr)
		: MappedDetector(src), m_iChannelNr(iChannelNr) {}

	int getChannelNumber() const { return m_iChannelNr; }

	void setChannelNumber(int nr) { m_iChannelNr=nr; }

private:
	int m_iChannelNr;
};

#endif // MAPPEDDETECTOR_H
