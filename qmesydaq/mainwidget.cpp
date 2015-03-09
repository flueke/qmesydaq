/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2014 by Jens Kr�ger <jens.krueger@frm2.tum.de>     *
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
#include "diskspace.h"
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QSvgGenerator>
#include <QCoreApplication>
#include <qwt_plot_curve.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_engine.h>

#include "plot.h"
#include "colormaps.h"
#include "mainwidget.h"
#include "mdefines.h"
#include "measurement.h"
#include "mesydaq2.h"
#include "histogram.h"
#include "mesydaqdata.h"
#include "CommandEvent.h"
#include "MultipleLoopApplication.h"
#include "QMesydaqDetectorInterface.h"
#include "LoopObject.h"
#include "generalsetup.h"
#include "modulewizard.h"
#include "usermapcorrect.h"
#include "mappedhistogram.h"
#include "modulesetup.h"
#include "mdllsetup.h"
#include "mcpdsetup.h"
#include "mpsdpulser.h"
#include "mdllpulser.h"
#include "logging.h"
#include "histogrammappingeditor.h"
#include "editormemory.h"
#include "mainwindow.h"
#include "stdafx.h"
#include "tacosetup.h"
#include "tcpsetup.h"
#include "caresssetup.h"

/*!
    \fn MainWidget::MainWidget(Mesydaq2 *, QWidget *parent = 0)

    constructor

    \param mesy Mesydaq2 object to control the hardware
    \param parent Qt parent object
*/
MainWidget::MainWidget(Mesydaq2 *mesy, QWidget *parent)
	: QWidget(parent)
	, Ui_MainWidget()
	, m_theApp(mesy)
	, m_dispThresh(false)
	, m_dispLoThresh(0)
	, m_dispHiThresh(0)
	, m_data(NULL)
	, m_histData(NULL)
	, m_meas(NULL)
	, m_dispTimer(0)
	, m_histogram(NULL)
	, m_histoType(Measurement::PositionHistogram)
	, m_pulserDialog(NULL)
	, m_remoteStart(true)
{
	setupUi(this);
#ifndef USE_CARESS
	statusTab->removeTab(statusTab->indexOf(statusCARESSTab));
#endif

	m_dataFrame = new Plot(this);
	m_dataFrame->setWindowTitle(tr("QMesyDAQ - plot window"));
	m_dataFrame->setWindowFlags(Qt::Window
				| Qt::CustomizeWindowHint
				| Qt::WindowTitleHint
				| Qt::WindowSystemMenuHint
				| Qt::WindowMaximizeButtonHint);
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
	settings.beginGroup("Plot");
	m_dataFrame->restoreGeometry(settings.value("geometry").toByteArray());
	settings.endGroup();

	m_dataFrame->setObjectName(QString::fromUtf8("m_dataFrame"));
	QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
	sizePolicy1.setHorizontalStretch(0);
	sizePolicy1.setVerticalStretch(0);
	sizePolicy1.setHeightForWidth(m_dataFrame->sizePolicy().hasHeightForWidth());
	m_dataFrame->setSizePolicy(sizePolicy1);
	m_dataFrame->setCursor(QCursor(Qt::CrossCursor));

	QObject::connect(linlogButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(setLinLog(int)));
#if QWT_VERSION >= 0x060000
	QObject::connect(m_dataFrame, SIGNAL(zoom(const QRectF &)), this, SLOT(zoomed(const QRectF &)));
#else
	QObject::connect(m_dataFrame, SIGNAL(zoom(const QwtDoubleRect &)), this, SLOT(zoomed(const QwtDoubleRect &)));
#endif
	QObject::connect(m_theApp, SIGNAL(newCmdPackageReceived()), this, SLOT(updateDisplay()));
	m_dataFrame->show();

	m_time = QTime(QTime::currentTime());
	m_time.start();
	realTimeLabel->setHidden(true);

	statusTab->setCurrentIndex(0);

	moduleStatus0->setId(0);
	moduleStatus1->setId(1);
	moduleStatus2->setId(2);
	moduleStatus3->setId(3);
	moduleStatus4->setId(4);
	moduleStatus5->setId(5);
	moduleStatus6->setId(6);
	moduleStatus7->setId(7);

	linlogButtonGroup->setId(lin, Plot::Linear);
	linlogButtonGroup->setId(log, Plot::Logarithmic);

	posampButtonGroup->setId(dispAllPos, Measurement::PositionHistogram);
	posampButtonGroup->setId(dispAllAmpl, Measurement::AmplitudeHistogram);
	posampButtonGroup->setId(dispAllCorrectedPos, Measurement::CorrectedPositionHistogram);

	displayModeButtonGroup->setId(dispSpectra, Plot::Spectrum);
	displayModeButtonGroup->setId(dispHistogram, Plot::Histogram);
	displayModeButtonGroup->setId(dispDiffractogram, Plot::Diffractogram);
	displayModeButtonGroup->setId(dispMstdSpectrum, Plot::SingleSpectrum);

	versionLabel->setText("QMesyDAQ " VERSION "\n" __DATE__);
	LoopObject *loop = dynamic_cast<LoopObject *>(dynamic_cast<MultipleLoopApplication*>(QApplication::instance())->getLoopObject());
	if (loop)
	{
		MSG_ERROR << loop->version();
		remoteInterfaceVersionLabel->setText(tr("Interface %1").arg(loop->version()));
	}
	else
		remoteInterfaceVersionLabel->setText("");
	libraryVersionLabel->setText(tr("Library %1").arg(m_theApp->libVersion()));

	connect(acquireListFile, SIGNAL(toggled(bool)), m_theApp, SLOT(acqListfile(bool)));
	connect(autoSaveHistogram, SIGNAL(toggled(bool)), m_theApp, SLOT(autoSaveHistogram(bool)));
	connect(m_theApp, SIGNAL(statusChanged(const QString &)), daqStatusLine, SLOT(setText(const QString &)));

	connect(parent, SIGNAL(loadConfiguration(const QString&)), this, SLOT(loadConfiguration(const QString&)), Qt::DirectConnection);

#if 0
	connect(this, SIGNAL(setCounter(quint32, quint64)), m_meas, SLOT(setCounter(quint32, quint64)));
	connect(devid, SIGNAL(valueChanged(int)), devid_2, SLOT(setValue(int)));
	connect(dispMcpd, SIGNAL(valueChanged(int)), devid, SLOT(setValue(int)));
	connect(devid, SIGNAL(valueChanged(int)), dispMcpd, SLOT(setValue(int)));

	connect(acquireListFile, SIGNAL(toggled(bool)), this, SLOT(checkListfilename(bool)));
#endif

	m_data = new MesydaqSpectrumData();
	for (int i = 0; i < 8; ++i)
		m_specData[i] = new MesydaqSpectrumData();
	m_histData = new MesydaqHistogramData();

	m_printer = new QPrinter;
	m_printer->setOrientation(QPrinter::Landscape);
	m_printer->setDocName("PlotCurves");
	m_printer->setCreator("QMesyDAQ Version: " VERSION);

#if 0
	init();

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());

	QSettings setup(settings.value("lastconfigfile", "mesycfg.mcfg").toString(), QSettings::IniFormat);
	acquireListFile->setChecked(setup.value("MESYDAQ/listmode", true).toBool());
	autoSaveHistogram->setChecked(setup.value("MESYDAQ/autosavehistogram", false).toBool());
	timerPreset->setChecked(setup.value("MESYDAQ/Preset/time", true).toBool());
#endif

	timerPreset->setLabel(tr("Timer"));
	eventsPreset->setLabel(tr("Events"));

	monitor1Preset->setLabel(tr("Monitor 1"));
	monitor2Preset->setLabel(tr("Monitor 2"));
	monitor3Preset->setLabel(tr("Monitor 3"));
	monitor4Preset->setLabel(tr("Monitor 4"));

	selectUserMode(User);
	setDisplayMode(Plot::Histogram);
	draw();
}

//! destructor
MainWidget::~MainWidget()
{
	UserMapCorrection* pUserCorrection(dynamic_cast<UserMapCorrection*>(m_meas->posHistMapCorrection()));
	if (pUserCorrection != NULL)
	{
		EditorMemory* pEM(pUserCorrection->getMESFData());
		if (pEM != NULL)
			pEM->wantToSave(this, pUserCorrection->getSourceSize().width());
	}

	if (m_dispTimer)
		killTimer(m_dispTimer);
	m_dispTimer = 0;

	delete m_data;
	for (int i = 0; i < 8; ++i)
		delete m_specData[i];
	delete m_histData;

	delete m_printer;

	delete m_meas;
	m_meas = NULL;
}

/*!
    \fn void MainWidget::about()

    opens an about dialog
*/
void MainWidget::about()
{
	QString text = tr("<h3>About QMesyDAQ </h3>")
			+ tr("<p>Authors</p><ul>")
			+ tr("<li>Copyright (C) 2008 <a href=\"mailto:g.montermann@mesytec.com\">Gregor Montermann</a</li>")
			+ tr("<li>Copyright (C) 2009-2014 <a href=\"mailto:jens.krueger@frm2.tum.de\">Jens Kr&uuml;ger</a></li>")
			+ tr("<li>Copyright (C) 2011-2014 <a href=\"mailto:rossa@helmholtz-berlin.de\">Lutz Rossa</a></li>")
			+ tr("</ul><p>Contributors</p><ul>")
			+ tr("<li><a href=\"mailto:alexander.lenz@frm2.tum.de\">Alexander Lenz</a> TACO remote control</li>")
			+ tr("<li><a href=\"mailto:m.drochner@fz-juelich.de\">Matthias Drochner</a> Bug reports</li>")
			+ tr("<li><a href=\"mailto:christian.randau@frm2.tum.de\">Christian Randau</a> Windows port</li>")
			+ tr("<li><a href=\"mailto:eric.faustmann@helmholtz-berlin.de\">Eric Faustmann</a>Histogram mapping editor</li>")
			+ tr("<li><a href=\"mailto:damian.rhein@helmholtz-berlin.de\">Damian Rhein</a>Histogram mapping editor</li>")
			+ tr("</ul><p>This program controls the data acquisition and display for the MesyTec MCPD-2/8 modules</p>");
	LoopObject *loop = dynamic_cast<LoopObject *>(dynamic_cast<MultipleLoopApplication*>(QApplication::instance())->getLoopObject());
	if (loop)
	{
		text += tr("<p>It may be remotely controlled by:")
			+ tr("<ul>")
			+ tr("<li><b>%1</b></li>").arg(loop->version())
			+ tr("</ul></p>");
	}
	text += tr("<p>It is published under GPL (GNU General Public License) <tt><a href=\"http://www.gnu.org/licenses/gpl.html\">http://www.gnu.org/licenses/gpl.html</a></tt></p>")
		+ tr("The data plot window is based in part on the work of the <a href=\"http://qwt.sf.net\">Qwt project</a>")
		+ tr("<p>Version : <b>%1</b></p>").arg(VERSION)
		+ tr("<p>Library Version : <b>%1</b></p>").arg(m_theApp->libVersion());

	QMessageBox msgBox(this);
	msgBox.setText(text);
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.setWindowTitle(tr("About QMesyDAQ"));
	msgBox.setIconPixmap(QPixmap(":/logo.png"));

	msgBox.exec();
}

