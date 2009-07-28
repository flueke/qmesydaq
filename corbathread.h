 /***************************************************************************
                        corbathread.cpp  -  description
                        -------------------
    begin              : Fri Nov 22 2002
    last change        : Wed Mar 26 2008
    copyright          : (C) 2002 by Gregor Montermann
                             g.montermann@mesytec.com, mesytec GmbH
                         (C) 2008 by Lutz Rossa
                             rossa@hmi.de, Hahn-Meitner-Institut Berlin GmbH
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CORBATHREAD_H
#define CORBATHREAD_H

#include <QThread>
// #include <unistd.h>
// #include <cstdio>


#define OK 0

const char FS_NONE = 0;
const char FS_X = 1;
const char FS_Y = 2;
const char FS_XY = 3;

class MainWidget;
class CORBADevice_i;
class ControlInterface;

/** implementation of corba server threads
  *@author Gregor Montermann
  * CORBA / CARESS modules by Lutz Rossa, Helmholtz Zentrum Berlin
  */

class CorbaThread : public QThread  
{
public: 
	CorbaThread();
	~CorbaThread();
  /** No descriptions */
	virtual void run();
  /** No descriptions */
	bool initializeCorba(MainWidget* App, ControlInterface* pcInt);
  /** stops and closes thread */
	void bye();
  /** No descriptions */
	bool asyncCmd(void);

protected:
	MainWidget 		*m_pApp;

	ControlInterface 	*m_pInt;

  /** implementation of generic CORBA device for CARESS */
	CORBADevice_i 		*m_pCORBADevice;

  /** size of desired histogram: 0=64*64, 1 = 128*128 */
//	quint8 			m_fullsize;
  /**  */
	quint8 			m_xsize;
  /**  */
	quint8 			ysize;
  /**  */
	qint32 			ret_val;
  /**  */
	bool 			terminate;
};

#endif
