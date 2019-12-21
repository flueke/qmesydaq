/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2016 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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
#include <QCloseEvent>
#include <QStatusBar>
#include "MultipleLoopApplication.h"
#include "mainwindow.h"
#include "mainwidget.h"
#include "passworddialog.h"
#include "mesydaq2.h"
#include "StatusBarEntry.h"
#include "qmlogging.h"
// #include "website.h"
#include "stdafx.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, Ui_MainWindow()
{
	setupUi(this);
#ifdef VERSION
	setWindowTitle("QMesyDAQ " + QString(VERSION) + " " __DATE__);
#else
	setWindowTitle("QMesyDAQ " __DATE__);
#endif
	m_mesy = new Mesydaq2();
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
	connect(actionLoad_Calibration_File, SIGNAL(triggered()), m_main, SLOT(loadCalibrationSlot()));
	connect(actionPrint, SIGNAL(triggered()), m_main, SLOT(printPlot()));
	connect(actionExport_PDF, SIGNAL(triggered()), m_main, SLOT(exportPDF()));
	connect(actionExport_SVG, SIGNAL(triggered()), m_main, SLOT(exportSVG()));
	connect(actionGeneral, SIGNAL(triggered()), m_main, SLOT(setupGeneral()));
	connect(actionModule, SIGNAL(triggered()), m_main, SLOT(setupModule()));
	connect(actionNewSetup, SIGNAL(triggered()), m_main, SLOT(newSetupSlot()));
	connect(actionSetupMCPD, SIGNAL(triggered()), m_main, SLOT(setupMCPD()));
	actionTACO->setVisible(false);
	actionTCP->setVisible(false);
	actionCARESS->setVisible(false);
	actionGeneral->setVisible(false);
	actionHistogram_Mapping->setVisible(false);
	actionNewSetup->setVisible(false);
	action_Save_Config_File->setVisible(false);
	action_Load_Config_File->setVisible(false);
	actionLoad_Calibration_File->setVisible(false);
#if USE_TACO
	connect(actionTACO, SIGNAL(triggered()), m_main, SLOT(setupTACO()));
#elif USE_TCP
	connect(actionTCP, SIGNAL(triggered()), m_main, SLOT(setupTCP()));
#elif USE_CARESS
	connect(actionCARESS, SIGNAL(triggered()), m_main, SLOT(setupCARESS()));
#endif
	connect(actionAddMCPD, SIGNAL(triggered()), m_main, SLOT(addMCPD()));
	connect(actionPulser, SIGNAL(triggered()), m_main, SLOT(toolPulser()));
	connect(action_About, SIGNAL(triggered()), m_main, SLOT(about()));
	connect(actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	connect(actionHistogram_Mapping, SIGNAL(triggered()), m_main, SLOT(editHistogramMapping()));

	connect(m_main, SIGNAL(started(bool)), action_Replay_List_File, SLOT(setDisabled(bool)));
	connect(m_main, SIGNAL(started(bool)), actionLoad_Histogram_File, SLOT(setDisabled(bool)));
	connect(m_main, SIGNAL(started(bool)), actionLoad_Calibration_File, SLOT(setDisabled(bool)));
	connect(m_main, SIGNAL(started(bool)), action_Load_Config_File, SLOT(setDisabled(bool)));
	connect(m_main, SIGNAL(started(bool)), actionNewSetup, SLOT(setDisabled(bool)));
	connect(m_main, SIGNAL(started(bool)), actionSetupMCPD, SLOT(setDisabled(bool)));
	connect(m_main, SIGNAL(started(bool)), actionAddMCPD, SLOT(setDisabled(bool)));
	connect(m_main, SIGNAL(started(bool)), this, SLOT(runningState(bool)));
	connect(m_mesy, SIGNAL(syncLost(quint16, bool)), this, SLOT(lostSync(quint16, bool)));

	m_daqStatus = new StatusBarEntry("DAQ stopped");
	m_pulserStatus = new StatusBarEntry("Pulser off");
	m_mode = new StatusBarEntry("Position Mode");
	m_sync = new StatusBarEntry("Internal");
	m_lostSync = new StatusBarEntry("<font color=\"#FF0000\"><b>sync lost</b></font>");
	m_lostSync->hide();

	statusBar()->addPermanentWidget(m_lostSync);
	statusBar()->addPermanentWidget(m_sync);
	statusBar()->addPermanentWidget(m_daqStatus);
	statusBar()->addPermanentWidget(m_pulserStatus);
	statusBar()->addPermanentWidget(m_mode);

	// m_wsTool = new WebsiteTool("https://forge.frm2.tum.de/redmine/projects/qmesydaq");

	restoreSettings();
}

void MainWindow::updateStatusBar()
{
	QList<int> list = m_mesy->mcpdId();
	MSG_DEBUG << tr("MCPD %1").arg(list.size());
	for (int i = 0; i < list.size(); ++i)
     		if (m_mesy->isExtsynced(list.at(i)))
			m_sync->setText("External");
}

MainWindow::~MainWindow()
{
	delete m_mesy;
}

void MainWindow::restoreSettings()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
	QPoint pos = settings.value("pos", QPoint(100, 0)).toPoint();
	QSize size = settings.value("size", QSize(1024, 768)).toSize();
	quint32 lastrunid = settings.value("config/lastrunid", 0).toUInt();
	m_mesy->setRunId(lastrunid);
	m_mesy->setAutoIncRunId(settings.value("config/autoincrunid", "true").toBool());
	m_mesy->setWriteProtection(settings.value("config/writeprotect", "false").toBool());
	setGeometry(QRect(pos, size));
	switch (settings.value("config/accesslevel", MainWidget::User).toUInt())
	{
		default:
		case MainWidget::User:
			actionUser->setChecked(true);
			selectUser();
			break;
		case MainWidget::Expert:
			actionExpert->setChecked(true);
			selectExpert();
			break;
		case MainWidget::SuperUser:
			actionSuperUser->setChecked(true);
			selectSuperuser();
			break;
	}
}