/*!
    \fn void MainWidget::init()

    initialising the widget
*/
void MainWidget::init()
{
	if (m_meas)
	{
		disconnect(m_meas, SIGNAL(stopSignal(bool)), startStopButton, SLOT(animateClick()));
		disconnect(m_meas, SIGNAL(mappingChanged()), this, SLOT(mappingChanged()));
		disconnect(dispMcpd, SIGNAL(valueChanged(int)), this, SLOT(displayMcpdSlot(int)));
		delete m_meas;
	}
	m_meas = NULL;
	m_meas = new Measurement(m_theApp, this);
	if (!QDir(m_meas->getConfigfilepath()).exists())
		QMessageBox::warning(this, tr("No config file path"),
					tr("The configuration file path<br><b>%1</b><br>does not exists!").arg(m_meas->getConfigfilepath()));
	setHistogramType(Measurement::PositionHistogram);

	QList<int> mcpdList;
	dispMcpd->setMCPDList(mcpdList);
	devid_2->setMCPDList(mcpdList);
	paramId->setMCPDList(mcpdList);
	startStopButton->setDisabled(true);
	acquireListFile->setDisabled(true);
	autoSaveHistogram->setDisabled(true);
	allPulsersoffButton->setDisabled(true);
	displayGroupBox->setDisabled(true);
	statusMeasTab->setDisabled(true);
	statusModuleTab->setDisabled(true);
	m_dataFrame->setAxisScale(QwtPlot::xBottom, 0, m_meas->width());

	connect(m_meas, SIGNAL(stopSignal(bool)), startStopButton, SLOT(animateClick()));
	connect(m_meas, SIGNAL(mappingChanged()), this, SLOT(mappingChanged()), Qt::QueuedConnection);
	connect(dispMcpd, SIGNAL(valueChanged(int)), this, SLOT(displayMcpdSlot(int)));
	displayMcpdSlot(dispMcpd->value());
#if 0
	connect(this, SIGNAL(setCounter(quint32, quint64)), m_meas, SLOT(setCounter(quint32, quint64)));
#endif
	emit redraw();
}

/*!
    \fn void MainWidget::timerEvent(QTimerEvent *event)

    callback for the timer

    \param event timer event
*/
void MainWidget::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == m_dispTimer)
		emit redraw();
}

/*!
    \fn void MainWidget::allPulserOff(void)

    callback to switch all pulsers off
*/
void MainWidget::allPulserOff(void)
{
	m_theApp->allPulserOff();
	if (m_pulserDialog)
	{
		if (dynamic_cast<MdllPulser *>(m_pulserDialog))
			dynamic_cast<MdllPulser *>(m_pulserDialog)->setMCPD();
		else if (dynamic_cast<MPSDPulser *>(m_pulserDialog))
			dynamic_cast<MPSDPulser *>(m_pulserDialog)->setMCPD();
	}
}

/*!
    \fn void MainWidget::zoomed(const QRectF &rect)

    callback if the zoomer has changed

    \param rect zoom area
*/
void MainWidget::zoomed(const QRectF &rect)
{
	if (m_meas)
	{
		qreal	x,
			y,
			w,
			h;
		rect.getRect(&x, &y, &w, &h);

		m_meas->setROI(QRectF(x, y, w, h));
		emit redraw();
	}
}

/*!
    \fn void MainWidget::statusTabChanged(int )

    callback for changing the tab in the status tab widget

 */
void MainWidget::statusTabChanged(int )
{
	if (statusTab->currentWidget() == statusModuleTab)
		scanPeriSlot(false);
	else
		updateDisplay();
}

/*!
    \fn void MainWidget::startStopSlot(bool checked)

    callback for start and stop

    \param checked
*/
void MainWidget::startStopSlot(bool checked)
{
	if (checked)
	{
		m_meas->setHistfilename("");
		checkListfilename(acquireListFile->isChecked());
		checkHistogramFilename(autoSaveHistogram->isChecked());
		m_remoteStart = false;
		// get timing binwidth
		// m_theApp->setTimingwidth(timingBox->value());

		// get latest preset entries
		if(m_meas->isMaster(TIMERID))
			m_meas->setPreset(TIMERID, quint64(timerPreset->presetValue() * 1000), true);
		if(m_meas->isMaster(EVID))
			m_meas->setPreset(EVID, eventsPreset->presetValue(), true);
		if(m_meas->isMaster(MON1ID))
			m_meas->setPreset(MON1ID, monitor1Preset->presetValue(), true);
		if(m_meas->isMaster(MON2ID))
			m_meas->setPreset(MON2ID, monitor2Preset->presetValue(), true);
		if(m_meas->isMaster(MON3ID))
			m_meas->setPreset(MON3ID, monitor3Preset->presetValue(), true);
		if(m_meas->isMaster(MON4ID) && (m_meas->setupType() != Measurement::Mdll && m_meas->setupType() != Measurement::Mdll2))
			m_meas->setPreset(MON4ID, monitor4Preset->presetValue(), true);
		startStopButton->setText("Stop");
		// set device id to 0 -> will be filled by mesydaq for master
		m_meas->start();
		m_time.restart();
		m_dispTimer = startTimer(500);
	}
	else
	{
		m_meas->stop();
		if (m_dispTimer)
			killTimer(m_dispTimer);
		m_dispTimer = 0;
		startStopButton->setText("Start");

		MultipleLoopApplication *app = dynamic_cast<MultipleLoopApplication*>(QApplication::instance());
		if(app)
		{
			QMesyDAQDetectorInterface *interface = dynamic_cast<QMesyDAQDetectorInterface*>(app->getQtInterface());
			QString sName;
			if (interface)
				sName = interface->getHistogramFileName();
			if (sName.isEmpty())
				sName = m_meas->getHistfilename();
#ifdef USE_CARESS
			else
				autoSaveHistogram->setChecked(true);
#endif
			if (!sName.isEmpty())
			{
				if (!sName.endsWith(".mtxt"))
					sName.append(".mtxt");
				m_meas->setHistfilename(sName);
				if (!sName.startsWith("/"))
				{
					sName.prepend("/");
					sName.prepend(m_meas->getHistfilepath());
				}
				if (autoSaveHistogram->isChecked())
					m_meas->writeHistograms(sName);
			}
		}
		emit redraw();
	}
	emit started(checked);
}

/*!
    \fn void MainWidget::setStreamSlot()
    \todo implementation is missing
*/
void MainWidget::setStreamSlot()
{
#if defined(_MSC_VER)
#	pragma message("TODO implementation is missing ")
#else
#	warning TODO implementation is missing
#endif
#if 0
	unsigned short id = (unsigned short) deviceId->value();
	m_cmdBuffer[0] = mcpdId->value();
	m_cmdBuffer[1] = QUIET;
	if(statusStream->isChecked())
		m_cmdBuffer[2] = 1;
	else
		m_cmdBuffer[2] = 0;
	MSG_WARNING << tr("Set stream %1").arg(m_cmdBuffer[2]);
	m_theApp->sendCommand(m_pBuffer);

	m_theApp->setStream(mcpdId->value(), statusStream->isChecked());
#endif
}

/*!
    \fn QString MainWidget::selectListfile(void)

    Opens a user dialog for selecting a file name to store the list mode data into.

    It checks also the extension of the file name and if not 'mdat' it will added.
    \returns selected file name, if no file name is selected or aborted it will return
             an empty string
*/
QString MainWidget::selectListfile(void)
{
	QString name = QFileDialog::getSaveFileName(this, tr("Select list mode file name"), m_meas->getListfilepath(),
							tr("mesydaq data files (*.mdat);;all files (*.*);;really all files (*)"));
	if(!name.isEmpty())
	{
		int i = name.indexOf(".mdat");
		if (i == -1)
			name.append(".mdat");
	}
	return name;
}

/*!
    \fn void MainWidget::checkListfilename(bool checked)

    checks whether a list mode file name is given, otherwise the checkbox will be disabled

    \param checked
*/
void MainWidget::checkListfilename(bool checked)
{
	if (checked)
	{
		QString name(QString::null);
		MultipleLoopApplication *app = dynamic_cast<MultipleLoopApplication*>(QApplication::instance());
		bool bAsk = true;
		if(app)
		{
			QMesyDAQDetectorInterface *interface = dynamic_cast<QMesyDAQDetectorInterface*>(app->getQtInterface());
			if (interface)
				name = interface->getListFileName();
		}

		if (name.isEmpty() && !m_remoteStart)
		{
			name = selectListfile();
			bAsk = false;
		}
		else
			name = m_meas->getListfilepath() + "/" + name;

		if (!name.isEmpty() && QFile::exists(name))
		{
			// files exists
			if (bAsk && m_meas->getWriteProtection()) // ask user for other file name
				name = selectListfile();
			if (!name.isEmpty() && QFile::exists(name) && !QFile::remove(name)) // try to delete file or do not acquire list file
				name.clear();
		}

		if(!name.isEmpty())
			m_theApp->setListfilename(name);
		else
		{
			MSG_DEBUG << tr("disable list file");
			acquireListFile->setChecked(false);
		}
	}
	emit redraw();
}

/*!
    \fn QString MainWidget::selectHistogramfile(QString sName)
    \param sName suggestion for a histogram file name

    Opens a user dialog for selecting a file name to store the histogram data into.

    It checks also the extension of the file name and if not 'mtxt' it will added.
    \returns selected file name, if no file name is selected or aborted it will return
	     an empty string
*/
QString MainWidget::selectHistogramfile(QString sName)
{
	QString sSuggestion(m_meas->getHistfilepath());
	if (!sSuggestion.endsWith('/'))
		sSuggestion += '/';
	if (!sName.isEmpty())
		sSuggestion += QFileInfo(sName).baseName() + ".mtxt";
	sName = QFileDialog::getSaveFileName(this, tr("Select histogram file name"), sSuggestion,
						tr("mesydaq histogram files (*.mtxt);;all files (*.*);;really all files (*)"));
	if (!sName.isEmpty())
		if (sName.indexOf(".mtxt") < 0)
			sName.append(".mtxt");
	return sName;
}

