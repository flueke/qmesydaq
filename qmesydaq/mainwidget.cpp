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
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QMessageBox>
#include <QFileDialog>
#include <QPainter>
#include <QPen>
#include <QRadioButton>
#include <QHostAddress>
#include <QTimer>

#include <qwt_plot_curve.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_color_map.h>
#include <qwt_scale_widget.h>

#include <cmath>

#include "mainwidget.h"
#include "mdefines.h"
#include "measurement.h"
#include "mesydaq2.h"
#include "histogram.h"
#include "mesydaqdata.h"

#if TACO
#	include "tacocontrol.h"
#elif CARESS
#	include "caresscontrol.h"
#endif

/*!
    \fn MainWidget::MainWidget(Mesydaq2 *, QWidget *parent = 0)

    constructor

    \param mesy Mesydaq2 object to control the hardware
    \param parent Qt parent object
*/
MainWidget::MainWidget(Mesydaq2 *mesy, QWidget *parent)
	: QWidget(parent)
	, Ui_Mesydaq2MainWidget()
	, m_theApp(mesy)
	, m_width(960)
	, m_dispThresh(false)
	, m_dispLoThresh(0)
	, m_dispHiThresh(0)
	, m_dispLog(false)
	, m_curve(NULL)
	, m_histogram(NULL)
	, m_data(NULL)
	, m_histData(NULL)
	, m_meas(NULL)
	, m_dispTimer(0)
	, m_zoomer(NULL)
	, m_zoomEnabled(false)
	, m_cInt(NULL)
{
	m_meas = new Measurement(mesy, this);
	setupUi(this);

//	deviceId->setMaximum(mesy->numMCPD() - 1);
	dispMcpd->setMaximum(mesy->numMCPD() - 1);
	devid->setMaximum(mesy->numMCPD() - 1);
	devid_2->setMaximum(mesy->numMCPD() - 1);
	paramId->setMaximum(mesy->numMCPD() - 1);
	mcpdId->setMaximum(mesy->numMCPD() - 1);
	
        connect(acqListfile, SIGNAL(toggled(bool)), m_theApp, SLOT(acqListfile(bool)));
        connect(allPulsersoffButton, SIGNAL(clicked()), this, SLOT(allPulserOff()));
        connect(m_theApp, SIGNAL(statusChanged(const QString &)), daqStatusLine, SLOT(setText(const QString &)));
        connect(m_meas, SIGNAL(stopSignal(bool)), startStopButton, SLOT(animateClick()));
//	connect(this, SIGNAL(setCounter(quint32, quint64)), m_meas, SLOT(setCounter(quint32, quint64)));
	connect(devid, SIGNAL(valueChanged(int)), devid_2, SLOT(setValue(int)));
	connect(devid, SIGNAL(valueChanged(int)), this, SLOT(displayMpsdSlot(int)));
	connect(devid_2, SIGNAL(valueChanged(int)), this, SLOT(displayMpsdSlot(int)));
	connect(acquireFile, SIGNAL(toggled(bool)), this, SLOT(checkListfilename(bool)));
	
	channelLabel->setHidden(comgain->isChecked());
	channel->setHidden(comgain->isChecked());
	scanPeriSlot();

	versionLabel->setText("QMesyDAQ " VERSION " " __DATE__);

	QRegExp ex("(([01]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])\\.){3}([01]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])"); 
	mcpdIPAddress->setValidator(new QRegExpValidator(ex, mcpdIPAddress));
	dataIPAddress->setValidator(new QRegExpValidator(ex, dataIPAddress));
	cmdIPAddress->setValidator(new QRegExpValidator(ex, cmdIPAddress));

	dataFrame->setAxisTitle(QwtPlot::xBottom, "channels");
	dataFrame->setAxisTitle(QwtPlot::yLeft, "counts");
	dataFrame->setAxisTitle(QwtPlot::yRight, "intensity");
	dataFrame->enableAxis(QwtPlot::yRight, false);
//	dataFrame->plotLayout()->setAlignCanvasToScales(true);
	dataFrame->setAxisScale(QwtPlot::xBottom, 0, 959);
	dataFrame->replot();

	m_zoomer = new QwtPlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft, QwtPicker::DragSelection, QwtPicker::ActiveOnly, dataFrame->canvas());

	connect(m_zoomer, SIGNAL(selected(const QwtDoubleRect &)), this, SLOT(zoomAreaSelected(const QwtDoubleRect &)));
        connect(m_zoomer, SIGNAL(zoomed(const QwtDoubleRect &)), this, SLOT(zoomed(const QwtDoubleRect &)));

#if 0
#if QT_VERSION < 0x040000
	m_zoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlButton);
#else
	m_zoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier);
#endif
	m_zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);
