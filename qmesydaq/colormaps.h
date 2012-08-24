/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2012 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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

#ifndef _COLORMAPS_H_
#define _COLORMAPS_H_

#include <qwt_color_map.h>

/**
 * \short base class for all color maps used in qmesydaq
 *
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class MesydaqColorMap : public QwtLinearColorMap
{
public:
	//! constructor
	MesydaqColorMap();

	/**
	 * Map a value into a color
	 *
	 * \param interval Valid interval for values
	 * \param value	Value
	 *
	 * \return Color corresponding to value
	 * 
	 * \warning This method is slow for Indexed color maps. If it is necessary 
	 * 	    to map many values, its better to get the color table once and 
	 *	    find the color using colorIndex(). 
	 */
	virtual QColor color(const QwtDoubleInterval &, double value) const; 

	/**
	 * Map a value of a given interval into a rgb value
	 *
	 * \param interval Range for all values
	 * \param value Value to map into a rgb value 
	 *
	 * \return Color corresponding to value
	 */
	virtual QRgb rgb(const QwtDoubleInterval &, double value) const;

	//! Clone the color map. 
	virtual QwtColorMap *copy () const;

	//! sets the colormap to linear scaling mode
	void setLinearScaling(void);

	//! sets the colormap to logarithmic scaling mode
	void setLogarithmicScaling(void);

private:
	bool m_log;
};

/**
 * \short The standard color map for scaling 
 *
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class StdColorMap : public MesydaqColorMap
{
public:
	//! constructor
	StdColorMap();
};

/**
 * \short The 'Jet' color map for scaling. 
 *
 * The colors are taken from the matplotlib 'jet' colormap
 *
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class JetColorMap : public MesydaqColorMap
{
public:
	//! constructor
	JetColorMap();
};

#endif
