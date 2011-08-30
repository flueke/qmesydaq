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

#ifndef MULTIPLE_LOOP_APPLICATION_H
#define MULTIPLE_LOOP_APPLICATION_H

#include <QApplication>

class LoopObject;
class QtInterface;

/*!
    \class MultipleLoopApplication

    \short This class handles the creation of a remote interface and interaction with the frontend

    \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class MultipleLoopApplication : public QApplication
{
	Q_OBJECT

	//! store the Qt interface
	Q_PROPERTY(QtInterface *m_interface READ getQtInterface WRITE setQtInterface)

        //! store the loop object
	Q_PROPERTY(LoopObject *m_loop READ getLoopObject WRITE setLoopObject)

public:
	MultipleLoopApplication(int &argc, char **argv, LoopObject *loop = 0);

	//! \return pointer to itself
	MultipleLoopApplication *instance() {return this;}

	void setLoopEventReceiver(QObject *receiver);
	QObject *getLoopEventReceiver();

	void setLoopObject(LoopObject *loop);
	LoopObject *getLoopObject();

	void setQtInterface(QtInterface *interface);
	QtInterface *getQtInterface();

        int exec();

private:
	LoopObject 	*m_loop;
	QtInterface 	*m_interface;
};

#endif // MULTIPLELOOPAPPLICATION_H