/*!
    \fn void MainWidget::checkHistogramFilename(bool bEnabled)

    checks whether a histogram file name is given, otherwise the checkbox will be disabled

    \param bEnabled
*/
void MainWidget::checkHistogramFilename(bool bEnabled)
{
 	if (!bEnabled)
		return;

	QString sFilename(QString::null);
	MultipleLoopApplication *pApp(dynamic_cast<MultipleLoopApplication*>(QApplication::instance()));
	bool bAsk = true;
	if (pApp)
	{
		QMesyDAQDetectorInterface *pInterface = dynamic_cast<QMesyDAQDetectorInterface*>(pApp->getQtInterface());
		if (pInterface)
			sFilename = pInterface->getHistogramFileName();
	}

	if (sFilename.isEmpty() && !m_remoteStart)
	{
		sFilename = selectHistogramfile(QString::null);
		bAsk = false;
	}
	else
		sFilename = m_meas->getHistfilepath() + "/" + sFilename;

	if (!sFilename.isEmpty() && QFile::exists(sFilename))
	{
		// files exists
		if (bAsk && m_meas->getWriteProtection()) // ask user for other file name
			sFilename = selectHistogramfile(sFilename);
		if (!sFilename.isEmpty() && QFile::exists(sFilename) && !QFile::remove(sFilename)) // try to delete file or do not acquire list file
			sFilename.clear();
	}

	m_meas->setHistfilename(sFilename);
	if (sFilename.isEmpty())
	{
		MSG_DEBUG << tr("disable autosaving histogram file");
		autoSaveHistogram->setChecked(false);
	}
	emit redraw();
}

/*!
    \fn void MainWidget::updateDisplay(void)

    updates the whole display
 */
void MainWidget::updateDisplay(void)
{
	dateTimeLabel->setText(QString("Date/Time: %1").arg(m_time.currentTime().toString("HH:mm:ss")));

	quint16 id = (quint16) paramId->value();
	int ci = statusTab->currentIndex();
	QTime tmpTime;
	tmpTime = tmpTime.addMSecs(m_time.elapsed());
	if (m_dispTimer)
		realTimeLabel->setText(QString("Real time: %1").arg(tmpTime.toString("HH:mm:ss.zzz")));
	if (statusTab->tabText(ci) == tr("Statistics"))
	{
		dataMissed->setText(tr("%1").arg(m_theApp->missedData()));
		dataRx->setText(tr("%1").arg(m_theApp->receivedData()));
		cmdTx->setText(tr("%1").arg(m_theApp->sentCmds()));
		cmdRx->setText(tr("%1").arg(m_theApp->receivedCmds()));
	}
	if (statusTab->tabText(ci) == tr("Disk space"))
	{
		listModeFileDir->setText(m_meas->getListfilepath());
		DiskSpace ds(m_meas->getListfilepath());
		listModeFileDirSpace->setText(QString("%1 GB").arg(ds.availableGB()));
	}
	if (!m_meas)
		return;
	hTimeText->setText(buildTimestring(m_meas->getHeadertime(), true));
	mTimeText->setText(buildTimestring(m_meas->timer(), /*getMeastime(),*/ false));

	// parameter values for selected ID
	param0->setText(tr("%1").arg(m_theApp->getParameter(id, 0)));
	param1->setText(tr("%1").arg(m_theApp->getParameter(id, 1)));
	param2->setText(tr("%1").arg(m_theApp->getParameter(id, 2)));
	param3->setText(tr("%1").arg(m_theApp->getParameter(id, 3)));
	m_meas->calcMeanRates();

	// measurement values counters and rates
	timerPreset->setValue(m_meas->timer() / 1000.);

	eventsPreset->setValue(m_meas->events());
	eventsPreset->setRate(m_meas->getRate(EVID));

	monitor1Preset->setValue(m_meas->mon1());
	monitor1Preset->setRate(m_meas->getRate(MON1ID));

	monitor2Preset->setValue(m_meas->mon2());
	monitor2Preset->setRate(m_meas->getRate(MON2ID));

	monitor3Preset->setValue(m_meas->mon3());
	monitor3Preset->setRate(m_meas->getRate(MON3ID));

	if(m_meas->setupType() != Measurement::Mdll && m_meas->setupType() != Measurement::Mdll2)
	{
		monitor4Preset->setValue(m_meas->mon4());
		monitor4Preset->setRate(m_meas->getRate(MON4ID));
	}

	lcdRunID->display((int)m_meas->runId());
	dispFiledata();
}

/*!
    \fn QString MainWidget::buildTimestring(quint64 timeval, bool nano)

    creates a string to display the a time

    \param timeval time in multiples of 100ns
    \param nano

    \return the time as a string in HH:MM::SS format
 */
QString MainWidget::buildTimestring(quint64 timeval, bool nano)
{
	// nsec = time in 100 nsecs
	//-> usec =
	//->
	QString str;
	quint64 val;
	ulong /*nsec,*/
		sec,
		min,
		hr;
// calculate raw seconds
	val = round(timeval / 1000.0);
//	nsec = timeval - (1000 * val);
	if(nano)
	{
		val = round(val / 10000.0);
//		nsec = timeval - (1000 * val);
	}
//  MSG_DEBUG << tr("%1 %2 %3").arg(timeval).arg(val).arg(nsec);
//	hours = val / 3600 (s/h)
	hr = val / 3600;
// remaining seconds:
	val -= hr * 3600;
// minutes:
	min = val / 60;
// remaining seconds:
	sec = val - (min * 60);
//	MSG_DEBUG << tr("%1 %2 %3 %4 %1").arg(nsecs).arg(hr).arg(min).arg(sec);
	str.sprintf("%02lu:%02lu:%02lu", hr, min, sec);
	return str;
}

/*!
    \fn void MainWidget::clearAllSlot()

    callback to clear the data
*/
void MainWidget::clearAllSlot()
{
	m_meas->clearAllHist();
	m_meas->setROI(QRectF(0, 0, 0, 0));
	m_meas->setHistfilename("");
	m_meas->clearCounter(TIMERID);
	m_meas->clearCounter(EVID);
	m_meas->clearCounter(MON1ID);
	m_meas->clearCounter(MON2ID);
	m_meas->clearCounter(MON3ID);
	m_meas->clearCounter(MON4ID);
	emit redraw();
}

/*!
    \fn void MainWidget::clearMcpdSlot()

    callback to clear the MCPD list
*/
void MainWidget::clearMcpdSlot()
{
	quint32 start = dispMcpd->value() * 64;
	for(quint32 i = start; i < start + 64; i++)
		m_meas->clearChanHist(i);
	emit redraw();
}

/*!
    \fn void MainWidget::clearMpsdSlot()

    callback to clear the MPSD list
*/
void MainWidget::clearMpsdSlot()
{
	quint32 start = dispMpsd->value() * 8 + dispMcpd->value() * 64;
//	MSG_DEBUG << tr("clearMpsd: %1").arg(start);
	for(quint32 i = start; i < start + 8; i++)
		m_meas->clearChanHist(i);
	emit redraw();
}

/*!
    \fn void MainWidget::clearChanSlot()

    callback to clear the channel list
*/
void MainWidget::clearChanSlot()
{
	m_meas->clearChanHist(dispMcpd->value(), dispMpsd->value(), dispChan->value());
	emit redraw();
}

/*!
    \fn void MainWidget::replayListfileSlot()

    callback function to replay a listmode file
*/
void MainWidget::replayListfileSlot()
{
	QString name = QFileDialog::getOpenFileName(this, tr("Load..."), m_meas->getListfilepath(), "mesydaq data files (*.mdat);;all files (*.*);;really all files (*)");
	if(!name.isEmpty())
	{
		startStopButton->setDisabled(true);
		clearAllSlot();
		displayGroupBox->setEnabled(true);
#if defined(_MSC_VER)
#		pragma message("TODO dynamic resizing of mapped histograms")
#else
#		warning TODO dynamic resizing of mapped histograms
#endif
#if 0
		m_meas->setROI(QRectF(0, 0, m_meas->width(), m_meas->height()));
#else
		m_meas->setROI(QRectF(0, 0, 0, 0));
#endif
		m_dispTimer = startTimer(1000);
		m_theApp->setListfilename(QFileInfo(name).fileName());
		m_meas->readListfile(name);
		if (m_dispTimer)
			killTimer(m_dispTimer);
		m_dispTimer = 0;
		startStopButton->setDisabled(m_theApp->mcpdId().empty());
//		startStopButton->setEnabled(true);
		emit redraw();
	}
}

/*!
    \fn void MainWidget::displayMcpdSlot(int id)

    callback to display MCPD

    \param id
 */
void MainWidget::displayMcpdSlot(int id)
{
	if (id < 0)
		return;
	if (!m_theApp->numMCPD())
		return;
	QList<int> modList = m_theApp->mpsdId(id);
	dispMpsd->setModuleList(modList);
}

/*!
    \fn void MainWidget::displayMpsdSlot(int iModule)

    callback to display the Module

    \param iModule the module number
 */
void MainWidget::displayMpsdSlot(int iModule)
{
// retrieve displayed ID
	quint8 mod = iModule;
	if (iModule < 0)
		mod = devid_2->value();
// firmware version
	firmwareVersion->setText(tr("%1").arg(m_theApp->getFirmware(mod), 0, 'f', 2));
// MCPD capabilities
	QString tmp("");
	quint16 cap = m_theApp->capabilities(mod, true);
	if (cap & TPA)
		tmp = "TPA";
	else if (cap & TP)
		tmp = "TP";
	else if (cap & P)
		tmp = "P";
	MSG_ERROR << tr("%1 %2").arg(cap).arg(tmp);
	capabilities->setText(tmp);

	quint16 txmod = m_theApp->getTxMode(mod);
	if (txmod & TPA)
		tmp = "TPA";
	else if (txmod & TP)
		tmp = "TP";
	else if (txmod & P)
		tmp = "P";
	MSG_ERROR << tr("%1 %2").arg(txmod).arg(tmp);
	txMode->setText(tmp);

// Status display:
	moduleStatus0->update(m_theApp->getModuleType(mod, 0), m_theApp->getModuleVersion(mod, 0), m_theApp->online(mod, 0), m_theApp->histogram(mod, 0), m_theApp->active(mod, 0));
	moduleStatus1->update(m_theApp->getModuleType(mod, 1), m_theApp->getModuleVersion(mod, 1), m_theApp->online(mod, 1), m_theApp->histogram(mod, 1), m_theApp->active(mod, 1));
	moduleStatus2->update(m_theApp->getModuleType(mod, 2), m_theApp->getModuleVersion(mod, 2), m_theApp->online(mod, 2), m_theApp->histogram(mod, 2), m_theApp->active(mod, 2));
	moduleStatus3->update(m_theApp->getModuleType(mod, 3), m_theApp->getModuleVersion(mod, 3), m_theApp->online(mod, 3), m_theApp->histogram(mod, 3), m_theApp->active(mod, 3));
	moduleStatus4->update(m_theApp->getModuleType(mod, 4), m_theApp->getModuleVersion(mod, 4), m_theApp->online(mod, 4), m_theApp->histogram(mod, 4), m_theApp->active(mod, 4));
	moduleStatus5->update(m_theApp->getModuleType(mod, 5), m_theApp->getModuleVersion(mod, 5), m_theApp->online(mod, 5), m_theApp->histogram(mod, 5), m_theApp->active(mod, 5));
	moduleStatus6->update(m_theApp->getModuleType(mod, 6), m_theApp->getModuleVersion(mod, 6), m_theApp->online(mod, 6), m_theApp->histogram(mod, 6), m_theApp->active(mod, 6));
	moduleStatus7->update(m_theApp->getModuleType(mod, 7), m_theApp->getModuleVersion(mod, 7), m_theApp->online(mod, 7), m_theApp->histogram(mod, 7), m_theApp->active(mod, 7));
}