#endif
	m_zoomer->setRubberBandPen(QColor(Qt::black));
	m_zoomer->setTrackerPen(QColor(Qt::black));
	m_zoomer->setEnabled(true);

	m_curve = new QwtPlotCurve("");
	m_curve->setStyle(QwtPlotCurve::Steps);
#if QT_VERSION >= 0x040000
	m_curve->setRenderHint(QwtPlotItem::RenderAntialiased);
#endif
	m_curve->setPen(QPen(Qt::black));
	m_curve->attach(dataFrame);

	m_data = new MesydaqSpectrumData();
	m_curve->setData(*m_data);

	m_histogram = new QwtPlotSpectrogram();
	QwtLinearColorMap colorMap(Qt::darkBlue, Qt::darkRed);
	colorMap.addColorStop(0.2, Qt::blue);
	colorMap.addColorStop(0.4, Qt::green);
	colorMap.addColorStop(0.6, Qt::yellow);
	colorMap.addColorStop(0.8, Qt::red);
	m_histogram->setColorMap(colorMap);

	dataFrame->axisWidget(QwtPlot::yRight)->setColorBarEnabled(true);
	m_histData = new MesydaqHistogramData();
	m_histogram->setData(*m_histData);

	displayMcpdSlot();
	dispFiledata();
#if TACO
	m_cInt = new TACOControl(this);
#elif CARESS
	m_cInt = new CARESSControl(this);
#endif
// display refresh timer
	m_dispTimer = startTimer(1000);
}

//! destructor
MainWidget::~MainWidget()
{
	if (m_dispTimer)
		killTimer(m_dispTimer);
	m_dispTimer = 0;

	delete m_meas;
	m_meas = NULL;
}

/*!
    \fn MainWidget::timerEvent(QTimerEvent *)

    callback for the timer
*/
void MainWidget::timerEvent(QTimerEvent * /* event */)
{
	draw();
}

/*!
    \fn MainWidget::allPulserOff(void)

    callback to switch all pulsers off 
*/
void MainWidget::allPulserOff(void)
{
	m_theApp->allPulserOff();
	pulserButton->setChecked(false);
}

void MainWidget::zoomAreaSelected(const QwtDoubleRect &)
{
        if(!m_zoomEnabled)
        {
                m_zoomer->setZoomBase();
                m_zoomEnabled = true;
        }
}

void MainWidget::zoomed(const QwtDoubleRect &rect)
{
        if(rect == m_zoomer->zoomBase())
        {
                dataFrame->setAxisAutoScale(QwtPlot::yLeft);
//		dataFrame->setAxisAutoScale(QwtPlot::xBottom);
		dataFrame->setAxisScale(QwtPlot::xBottom, 0, 959);
                dataFrame->replot();
                m_zoomEnabled = false;
        }
}

void MainWidget::startStopSlot(bool checked)
{
	if(checked)
	{
		// get timing binwidth
		m_theApp->setTimingwidth(timingBox->value());
		
		// get latest preset entry
		if(m_meas->isMaster(TCT))
			m_meas->setPreset(TCT, tPreset->value() * 1000, true);
		if(m_meas->isMaster(EVCT))
			m_meas->setPreset(EVCT, evPreset->value(), true);
		if(m_meas->isMaster(M1CT))
			m_meas->setPreset(M1CT, m1Preset->value(), true);
		if(m_meas->isMaster(M2CT))
			m_meas->setPreset(M2CT, m2Preset->value(), true);
		
		startStopButton->setText("Stop");
		// set device id to 0 -> will be filled by mesydaq for master
		m_meas->start(); 
	}
	else
	{
		startStopButton->setText("Start");
		// set device idto 0 -> will be filled by mesydaq for master
		m_meas->stop();
	}
}


void MainWidget::sendCellSlot()
{
	quint16 id = mcpdId->value();
	m_theApp->setCounterCell(id, cellSource->currentIndex(), cellTrigger->currentIndex(), cellCompare->value());
}

void MainWidget::sendParamSlot()
{
	qint16 id = mcpdId->value();	
	m_theApp->setParamSource(id, param->value(), paramSource->currentIndex()); 
}

void MainWidget::sendAuxSlot()
{
	m_theApp->protocol("set aux timer", NOTICE);
	bool ok;
	quint16 compare = (quint16)compareAux->text().toInt(&ok, 0);
	m_theApp->setAuxTimer(mcpdId->value(), timer->value(), compare); 
}

void MainWidget::resetTimerSlot()
{
	quint16 id = mcpdId->value();
	m_theApp->protocol("reset timer", NOTICE);
	m_theApp->setMasterClock(id, 0LL); 
}

void MainWidget::setTimingSlot()
{
	quint16 id = mcpdId->value();
	resetTimer->setEnabled(master->isChecked());
	m_theApp->protocol("set timing", NOTICE);
	m_theApp->setTimingSetup(id, master->isChecked(), terminate->isChecked());
}

