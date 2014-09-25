/***************************************************************************
 *   Copyright (C) 2002 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2008-2014 by Lutz Rossa <rossa@hmi.de>                  *
 *   Copyright (C) 2009-2014 by Jens Krüger <jens.krueger@frm2.tum.de>     *
 *   Copyright (C) 2010 by Alexander Lenz <alexander.lenz@frm2.tum.de>     *
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

#include "LoopObject.h"

/*!
    constructor
 */
LoopObject::LoopObject()
{
}

/*!
    \fn void LoopObject::run()

    running thread
 */
void LoopObject::run()
{
	this->runLoop();
}

/*!
    \fn void LoopObject::runLoop()

    does all in the thread
 */
void LoopObject::runLoop()
{
}

/*!
    \fn void LoopObject::pSleep(long ms)

    \param ms
 */
void LoopObject::pSleep(long ms)
{
	this->msleep(ms);
}