/*!
    \fn void MainWidget::scanPeriSlot(bool real)

    callback to rescan the modules behind the MCPD

    \param real
*/
void MainWidget::scanPeriSlot(bool real)
{
	quint16 id = devid_2->value();
	if (real)
		m_theApp->scanPeriph(id);

	QList<int> modList = m_theApp->mpsdId(id);
	dispMpsd->setModuleList(modList);
	displayMpsdSlot(id);
}

/*!
    \fn void MainWidget::saveSetupSlot()

    callback to save a configuration into a file
*/
void MainWidget::saveSetupSlot()
{
	QString name = QFileDialog::getSaveFileName(this, tr("Save Config File..."), m_meas->getConfigfilepath(), tr("mesydaq config files (*.mcfg);;all files (*.*)"));
	if (!name.isEmpty())
		m_meas->saveSetup(name);
}

/*!
    \fn void MainWidget::newSetupSlot()

    callback for a new setup (empty)
*/
void MainWidget::newSetupSlot()
{
	loadConfiguration(QString());
}

void MainWidget::loadConfiguration(const QString& sFilename)
{
	init();
	m_meas->loadSetup(sFilename);
	if (!QDir(m_meas->getHistfilepath()).exists())
		QMessageBox::warning(this, tr("No histogram storing file path"),
					tr("The histogram storing file path<br><b>%1</b><br>does not exists!").arg(m_meas->getHistfilepath()));
	if (!QDir(m_meas->getListfilepath()).exists())
		QMessageBox::warning(this, tr("No list mode file storing file path"),
					tr("The list mode file storing file path<br><b>%1</b><br>does not exists!").arg(m_meas->getListfilepath()));
	configfilename->setText(m_meas->getConfigfilename());
	acquireListFile->setChecked(m_meas->acqListfile());
	updateMeasurement();
}

void MainWidget::updateMeasurement(void)
{
	QSettings settings(m_meas->getConfigfilename(), QSettings::IniFormat);
	settings.beginGroup("MESYDAQ");
	acquireListFile->setChecked(settings.value("listmode", true).toBool());
	autoSaveHistogram->setChecked(settings.value("autosavehistogram", false).toBool());
	timerPreset->setChecked(settings.value("Preset/time", true).toBool());
	settings.endGroup();

	QList<int> mcpdList = m_theApp->mcpdId();
	dispMcpd->setMCPDList(mcpdList);
	devid_2->setMCPDList(mcpdList);
	devid_2->setDisabled(mcpdList.empty());
	paramId->setMCPDList(mcpdList);
	startStopButton->setDisabled(mcpdList.empty());
	acquireListFile->setDisabled(mcpdList.empty());
	autoSaveHistogram->setDisabled(mcpdList.empty());
	allPulsersoffButton->setDisabled(mcpdList.empty());
	displayGroupBox->setDisabled(mcpdList.empty());
	statusMeasTab->setDisabled(mcpdList.empty());
	statusModuleTab->setDisabled(mcpdList.empty());
	dispMstdSpectrum->setVisible(m_meas->setupType() != Measurement::Mpsd);
	if (m_meas->setupType() == Measurement::Mdll || m_meas->setupType() == Measurement::Mdll2)
	{
		dispMstdSpectrum->setText(tr("Amplitude spectrum"));
		dispDiffractogram->setText(tr("Projection to X"));
		dispSpectra->setText(tr("Projection to Y"));
	}
	else if (m_meas->setupType() == Measurement::Mstd)
	{
		dispMstdSpectrum->setText(tr("Single spectrum"));
	}
	else
	{
		dispDiffractogram->setText(tr("Diffractogram"));
		dispSpectra->setText(tr("Spectra"));
	}
	dispHistogram->setHidden(m_meas->setupType() == Measurement::Mstd);
	moduleStatus1->setHidden(m_meas->setupType() == Measurement::Mdll || m_meas->setupType() != Measurement::Mdll2);
	moduleStatus2->setHidden(m_meas->setupType() == Measurement::Mdll || m_meas->setupType() != Measurement::Mdll2);
	moduleStatus3->setHidden(m_meas->setupType() == Measurement::Mdll || m_meas->setupType() != Measurement::Mdll2);
	moduleStatus4->setHidden(m_meas->setupType() == Measurement::Mdll || m_meas->setupType() != Measurement::Mdll2);
	moduleStatus5->setHidden(m_meas->setupType() == Measurement::Mdll || m_meas->setupType() != Measurement::Mdll2);
	moduleStatus6->setHidden(m_meas->setupType() == Measurement::Mdll || m_meas->setupType() != Measurement::Mdll2);
	moduleStatus7->setHidden(m_meas->setupType() == Measurement::Mdll || m_meas->setupType() != Measurement::Mdll2);
// MDLL has only three monitor inputs
	monitor4Preset->setHidden(m_meas->setupType() == Measurement::Mdll || m_meas->setupType() != Measurement::Mdll2);
	monitor4Preset->setChecked(false);
	if (m_meas->setupType() == Measurement::Mstd)
	{
		dispMstdSpectrum->setChecked(true);
		setDisplayMode(Plot::SingleSpectrum);
	}
	emit redraw();
}

/*!
    \fn Measurement::Setup MainWidget::setupType(void)

    \returns the currently configured measurement, the default is the Mpsd mode
 */
Measurement::Setup MainWidget::setupType(void)
{
	if (m_meas)
		return m_meas->setupType();
	return Measurement::Mpsd;
}

/*!
    \fn void MainWidget::restoreSetupSlot()

    callback to load a configuration from a file
 */
void MainWidget::restoreSetupSlot()
{
	QString name = QFileDialog::getOpenFileName(this, tr("Load Config File..."), m_meas->getConfigfilepath(), tr("mesydaq config files (*.mcfg);;all files (*.*)"));
	if (!name.isEmpty())
		emit(loadConfiguration(name));
}

/*!
    \fn void MainWidget::applyThreshSlot()

    callback to apply the threshold settings
 */
void MainWidget::applyThreshSlot()
{
	bool ok;
	m_dispThresh = useThresh->isChecked();
	if(m_dispThresh)
	{
		m_dispHiThresh = hiLim->text().toUInt(&ok, 0);
		m_dispLoThresh = loLim->text().toUInt(&ok, 0);
		m_dataFrame->setThresholds(QwtDoubleInterval(m_dispLoThresh, m_dispHiThresh));
	}
	else
		m_dataFrame->setThresholds(QwtDoubleInterval(0.0, 0.0));
	emit redraw();
}
/*!
    \fn void MainWidget::drawOpData()

    draw all operational data
 */
void MainWidget::drawOpData()
{
	float	mean,
		sigma;

	// display mean and sigma:
	if(dispAll->isChecked())
	{
		if(dispAllPos->isChecked())
			m_meas->getMean(Measurement::PositionHistogram, mean, sigma);
		else if (dispAllAmpl->isChecked())
			m_meas->getMean(Measurement::AmplitudeHistogram, mean, sigma);
		else
			m_meas->getMean(Measurement::CorrectedPositionHistogram, mean, sigma);
	}
	else
	{
		if(dispAllPos->isChecked())
			m_meas->getMean(Measurement::PositionHistogram, dispMpsd->value() * 8 + dispChan->value(), mean, sigma);
		else if (dispAllAmpl->isChecked())
			m_meas->getMean(Measurement::AmplitudeHistogram, dispMpsd->value() * 8 + dispChan->value(), mean, sigma);
		else if (dispAllCorrectedPos->isChecked())
			m_meas->getMean(Measurement::CorrectedPositionHistogram, dispMpsd->value() * 8 + dispChan->value(), mean, sigma);
		else if(specialBox->isChecked())
			m_meas->getMean(Measurement::TimeSpectrum, mean, sigma);
	}
	meanText->setText(tr("%1").arg(mean, 3, 'f', 1));
	sigmaText->setText(tr("%1").arg(sigma, 3, 'f', 1));

// pulser warning
	if(m_theApp->isPulserOn())
		pulserWarning->setText(tr("<p align=\"center\">PULSER ON!</p>"));
	else
		pulserWarning->setText("");
}

/*!
    \fn void MainWidget::dispFiledata(void)

    display informations about the data
 */
void MainWidget::dispFiledata(void)
{
	configfilename->setText(m_meas ? m_meas->getConfigfilename() : "-");
	if(!m_meas || m_meas->getHistfilename().isEmpty())
		histfilename->setText("-");
	else
		histfilename->setText(m_meas->getHistfilename());
	if (!m_meas || m_meas->getCalibrationfilename().isEmpty())
		calibrationFilename->setText("-");
	else
		calibrationFilename->setText(m_meas->getCalibrationfilename());
	if (m_theApp->getListfilename().isEmpty())
		listFilename->setText("-");
	else
		listFilename->setText(m_theApp->getListfilename());
}

/*!
    \fn void MainWidget::writeHistSlot()

    callback to write a histogram data file
 */
void MainWidget::writeHistSlot()
{
	QString sName(selectHistogramfile(m_theApp->getListfilename()));
	if (!sName.isEmpty())
		m_meas->writeHistograms(sName);
}

/*!
   \fn void MainWidget::loadHistSlot()

   callback to read a histogram data file
*/
void MainWidget::loadHistSlot()
{
	QString name = QFileDialog::getOpenFileName(this, tr("Load Histogram..."), m_meas->getHistfilepath(), "mesydaq histogram files (*.mtxt);;all files (*.*)");
	if(!name.isEmpty())
	{
		m_meas->readHistograms(name);
		displayGroupBox->setEnabled(true);
		emit redraw();
	}
}

/*!
   \fn void MainWidget::loadCalibrationSlot()

   callback to read a calibration data file
 */
void MainWidget::loadCalibrationSlot()
{
	QString name = QFileDialog::getOpenFileName(this, tr("Load Calibration File ..."), m_meas->getConfigfilepath(), "mesydaq calibration files(*.mcal *.mesf *.txt);;all files (*.*)");
	if (!name.isEmpty())
	{
		m_meas->readCalibration(name, true);
		emit redraw();
	}
}

/*!
   \fn void MainWidget::ePresetSlot(bool pr)

   callback to enable/disable event counter

   \param pr enable the preset of the event counter
 */