void MainWidget::setMcpdIdSlot()
{
	m_theApp->setId(mcpdId->value(), deviceId->value()); 
}

void MainWidget::setStreamSlot()
{
#warning TODO  MainWidget::setStreamSlot()
#if 0	
	unsigned short id = (unsigned short) deviceId->value();	
	m_cmdBuffer[0] = mcpdId->value();
	m_cmdBuffer[1] = QUIET;
	if(statusStream->isChecked())
		m_cmdBuffer[2] = 1;
	else
		m_cmdBuffer[2] = 0;	
	m_pstring.sprintf("Set stream %d", m_cmdBuffer[2]);
	m_theApp->protocol(m_pstring, 2);
	m_theApp->sendCommand(m_pBuffer);

	m_theApp->setStream(mcpdId->value(), statusStream->isChecked());
#endif
}

void MainWidget::setIpUdpSlot()
{
	quint16 id =  mcpdId->value();
	QString mcpdIP = modifyIp->isChecked() ? mcpdIPAddress->text() : "0.0.0.0",
		cmdIP = !cmdThisPc->isChecked() ? cmdIPAddress->text() : "0.0.0.0",
		dataIP = !dataThisPc->isChecked() ? dataIPAddress->text() : "0.0.0.0";
	quint16 cmdPort = (quint16) cmdUdpPort->value(),
		dataPort = (quint16) dataUdpPort->value();
	m_theApp->setProtocol(id, mcpdIP, dataIP, dataPort, cmdIP, cmdPort);	
}

void MainWidget::setPulserSlot()
{
	bool ok;
	quint16 id = (quint16) devid->value();	
	quint16 mod = module->value();
	quint16 chan = pulsChan->value();
	
	quint8 ampl;
	if(pulsampRadio1->isChecked())
		ampl = (quint8) pulsAmp1->text().toInt(&ok);
	else
		ampl = (quint8) pulsAmp2->text().toInt(&ok);
	
	quint16 pos = MIDDLE;
	if(pulsLeft->isChecked())
		pos = LEFT;
	else if(pulsRight->isChecked())
		pos = RIGHT;
	else if(pulsMid->isChecked())
		pos = MIDDLE;
	
	bool pulse = pulserButton->isChecked();
	if (pulse)
		const_cast<QPalette &>(pulserButton->palette()).setColor(QPalette::ButtonText, QColor(Qt::red));
	else
		const_cast<QPalette &>(pulserButton->palette()).setColor(QPalette::ButtonText, QColor(Qt::black));
		
	m_theApp->setPulser(id, mod, chan, pos, ampl, pulse); 
}

void MainWidget::setGainSlot()
{
	bool 	ok;
	quint16 chan = comgain->isChecked() ? 8 : channel->text().toUInt(&ok, 0),
		id = (quint16) devid->value(),
		addr = module->value();
	float 	gainval = gain->text().toFloat(&ok);
	m_theApp->setGain(id, addr, chan, gainval); 
}

void MainWidget::setThresholdSlot()
{
	bool ok;
	quint16 id = (quint16) devid->value();	
	quint16 addr = module->value();
	quint16 thresh = threshold->text().toUInt(&ok, 0);
	m_theApp->setThreshold(id, addr, thresh); 
}

void MainWidget::selectListfileSlot()
{
	QString name = QFileDialog::getSaveFileName(this, tr("Save as..."), m_theApp->getListfilepath(), 
			"mesydaq data files (*.mdat);;all files (*.*);;really all files (*)");
  	if(!name.isEmpty())
	{
    		int i = name.indexOf(".mdat");
		if(i == -1)
			name.append(".mdat");
 		listFilename->setText(name);
		m_theApp->setListfilename(name);
  	}
	dispFiledata();
}

void MainWidget::checkListfilename(bool checked)
{
	if (checked && listFilename->text().isEmpty())
	{
		selectListfileSlot();
		if (listFilename->text().isEmpty())
			acquireFile->setChecked(false);
	}
}	

/*!
    \fn MainWidget::update(void)
 */
