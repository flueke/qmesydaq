/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009 by Jens Krüger <jens.krueger@frm2.tum.de>          *
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
#include "mpsd8.h"
#include "mstd16.h"
#include "mdll.h"

/*!
    \fn MPSD8 *MPSD8::create(int bus, int typ, QObject *parent)

    factory to create a module

    \param bus
    \param typ
    \param parent
  
    \return pointer to new created module
 */   
MPSD8 *MPSD8::create(int bus, int typ, QObject *parent)
{
	switch (typ)
	{
		case TYPE_MPSD8P :
			return new MPSD8plus(bus, parent);
		case TYPE_MPSD8OLD:
			return new MPSD8old(bus, parent);
		case TYPE_MPSD8SADC:
			return new MPSD8SingleADC(bus, parent);
		case TYPE_MPSD8 :
			return new MPSD8(bus, parent);
		case TYPE_MSTD16 :
			return new MSTD16(bus, parent);
		case TYPE_MDLL :
			return new MDLL(bus, parent);
		default :
			return new NoModule(bus, parent);
	}
}