void MainWidget::ePresetSlot(bool pr)
{
	if(pr)
	{
		timerPreset->setChecked(false);
		monitor1Preset->setChecked(false);
		monitor2Preset->setChecked(false);
		monitor3Preset->setChecked(false);
		monitor4Preset->setChecked(false);
	}
	eventsPreset->setChecked(pr);
	m_meas->setPreset(EVID, eventsPreset->presetValue(), pr);
}

/*!
   \fn void MainWidget::tPresetSlot(bool pr)

   callback to enable/disable timer

   \param pr enable the preset of the timer
 */
void MainWidget::tPresetSlot(bool pr)
{
	if(pr)
	{
		eventsPreset->setChecked(false);
		monitor1Preset->setChecked(false);
		monitor2Preset->setChecked(false);
		monitor3Preset->setChecked(false);
		monitor4Preset->setChecked(false);
	}
	timerPreset->setChecked(pr);
	m_meas->setPreset(TIMERID, quint64(timerPreset->presetValue() * 1000), pr);
}

/*!
   \fn void MainWidget::m1PresetSlot(bool pr)

   callback to enable/disable monitor 1

   \param pr enable the preset of the monitor 1
 */
void MainWidget::m1PresetSlot(bool pr)
{
	if(pr)
	{
		timerPreset->setChecked(false);
		eventsPreset->setChecked(false);
		monitor2Preset->setChecked(false);
		monitor3Preset->setChecked(false);
		monitor4Preset->setChecked(false);
	}
	monitor1Preset->setChecked(pr);
	m_meas->setPreset(MON1ID, monitor1Preset->presetValue(), pr);
}

/*!
   \fn void MainWidget::m2PresetSlot(bool pr)

   callback to enable/disable monitor 2

   \param pr enable the preset of the monitor 2
 */
void MainWidget::m2PresetSlot(bool pr)
{
	if(pr)
	{
		timerPreset->setChecked(false);
		eventsPreset->setChecked(false);
		monitor1Preset->setChecked(false);
		monitor3Preset->setChecked(false);
		monitor4Preset->setChecked(false);
	}
	monitor2Preset->setChecked(pr);
	m_meas->setPreset(MON2ID, monitor2Preset->presetValue(), pr);
}

/*!
   \fn void MainWidget::m3PresetSlot(bool pr)

   callback to enable/disable monitor 3

   \param pr enable the preset of the monitor 3
 */
void MainWidget::m3PresetSlot(bool pr)
{
	if(pr)
	{
		timerPreset->setChecked(false);
		eventsPreset->setChecked(false);
		monitor1Preset->setChecked(false);
		monitor2Preset->setChecked(false);
		monitor4Preset->setChecked(false);
	}
	monitor3Preset->setChecked(pr);
	m_meas->setPreset(MON3ID, monitor3Preset->presetValue(), pr);
}

/*!
   \fn void MainWidget::m4PresetSlot(bool pr)

   callback to enable/disable monitor 4

   \param pr enable the preset of the monitor 4
 */
void MainWidget::m4PresetSlot(bool pr)
{
	if(pr)
	{
		timerPreset->setChecked(false);
		eventsPreset->setChecked(false);
		monitor1Preset->setChecked(false);
		monitor2Preset->setChecked(false);
		monitor3Preset->setChecked(false);
	}
	if (m_meas->setupType() != Measurement::Mdll && m_meas->setupType() != Measurement::Mdll2)
	{
		monitor4Preset->setChecked(pr);
		m_meas->setPreset(MON4ID, monitor4Preset->presetValue(), pr);
	}
}

/*!
    \fn void MainWidget::updatePresets(void)

    callback to display all preset values
 */
void MainWidget::updatePresets(void)
{
	// presets
	timerPreset->setPresetValue(m_meas->getPreset(TIMERID));
	eventsPreset->setPresetValue(m_meas->getPreset(EVID));
	monitor1Preset->setPresetValue(m_meas->getPreset(MON1ID));
	monitor2Preset->setPresetValue(m_meas->getPreset(MON2ID));
	monitor3Preset->setPresetValue(m_meas->getPreset(MON3ID));
	if (m_meas->setupType() != Measurement::Mdll && m_meas->setupType() != Measurement::Mdll2)
		monitor4Preset->setPresetValue(m_meas->getPreset(MON4ID));

	// check for master preset counter
	timerPreset->setChecked(m_meas->isMaster(TIMERID));
	eventsPreset->setChecked(m_meas->isMaster(EVID));
	monitor1Preset->setChecked(m_meas->isMaster(MON1ID));
	monitor2Preset->setChecked(m_meas->isMaster(MON2ID));
	monitor3Preset->setChecked(m_meas->isMaster(MON3ID));
	if (m_meas->setupType() != Measurement::Mdll && m_meas->setupType() != Measurement::Mdll2)
		monitor4Preset->setChecked(m_meas->isMaster(MON4ID));
}

/*!
    \fn void MainWidget::tResetSlot()

    clears the timer
 */
void MainWidget::tResetSlot()
{
	m_meas->clearCounter(TIMERID);
	updateDisplay();
}

/*!
    \fn void MainWidget::eResetSlot()

    clears the event counter
 */
void MainWidget::eResetSlot()
{
	m_meas->clearCounter(EVID);
	updateDisplay();
}

/*!
    \fn void MainWidget::m1ResetSlot()

    clears the Monitor 1 counter
 */
void MainWidget::m1ResetSlot()
{
	m_meas->clearCounter(MON1ID);
	updateDisplay();
}

/*!
    \fn void MainWidget::m2ResetSlot()

    clears the Monitor 2 counter
 */
void MainWidget::m2ResetSlot()
{
	m_meas->clearCounter(MON2ID);
	updateDisplay();
}

/*!
    \fn void MainWidget::m3ResetSlot()

    clears the Monitor 3 counter
 */
void MainWidget::m3ResetSlot()
{
	m_meas->clearCounter(MON3ID);
	updateDisplay();
}

/*!
    \fn void MainWidget::m4ResetSlot()

    clears the Monitor 4 counter
 */
void MainWidget::m4ResetSlot()
{
	m_meas->clearCounter(MON4ID);
	updateDisplay();
}

/*!
    \fn void MainWidget::mpsdCheck(int mod)

    \param mod
*/
void MainWidget::mpsdCheck(int mod)
{
	MSG_DEBUG << tr("MainWidget::mpsdCheck() : module %1").arg(mod);
	displayMpsdSlot(mod);
}

/*!
    \fn void MainWidget::setHistogramType(int val)

    sets the histogram type

    \param val new histogram type
 */
void MainWidget::setHistogramType(int val)
{
	m_histoType = Measurement::HistogramType(val);
#if 0
	switch (val)
	{
		case Measurement::PositionHistogram :
		case Measurement::AmplitudeHistogram:
		case Measurement::CorrectedPositionHistogram:
			{
                		m_histogram = m_meas->hist(Measurement::HistogramType(val));
                		m_histData->setData(m_histogram);
                		m_dataFrame->setHistogramData(m_histData);
			}
			break;
		default :
			break;
	}
#endif
	emit redraw();
}

/*!
    \fn void MainWidget::setLinLog(int val)

    sets the lin/log scaling

    \param val new scaling mode
 */
void MainWidget::setLinLog(int val)
{
	m_dataFrame->setLinLog(Plot::Scale(val));
}

/*!
    \fn void MainWidget::setDisplayMode(int val)

    sets the display mode

    \param val new display mode
 */
void MainWidget::setDisplayMode(int val)
{
	m_mode = Plot::Mode(val);
	m_dataFrame->setDisplayMode(m_mode);
	switch(m_mode)
	{
		case Plot::Spectrum:
			dispAll->setEnabled(true);
			if (!dispAll->isChecked())
			{
				dispMcpd->setEnabled(true);
				dispMpsd->setEnabled(true);
				dispChan->setEnabled(true);
				dispAllChannels->setEnabled(true);
				if (dispAllChannels->isChecked())
					m_dataFrame->setDisplayMode(Plot::ModuleSpectrum);
			}
			break;
		case Plot::Histogram:
		case Plot::Diffractogram:
		case Plot::SingleSpectrum:
			dispAll->setEnabled(false);
			dispMcpd->setEnabled(false);
			dispMpsd->setEnabled(false);
 			dispChan->setEnabled(false);
			dispAllChannels->setEnabled(false);
		default:
			break;
	}
	emit redraw();
}

/*!
    \fn void MainWidget::draw(void)

    callback to redraw the data and additional informations
 */