void MainWidget::update(void)
{
   	quint16 id = (quint16) paramId->value();	
	dataRx->setText(tr("%1").arg(m_theApp->receivedData()));
	cmdTx->setText(tr("%1").arg(m_theApp->sentCmds()));
	cmdRx->setText(tr("%1").arg(m_theApp->receivedCmds()));
	hTimeText->setText(buildTimestring(m_meas->getHeadertime(), true));
	mTimeText->setText(buildTimestring(m_meas->getMeastime(), false));   
    
// parameter values for selected ID
	param0->setText(tr("%1").arg(m_theApp->getParameter(id, 0)));
	param1->setText(tr("%1").arg(m_theApp->getParameter(id, 1)));
	param2->setText(tr("%1").arg(m_theApp->getParameter(id, 2)));
	param3->setText(tr("%1").arg(m_theApp->getParameter(id, 3)));
	m_meas->calcMeanRates();
    
// measurement values counters and rates
	tSecsText->setText(tr("%1").arg(m_meas->getCounter(TCT) / 1000.));
	totalCounts->setText(tr("%1").arg(m_meas->getCounter(EVCT)));
	eventRate->setText(tr("%1").arg(m_meas->getRate(EVCT)));
	monitor1->setText(tr("%1").arg(m_meas->getCounter(M1CT)));
	monRate1->setText(tr("%1").arg(m_meas->getRate(M1CT)));
	monitor2->setText(tr("%1").arg(m_meas->getCounter(M2CT)));
	monRate2->setText(tr("%1").arg(m_meas->getRate(M2CT)));
	monitor3->setText(tr("%1").arg(m_meas->getCounter(M3CT)));
	monRate3->setText(tr("%1").arg(m_meas->getRate(M3CT)));
	monitor4->setText(tr("%1").arg(m_meas->getCounter(M4CT)));
	monRate4->setText(tr("%1").arg(m_meas->getRate(M4CT)));
}

/*!
    \fn MainWidget::buildTimestring(quint64 timeval, bool nano)
 */
QString MainWidget::buildTimestring(quint64 timeval, bool nano)
{
// nsec = time in 100 nsecs
//-> usec = 
//->
	QString str;
	quint64 val;
	ulong nsec, sec, min, hr;
// calculate raw seconds
	if(nano)
	{
		val = timeval / 10000000;
		nsec = timeval - (10000000 * val);
	}
	else
	{
		val = timeval / 1000;
		nsec = timeval - (1000 * val);
	}
//	qDebug("%d %d %d", timeval, val, nsec);
// hours = val / 3600 (s/h)
	hr = val / 3600;
// remaining seconds:
	val -= hr * 3600;
// minutes:
	min = val / 60;
// remaining seconds:
	sec = val - (min * 60);
//	qDebug("%lu %lu %lu %lu %lu", nsecs, hr, min, sec, nsec);
	str.sprintf("%02lu:%02lu:%02lu", hr, min, sec);
	return str;
}

void MainWidget::clearAllSlot()
{
	m_meas->clearAllHist();
	draw();
}

void MainWidget::clearMcpdSlot()
{
	quint32 start = dispMcpd->value() * 64;
	for(quint32 i = start; i < start + 64; i++)
		m_meas->clearChanHist(i);
	draw();
}

void MainWidget::clearMpsdSlot()
{
	quint32 start = dispMpsd->value() * 8 + dispMcpd->value() * 64;
//	qDebug("clearMpsd: %d", start);
	for(quint32 i = start; i < start + 8; i++)
		m_meas->clearChanHist(i);
	draw();
}

void MainWidget::clearChanSlot()
{
	ulong chan = dispChan->value() + dispMpsd->value() * 8 + dispMcpd->value() * 64;
	m_meas->clearChanHist(chan);
	draw();
}


void MainWidget::replayListfileSlot()
{
	QString name = QFileDialog::getOpenFileName(this, "Load...", m_theApp->getListfilepath(), "mesydaq data files (*.mdat);;all files (*.*);;really all files (*)");
	if(!name.isEmpty())
		m_meas->readListfile(name);
}

void MainWidget::setRunIdSlot()
{
	quint16 runid = (quint16) devid->value();
#warning TODO 0 !!!!
	m_theApp->setRunId(0, runid); 
	m_theApp->protocol(tr("Set run ID to %1").arg(runid), NOTICE);
}

/*!
    \fn MainWidget::displayMcpdSlot(int)
 */
void MainWidget::displayMcpdSlot(int id)
{
// retrieve displayed ID
	if (!m_theApp->numMCPD())
		return;
	if (id < 0)
		id = mcpdId->value();

// store the current termination value it will be change if switch from master to slave
	bool term = m_theApp->isTerminated(id);
	master->setChecked(m_theApp->isMaster(id));
	if (!master->isChecked())
		terminate->setChecked(term);
     
// now get and display parameters:
	quint16 values[4];
    
// get cell parameters
	m_theApp->getCounterCell(id, cellSource->currentIndex(), values);
	cellTrigger->setCurrentIndex(values[0]);
	cellCompare->setValue(values[1]);
    
// get parameter settings
	paramSource->setCurrentIndex(m_theApp->getParamSource(id, param->value()));

// get timer settings
	compareAux->setText(tr("%1").arg(m_theApp->getAuxTimer(id, timer->value()), 0, 16)); 
	
// get stream setting
//	statusStream->setChecked(m_theApp->myMcpd[id]->getStream());	
}

/*!
    \fn MainWidget::displayMpsdSlot(int)
 */
