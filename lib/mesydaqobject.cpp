/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009 by Jens Kr�ger <jens.krueger@frm2.tum.de>          *
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

#include "mdefines.h"
#include "mesydaqobject.h"

#include <QDateTime>
#include <QThread>

int DEBUGLEVEL = NOTICE;

QMutex MesydaqObject::m_Mutex;

MesydaqObject::MesydaqObject(QObject *parent)
	: QObject(parent)
{
}

MesydaqObject::~MesydaqObject()
{
}

/*!
    \fn MesydaqObject::protocol(QString str, quint8 level)
   
    puts out a log message if the level value is lower than the global value %DEBUGLEVEL%
    
    logging levels are:
	- FATAL = 0
        - ERROR    
        - WARNING  
        - NOTICE  
        - INFO   
        - DEBUG 

    \param str message to be put out
    \param level logging level 
 */
void MesydaqObject::protocol(QString str, quint8 level)
{
	if(level <= DEBUGLEVEL)
	{
		m_Mutex.lock();
		QString datestring = QDateTime::currentDateTime().toString("yyyy/dd/MM hh:mm:ss.zzz");
		m_Mutex.unlock();

		str.prepend(" - ");
		str.prepend(datestring);
		qDebug("[%d] %s", level, str.toStdString().c_str());
	}
}