void MainWindow::saveSettings()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
	settings.setValue("pos", pos());
	QSize s = size();
	QSize fs = frameSize();
	settings.setValue("size", QSize(s.width(), fs.height()));
	settings.setValue("config/lastrunid", m_mesy->runId());
	settings.setValue("config/autoincrunid", m_mesy->getAutoIncRunId());
	settings.setValue("config/writeprotect", m_mesy->getWriteProtection());
	if (actionExpert->isChecked())
		settings.setValue("config/accesslevel", MainWidget::Expert);
	else if (actionSuperUser->isChecked())
		settings.setValue("config/accesslevel", MainWidget::SuperUser);
	else
		settings.setValue("config/accesslevel", MainWidget::User);
}

/*!
    if the the windows closes it will be called

    \param event close event
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
	saveSettings();
        m_main->closeEvent(event);
	event->accept();
}

void MainWindow::selectUser(void)
{
	actionExpert->setChecked(false);
	actionSuperUser->setChecked(false);
	actionSetupMCPD->setVisible(false);
	actionAddMCPD->setVisible(false);
	actionModule->setVisible(false);
	actionGeneral->setVisible(false);
	actionNewSetup->setVisible(false);
	action_Save_Config_File->setVisible(false);
	action_Load_Config_File->setVisible(false);
	actionLoad_Calibration_File->setVisible(false);
#if USE_TACO
	actionTACO->setVisible(false);
#endif
	actionPulser->setVisible(false);
	actionHistogram_Mapping->setVisible(false);
	m_main->selectUserMode(MainWidget::User);
}

void MainWindow::selectExpert(void)
{
	if (checkPasswd("expert"))
	{
		actionUser->setChecked(false);
		actionSuperUser->setChecked(false);
		actionSetupMCPD->setVisible(false);
		actionAddMCPD->setVisible(false);
		actionModule->setVisible(m_main->setupType() != Mdll2);
		actionGeneral->setVisible(true);
		actionNewSetup->setVisible(false);
		action_Save_Config_File->setVisible(true);
		action_Load_Config_File->setVisible(true);
		actionLoad_Calibration_File->setVisible(true);
#if USE_TACO
		actionTACO->setVisible(false);
#endif
		actionPulser->setVisible(m_main->setupType() != Mdll2);
		actionPulser->setEnabled(m_main->setupType() != Mdll2);
		actionHistogram_Mapping->setVisible(true);
		m_main->selectUserMode(MainWidget::Expert);
	}
	else
		actionUser->setChecked(true);
}

void MainWindow::selectSuperuser(void)
{
	if (checkPasswd("superuser"))
	{
		actionUser->setChecked(false);
		actionExpert->setChecked(false);
		actionSetupMCPD->setVisible(true);
		actionAddMCPD->setVisible(true);
		actionModule->setVisible(m_main->setupType() != Mdll2);
		actionGeneral->setVisible(true);
		actionNewSetup->setVisible(true);
		action_Save_Config_File->setVisible(true);
		action_Load_Config_File->setVisible(true);
		actionLoad_Calibration_File->setVisible(true);
#if USE_TACO
		actionTACO->setVisible(true);
#endif
		actionPulser->setVisible(m_main->setupType() != Mdll2);
		actionPulser->setEnabled(m_main->setupType() != Mdll2);
		actionHistogram_Mapping->setVisible(true);
		m_main->selectUserMode(MainWidget::SuperUser);
	}
	else
		actionUser->setChecked(true);
}

/*!
    checks the password for the user level

    \param section the section name to protect
    \return true if no password required or password hash fits the configured password hash
 */
bool MainWindow::checkPasswd(const QString &section)
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
	QString passwd = settings.value(section + "/passwd", "").toString();
	QString usertyped("");
	if (!passwd.isEmpty())
	{
		PasswordDialog d;
		if (d.exec() == QDialog::Accepted)
		{
			usertyped = d.password();
		}
	}
	return passwd == usertyped;
}

/*!
     displays the running state

     \param started indicates a started measurement or idle
 */
void MainWindow::runningState(bool started)
{
	if (started)
		m_daqStatus->setText("DAQ started");
	else
		m_daqStatus->setText("DAQ stopped");
}

/*!
     will be called, if the MCPD's has lost his sync (lost = true)
     or if it was resynchronized (lost = false)

     \param id id of the MCPD the connection
     \param bLost true=lost  false=resynced
 */
void MainWindow::lostSync(quint16 /* id */, bool /* bLost */)
{
	m_lostSync->setVisible(!m_mesy->isSynced());
}

/*!
 * Opens a browser window to the bug report page.
 */
void MainWindow::bugReport(void)
{
	// m_wsTool->show();
}

void MainWindow::doLoadConfiguration(const QString &sFilename)
{
	emit loadConfiguration(sFilename);
}