void MainWidget::displayMpsdSlot(int)
{
	QString dstr;
    
// retrieve displayed ID
	quint8 id = devid_2->value();
	QList<int> modList;
	for (int i = 0; i < 8; ++i)
		if (m_theApp->getMpsdId(id, i))
			modList << i;

	module->setModuleList(modList);
	dispMpsd->setModuleList(modList);

// firmware version
	firmwareVersion->setText(tr("%1").arg(m_theApp->getFirmware(id)));
    
// Status display:
	status0->setText(tr("%1\n%2").arg(m_theApp->getMpsdType(id, 0)).arg(m_theApp->getMpsdVersion(id, 0)));
	status1->setText(tr("%1\n%2").arg(m_theApp->getMpsdType(id, 1)).arg(m_theApp->getMpsdVersion(id, 1)));
	status2->setText(tr("%1\n%2").arg(m_theApp->getMpsdType(id, 2)).arg(m_theApp->getMpsdVersion(id, 2)));
	status3->setText(tr("%1\n%2").arg(m_theApp->getMpsdType(id, 3)).arg(m_theApp->getMpsdVersion(id, 3)));
	status4->setText(tr("%1\n%2").arg(m_theApp->getMpsdType(id, 4)).arg(m_theApp->getMpsdVersion(id, 4)));
	status5->setText(tr("%1\n%2").arg(m_theApp->getMpsdType(id, 5)).arg(m_theApp->getMpsdVersion(id, 5)));
	status6->setText(tr("%1\n%2").arg(m_theApp->getMpsdType(id, 6)).arg(m_theApp->getMpsdVersion(id, 6)));
	status7->setText(tr("%1\n%2").arg(m_theApp->getMpsdType(id, 7)).arg(m_theApp->getMpsdVersion(id, 7)));
		
	id = /*mcpdId */devid->value();
	quint8 mod = module->value();
	quint8 chan = channel->value();
// gain:
	gain->setText(tr("%1").arg(double(m_theApp->getGain(id, mod, chan)), 4, 'f', 2));	
	
// threshold:
	threshold->setText(tr("%1").arg(m_theApp->getThreshold(id, mod)));
		
// pulser:  on/off
	if(m_theApp->isPulserOn(id, mod))
	{
		pulserButton->setChecked(true);
		const_cast<QPalette &>(pulserButton->palette()).setColor(QPalette::ButtonText, QColor(Qt::red));
	}		
	else
	{
		pulserButton->setChecked(false);
		const_cast<QPalette &>(pulserButton->palette()).setColor(QPalette::ButtonText, QColor(Qt::black));
	}
// channel
	pulsChan->setValue((int)m_theApp->getPulsChan(id, mod));
// amplitude
	dstr.sprintf("%3d", m_theApp->getPulsAmp(id, mod));
//	pulsAmp->setValue((int)m_theApp->getPulsAmp(id, mod));
// position
	switch(m_theApp->getPulsPos(id, mod))
	{
		case LEFT:
			pulsLeft->setChecked(true);
			break;
		case RIGHT:
			pulsRight->setChecked(true);
			break;
		case MIDDLE:
			pulsMid->setChecked(true);
			break;
	}
// mode
	if(m_theApp->getMode(id, mod))
		amp->setChecked(true);
	else
		Ui_Mesydaq2MainWidget::pos->setChecked(true);
}

void MainWidget::scanPeriSlot()
{
	quint16 id = devid_2->value();
	m_theApp->scanPeriph(id);
	displayMpsdSlot();
}

void MainWidget::setModeSlot(int mode)
{
	m_theApp->setMode(devid->value(), module->value(), mode); 
}

void MainWidget::saveSetupSlot()
{
	QString name = QFileDialog::getSaveFileName(this, tr("Save Config File..."), m_theApp->getConfigfilepath(), "mesydaq config files (*.mcfg);;all files (*.*)");
	if (!name.isEmpty())
		m_theApp->saveSetup(name);
}

void MainWidget::restoreSetupSlot()
{
	QString name = QFileDialog::getOpenFileName(this, tr("Load Config File..."), m_theApp->getConfigfilepath(), "mesydaq config files (*.mcfg);;all files (*.*)");
	if (!name.isEmpty())
	{
		m_theApp->loadSetup(name);
		configfilename->setText(m_theApp->getConfigfilename());
	}
}

/*!
    \fn MainWidget::processDispData()
 */
void MainWidget::processDispData()
{
    /// @todo implement me
}

/*!
    \fn MainWidget::applyThreshSlot()
 */
void MainWidget::applyThreshSlot()
{
	bool ok;
	m_dispThresh = useThresh->isChecked();
	if(m_dispThresh)
	{
		m_dispHiThresh = hiLim->text().toUInt(&ok, 0);
		m_dispLoThresh = loLim->text().toUInt(&ok, 0);
	}
}

/*!
    \fn MainWidget::linlogSlot()
 */
