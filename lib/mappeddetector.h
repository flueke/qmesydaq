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

#ifndef MAPPEDDETECTOR_H
#define MAPPEDDETECTOR_H

#include "libqmesydaq_global.h"

/**
	\short Data of mapped detector tube without channel

	\author Eric Faustmann <eric.faustmann@helmholtz-berlin.de>
	\author Damian Rhein <damian.rhein@helmholtz-berlin.de>
	\author Lutz Rossa <rossa@helmholtz-berlin.de>
*/
class LIBQMESYDAQ_EXPORT MappedDetector
{
public:
	MappedDetector();

	MappedDetector(int iStartInput, int iEndInput, int iStartOutput, int iEndOutput, float fFactor);

	MappedDetector(const MappedDetector &src);

	int   getStartInput()  const;

	int   getEndInput()    const;

	int   getStartOutput() const;

	int   getEndOutput()   const;

	float getFactor()      const;

	void clr();

	void operator=(const MappedDetector &src);

	void set(const MappedDetector &src);

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

	\author Eric Faustmann <eric.faustmann@helmholtz-berlin.de>
        \author	Damian Rhein <damian.rhein@helmholtz-berlin.de>
        \author	Lutz Rossa <rossa@helmholtz-berlin.de>
*/
class LIBQMESYDAQ_EXPORT ListedMappedDetector : public MappedDetector
{
public:
	ListedMappedDetector();

	ListedMappedDetector(const MappedDetector &src, int iChannelNr);

	int getChannelNumber() const;

	void setChannelNumber(int nr);

private:
	int m_iChannelNr;
};

#endif // MAPPEDDETECTOR_H