void MainWidget::draw(void)
{
	if (!m_meas)
	{
		m_dataFrame->setAxisScale(QwtPlot::yRight, 0, 1.0);
		return;
	}
	if (m_meas->getROI().isEmpty())
		m_meas->setROI(QRectF(0, 0, width(), height()));
	Spectrum *spec(NULL);
	Histogram *histogram(NULL);
	switch (m_mode)
	{
		case Plot::Histogram :
			if (m_meas->setupType() == Measurement::Mdll || m_meas->setupType() != Measurement::Mdll2)
			{
				m_dataFrame->setAxisTitle(QwtPlot::xBottom, "X (channel)");
				m_dataFrame->setAxisTitle(QwtPlot::yLeft, "Y (channel)");
			}
			histogram = m_meas->hist(Measurement::HistogramType(m_histoType));
			histogram->calcMinMaxInROI(m_meas->getROI());
			m_histData->setData(histogram);
			m_dataFrame->setHistogramData(m_histData);
			labelCountsInROI->setText(tr("Counts in ROI"));
			countsInROI->setText(tr("%1").arg(histogram->getCounts(m_meas->getROI())));
			break;
		case Plot::Diffractogram :
			spec = m_meas->spectrum(Measurement::Diffractogram);
			if (m_meas->setupType() == Measurement::Mdll || m_meas->setupType() != Measurement::Mdll2)
				m_dataFrame->setAxisTitle(QwtPlot::xBottom, "X (channel)");
			m_data->setData(spec);
			m_dataFrame->setSpectrumData(m_data);
			labelCountsInROI->setText(tr("Counts"));
			countsInROI->setText(tr("%1").arg(spec->getTotalCounts()));
			break;
		case Plot::SingleSpectrum :
			if (m_meas->setupType() == Measurement::Mdll || m_meas->setupType() != Measurement::Mdll2)
			{
				spec = m_meas->spectrum(Measurement::AmplitudeSpectrum);
				m_dataFrame->setAxisTitle(QwtPlot::xBottom, "amplitude");
			}
			else
				spec = m_meas->spectrum(Measurement::SingleTubeSpectrum);
			m_data->setData(spec);
			m_dataFrame->setSpectrumData(m_data);
			labelCountsInROI->setText(tr("Counts"));
			countsInROI->setText(tr("%1").arg(spec->getTotalCounts()));
			break;
		case Plot::Spectrum :
			if (m_meas->setupType() == Measurement::Mdll || m_meas->setupType() != Measurement::Mdll2)
				m_dataFrame->setAxisTitle(QwtPlot::xBottom, "Y (channel)");
			if(dispAll->isChecked())
			{
				if (dispAllPos->isChecked())
					spec = m_meas->data(Measurement::PositionHistogram);
				else if (dispAllAmpl->isChecked())
					spec = m_meas->data(Measurement::AmplitudeHistogram);
				else
					spec = m_meas->data(Measurement::CorrectedPositionHistogram);
				m_data->setData(spec);
				labelCountsInROI->setText(tr("Counts"));
				m_dataFrame->setSpectrumData(m_data);
				countsInROI->setText(tr("%1").arg(spec->getTotalCounts()));
			}
			else
			{
				if (!dispAllChannels->isChecked())
					for (int i = 1; i < 8; ++i)
						m_dataFrame->setSpectrumData((SpectrumData *)NULL, i);
				if (specialBox->isChecked())
				{
					spec = m_meas->spectrum(Measurement::TimeSpectrum);
					m_data->setData(spec);

					labelCountsInROI->setText(tr("Time"));
					countsInROI->setText(tr("%1").arg(spec->getTotalCounts()));
				}
				else if (dispAllChannels->isChecked())
				{
					quint32 chan = dispMcpd->value() * 64 + dispMpsd->value() * 8;
					labelCountsInROI->setText(tr("Counts in MCPD: %1 MPSD: %2").arg(dispMcpd->value()).arg(dispMpsd->value()));
					quint64 counts(0);
					for (int i = 7; i >= 0; --i)
					{
						if (dispAllPos->isChecked())
							spec = m_meas->data(Measurement::PositionHistogram, chan + i);
						else
							spec = m_meas->data(Measurement::AmplitudeHistogram, chan + i);
						if (spec)
							counts += spec->getTotalCounts();
						m_specData[i]->setData(spec);
						m_dataFrame->setSpectrumData(m_specData[i], i);
						countsInROI->setText(tr("%1").arg(counts));
					}
				}
				else
				{
					labelCountsInROI->setText(tr("MCPD: %1 MPSD: %2 Channel: %3").arg(dispMcpd->value()).arg(dispMpsd->value()).arg(dispChan->value()));
					quint64 counts(0);
					if (dispAllPos->isChecked())
						spec = m_meas->data(Measurement::PositionHistogram, dispMcpd->value(), dispMpsd->value(), dispChan->value());
					else
						spec = m_meas->data(Measurement::AmplitudeHistogram, dispMcpd->value(), dispMpsd->value(), dispChan->value());
					m_data->setData(spec);
					if (spec)
						counts = spec->getTotalCounts();
					countsInROI->setText(tr("%1").arg(counts));
					m_dataFrame->setSpectrumData(m_data);
				}
			}
// reduce data in case of threshold settings:
			if (m_dispThresh)
				m_dataFrame->setAxisScale(QwtPlot::yLeft, m_dispLoThresh, m_dispHiThresh);
			break;
		default:
            break;
	}
	m_dataFrame->replot();
	drawOpData();
	updateDisplay();
}

/*!
    \fn void MainWidget::exportPDF(void)

    callback to export the plot data as PDF
 */
void MainWidget::exportPDF()
{
	QString fileName = QFileDialog::getSaveFileName(this, "Export PDF File", "Plot1.pdf", "PDF Documents (*.pdf)");
	if(!fileName.isEmpty())
	{
		if(!fileName.endsWith(".pdf"))
			fileName += ".pdf";
		m_printer->setOutputFormat(QPrinter::PdfFormat);
		m_printer->setOutputFileName(fileName);
		QwtPlotPrintFilter filter;
		filter.setOptions(QwtPlotPrintFilter::PrintAll & ~QwtPlotPrintFilter::PrintBackground);
		m_dataFrame->print(*m_printer, filter);
	}
}

/*!
    \fn void MainWidget::exportSVG(void)

    callback to export the plot data as SVG
 */
void MainWidget::exportSVG()
{
	QString fileName = QFileDialog::getSaveFileName(this, "Export SVG File", "Plot1.svg", "SVG Documents (*.svg)");
	if(!fileName.isEmpty())
	{
		if(!fileName.endsWith(".svg"))
			fileName += ".svg";
		QSvgGenerator generator;
		generator.setFileName(fileName);
#if QT_VERSION >= 0x040500
		generator.setTitle(tr("QMesyDAQ data plot"));
		generator.setDescription(tr("QMesydaq generated data plot"));
#endif
		generator.setSize(QSize(800, 600));
		m_dataFrame->print(generator);
	}
}

/*!
    \fn void MainWidget::setupMCPD()

    opens a dialog to handle the MCPD setup
*/
void MainWidget::setupMCPD(void)
{
	MCPDSetup d(m_theApp);
	d.exec();
}

/*!
    \fn void MainWidget::addMCPD()

    opens the add MCPD wizard dialog for adding a MCPD module.

    It adds this module to the setup if the 'finish' button is pressed.
*/
void MainWidget::addMCPD(void)
{
	ModuleWizard d("192.168.168.121", quint16(0), this); // m_theApp);
	if (d.exec() == QDialog::Accepted)
	{
		QMetaObject::invokeMethod(m_theApp, "addMCPD", Qt::BlockingQueuedConnection, Q_ARG(quint8, d.id()), Q_ARG(QString, d.ip()));
		m_theApp->scanPeriph(d.id());
		m_theApp->setTimingSetup(d.id(), d.master(), d.terminate(), d.externsync());
		m_meas->updateSetupType();
		updateMeasurement();
	}
}

void MainWidget::setupTACO(void)
{
	TACOSetup d;
	d.exec();
}

void MainWidget::setupTCP(void)
{
	TCPSetup d;
	d.exec();
}

void MainWidget::setupCARESS(void)
{
	CARESSSetup d;
	d.exec();
}

/*!
    \fn void MainWidget::toolPulser(void)

    opens the dialog to handle the MPSD Pulser dialog
 */
void MainWidget::toolPulser(void)
{
	if (m_meas->setupType() == Measurement::Mdll)
	{
		if (!m_pulserDialog)
			m_pulserDialog = new MdllPulser(m_theApp, this);
	}
	else
	{
		if (!m_pulserDialog)
			m_pulserDialog = new MPSDPulser(m_theApp, this);
		connect(m_pulserDialog, SIGNAL(pulserTest(bool)), startStopButton, SLOT(setChecked(bool)));
		connect(m_pulserDialog, SIGNAL(pulserTest(bool)), this, SLOT(startStopSlot(bool)));
		connect(m_pulserDialog, SIGNAL(clear()), clearAll, SLOT(animateClick()));
	}
	if (m_pulserDialog)
		m_pulserDialog->show();
}

/*!
    \fn void MainWidget::setupModule(quint8)

    opens the dialog to handle the a distinct MPSD module setup

    \param id module id of the currently select MCPD
 */
void MainWidget::setupModule(quint8 id)
{
	ModuleSetup d(m_theApp, this);
	d.setMCPD(devid_2->value());
	d.setModule(id);
	d.exec();
}

/*!
    \fn void MainWidget::setupModule()

    opens the dialog to handle the MPSD Module setup
*/
void MainWidget::setupModule(void)
{
	QDialog *d;
	if (m_meas->setupType() == Measurement::Mdll)
		d = new MdllSetup(m_theApp, this);
	else
		d = new ModuleSetup(m_theApp, this);
	d->exec();
	delete d;
}

/*!
    \fn void MainWidget::setupMdll(quint8)

    opens the dialog to handle the a distinct MDLL module setup

    \param id module id of the currently select MCPD
 */
void MainWidget::setupMdll(quint8 id)
{
 	Q_UNUSED(id);
	MdllSetup d(m_theApp, this);
//	d.setMCPD(devid_2->value());
	d.exec();
}

/*!
    \fn void MainWidget::setupGeneral(void)

    callback to setup the directories to store the different files like
    - histograms
    - listmode data
    - configuration files
 */
void MainWidget::setupGeneral()
{
	GeneralSetup d(m_meas);
	if (d.exec() == QDialog::Accepted)
	{
		QSettings settings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
		settings.setValue("config/configfilepath", d.configFilePath());
		settings.setValue("config/lastrunid", d.lastRunId());
//		settings.setValue("config/listfilepath", d.listFilePath());
//		settings.setValue("config/histfilepath", d.histFilePath());

		m_meas->setListfilepath(d.listFilePath());
		m_meas->setHistfilepath(d.histFilePath());
		m_meas->setConfigfilepath(d.configFilePath());
		m_meas->setRunId(d.lastRunId());
		m_meas->setAutoIncRunId(d.getAutoIncRunId());
		m_meas->setWriteProtection(d.getWriteProtection());
	}
}

/*!
    \fn void MainWidget::editHistogramMapping()

    callback to edit the histogram mapping
 */
void MainWidget::editHistogramMapping()
{
	MapCorrection* pDefaultCorrection(m_meas->posHistMapCorrection());
	UserMapCorrection* pUserCorrection(dynamic_cast<UserMapCorrection*>(pDefaultCorrection));
	if (pUserCorrection == NULL)
		// the user may overwrite this default mapping, check comes later
		m_meas->posHistMapCorrection() = pUserCorrection = new UserMapCorrection(QSize(m_meas->width(), m_meas->height()), MapCorrection::OrientationUp, MapCorrection::CorrectSourcePixel);
	else
		// default is already a UserMapCorrection -> unable to restore it
		pDefaultCorrection = NULL;
	if (pUserCorrection != NULL)
	{
		HistogramMappingEditor d(m_meas, this);
		connect(&d, SIGNAL(applyedData()), this, SIGNAL(redraw()));
		d.exec();
		disconnect(&d, SIGNAL(applyedData()), this, SIGNAL(redraw()));
        if (pDefaultCorrection != NULL && d.hasChanged())
        {
		// user has changed the mapping
		delete pDefaultCorrection;
		pDefaultCorrection = NULL;
	}
	}
	MapCorrection* pCorrection(m_meas->posHistMapCorrection());
	if (pDefaultCorrection != NULL && pCorrection != pDefaultCorrection)
	{
		// the user did not change the default mapping, restore it
		delete pCorrection;
		m_meas->posHistMapCorrection() = pDefaultCorrection;
	}
}

/*!
    \fn void MainWidget::mappingChanged()

    callback to show or hide the menu entry for the "histogram mapping editor"
 */
void MainWidget::mappingChanged()
{
	MainWindow* pMainWindow = dynamic_cast<MainWindow*>(parent());
	if (pMainWindow != NULL)
	{
		bool bVisible(false);
		MapCorrection* pCorrection(m_meas->posHistMapCorrection());
		UserMapCorrection* pUserCorrection = dynamic_cast<UserMapCorrection*>(pCorrection);
		LinearMapCorrection* pLinearCorrection = dynamic_cast<LinearMapCorrection*>(pCorrection);
		if (pMainWindow->actionExpert->isChecked() || pMainWindow->actionSuperUser->isChecked())
		{
			if (pLinearCorrection != NULL)
				bVisible = true;
			else if (pUserCorrection != NULL)
				bVisible = (pUserCorrection->getMESFData() != NULL);
			pMainWindow->actionHistogram_Mapping->setVisible(bVisible);
		}
	}
}

