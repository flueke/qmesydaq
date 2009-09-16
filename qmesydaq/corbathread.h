/***************************************************************************
 *   Copyright (C) 2002 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2008 by Lutz Rossa <rossa@hmi.de>                       *
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
#ifndef CORBATHREAD_H
#define CORBATHREAD_H

#include <QThread>

#define OK 0

const char FS_NONE = 0;
const char FS_X = 1;
const char FS_Y = 2;
const char FS_XY = 3;

class CORBADevice_i;
class CARESSControl;

/** implementation of corba server threads
  * \author Gregor Montermann
  * CORBA / CARESS modules by Lutz Rossa, Helmholtz Zentrum Berlin
  */

class CorbaThread : public QThread  
{
public: 
	//! Constructor
	CorbaThread(CARESSControl *pcInt);
	
	//! Destructor
	~CorbaThread();

	//! No descriptions
	virtual void run();

private:
	//! No descriptions
	bool initialize(CARESSControl *pcInt);

	//! No descriptions 
	bool asyncCmd(void);

protected:
	CARESSControl 	*m_pInt;

	//! implementation of generic CORBA device for CARESS 
	CORBADevice_i 	*m_pCORBADevice;

	//! size of desired histogram: 0=64*64, 1 = 128*128 
//	quint8 		m_fullsize;

	//! 
	quint8 		m_xsize;

	//!
	quint8 		m_ysize;

	//!
	qint32 		m_retval;

	//!
	bool 		m_terminate;
};

#endif
