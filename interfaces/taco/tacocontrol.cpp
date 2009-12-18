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
#include <QCoreApplication>
#include <QDebug>

#include "tacocontrol.h"
#include "tacothread.h"

#include <sys/types.h>
#include <signal.h>

TACOControl::TACOControl(QObject *parent)
	: ControlInterface(parent)
	, m_tt(NULL)
{
	m_tt = new TACOThread(this);
	m_tt->start();
}

TACOControl::~TACOControl()
{
	if (m_tt && m_tt->isRunning())
	{
		kill(QCoreApplication::applicationPid(), SIGTERM);
		m_tt->terminate();
		m_tt->wait();
	}
}

void TACOControl::setHistogramFileName(std::string name) 
{
	m_histFileName = name.c_str();
}

void TACOControl::setListFileName(std::string name) 
{
	m_listFileName = name.c_str();
}