void MainWidget::linlogSlot()
{
	m_dispLog = log->isChecked();
	if (m_dispLog)
		qDebug("log");
	else
		qDebug("lin");
}

/*!
    \fn MainWidget::drawOpData()
 */
void MainWidget::drawOpData()
{
	float	mean,
		sigma;
	
// display mean and sigma:
	if(dispAll->isChecked())
	{
		if(dispAllPos->isChecked())
			m_meas->getPosMean(mean, sigma);
		else
			m_meas->getAmpMean(mean, sigma);
	}
	else{
		if(specialBox->isChecked())
			m_meas->getTimeMean(mean, sigma);
		else if(dispAllPos->isChecked())
			m_meas->getPosMean(dispMpsd->value()*8 + dispChan->value(), mean, sigma);
		else
			m_meas->getAmpMean(dispMpsd->value()*8 + dispChan->value(), mean, sigma);
	}
	meanText->setText(tr("%1").arg(mean, 3, 'f', 1));
	sigmaText->setText(tr("%1").arg(sigma, 3, 'f', 1));

// pulser warning
	if(m_theApp->isPulserOn())
		pulserWarning->setText("<p align=\"center\">PULSER ON!</p>");
	else
		pulserWarning->setText("");
}

/*!
    \fn MainWidget::writeRegisterSlot()

    callback to write into a MPSD register
 */
void MainWidget::writeRegisterSlot()
{
	bool ok;
	quint16 id = (quint16) devid->value();	
	quint16 addr = module->value();
	quint16 reg = registerSelect->value();
	quint16 val = registerValue->text().toUInt(&ok, 0);
	m_theApp->writePeriReg(id, addr, reg, val);
}

/*!
    \fn MainWidget::readRegisterSlot()

    callback to read from a MPSD register
*/
void MainWidget::readRegisterSlot()
{
//! \todo display read values
#warning TODO
	quint16 id = (quint16) devid->value();	
	quint16 addr = module->value();
	quint16 reg = registerSelect->value();
	m_theApp->readPeriReg(id, addr, reg);
}

/*!
    \fn MainWidget::selectConfigpathSlot()

    callback to set the path for the configuration files
*/
void MainWidget::selectConfigpathSlot()
{
	QString name = QFileDialog::getExistingDirectory((QWidget *)this, tr("Select Config File Path..."), m_theApp->getConfigfilepath());
	if(!name.isEmpty())
		m_theApp->setConfigfilepath(name);
	dispFiledata();
}

/*!
    \fn MainWidget::selectConfigpathSlot()

    callback to set the path for the histogram data files
*/
void MainWidget::selectHistpathSlot()
{
	QString name = QFileDialog::getExistingDirectory((QWidget *)this, tr("Select Histogram File  Path..."), m_theApp->getHistfilepath());
	if(!name.isEmpty())
		m_theApp->setHistfilepath(name);
	dispFiledata();
}

/*!
    \fn MainWidget::selectConfigpathSlot()

    callback to set the path for the list mode data files
*/
void MainWidget::selectListpathSlot()
{
	QString name = QFileDialog::getExistingDirectory((QWidget *)this, tr("Select List File Path..."), m_theApp->getListfilepath());
	if(!name.isEmpty())
		m_theApp->setListfilepath(name);
	dispFiledata();
}

/*!
    \fn MainWidget::dispFiledata(void)
 */
void MainWidget::dispFiledata(void)
{
	configfilename->setText(m_theApp->getConfigfilename());
	if(m_theApp->getHistfilename().isEmpty())
		histfilename->setText("-");
	else
		histfilename->setText(m_theApp->getHistfilename());
 
	configfilepath->setText(m_theApp->getConfigfilepath());
	histfilepath->setText(m_theApp->getHistfilepath());
	listfilepath->setText(m_theApp->getListfilepath());
}

/*!
    \fn MainWidget::writeHistSlot()

    callback to write a histogram data file
*/
void MainWidget::writeHistSlot()
{
	QString name = QFileDialog::getSaveFileName(this, tr("Write Histogram..."), m_theApp->getHistfilepath(), 
				"mesydaq histogram files (*.mtxt);;all files (*.*)");
  	if(!name.isEmpty())
	{
    		int i = name.indexOf(".mtxt");
		if(i == -1)
			name.append(".mtxt");
		m_meas->writeHistograms(name);
  	}
}

/*!
   \fn MainWidget::ePresetSlot(bool pr)

   callback to enable/disable event counter

   \param pr enable the preset of the event counter
 */
void MainWidget::ePresetSlot(bool pr)
{
	if(pr)
	{
		tPresetButton->setChecked(false);
		tPreset->setEnabled(false);
		m1PresetButton->setChecked(false);
		m1Preset->setEnabled(false);
		m2PresetButton->setChecked(false);
		m2Preset->setEnabled(false);
		m3PresetButton->setChecked(false);
		m3Preset->setEnabled(false);
		m4PresetButton->setChecked(false);
		m4Preset->setEnabled(false);
	}
	evPreset->setEnabled(pr);
	m_meas->setPreset(EVCT, evPreset->value(), pr);
}

