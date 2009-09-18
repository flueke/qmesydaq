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

#include "mainwindow.h"
#include "mainwidget.h"
#include "mesydaq2.h"

Mesydaq2MainWindow::Mesydaq2MainWindow()
	: QMainWindow( 0 )
	, Ui_Mesydaq2MainWindow()
{
	setupUi(this);

	QString ip[] = {"192.168.168.121", "192.168.169.121", };	
	quint16 port[] = {54321, 54321, };
	QString sourceIP[] = {"192.168.168.1", "192.168.169.1", };

	Mesydaq2 *mesy = new Mesydaq2(this);

	for (int i = 0; i < 2; i++)
		mesy->addMCPD(i, ip[i], port[i], sourceIP[i]);

	m_main = new MainWidget(mesy, this);
	setCentralWidget(m_main);
	connect(action_Load_Config_File, SIGNAL(activated()), m_main, SLOT(restoreSetupSlot()));
	connect(action_Save_Config_File, SIGNAL(activated()), m_main, SLOT(saveSetupSlot()));
	connect(action_Replay_List_File, SIGNAL(activated()), m_main, SLOT(replayListfileSlot()));
	connect(actionSave_Histogram_File, SIGNAL(activated()), m_main, SLOT(writeHistSlot()));
	m_main->resize(1280, 980);
}

Mesydaq2MainWindow::~Mesydaq2MainWindow()
{
}
