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
#include "MultipleLoopApplication.h"
#include "mainwindow.h"
#include "mainwidget.h"
#include "mesydaq2.h"

Mesydaq2MainWindow::Mesydaq2MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, Ui_Mesydaq2MainWindow()
{
	setupUi(this);

	m_mesy = new Mesydaq2(NULL);
	m_main = new MainWidget(m_mesy, this);
	setCentralWidget(m_main);

        MultipleLoopApplication *app = dynamic_cast<MultipleLoopApplication*>(QApplication::instance());
        if(app)
            app->setLoopEventReceiver(m_main);

	connect(action_Load_Config_File, SIGNAL(triggered()), m_main, SLOT(restoreSetupSlot()));
	connect(action_Save_Config_File, SIGNAL(triggered()), m_main, SLOT(saveSetupSlot()));
	connect(action_Replay_List_File, SIGNAL(triggered()), m_main, SLOT(replayListfileSlot()));
	connect(actionSave_Histogram_File, SIGNAL(triggered()), m_main, SLOT(writeHistSlot()));
	connect(actionLoad_Histogram_File, SIGNAL(triggered()), m_main, SLOT(loadHistSlot()));
	connect(actionPrint, SIGNAL(triggered()), m_main, SLOT(printPlot()));
	connect(actionExport_PDF, SIGNAL(triggered()), m_main, SLOT(exportPDF()));
	connect(actionExport_SVG, SIGNAL(triggered()), m_main, SLOT(exportSVG()));
	connect(actionGeneral, SIGNAL(triggered()), m_main, SLOT(setupGeneral()));
	connect(actionModule, SIGNAL(triggered()), m_main, SLOT(setupModule()));
	connect(actionNewSetup, SIGNAL(triggered()), m_main, SLOT(newSetupSlot()));
	connect(actionSetupMCPD, SIGNAL(triggered()), m_main, SLOT(setupMCPD()));
	connect(actionAddMCPD, SIGNAL(triggered()), m_main, SLOT(addMCPD()));
	connect(action_About, SIGNAL(triggered()), m_main, SLOT(about()));
	connect(actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	connect(m_main, SIGNAL(started(bool)), action_Replay_List_File, SLOT(setDisabled(bool)));
	restoreSettings();
}

Mesydaq2MainWindow::~Mesydaq2MainWindow()
{
	saveSettings();
	delete m_mesy;
}

void Mesydaq2MainWindow::restoreSettings()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "MesyTec", "QMesyDAQ");
	QPoint pos = settings.value("pos", QPoint(200, 0)).toPoint();
	QSize size = settings.value("size", QSize(1280, 980)).toSize();
	m_main->resize(size);
	move(pos);
}

void Mesydaq2MainWindow::saveSettings()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "MesyTec", "QMesyDAQ");
	settings.setValue("pos", pos());
	settings.setValue("size", m_main->size());
	settings.setValue("config/lastrunid", m_mesy->runId());
}