/*!
    \fn void MainWidget::printPlot(void)

    opens a dialog to select the printer and if the ok button pressed printout the plot area widget
*/
void MainWidget::printPlot(void)
{
//	QPrinter printer(QPrinter::HighResolution);
	m_printer->setOutputFormat(QPrinter::NativeFormat);
	QPrintDialog dialog(m_printer);
	if(dialog.exec() == QDialog::Accepted)
	{
#if QWT_VERSION < 0x060000
		QwtPlotPrintFilter filter;
		filter.setOptions(QwtPlotPrintFilter::PrintAll & ~QwtPlotPrintFilter::PrintBackground);
		m_dataFrame->print(*m_printer, filter);
#else
		QwtPlotRenderer renderer;

		renderer.setDiscardFlag(QwtPlotRenderer::DiscardBackground, false);
		renderer.setLayoutFlag(QwtPlotRenderer::KeepFrames, true);

		renderer.renderTo(this, printer);
#endif
	}
}

/*!
    \fn void MainWidget::quitContinue(void)

    asks for continue or abort in case of not initialized remote interface
*/
void MainWidget::quitContinue(void)
{
#if USE_TACO
	QString interface("TACO");
#elif USE_CARESS
	QString interface("CARESS");
#elif USE_TCP
	QString interface("TCP");
#else
	QString interface("");
#endif
	if (QMessageBox::warning(this, tr("Remote control interface").arg(interface), tr("The %1 remote control interface is not initialized!<br>"
				"Please have a look on the debug output.<br>Do you want continue?").arg(interface),
				QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No)
	{
		MultipleLoopApplication *app = dynamic_cast<MultipleLoopApplication*>(QApplication::instance());
		app->quit();
	}
}

/*!
    \fn void MainWidget::closeEvent(QCloseEvent *)

    overwrites the default closeEvent method
*/
void MainWidget::closeEvent(QCloseEvent *)
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
	settings.beginGroup("Plot");
	settings.setValue("geometry", m_dataFrame->saveGeometry());
	settings.endGroup();
}

