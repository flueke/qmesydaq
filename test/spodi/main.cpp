/*
   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>
   Copyright (C) 2009-2020 by Jens Krüger <jens.krueger@frm2.tum.de>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the
   Free Software Foundation, Inc.,
   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "qmlogging.h"
#include "histogram.h"

Histogram &generateData(const quint16 slice)
{
	static Histogram tmp;
	tmp.resize(80, 256);

	for (quint16 i = 0; i < tmp.width(); ++i)
	{
		tmp.setValue(i, 0, slice);
		for (quint16 j = 1; j < tmp.height(); ++j)
			tmp.setValue(i, j, 0);
	}
	return tmp;
}

int main(int, char **)
{
	quint16 resosteps(40);

	Histogram fullHistogram;

	fullHistogram.resize(80 * resosteps, 256);

	for (quint16 i = 0; i < resosteps; ++i)
		fullHistogram.addSlice(i, resosteps, generateData(i));

	for (quint16 i = 0; i < fullHistogram.width(); ++i)
		MSG_ERROR << fullHistogram.value(i, 0);

//	MSG_ERROR << fullHistogram;
	return 0;
}
