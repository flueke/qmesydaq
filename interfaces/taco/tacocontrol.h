/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2010 by Jens Kr�ger <jens.krueger@frm2.tum.de>     *
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
#ifndef TACOCONTROL_H
#define TACOCONTROL_H

#include "controlinterface.h"
#include <string>

class TACOThread;

/**
 * Interface class for external control via TACO
 *
 * \author Jens Kr�ger <jens.kruger@frm2.tum.de>
*/

class TACOControl : public ControlInterface
{
	Q_OBJECT
public:
	TACOControl(QObject *parent = NULL);

	~TACOControl();

	QString getListFileName(void) {return m_listFileName;}

	void setListFileName(std::string name); 

	QString getHistogramFileName(void) {return m_histFileName;}

	void setHistogramFileName(std::string name);
protected:

private:
	TACOThread	*m_tt;

	QString		m_listFileName;

	QString		m_histFileName;
};
#endif