/*!
   \fn MainWidget::tPresetSlot(bool pr)

   callback to enable/disable timer

   \param pr enable the preset of the timer
 */
void MainWidget::tPresetSlot(bool pr)
{
	if(pr)
	{
		ePresetButton->setChecked(false);
		evPreset->setEnabled(false);
		m1PresetButton->setChecked(false);
		m2PresetButton->setChecked(false);
		m1Preset->setEnabled(false);
		m2Preset->setEnabled(false);
		m3PresetButton->setChecked(false);
		m3Preset->setEnabled(false);
		m4PresetButton->setChecked(false);
		m4Preset->setEnabled(false);
	}
	tPreset->setEnabled(pr);
	m_meas->setPreset(TCT, tPreset->value() * 1000, pr);
}

/*!
   \fn MainWidget::m1PresetSlot(bool pr)

   callback to enable/disable monitor 1

   \param pr enable the preset of the monitor 1
 */
void MainWidget::m1PresetSlot(bool pr)
{
	if(pr)
	{
		tPresetButton->setChecked(false);
		tPreset->setEnabled(false);
		ePresetButton->setChecked(false);
		evPreset->setEnabled(false);
		m2Preset->setEnabled(false);
		m2PresetButton->setChecked(false);
		m3Preset->setEnabled(false);
		m3PresetButton->setChecked(false);
		m4Preset->setEnabled(false);
		m4PresetButton->setChecked(false);
	}
	m1Preset->setEnabled(pr);
	m_meas->setPreset(M1CT, m1Preset->value(), pr);
}

/*!
   \fn MainWidget::m2PresetSlot(bool pr)

   callback to enable/disable monitor 2

   \param pr enable the preset of the monitor 2
 */
void MainWidget::m2PresetSlot(bool pr)
{
	if(pr)
	{
		tPresetButton->setChecked(false);
		tPreset->setEnabled(false);
		ePresetButton->setChecked(false);
		evPreset->setEnabled(false);
		m1PresetButton->setChecked(false);
		m1Preset->setEnabled(false);
		m3Preset->setEnabled(false);
		m3PresetButton->setChecked(false);
		m4Preset->setEnabled(false);
		m4PresetButton->setChecked(false);
	}
	m2Preset->setEnabled(pr);
	m_meas->setPreset(M2CT, m2Preset->value(), pr);
}

/*!
   \fn MainWidget::m3PresetSlot(bool pr)

   callback to enable/disable monitor 3

   \param pr enable the preset of the monitor 3
 */
void MainWidget::m3PresetSlot(bool pr)
{
	if(pr)
	{
		tPresetButton->setChecked(false);
		tPreset->setEnabled(false);
		ePresetButton->setChecked(false);
		evPreset->setEnabled(false);
		m1Preset->setEnabled(false);
		m1PresetButton->setChecked(false);
		m2Preset->setEnabled(false);
		m2PresetButton->setChecked(false);
		m4Preset->setEnabled(false);
		m4PresetButton->setChecked(false);
	}
	m3Preset->setEnabled(pr);
	m_meas->setPreset(M3CT, m1Preset->value(), pr);
}

/*!
   \fn MainWidget::m4PresetSlot(bool pr)

   callback to enable/disable monitor 4

   \param pr enable the preset of the monitor 4
 */
void MainWidget::m4PresetSlot(bool pr)
{
	if(pr)
	{
		tPresetButton->setChecked(false);
		tPreset->setEnabled(false);
		ePresetButton->setChecked(false);
		evPreset->setEnabled(false);
		m1Preset->setEnabled(false);
		m1PresetButton->setChecked(false);
		m2Preset->setEnabled(false);
		m2PresetButton->setChecked(false);
		m3Preset->setEnabled(false);
		m3PresetButton->setChecked(false);
	}
	m3Preset->setEnabled(pr);
	m_meas->setPreset(M4CT, m1Preset->value(), pr);
}

/*!
    \fn MainWidget::updatePresets(void)

    callback to display all preset values
 */
void MainWidget::updatePresets(void)
{
// presets
	tPreset->setValue(m_meas->getPreset(TCT));
	evPreset->setValue(m_meas->getPreset(EVCT));
	m1Preset->setValue(m_meas->getPreset(M1CT));
	m2Preset->setValue(m_meas->getPreset(M2CT));
	m3Preset->setValue(m_meas->getPreset(M3CT));
	m4Preset->setValue(m_meas->getPreset(M4CT));
    
// check for master preset counter
    	tPresetButton->setChecked(m_meas->isMaster(TCT));
	ePresetButton->setChecked(m_meas->isMaster(EVCT));
    	m1PresetButton->setChecked(m_meas->isMaster(M1CT));
    	m2PresetButton->setChecked(m_meas->isMaster(M2CT));
    	m1PresetButton->setChecked(m_meas->isMaster(M3CT));
    	m2PresetButton->setChecked(m_meas->isMaster(M4CT));
   
// Caress values
	updateCaress();
}

