/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2013 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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
 * The colors are taken from the <a href="http://matplotlib.sourceforge.net">matplotlib</a> 'jet' colormap
 *
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class JetColorMap : public MesydaqColorMap
{
public:
	//! constructor
	JetColorMap();
};

/**
 * \short The 'Hot' color map for scaling. 
 *
 * The colors are taken from the <a href="http://matplotlib.sourceforge.net">matplotlib</a> 'hot' colormap
 *
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class HotColorMap : public MesydaqColorMap
{
public:
	//! constructor
	HotColorMap();
};

/**
 * \short The 'Hsv' color map for scaling. 
 *
 * The colors are taken from the <a href="http://matplotlib.sourceforge.net">matplotlib</a> 'hsv' colormap
 *
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class HsvColorMap : public MesydaqColorMap
{
public:
	//! constructor
	HsvColorMap();
};

/**
 * \short The 'Spring' color map for scaling. 
 *
 * The colors are taken from the <a href="http://matplotlib.sourceforge.net">matplotlib</a> 'spring' colormap
 *
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class SpringColorMap : public MesydaqColorMap
{
public:
	//! constructor
	SpringColorMap();
};

/**
 * \short The 'Summer' color map for scaling. 
 *
 * The colors are taken from the <a href="http://matplotlib.sourceforge.net">matplotlib</a> 'summer' colormap
 *
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class SummerColorMap : public MesydaqColorMap
{
public:
	//! constructor
	SummerColorMap();
};

/**
 * \short The 'Autumn' color map for scaling. 
 *
 * The colors are taken from the <a href="http://matplotlib.sourceforge.net">matplotlib</a> 'autumn' colormap
 *
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class AutumnColorMap : public MesydaqColorMap
{
public:
	//! constructor
	AutumnColorMap();
};

/**
 * \short The 'Winter' color map for scaling. 
 *
 * The colors are taken from the <a href="http://matplotlib.sourceforge.net">matplotlib</a> 'winter' colormap
 *
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class WinterColorMap : public MesydaqColorMap
{
public:
	//! constructor
	WinterColorMap();
};

/**
 * \short The 'Bone' color map for scaling. 
 *
 * The colors are taken from the <a href="http://matplotlib.sourceforge.net">matplotlib</a> 'bone' colormap
 *
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class BoneColorMap : public MesydaqColorMap
{
public:
	//! constructor
	BoneColorMap();
};

/**
 * \short The 'Cool' color map for scaling. 
 *
 * The colors are taken from the <a href="http://matplotlib.sourceforge.net">matplotlib</a> 'cool' colormap
 *
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class CoolColorMap : public MesydaqColorMap
{
public:
	//! constructor
	CoolColorMap();
};

/**
 * \short The 'Copper' color map for scaling. 
 *
 * The colors are taken from the <a href="http://matplotlib.sourceforge.net">matplotlib</a> 'copper' colormap
 *
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class CopperColorMap : public MesydaqColorMap
{
public:
	//! constructor
	CopperColorMap();
};

/**
 * \short The 'Gray' color map for scaling. 
 *
 * The colors are taken from the <a href="http://matplotlib.sourceforge.net">matplotlib</a> 'gray' colormap
 *
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class GrayColorMap : public MesydaqColorMap
{
public:
	//! constructor
	GrayColorMap();
};

/**
 * \short The 'Spectral' color map for scaling. 
 *
 * The colors are taken from the <a href="http://matplotlib.sourceforge.net">matplotlib</a> 'spectral' colormap
 *
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class SpectralColorMap : public MesydaqColorMap
{
public:
	//! constructor
	SpectralColorMap();
};

#endif