/*!
   \fn void MainWidget::customEvent(QEvent *e)

   handles the customEvents sent via the remote interface
*/
void MainWidget::customEvent(QEvent *e)
{
	CommandEvent *event = dynamic_cast<CommandEvent*>(e);
	if (!event)
	{
		QWidget::customEvent(e);
		return;
	}

	CommandEvent::Command cmd = event->getCommand();
	QList<QVariant> args = event->getArgs(); // command arguments
	QList<QVariant> answer; // answer data

	MultipleLoopApplication *app = dynamic_cast<MultipleLoopApplication*>(QApplication::instance());
	QMesyDAQDetectorInterface *interface = NULL;
	if (app)
		interface = dynamic_cast<QMesyDAQDetectorInterface*>(app->getQtInterface());

	if (!interface) // we need an interface: every command needs an answer
		return;

	switch(cmd)
	{
		case CommandEvent::C_INIT:
			emit(loadConfiguration(m_meas->getConfigfilename()));
			break;
		case CommandEvent::C_START:
#if 0
			clearAll->animateClick();
			QCoreApplication::processEvents();
#endif
		case CommandEvent::C_RESUME:
			if (!startStopButton->isChecked())
			{
				m_remoteStart = true;
				startStopButton->click();
				QCoreApplication::processEvents();
			}
			break;
		case CommandEvent::C_STOP:
			if (startStopButton->isChecked())
				startStopButton->click();
			break;
		case CommandEvent::C_CLEAR:
			clearAll->animateClick();
			break;
		case CommandEvent::C_PRESELECTION:
			{
				double value(0);
				if (!args.isEmpty())
				{
					int id(args[0].toInt());
					switch (id)
					{
						case M1CT:
							value = monitor1Preset->presetValue();
							break;
						case M2CT:
							value = monitor2Preset->presetValue();
							break;
						case M3CT:
							value = monitor3Preset->presetValue();
							break;
						case M4CT:
							if (m_meas->setupType() != Measurement::Mdll && m_meas->setupType() != Measurement::Mdll2)
								value = monitor4Preset->presetValue();
							break;
						case EVCT:
							value = eventsPreset->presetValue();
							break;
						case TCT:
							value = timerPreset->presetValue();
							break;
					}
				}
				else
				{
					if (timerPreset->isChecked())
						value = timerPreset->presetValue();
					else if (eventsPreset->isChecked())
						value = eventsPreset->presetValue();
					else if (monitor1Preset->isChecked())
						value = monitor1Preset->presetValue();
					else if (monitor2Preset->isChecked())
						value = monitor2Preset->presetValue();
					else if (monitor3Preset->isChecked())
						value = monitor3Preset->presetValue();
					else if (monitor4Preset->isChecked())
						value = monitor4Preset->presetValue();
				}
				answer << value;
			}
			break;
		case CommandEvent::C_COUNTER_SELECTED:
			{
				int id(args[0].toInt());
				bool value(false);
				switch (id)
				{
					case M1CT:
						value = monitor1Preset->isChecked();
						break;
					case M2CT:
						value = monitor2Preset->isChecked();
						break;
					case M3CT:
						value = monitor3Preset->isChecked();
						break;
					case M4CT:
						if (m_meas->setupType() != Measurement::Mdll && m_meas->setupType() != Measurement::Mdll2)
							value = monitor4Preset->isChecked();
						break;
					case EVCT:
						value = eventsPreset->isChecked();
						break;
					case TCT:
						value = timerPreset->isChecked();
						break;
					default:
						break;
				}
				answer << value;
			}
			break;
		case CommandEvent::C_SELECT_COUNTER:
			{
				bool bEnabled(true);
				if (args.count() > 1 && args[1].canConvert(QVariant::Bool))
					bEnabled = args[1].toBool();
				switch (args[0].toInt())
				{
					case M1CT:
						monitor1Preset->setChecked(bEnabled);
						break;
					case M2CT:
						monitor2Preset->setChecked(bEnabled);
						break;
					case M3CT:
						monitor3Preset->setChecked(bEnabled);
						break;
					case M4CT:
						if (m_meas->setupType() != Measurement::Mdll && m_meas->setupType() != Measurement::Mdll2)
							monitor4Preset->setChecked(bEnabled);
						break;
					case EVCT:
						eventsPreset->setChecked(bEnabled);
						break;
					case TCT:
						timerPreset->setChecked(bEnabled);
						break;
				}
				if (args.count() > 2 && args[2].canConvert(QVariant::Double))
				{
					double dblPreset = args[2].toDouble();
					switch (args[0].toInt())
					{
						case M1CT:
							monitor1Preset->setPresetValue(dblPreset);
							m_meas->setPreset(MON1ID, monitor1Preset->presetValue(), true);
							break;
						case M2CT:
							monitor2Preset->setPresetValue(dblPreset);
							m_meas->setPreset(MON2ID, monitor2Preset->presetValue(), true);
							break;
						case M3CT:
							monitor3Preset->setPresetValue(dblPreset);
							m_meas->setPreset(MON3ID, monitor3Preset->presetValue(), true);
							break;
						case M4CT:
							if (m_meas->setupType() != Measurement::Mdll && m_meas->setupType() != Measurement::Mdll2)
							{
								monitor4Preset->setPresetValue(dblPreset);
								m_meas->setPreset(MON4ID, monitor4Preset->presetValue(), true);
							}
							break;
						case EVCT:
							eventsPreset->setPresetValue(dblPreset);
							m_meas->setPreset(EVID, eventsPreset->presetValue(), true);
							break;
						case TCT:
							timerPreset->setPresetValue(dblPreset);
							m_meas->setPreset(TIMERID, quint64(timerPreset->presetValue() * 1000), true);
							break;
					}
				}
			}
			break;
		case CommandEvent::C_SET_PRESELECTION:
			{
				double dblPreset = args[0].toDouble();
				if (args.count() > 1)
				{
					int id(args[1].toInt());
					switch (id)
					{
						case M1CT:
							monitor1Preset->setPresetValue(dblPreset);
							m_meas->setPreset(MON1ID, dblPreset, true);
							break;
						case M2CT:
							monitor2Preset->setPresetValue(dblPreset);
							m_meas->setPreset(MON2ID, dblPreset, true);
							break;
						case M3CT:
							monitor3Preset->setPresetValue(dblPreset);
							m_meas->setPreset(MON3ID, dblPreset, true);
							break;
						case M4CT:
							if (m_meas->setupType() != Measurement::Mdll && m_meas->setupType() != Measurement::Mdll2)
							{
								monitor4Preset->setPresetValue(dblPreset);
								m_meas->setPreset(MON4ID, dblPreset, true);
							}
							break;
						case EVCT:
							eventsPreset->setPresetValue(dblPreset);
							m_meas->setPreset(EVID, dblPreset, true);
							break;
						case TCT:
							timerPreset->setPresetValue(dblPreset);
							m_meas->setPreset(TIMERID, quint64(dblPreset * 1000), true);
							break;
						default:
							break;
					}
				}
				else
				{
					if (timerPreset->isChecked())
					{
						timerPreset->setPresetValue(dblPreset);
						m_meas->setPreset(TIMERID, quint64(dblPreset * 1000), true);
					}
					else if (eventsPreset->isChecked())
					{
						eventsPreset->setPresetValue(dblPreset);
						m_meas->setPreset(EVID, dblPreset, true);
					}
					else if (monitor1Preset->isChecked())
					{
						monitor1Preset->setPresetValue(dblPreset);
						m_meas->setPreset(MON1ID, dblPreset, true);
					}
					else if (monitor2Preset->isChecked())
					{
						monitor2Preset->setPresetValue(dblPreset);
						m_meas->setPreset(MON2ID, dblPreset, true);
					}
					else if (monitor3Preset->isChecked())
					{
						monitor3Preset->setPresetValue(dblPreset);
						m_meas->setPreset(MON3ID, dblPreset, true);
					}
					else if (monitor4Preset->isChecked() && m_meas->setupType() != Measurement::Mdll && m_meas->setupType() != Measurement::Mdll2)
					{
						monitor4Preset->setPresetValue(dblPreset);
						m_meas->setPreset(MON4ID, dblPreset, true);
					}
				}
			}
			break;
		case CommandEvent::C_READ_COUNTER:
			{
				double value(0);
				int id(args[0].toInt());
				switch (id)
				{
					case M1CT:
						value = m_meas->mon1();
						break;
					case M2CT:
						value = m_meas->mon2();
						break;
					case M3CT:
						value = m_meas->mon3();
						break;
					case M4CT:
						value = m_meas->mon4();
						break;
					case EVCT:
						value = m_meas->events();
						break;
					case TCT:
						value = m_meas->timer() / 1000.0;
						break;
					default:
						break;
				}
				answer  << value;
			}
			break;
		case CommandEvent::C_READ_DIFFRACTOGRAM:
			{
				Spectrum *tmpSpectrum = m_meas->spectrum(Measurement::Diffractogram);
				if (tmpSpectrum->width() > 0)
				{
					for (int i = 0; i < tmpSpectrum->width(); ++i)
						answer << tmpSpectrum->value(i);
				}
				else
					answer << m_meas->events();
			}
			break;
		case CommandEvent::C_READ_HISTOGRAM_SIZE:
			{
				if (m_meas->setupType() == Measurement::Mstd)
				{
					Spectrum *tmpSpectrum = m_meas->spectrum(Measurement::SingleTubeSpectrum);
					answer << tmpSpectrum->width();
					answer << 1;
				}
				else
				{
					Measurement::HistogramType id = static_cast<Measurement::HistogramType>(args[0].toInt());
					switch (id)
					{
						case Measurement::PositionHistogram:
						case Measurement::AmplitudeHistogram:
						case Measurement::CorrectedPositionHistogram:
							{
								Histogram *tmpHistogram = m_meas->hist(id);
								answer << tmpHistogram->width();       // width  (should be equal to number of MPSD inputs)
								answer << tmpHistogram->height();      // width  (should be equal to number of MPSD inputs)
								answer << (m_meas->width() + 1);    // height (should be 960)
							}
							break;
						default:
							answer << 0 << 0 << 0;
							break;
					}
				}
			}
			break;
		case CommandEvent::C_READ_HISTOGRAM:
			{
				QList<quint64>* tmpData = new QList<quint64>();
				if (m_meas->setupType() == Measurement::Mstd)
				{
					Spectrum *tmpSpectrum = m_meas->spectrum(Measurement::SingleTubeSpectrum);
					for (int x = 0; x < tmpSpectrum->width(); ++x)
						tmpData->append(tmpSpectrum->value(x));
				}
				else
				{
					Measurement::HistogramType id = static_cast<Measurement::HistogramType>(args[0].toInt());
					switch (id)
					{
						case Measurement::PositionHistogram:
						case Measurement::AmplitudeHistogram:
						case Measurement::CorrectedPositionHistogram:
							{
								Histogram *tmpHistogram = m_meas->hist(id);
								if (tmpHistogram->height() > 0 && tmpHistogram->width() > 0)
								{
									// CARESS has it's x=0:y=0 position at top left corner
									for (int y = tmpHistogram->height() - 1; y >= 0; --y)
										for (int x = 0; x < tmpHistogram->width(); ++x)
											tmpData->append(tmpHistogram->value(x, y));
								}
								else
									tmpData->append(m_meas->events());
							}
							break;
						default:
							break;
					}
				}
				// hack to transfer a QList<quint64> to QtInterface without to copy it
				answer << ((quint64)tmpData);
			}
			break;
		case CommandEvent::C_READ_SPECTROGRAM:
			{
				Histogram *tmpHistogram = m_meas->hist(Measurement::PositionHistogram);
				Spectrum* tmpSpectrum = NULL;
				int i(-1);
				if (!args.isEmpty())
				{
					i = args[0].toInt();
					if (i < 0 || i > tmpHistogram->height())
						i =- 1;
				}
				if (i >= 0)
					tmpSpectrum = tmpHistogram->spectrum(i);
				else
					tmpSpectrum = tmpHistogram->xSumSpectrum();
				if (tmpSpectrum->width() > 0)
				{
					for (int x = 0; x < tmpSpectrum->width(); ++x)
						answer << tmpSpectrum->value(x);
				}
				else
					answer << m_meas->events();
			}
			break;
		case CommandEvent::C_STATUS:
			{
				bool bRunning, bRunAck = false;
				int i;
				bRunning = m_meas->hwstatus(&bRunAck);
				i = (bRunning || (startStopButton->isChecked() != 0) || (m_meas->status() == Measurement::Started)) ? 1 : 0;
				answer << i << bRunAck;
			}
			break;
		case CommandEvent::C_SET_LISTMODE:
			acquireListFile->setChecked(args[0].toBool());
			if (args.size() > 1)
				m_meas->setWriteProtection(args[1].toBool());
			break;
		case CommandEvent::C_GET_CONFIGFILE:
			{
				QFileInfo fi = m_meas->getConfigfilename();
				answer << fi.fileName();
			}
			break;
		case CommandEvent::C_SET_CONFIGFILE:
			{
				// TODO Should we check for an file extension like 'mcfg'?
				QString fn = args[0].toString();
				QFileInfo fi(m_meas->getConfigfilepath(), fn);
				if (fi.isFile() && fi.isReadable())
					loadConfiguration(fi.filePath());
				else
					MSG_ERROR << "Could not find : " << fi.absoluteFilePath();
			}
			break;
		case CommandEvent::C_SET_SAVEHISTOGRAM:
			autoSaveHistogram->setChecked(args[0].toBool());
			break;
		case CommandEvent::C_SET_LISTHEADER:
			{
				QByteArray header((const char*)args[0].toULongLong(), args[1].toInt());
				bool bInsertHeaderLength(true);
				if (args.size() > 2 && args[2].canConvert(QVariant::Bool))
					bInsertHeaderLength = args[2].toBool();
				m_meas->setListFileHeader(header, bInsertHeaderLength);
			}
			break;
		case CommandEvent::C_MAPCORRECTION: // mapping and correction data
			{
				MapCorrection *&pMap = m_meas->posHistMapCorrection();
				if (!args.isEmpty())
				{
					MappedHistogram *pHist = reinterpret_cast<MappedHistogram *>(m_meas->hist(Measurement::CorrectedPositionHistogram));
					MapCorrection *pNewMap = dynamic_cast<MapCorrection *>((QObject*)args[0].toULongLong());
					if (pNewMap == NULL)
					{
						QSize size(m_meas->width(),m_meas->height());
						pNewMap = new LinearMapCorrection(size, size, MapCorrection::OrientationDownRev);
					}
					if (pNewMap != NULL)
					{
						// new mapping and correction data
						QRect mapRect;
						if (pMap != NULL)
							delete pMap;
						pMap = pNewMap;
						mapRect = pMap->getMapRect();
						if (pHist == NULL)
							pHist = new MappedHistogram(pMap);
						else
							pHist->setMapCorrection(pMap, m_meas->hist(Measurement::PositionHistogram));
					}
					else
					{
						// delete existing mapping
						delete pHist;
						pHist = NULL;
						delete pMap;
						pMap = NULL;
					}
				}
				else
				{
					// query for current mapping and correction data
					answer << ((quint64)pMap);
				}
			}
			break;
		case CommandEvent::C_MAPPEDHISTOGRAM: // mapped and corrected position histogram
			{
				MappedHistogram *pHist = reinterpret_cast<MappedHistogram *>(m_meas->hist(Measurement::CorrectedPositionHistogram));
				answer << ((quint64)pHist);
				break;
			}
		case CommandEvent::C_GET_RUNID:
			{
				quint32 tmp = m_meas->runId();
				bool b = m_meas->getAutoIncRunId();
				answer << (tmp) << b;
			}
			break;
		case CommandEvent::C_SET_RUNID:
			{
				int id(args[0].toInt());
				m_meas->setRunId(id);
				if (args.size() > 1)
					m_meas->setAutoIncRunId(args[1].toBool());
			}
			break;
		case CommandEvent::C_GET_LISTMODE:
			{
				bool bListmode = acquireListFile->isChecked();
				bool bWriteProtect = m_meas->getWriteProtection();
				answer << bListmode << bWriteProtect;
			}
			break;
		case CommandEvent::C_GET_SAVEHISTOGRAM:
			{
				bool bAutoSaveHistogram = autoSaveHistogram->isChecked();
				answer << bAutoSaveHistogram;
			}
			break;
		case CommandEvent::C_UPDATEMAINWIDGET:
			if (args.count() >= 4)
			{
				bool b;
				int i;
				QString s;

				QVariant& v = args[0];
				i = v.toInt(&b);
				if (!b || i <= 0)
					s = "unknown";
				else
					s = v.toString();
				caressWidth->setText(s);
				v = args[1];
				i = v.toInt(&b);
				if (!b || i <= 0)
					s = "unknown";
				else
					s = v.toString();
				caressHeight->setText(s);
				v = args[2];
				i = v.toInt(&b);
				if (!b || i <= 0)
					s = (i == 0) ? "scratch file" : "unknown";
				else
					s = v.toString();
				caressRun->setText(s);
				v = args[3];
				s = v.toString();
				caressActive->setText(s);
			}
			break;
		case CommandEvent::C_VERSIONTEXT:
			answer << QString("QMesyDAQ " VERSION ",lib %1").arg(m_meas->version());
			break;
		default:
			MSG_DEBUG << tr("ignoring invalid interface command %1").arg(cmd) << args;
			return;
	}
	interface->postCommandToInterface(cmd, answer);
}

/*!
    \fn void MainWidget::moduleHistogramSlot(quint8 id, bool set)

     ????

    \param id
    \param set
 */
void MainWidget::moduleHistogramSlot(quint8 id, bool set)
{
	MSG_DEBUG << tr("MainWidget::moduleHistogramSlot %1 %2").arg(id).arg(set);
	m_theApp->setHistogram(devid_2->value(), id, set);
}

/*!
    \fn void MainWidget::moduleActiveSlot(quint8 id, bool set)
     ????

    \param id
    \param set
 */
void MainWidget::moduleActiveSlot(quint8 id, bool set)
{
	m_theApp->setActive(devid_2->value(), id, set);
}

/*!
    sets the user mode

    \param val new user mode
 */
void MainWidget::selectUserMode(int val)
{
	switch (val)
	{
		case MainWidget::User :
			realTimeLabel->setHidden(true);
			slidingFrame->setHidden(true);
			break;
		case MainWidget::Expert:
			realTimeLabel->setVisible(true);
			slidingFrame->setHidden(true);
			break;
		case MainWidget::SuperUser:
			realTimeLabel->setVisible(true);
			slidingFrame->setVisible(true);
			break;
		default:
			break;
	}
}

/*!
    \fn void MainWidget::dispAllChannelsChanged(bool val)

    callback to set the display mode in case of all spectra option is selected or deselected

    \param val selected or not
 */
void MainWidget::dispAllChannelsChanged(bool val)
{
	if (val)
		setDisplayMode(Plot::ModuleSpectrum);
	else
		setDisplayMode(displayModeButtonGroup->checkedId());
}

/*!
    \fn void MainWidget::sumSpectra(bool val)

    callback to enable/disable sum/single spectrum

    \param val true = sum spectrum, false single spectrum
 */
void MainWidget::sumSpectra(bool val)
{
	if (!val)
		displayMcpdSlot(dispMcpd->value());
	else
		dispAllChannels->setChecked(false);
	emit redraw();
}