/*!
    \fn MainWidget::tResetSlot()

    clears the timer
 */
void MainWidget::tResetSlot()
{
	m_meas->clearCounter(TCT);
	update();
}

/*!
    \fn MainWidget::eResetSlot()

    clears the event counter
 */
void MainWidget::eResetSlot()
{
	m_meas->clearCounter(EVCT);
	update();
}

/*!
    \fn MainWidget::m1ResetSlot()

    clears the Monitor 1 counter
 */
void MainWidget::m1ResetSlot()
{
	m_meas->clearCounter(M1CT);
	update();
}

/*!
    \fn MainWidget::m2ResetSlot()

    clears the Monitor 2 counter
 */
void MainWidget::m2ResetSlot()
{
	m_meas->clearCounter(M2CT);
	update();
}

/*!
    \fn MainWidget::m3ResetSlot()

    clears the Monitor 3 counter
 */
void MainWidget::m3ResetSlot()
{
	m_meas->clearCounter(M3CT);
	update();
}

/*!
    \fn MainWidget::m4ResetSlot()

    clears the Monitor 4 counter
 */
void MainWidget::m4ResetSlot()
{
	m_meas->clearCounter(M4CT);
	update();
}

/*!
    \fn MainWidget::updateCaress(void)
    \todo remove the CARESS specific part
 */
void MainWidget::updateCaress(void)
{
#warning TODO remove the CARESS specific part
#if 0
	caressWidth->setText(tr("%1").arg(m_meas->getCarWidth()));
	caressHeight->setText(tr("%1").arg(m_meas->getCarHeight()));
	caressRun->setText(tr("%1").arg(m_meas->getRun()));
#endif
}

/*!
    \fn MainWidget::saveConfigSlot(void)

    callback to save configuration
 */
void MainWidget::saveConfigSlot(void)
{
//! \todo implement me
}

void MainWidget::mpsdCheck(int mod)
{
	m_theApp->protocol(tr("MainWidget::mpsdCheck() : module %1").arg(mod), DEBUG);
	displayMpsdSlot(mod);
}

/*!
    \fn MainWidget::draw(void)

    callback to redraw the data and additional informations
 */
void MainWidget::draw(void)
{
	bool on = dispHistogram->isChecked();

	m_histogram->setDisplayMode(QwtPlotSpectrogram::ImageMode, on);
	m_histogram->setDefaultContourPen(on ? QPen() : QPen(Qt::NoPen));
	dataFrame->enableAxis(QwtPlot::yRight, on);

	if (on)
	{
		m_curve->detach();
		m_histogram->attach(dataFrame);
		m_histData->setData(m_meas->posHist());
		
		QwtDoubleInterval interval = m_histogram->data().range();

		dataFrame->setAxisScale(QwtPlot::yRight,  interval.minValue(), interval.maxValue());
		dataFrame->axisWidget(QwtPlot::yRight)->setColorMap(interval, m_histogram->colorMap());
		m_histogram->setData(*m_histData);
		if (!m_zoomEnabled)
			dataFrame->setAxisScale(QwtPlot::yLeft, 0, m_meas->posHist()->height());
		dataFrame->setAxisTitle(QwtPlot::yLeft, tr("tube"));
	}
	else
	{
		dataFrame->setAxisTitle(QwtPlot::yLeft, tr("counts"));
		if(dispAll->isChecked())
		{
			if(dispAllPos->isChecked())
				m_data->setData(m_meas->posData());
			else
				m_data->setData(m_meas->ampData());
		}
		else
		{
			if (specialBox->isChecked())
				m_data->setData(m_meas->timeData());
			else
			{
				quint32 chan = dispMcpd->value() * 64;
				chan += dispMpsd->value() * 8;
				chan += dispChan->value();
				if(dispAllPos->isChecked())
					m_data->setData(m_meas->posData(chan));
				else
					m_data->setData(m_meas->ampData(chan));
			}
		}
		m_histogram->detach();
		m_curve->attach(dataFrame);	
		m_curve->setData(*m_data);
// reduce data in case of threshold settings:
		if (m_dispThresh)
			dataFrame->setAxisScale(QwtPlot::yLeft, m_dispLoThresh, m_dispHiThresh);
		else if (!m_zoomEnabled)
        	        dataFrame->setAxisAutoScale(QwtPlot::yLeft);
	}
	dataFrame->replot();
	drawOpData();
	update();
}
