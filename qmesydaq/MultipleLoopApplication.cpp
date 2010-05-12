/***************************************************************************
 *   Copyright (C) 2002 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2008 by Lutz Rossa <rossa@hmi.de>                       *
 *   Copyright (C) 2009-2010 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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

#include "MultipleLoopApplication.h"

#include "QtInterface.h"
#include "LoopObject.h"

#include <iostream>

MultipleLoopApplication::MultipleLoopApplication(int argc, char**argv, LoopObject *loop)
	: QApplication(argc, argv)
	, m_loop(loop)
	, m_interface(NULL)
{
	setAttribute(Qt::AA_ImmediateWidgetCreation);
}

void MultipleLoopApplication::setLoopEventReceiver(QObject *receiver)
{
	if (m_interface)
		m_interface->setReceiver(receiver);
}

QObject *MultipleLoopApplication::getLoopEventReceiver()
{
	return m_interface ? m_interface->getReceiver() : NULL;
}

void MultipleLoopApplication::setLoopObject(LoopObject *loop)
{
	if(m_loop)
		m_loop->terminate();
	m_loop = loop;
}

LoopObject *MultipleLoopApplication::getLoopObject()
{
	return m_loop;
}

int MultipleLoopApplication::exec()
{
	if (m_loop)
		m_loop->start();
	return QApplication::exec();
}

void MultipleLoopApplication::setQtInterface(QtInterface *interface)
{
	m_interface = interface;
}

QtInterface *MultipleLoopApplication::getQtInterface()
{
	return m_interface;
}

