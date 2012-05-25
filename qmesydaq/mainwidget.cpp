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
#include <QMessageBox>
#include <QFileDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QSvgGenerator>
#include <qwt_plot_curve.h>
#include <qwt_plot_zoomer.h>
#include <qwt_scale_widget.h>
#include <qwt_color_map.h>
#include <qwt_scale_engine.h>
#include "mainwidget.h"
#include "mdefines.h"
#include "measurement.h"
#include "mesydaq2.h"
#include "histogram.h"
#include "mesydaqdata.h"
#include "CommandEvent.h"
#include "MultipleLoopApplication.h"
#include "QMesydaqDetectorInterface.h"
#include "generalsetup.h"
#include "modulewizard.h"
#include "mapcorrect.h"
#include "modulesetup.h"
#include "mdllsetup.h"
#include "mcpdsetup.h"
#include "logging.h"
#if defined(_MSC_VER)
	#include "stdafx.h"
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
    , m_dispThresh(false)
    , m_dispLoThresh(0)
    , m_dispHiThresh(0)
    , m_diffractogram(NULL)
    , m_histogram(NULL)
    , m_data(NULL)
    , m_histData(NULL)
    , m_meas(NULL)
    , m_dispTimer(0)
    , m_zoomer(NULL)
    , m_controlInt(NULL)
{
    setupUi(this);
    statusTab->setCurrentIndex(0);

    moduleStatus0->setId(0);
    moduleStatus1->setId(1);
    moduleStatus2->setId(2);
    moduleStatus3->setId(3);
    moduleStatus4->setId(4);
    moduleStatus5->setId(5);
    moduleStatus6->setId(6);
    moduleStatus7->setId(7);

    connect(moduleStatus0, SIGNAL(clicked(quint8)), this, SLOT(setupModule(quint8)));
    connect(moduleStatus0, SIGNAL(histogram(quint8, bool)), this, SLOT(moduleHistogramSlot(quint8, bool)));
    connect(moduleStatus0, SIGNAL(active(quint8, bool)), this, SLOT(moduleActiveSlot(quint8, bool)));

    connect(moduleStatus1, SIGNAL(clicked(quint8)), this, SLOT(setupModule(quint8)));
    connect(moduleStatus1, SIGNAL(histogram(quint8, bool)), this, SLOT(moduleHistogramSlot(quint8, bool)));
    connect(moduleStatus1, SIGNAL(active(quint8, bool)), this, SLOT(moduleActiveSlot(quint8, bool)));

    connect(moduleStatus2, SIGNAL(clicked(quint8)), this, SLOT(setupModule(quint8)));
    connect(moduleStatus2, SIGNAL(histogram(quint8, bool)), this, SLOT(moduleHistogramSlot(quint8, bool)));
    connect(moduleStatus2, SIGNAL(active(quint8, bool)), this, SLOT(moduleActiveSlot(quint8, bool)));

    connect(moduleStatus3, SIGNAL(clicked(quint8)), this, SLOT(setupModule(quint8)));
    connect(moduleStatus3, SIGNAL(histogram(quint8, bool)), this, SLOT(moduleHistogramSlot(quint8, bool)));
    connect(moduleStatus3, SIGNAL(active(quint8, bool)), this, SLOT(moduleActiveSlot(quint8, bool)));

    connect(moduleStatus4, SIGNAL(clicked(quint8)), this, SLOT(setupModule(quint8)));
    connect(moduleStatus4, SIGNAL(histogram(quint8, bool)), this, SLOT(moduleHistogramSlot(quint8, bool)));
    connect(moduleStatus4, SIGNAL(active(quint8, bool)), this, SLOT(moduleActiveSlot(quint8, bool)));

    connect(moduleStatus5, SIGNAL(clicked(quint8)), this, SLOT(setupModule(quint8)));
    connect(moduleStatus5, SIGNAL(histogram(quint8, bool)), this, SLOT(moduleHistogramSlot(quint8, bool)));
    connect(moduleStatus5, SIGNAL(active(quint8, bool)), this, SLOT(moduleActiveSlot(quint8, bool)));

    connect(moduleStatus6, SIGNAL(clicked(quint8)), this, SLOT(setupModule(quint8)));
    connect(moduleStatus6, SIGNAL(histogram(quint8, bool)), this, SLOT(moduleHistogramSlot(quint8, bool)));
    connect(moduleStatus6, SIGNAL(active(quint8, bool)), this, SLOT(moduleActiveSlot(quint8, bool)));

    connect(moduleStatus7, SIGNAL(clicked(quint8)), this, SLOT(setupModule(quint8)));
    connect(moduleStatus7, SIGNAL(histogram(quint8, bool)), this, SLOT(moduleHistogramSlot(quint8, bool)));
    connect(moduleStatus7, SIGNAL(active(quint8, bool)), this, SLOT(moduleActiveSlot(quint8, bool)));

    versionLabel->setText("QMesyDAQ " VERSION "\n" __DATE__);

    connect(acquireFile, SIGNAL(toggled(bool)), m_theApp, SLOT(acqListfile(bool)));
    connect(allPulsersoffButton, SIGNAL(clicked()), this, SLOT(allPulserOff()));
    connect(m_theApp, SIGNAL(statusChanged(const QString &)), daqStatusLine, SLOT(setText(const QString &)));
    connect(this, SIGNAL(redraw()), this, SLOT(draw()));
#if 0
    connect(this, SIGNAL(setCounter(quint32, quint64)), m_meas, SLOT(setCounter(quint32, quint64)));
    connect(devid, SIGNAL(valueChanged(int)), devid_2, SLOT(setValue(int)));
    connect(dispMcpd, SIGNAL(valueChanged(int)), devid, SLOT(setValue(int)));
    connect(devid, SIGNAL(valueChanged(int)), dispMcpd, SLOT(setValue(int)));
#endif
    connect(dispHistogram, SIGNAL(toggled(bool)), this, SLOT(setHistogramMode(bool)));
    connect(dispSpectra, SIGNAL(toggled(bool)), this, SLOT(setSpectraMode(bool)));
    connect(dispDiffractogram, SIGNAL(toggled(bool)), this, SLOT(setDiffractogramMode(bool)));
    connect(dispChan, SIGNAL(valueChanged(int)), this, SLOT(draw()));
    connect(dispAll, SIGNAL(toggled(bool)), this, SLOT(draw()));
    connect(dispMcpd, SIGNAL(valueChanged(int)), this, SLOT(draw()));
    connect(dispMpsd, SIGNAL(valueChanged(int)), this, SLOT(draw()));
    connect(dispAllChannels, SIGNAL(toggled(bool)), this, SLOT(draw()));
    connect(dispAllPos, SIGNAL(toggled(bool)), this, SLOT(draw()));
    connect(dispAllAmpl, SIGNAL(toggled(bool)), this, SLOT(draw()));
    connect(devid_2, SIGNAL(valueChanged(int)), this, SLOT(scanPeriSlot()));
    connect(parent, SIGNAL(loadConfiguration(const QString&)), this, SLOT(loadConfiguration(const QString&)), Qt::DirectConnection);
    connect(statusTab, SIGNAL(currentChanged(int)), this, SLOT(statusTabChanged(int)));

    connect(dispChan, SIGNAL(changeModule(int)), dispMpsd, SLOT(steps(int)));
    connect(dispMpsd, SIGNAL(changeModule(int)), dispMcpd, SLOT(steps(int)));
    connect(paramId, SIGNAL(valueChanged(int)), this, SLOT(draw()));

//	connect(acquireFile, SIGNAL(toggled(bool)), this, SLOT(checkListfilename(bool)));

    clearMcpd->setHidden(true);
    clearMpsd->setHidden(true);
    clearChan->setHidden(true);

    m_dataFrame->setAxisTitle(QwtPlot::xBottom, tr("channel"));
    m_dataFrame->setAxisTitle(QwtPlot::yLeft, tr("counts"));
    m_dataFrame->setAxisTitle(QwtPlot::yRight, tr("intensity"));
    m_dataFrame->enableAxis(QwtPlot::yRight, false);
//  m_dataFrame->plotLayout()->setAlignCanvasToScales(true);
    m_dataFrame->setAutoReplot(false);

    m_zoomer = new QwtPlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft, QwtPicker::DragSelection, QwtPicker::ActiveOnly, m_dataFrame->canvas());

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
    m_zoomer->setTrackerMode(QwtPicker::ActiveOnly);
    m_zoomer->setEnabled(true);
    m_zoomer->setZoomBase();

    m_picker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
                                 QwtPicker::PointSelection | QwtPicker::DragSelection,
                                 QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,
                                 m_dataFrame->canvas());
    m_picker->setRubberBandPen(QColor(Qt::green));
    m_picker->setRubberBand(QwtPicker::CrossRubberBand);
    m_picker->setTrackerPen(QColor(Qt::black));

    for (int i = 0; i < 8; ++i)
    {
        m_curve[i] = new QwtPlotCurve("");
        m_curve[i]->setStyle(QwtPlotCurve::Steps);
#if QT_VERSION >= 0x040000
        m_curve[i]->setRenderHint(QwtPlotItem::RenderAntialiased);
#endif
        m_curve[i]->attach(m_dataFrame);
    }
    m_curve[0]->setPen(QPen(Qt::red));
    m_curve[1]->setPen(QPen(Qt::black));
    m_curve[2]->setPen(QPen(Qt::green));
    m_curve[3]->setPen(QPen(Qt::blue));
    m_curve[4]->setPen(QPen(Qt::yellow));
    m_curve[5]->setPen(QPen(Qt::magenta));
    m_curve[6]->setPen(QPen(Qt::cyan));
    m_curve[7]->setPen(QPen(Qt::white));

    m_diffractogram = new QwtPlotCurve("");
//    m_diffractogram->setStyle(QwtPlotCurve::Steps);
#if QT_VERSION >= 0x040000
    m_diffractogram->setRenderHint(QwtPlotItem::RenderAntialiased);
#endif
    m_diffractogram->setPen(QPen(Qt::black));

    m_data = new MesydaqSpectrumData();
    m_curve[0]->setData(*m_data);

    m_histogram = new MesydaqPlotSpectrogram();

    m_linColorMap = new QwtLinearColorMap(Qt::darkBlue, Qt::darkRed);
    m_linColorMap->addColorStop(0.2, Qt::blue);
    m_linColorMap->addColorStop(0.4, Qt::green);
    m_linColorMap->addColorStop(0.6, Qt::yellow);
    m_linColorMap->addColorStop(0.8, Qt::red);

    m_logColorMap = new QwtLinearColorMap(Qt::darkBlue, Qt::darkRed);
    m_logColorMap->addColorStop(0.1585, Qt::blue);
    m_logColorMap->addColorStop(0.2511, Qt::green);
    m_logColorMap->addColorStop(0.3981, Qt::yellow);
    m_logColorMap->addColorStop(0.631, Qt::red);

    m_histogram->setColorMap(*m_linColorMap);

    m_dataFrame->axisWidget(QwtPlot::yRight)->setColorBarEnabled(true);
    m_histData = new MesydaqHistogramData();
    m_histogram->setData(*m_histData);

    scanPeriSlot(false);
    dispFiledata();

    m_printer = new QPrinter;
    m_printer->setOrientation(QPrinter::Landscape);
    m_printer->setDocName("PlotCurves");
    m_printer->setCreator("QMesyDAQ Version: " VERSION);

    // display refresh timer
    //	m_dispTimer = startTimer(1000);

#if 0
    init();

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());

    QSettings setup(settings.value("lastconfigfile", "mesycfg.mcfg").toString(), QSettings::IniFormat);
    acquireFile->setChecked(setup.value("MESYDAQ/listmode", true).toBool());
    timerPreset->setChecked(setup.value("MESYDAQ/Preset/time", true).toBool());
#endif

    timerPreset->setLabel(tr("Timer"));
    eventsPreset->setLabel(tr("Events"));
    
    monitor1Preset->setLabel(tr("Monitor 1"));
    monitor2Preset->setLabel(tr("Monitor 2"));
    monitor3Preset->setLabel(tr("Monitor 3"));
    monitor4Preset->setLabel(tr("Monitor 4"));

    setHistogramMode(true);
    draw();
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
    \fn void MainWidget::about()

    opens an about dialog
*/
void MainWidget::about()
{
    QString text = tr("<h3>About QMesyDAQ </h3>")
		   + tr("<p>Authors</p><ul>")
                   + tr("<li>Copyright (C) 2008 <a href=\"mailto:g.montermann@mesytec.com\">Gregor Montermann</a</li>")
                   + tr("<li>Copyright (C) 2009-2011 <a href=\"mailto:jens.krueger@frm2.tum.de\">Jens Kr&uuml;ger</a></li>")
		   + tr("<li>Copyright (C) 2011 <a href=\"mailto:rossa@helmholtz-berlin.de\">Lutz Rossa</a></li>")
		   + tr("</ul><p>Contributors</p><ul>")
		   + tr("<li><a href=\"mailto:alexander.lenz@frm2.tum.de\">Alexander Lenz</a> TACO remote control</li>")
                   + tr("<li><a href=\"mailto:m.drochner@fz-juelich.de\">Matthias Drochner</a> Bug reports</li>")
                   + tr("</ul><p>This program controls the data acquisition and display for the MesyTec MCPD-2/8 modules</p>")
#if USE_TACO || USE_CARESS
		   + tr("<p>It may be remotely controlled by:")
		   + tr("<ul>")
#if USE_TACO
		   + tr("<li><b>TACO</b></li>")
#endif
#if USE_CARESS
		   + tr("<li><b>CARESS</b></li>")
#endif
		   + tr("</ul></p>")
#endif
                   + tr("<p>It is published under GPL (GNU General Public License) <tt><a href=\"http://www.gnu.org/licenses/gpl.html\">http://www.gnu.org/licenses/gpl.html</a></tt></p>")
                   + tr("<p>Version : <b>%1</b></p>").arg(VERSION);

    QMessageBox msgBox(this);
    msgBox.setText(text);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setWindowTitle(tr("About QMesyDAQ"));
    msgBox.setIconPixmap(QPixmap(":/logo.png"));

    msgBox.exec();
}

/*!
    \fn void MainWidget::init()

*/
void MainWidget::init()
{
    if (m_meas)
    {
        disconnect(m_meas, SIGNAL(stopSignal(bool)), startStopButton, SLOT(animateClick()));
        disconnect(m_meas, SIGNAL(draw()), this, SLOT(draw()));
    	disconnect(dispMcpd, SIGNAL(valueChanged(int)), this, SLOT(displayMcpdSlot(int)));
        delete m_meas;
    }
    m_meas = NULL;
    m_meas = new Measurement(m_theApp, this);

    QList<int> mcpdList;
    dispMcpd->setMCPDList(mcpdList);
    devid_2->setMCPDList(mcpdList);
    paramId->setMCPDList(mcpdList);
    startStopButton->setDisabled(true);
    acquireFile->setDisabled(true);
    allPulsersoffButton->setDisabled(true);
    displayTabWidget->setDisabled(true);
    statusGroupBox->setDisabled(true);
    displayMpsdSlot();
    m_dataFrame->setAxisScale(QwtPlot::xBottom, 0, m_meas->width());

    connect(m_meas, SIGNAL(stopSignal(bool)), startStopButton, SLOT(animateClick()));
    connect(m_meas, SIGNAL(draw()), this, SLOT(draw()));
    connect(dispMcpd, SIGNAL(valueChanged(int)), this, SLOT(displayMcpdSlot(int)));
    displayMcpdSlot(dispMcpd->value());
#if 0
    connect(this, SIGNAL(setCounter(quint32, quint64)), m_meas, SLOT(setCounter(quint32, quint64)));
#endif
    emit redraw();
}

/*!
    \fn void MainWidget::timerEvent(QTimerEvent *)

    callback for the timer
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
#if 0
    pulserButton->setChecked(false);
#endif
}

/*!
    \fn void MainWidget::zoomAreaSelected(const QwtDoubleRect &)

    \param 
*/
void MainWidget::zoomAreaSelected(const QwtDoubleRect &)
{
    if (!m_zoomer->zoomRectIndex())
        m_zoomer->setZoomBase();
}

/*!
    \fn void MainWidget::zoomed(const QwtDoubleRect &rect)

    \param rect
*/
void MainWidget::zoomed(const QwtDoubleRect &rect)
{
    if (rect == m_zoomer->zoomBase())
    {
        if (!dispHistogram->isChecked())
        {
            m_dataFrame->setAxisAutoScale(QwtPlot::yLeft);
            m_dataFrame->setAxisScale(QwtPlot::xBottom, 0, m_meas ? m_meas->width() : 1.0);
        }
        else
        {
            m_dataFrame->setAxisScale(QwtPlot::xBottom, 0, m_meas ? m_meas->height() : 1.0);
            m_dataFrame->setAxisScale(QwtPlot::yLeft, 0, m_meas ? m_meas->width() : 1.0);
        }
        if (m_meas)
            m_meas->setROI(QRectF(0, 0, m_meas->width(), m_meas->height()));
    }
    else
    {
	qreal 	x, 
	    	y,
	    	w, 
	    	h;
	rect.getRect(&x, &y, &w, &h);

        if (m_meas)
            m_meas->setROI(QRectF(x, y, h, w));
    }
    if (!m_dispTimer)
	emit redraw();
}

/*!
    \fn void MainWidget::statusTabChanged(int )

    callback for changing the tab in the status tab widget

 */
void MainWidget::statusTabChanged(int )
{
	if (statusTab->currentWidget() == statusModuleTab)
	{
		scanPeri->animateClick();
	}
}

/*!
    \fn void MainWidget::startStopSlot(bool checked)

    \param checked
*/
void MainWidget::startStopSlot(bool checked)
{
    if(checked)
    {
        checkListfilename(acquireFile->isChecked());
        // get timing binwidth
        m_theApp->setTimingwidth(timingBox->value());

        // get latest preset entry
        if(m_meas->isMaster(TIMERID))
            m_meas->setPreset(TIMERID, quint64(timerPreset->value() * 1000), true);
        if(m_meas->isMaster(EVID))
            m_meas->setPreset(EVID, eventsPreset->value(), true);
        if(m_meas->isMaster(MON1ID))
            m_meas->setPreset(MON1ID, monitor1Preset->value(), true);
        if(m_meas->isMaster(MON2ID))
            m_meas->setPreset(MON2ID, monitor2Preset->value(), true);
        if(m_meas->isMaster(MON3ID))
            m_meas->setPreset(MON3ID, monitor3Preset->value(), true);
        if(m_meas->isMaster(MON4ID))
            m_meas->setPreset(MON4ID, monitor4Preset->value(), true);
        startStopButton->setText("Stop");
        // set device id to 0 -> will be filled by mesydaq for master
        m_meas->start();
	m_dispTimer = startTimer(500);
    }
    else
    {
        m_meas->stop();
        if (m_dispTimer)
            killTimer(m_dispTimer);
        m_dispTimer = 0;
        startStopButton->setText("Start");
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
		MSG_WARNING << "Set stream " << m_cmdBuffer[2];
    m_theApp->sendCommand(m_pBuffer);

    m_theApp->setStream(mcpdId->value(), statusStream->isChecked());
#endif
}

/*!
    \fn QString MainWidget::selectListfile(void)

    Opens a user dialog for selecting a file name to store the list mode data into.

    It checks also the extension of the file name and if not 'mdat' it will added.
    \returns selected file name, if no file name is selected or aborted it will retur
             an empty string
*/
QString MainWidget::selectListfile(void)
{
    QString name = QFileDialog::getSaveFileName(this, tr("Save as..."), m_meas->getListfilepath(),
                                                "mesydaq data files (*.mdat);;all files (*.*);;really all files (*)");
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

    \param checked
*/
void MainWidget::checkListfilename(bool checked)
{
    if (checked)
    {
        MultipleLoopApplication *app = dynamic_cast<MultipleLoopApplication*>(QApplication::instance());
        QMesyDAQDetectorInterface *interface;
        QString name(QString::null); 
        if(app)
        {
            interface = dynamic_cast<QMesyDAQDetectorInterface*>(app->getQtInterface());
            if (interface)
                name = interface->getListFileName();
        }

	
        if (name.isEmpty())
            name = selectListfile();
        else
            name = m_meas->getListfilepath() + "/" + name;
        if(!name.isEmpty())
            m_theApp->setListfilename(name);
        else
            acquireFile->setChecked(false);
        emit redraw();
    }
}	

/*!
    \fn void MainWidget::updateDisplay(void)
 */
void MainWidget::updateDisplay(void)
{
    quint16 id = (quint16) paramId->value();
    int ci = statusTab->currentIndex();
    if (statusTab->tabText(ci) == tr("Statistics"))
    {
        dataRx->setText(tr("%1").arg(m_theApp->receivedData()));
        cmdTx->setText(tr("%1").arg(m_theApp->sentCmds()));
        cmdRx->setText(tr("%1").arg(m_theApp->receivedCmds()));
    }
    hTimeText->setText(buildTimestring(m_meas->getHeadertime(), true));
    mTimeText->setText(buildTimestring(m_meas->getMeastime(), false));
    
    // parameter values for selected ID
    param0->setText(tr("%1").arg(m_theApp->getParameter(id, 0)));
    param1->setText(tr("%1").arg(m_theApp->getParameter(id, 1)));
    param2->setText(tr("%1").arg(m_theApp->getParameter(id, 2)));
    param3->setText(tr("%1").arg(m_theApp->getParameter(id, 3)));
    m_meas->calcMeanRates();
    
    // measurement values counters and rates
    tSecsText->setText(tr("%1").arg(m_meas->timer() / 1000., 0, 'f', 1));
    totalCounts->setText(tr("%1").arg(m_meas->events(), 20));
    eventRate->setText(tr("%1").arg(m_meas->getRate(EVID)));
    monitor1->setText(tr("%1").arg(m_meas->mon1()));
    monRate1->setText(tr("%1").arg(m_meas->getRate(MON1ID)));
    monitor2->setText(tr("%1").arg(m_meas->mon2()));
    monRate2->setText(tr("%1").arg(m_meas->getRate(MON2ID)));
    monitor3->setText(tr("%1").arg(m_meas->mon3()));
    monRate3->setText(tr("%1").arg(m_meas->getRate(MON3ID)));
    monitor4->setText(tr("%1").arg(m_meas->mon4()));
    monRate4->setText(tr("%1").arg(m_meas->getRate(MON4ID)));

    lcdRunID->display(m_meas->runId());
    dispFiledata();
}

/*!
    \fn QString MainWidget::buildTimestring(quint64 timeval, bool nano)
 */
QString MainWidget::buildTimestring(quint64 timeval, bool nano)
{
    // nsec = time in 100 nsecs
    //-> usec =
    //->
    QString str;
    quint64 val;
    ulong /*nsec,*/ sec, min, hr;
    // calculate raw seconds
    val = round(timeval / 1000.0);
//  nsec = timeval - (1000 * val);
    if(nano)
    {
        val = round(val / 10000.0);
//      nsec = timeval - (1000 * val);
    }
//  MSG_DEBUG << timeval << ' ' << val << ' ' << nsec;
// hours = val / 3600 (s/h)
    hr = val / 3600;
// remaining seconds:
    val -= hr * 3600;
// minutes:
    min = val / 60;
// remaining seconds:
    sec = val - (min * 60);
//  MSG_DEBUG << nsecs << ' ' << hr << ' ' << min << ' ' << sec << ' ' << nsec;
    str.sprintf("%02lu:%02lu:%02lu", hr, min, sec);
    return str;
}

/*!
    \fn void MainWidget::clearAllSlot()

*/
void MainWidget::clearAllSlot()
{
    m_meas->clearAllHist();
    m_meas->setROI(QwtDoubleRect(0,0,0,0));
    m_meas->setHistfilename("");
    m_zoomer->setZoomBase();
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

*/
void MainWidget::clearMpsdSlot()
{
    quint32 start = dispMpsd->value() * 8 + dispMcpd->value() * 64;
//  MSG_DEBUG << "clearMpsd: " << start;
    for(quint32 i = start; i < start + 8; i++)
        m_meas->clearChanHist(i);
    emit redraw();
}

/*!
    \fn void MainWidget::clearChanSlot()

*/
void MainWidget::clearChanSlot()
{
    ulong chan = dispChan->value() + dispMpsd->value() * 8 + dispMcpd->value() * 64;
    m_meas->clearChanHist(chan);
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
        displayTabWidget->setEnabled(true);
        m_meas->readListfile(name);
        startStopButton->setEnabled(true);
    }
}

/*!
    \fn void MainWidget::displayMcpdSlot(int)

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
    \fn void MainWidget::displayMpsdSlot(int)

    \param id
 */
void MainWidget::displayMpsdSlot(int iModule)
{
// retrieve displayed ID
    quint8 mod = iModule;
    if (iModule<0)
      mod = devid_2->value();
// firmware version
    firmwareVersion->setText(tr("%1").arg(m_theApp->getFirmware(mod)));
    
// Status display:
    moduleStatus0->update(m_theApp->getMpsdType(mod, 0), m_theApp->getMpsdVersion(mod, 0), m_theApp->online(mod, 0), m_theApp->histogram(mod, 0), m_theApp->active(mod, 0));
    moduleStatus1->update(m_theApp->getMpsdType(mod, 1), m_theApp->getMpsdVersion(mod, 1), m_theApp->online(mod, 1), m_theApp->histogram(mod, 1), m_theApp->active(mod, 1));
    moduleStatus2->update(m_theApp->getMpsdType(mod, 2), m_theApp->getMpsdVersion(mod, 2), m_theApp->online(mod, 2), m_theApp->histogram(mod, 2), m_theApp->active(mod, 2));
    moduleStatus3->update(m_theApp->getMpsdType(mod, 3), m_theApp->getMpsdVersion(mod, 3), m_theApp->online(mod, 3), m_theApp->histogram(mod, 3), m_theApp->active(mod, 3));
    moduleStatus4->update(m_theApp->getMpsdType(mod, 4), m_theApp->getMpsdVersion(mod, 4), m_theApp->online(mod, 4), m_theApp->histogram(mod, 4), m_theApp->active(mod, 4));
    moduleStatus5->update(m_theApp->getMpsdType(mod, 5), m_theApp->getMpsdVersion(mod, 5), m_theApp->online(mod, 5), m_theApp->histogram(mod, 5), m_theApp->active(mod, 5));
    moduleStatus6->update(m_theApp->getMpsdType(mod, 6), m_theApp->getMpsdVersion(mod, 6), m_theApp->online(mod, 6), m_theApp->histogram(mod, 6), m_theApp->active(mod, 6));
    moduleStatus7->update(m_theApp->getMpsdType(mod, 7), m_theApp->getMpsdVersion(mod, 7), m_theApp->online(mod, 7), m_theApp->histogram(mod, 7), m_theApp->active(mod, 7));
}

/*!
    \fn void MainWidget::scanPeriSlot(bool real)

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
    {
        m_meas->saveSetup(name);
    }
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
    configfilename->setText(m_meas->getConfigfilename());
		acquireFile->setChecked(m_meas->acqListfile());

    QSettings settings(m_meas->getConfigfilename());
    settings.beginGroup("MESYDAQ");
    acquireFile->setChecked(settings.value("listmode", true).toBool());
    timerPreset->setChecked(settings.value("Preset/time", true).toBool());
    settings.endGroup();

    QList<int> mcpdList = m_theApp->mcpdId();
    dispMcpd->setMCPDList(mcpdList);
    devid_2->setMCPDList(mcpdList);
    paramId->setMCPDList(mcpdList);
    startStopButton->setDisabled(mcpdList.empty());
    acquireFile->setDisabled(mcpdList.empty());
    allPulsersoffButton->setDisabled(mcpdList.empty());
    displayTabWidget->setDisabled(mcpdList.empty());
    statusGroupBox->setDisabled(mcpdList.empty());
    emit redraw();
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
    \fn void MainWidget::linlogSlot(bool bLog)

    \param bLog
 */
void MainWidget::linlogSlot(bool bLog)
{
    if (dispHistogram->isChecked())
    {
        if (bLog)
            m_histogram->setColorMap(*m_logColorMap);
        else
            m_histogram->setColorMap(*m_linColorMap);
    }
    else
    {
        if (bLog)
            m_dataFrame->setAxisScaleEngine(QwtPlot::yLeft, new QwtLog10ScaleEngine);
        else
            m_dataFrame->setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
    }
    emit redraw();
}

/*!
    \fn void MainWidget::drawOpData()
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
        else
            m_meas->getMean(Measurement::AmplitudeHistogram, mean, sigma);
    }
    else
    {
        if(specialBox->isChecked())
            m_meas->getMean(Measurement::TimeSpectrum, mean, sigma);
        else if(dispAllPos->isChecked())
            m_meas->getMean(Measurement::PositionHistogram, dispMpsd->value() * 8 + dispChan->value(), mean, sigma);
        else
            m_meas->getMean(Measurement::AmplitudeHistogram, dispMpsd->value() * 8 + dispChan->value(), mean, sigma);
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
 */
void MainWidget::dispFiledata(void)
{
    configfilename->setText(m_meas ? m_meas->getConfigfilename() : "-");
    if(!m_meas || m_meas->getHistfilename().isEmpty())
        histfilename->setText("-");
    else
        histfilename->setText(m_meas->getHistfilename());
}

/*!
    \fn void MainWidget::writeHistSlot()

    callback to write a histogram data file
*/
void MainWidget::writeHistSlot()
{
    QString name = QFileDialog::getSaveFileName(this, tr("Write Histogram..."), m_meas->getHistfilepath(),
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
   \fn void MainWidget::loadHistSlot()

   callback to read a histogram data file
*/
void MainWidget::loadHistSlot()
{
    QString name = QFileDialog::getOpenFileName(this, tr("Load Histogram..."), m_meas->getHistfilepath(), "mesydaq histogram files (*.mtxt);;all files (*.*)");
    if(!name.isEmpty())
    {
        m_meas->readHistograms(name);
        displayTabWidget->setEnabled(true);
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
    m_meas->setPreset(EVID, eventsPreset->value(), pr);
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
    m_meas->setPreset(TIMERID, quint64(timerPreset->value() * 1000), pr);
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
    m_meas->setPreset(MON1ID, monitor1Preset->value(), pr);
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
    m_meas->setPreset(MON2ID, monitor2Preset->value(), pr);
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
    m_meas->setPreset(MON3ID, monitor3Preset->value(), pr);
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
    monitor4Preset->setChecked(pr);
    m_meas->setPreset(MON4ID, monitor4Preset->value(), pr);
}

/*!
    \fn void MainWidget::updatePresets(void)

    callback to display all preset values
 */
void MainWidget::updatePresets(void)
{
    // presets
    timerPreset->setValue(m_meas->getPreset(TIMERID));
    eventsPreset->setValue(m_meas->getPreset(EVID));
    monitor1Preset->setValue(m_meas->getPreset(MON1ID));
    monitor2Preset->setValue(m_meas->getPreset(MON2ID));
    monitor3Preset->setValue(m_meas->getPreset(MON3ID));
    monitor4Preset->setValue(m_meas->getPreset(MON4ID));
    
    // check for master preset counter
    timerPreset->setChecked(m_meas->isMaster(TIMERID));
    eventsPreset->setChecked(m_meas->isMaster(EVID));
    monitor1Preset->setChecked(m_meas->isMaster(MON1ID));
    monitor2Preset->setChecked(m_meas->isMaster(MON2ID));
    monitor1Preset->setChecked(m_meas->isMaster(MON3ID));
    monitor2Preset->setChecked(m_meas->isMaster(MON4ID));
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
	MSG_DEBUG << "MainWidget::mpsdCheck() : module " << mod;
	displayMpsdSlot(mod);
}

/*!
    \fn void MainWidget::setHistogramMode(bool histo)

    \param histo
*/
void MainWidget::setHistogramMode(bool histo)
{
    if (histo)
    {
        m_dataFrame->enableAxis(QwtPlot::yRight, true);
        m_histogram->setDisplayMode(QwtPlotSpectrogram::ImageMode, true);
        m_histogram->setDefaultContourPen(histo ? QPen() : QPen(Qt::NoPen));
	
//      QRectF tmpRect = m_zoomer->zoomRect();
	
        m_diffractogram->detach();
        for (int i = 0; i < 8; ++i)
            m_curve[i]->detach();
        if (log->isChecked())
            m_dataFrame->setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
        m_dataFrame->setAxisTitle(QwtPlot::xBottom, tr("tube"));
        m_dataFrame->setAxisTitle(QwtPlot::yLeft, tr("channel"));
        m_dataFrame->setAxisScale(QwtPlot::yLeft, 0, m_meas ? m_meas->width() : 1.0);
        m_dataFrame->setAxisAutoScale(QwtPlot::xBottom);
        m_picker->setTrackerPen(QColor(Qt::white));
        m_zoomer->setRubberBandPen(QColor(Qt::white));
        m_zoomer->setTrackerPen(QColor(Qt::white));
#if 0
        QPointF left(ceil(m_zoomer->zoomRect().x()), ceil(m_zoomer->zoomRect().y())),
        	right(trunc(m_zoomer->zoomRect().x() + m_zoomer->zoomRect().width()), trunc(m_zoomer->zoomRect().y() + m_zoomer->zoomRect().height()));
        m_meas->setROI(QwtDoubleRect(right, left));
#endif
        m_histogram->attach(m_dataFrame);
        m_zoomer->zoom(0);
        emit redraw();
    }
}

/*!
    \fn void MainWidget::setSpectraMode(bool spectra)

    \param spectra
*/
void MainWidget::setSpectraMode(bool spectra)
{
    if (spectra)
    {
        m_histogram->detach();
        m_diffractogram->detach();
        m_dataFrame->enableAxis(QwtPlot::yRight, false);
        
        m_dataFrame->setAxisScaleEngine(QwtPlot::yLeft, log->isChecked() ? (QwtScaleEngine *)new QwtLog10ScaleEngine : (QwtScaleEngine *)new QwtLinearScaleEngine);
        m_dataFrame->setAxisTitle(QwtPlot::xBottom, tr("channel"));
        m_dataFrame->setAxisTitle(QwtPlot::yLeft, tr("counts"));
        m_dataFrame->setAxisScale(QwtPlot::xBottom, 0, m_meas->width());
        m_dataFrame->setAxisAutoScale(QwtPlot::yLeft);
        m_picker->setTrackerPen(QColor(Qt::black));
        m_zoomer->setRubberBandPen(QColor(Qt::black));
        m_zoomer->setTrackerPen(QColor(Qt::black));
        for (int i = 0; i < 8; ++i)
            m_curve[i]->attach(m_dataFrame);
        m_zoomer->zoom(0);
#if 0
	if (!m_lastZoom.isEmpty())
		m_zoomer->zoom(m_lastZoom);
	m_lastZoom = tmpRect;
#endif
        emit redraw();
    }
}

/*!
    \fn void MainWidget::setDiffractogramMode(bool diff)

    \param diff
*/
void MainWidget::setDiffractogramMode(bool diff)
{
    if (diff)
    {
        for (int i = 0; i < 8; ++i)
            m_curve[i]->detach();
        m_histogram->detach();
        m_dataFrame->enableAxis(QwtPlot::yRight, false);
        
        m_dataFrame->setAxisScaleEngine(QwtPlot::yLeft, log->isChecked() ? (QwtScaleEngine *)new QwtLog10ScaleEngine : (QwtScaleEngine *)new QwtLinearScaleEngine);
        m_dataFrame->setAxisTitle(QwtPlot::xBottom, tr("tube"));
        m_dataFrame->setAxisTitle(QwtPlot::yLeft, tr("counts"));
        m_dataFrame->setAxisScale(QwtPlot::xBottom, 0, m_meas->hist(Measurement::PositionHistogram)->height());
        m_dataFrame->setAxisAutoScale(QwtPlot::yLeft);
        m_picker->setTrackerPen(QColor(Qt::black));
        m_zoomer->setRubberBandPen(QColor(Qt::black));
        m_zoomer->setTrackerPen(QColor(Qt::black));
        m_diffractogram->attach(m_dataFrame);
        m_zoomer->zoom(0);
        emit redraw();
    }
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
    if (dispHistogram->isChecked())
    {
        labelCountsInROI->setText(tr("Counts in ROI"));
	enum Measurement::HistogramType t;
        if(dispAllPos->isChecked())
            t = Measurement::PositionHistogram;
        else
            t = Measurement::AmplitudeHistogram;
        m_histData->setData(m_meas->hist(t));
        countsInROI->setText(tr("%1").arg(m_meas->eventsInROI(t)));

        m_histogram->setData(*m_histData);
        QwtDoubleInterval interval = m_histogram->data().range();

        m_dataFrame->setAxisScale(QwtPlot::yRight, interval.minValue(), interval.maxValue());
        m_dataFrame->axisWidget(QwtPlot::yRight)->setColorMap(interval, m_histogram->colorMap());

        if (!m_zoomer->zoomRectIndex())
	{
            m_dataFrame->setAxisScale(QwtPlot::xBottom, 0, m_meas->height());
            m_dataFrame->setAxisScale(QwtPlot::yLeft, 0, m_meas->width());
	}
    }
    else if (dispDiffractogram->isChecked())
    {
        labelCountsInROI->setText(tr("Counts"));
        Spectrum *spec = m_meas->spectrum(Measurement::Diffractogram); 
        m_data->setData(spec);
        m_diffractogram->setData(*m_data);
        if (!m_zoomer->zoomRectIndex())
            m_dataFrame->setAxisScale(QwtPlot::xBottom, 0, m_meas->height());
        countsInROI->setText(tr("%1").arg(spec->getTotalCounts()));
    }
    else
    {
        if(dispAll->isChecked())
        {
            labelCountsInROI->setText(tr("Counts"));
            for (int i = 1; i < 8; ++i)
                m_curve[i]->detach();
            Spectrum *spec(NULL);
            if(dispAllPos->isChecked())
                spec = m_meas->data(Measurement::PositionHistogram);
            else
                spec = m_meas->data(Measurement::AmplitudeHistogram);
            m_data->setData(spec);
            countsInROI->setText(tr("%1").arg(spec->getTotalCounts()));
        }
        else
        {
            for (int i = 1; i < 8; ++i)
            {
                if (!dispAllChannels->isChecked())
                    m_curve[i]->detach();
                else
                    m_curve[i]->attach(m_dataFrame);
            }
            if (specialBox->isChecked())
            {
                labelCountsInROI->setText(tr("Time"));
		Spectrum *spec = m_meas->spectrum(Measurement::TimeSpectrum);
                m_data->setData(spec);
                countsInROI->setText(tr("%1").arg(spec->getTotalCounts()));
            }
            else if (dispAllChannels->isChecked())
            {
                quint32 chan = dispMcpd->value() * 64 + dispMpsd->value() * 8;
                labelCountsInROI->setText(tr("Counts in MCPD: %1 MPSD: %2").arg(dispMcpd->value()).arg(dispMpsd->value()));
                quint64 counts(0);
                for (int i = 7; i >= 0; --i)
                {
                    Spectrum *spec(NULL);
                    if(dispAllPos->isChecked())
                        spec = m_meas->data(Measurement::PositionHistogram, chan + i);
                    else
                        spec = m_meas->data(Measurement::AmplitudeHistogram, chan + i);
                    if (spec)
                        counts += spec->getTotalCounts();
                    m_data->setData(spec);
                    if (m_data->size())
                        m_curve[i]->setData(*m_data);
                    countsInROI->setText(tr("%1").arg(counts));
                }
            }
            else
            {
                quint32 chan = dispMcpd->value() * 64 + dispMpsd->value() * 8 + dispChan->value();
                labelCountsInROI->setText(tr("MCPD: %1 MPSD: %2 Channel: %3").arg(dispMcpd->value()).arg(dispMpsd->value()).arg(dispChan->value()));
                quint64 counts(0);
                Spectrum *spec(NULL);
                if(dispAllPos->isChecked())
                    spec = m_meas->data(Measurement::PositionHistogram, chan);
                else
                    spec = m_meas->data(Measurement::AmplitudeHistogram, chan);
                m_data->setData(spec);
                if (spec)
                    counts = spec->getTotalCounts();
                countsInROI->setText(tr("%1").arg(counts));
            }
        }
        m_curve[0]->setData(*m_data);
        // reduce data in case of threshold settings:
        if (m_dispThresh)
            m_dataFrame->setAxisScale(QwtPlot::yLeft, m_dispLoThresh, m_dispHiThresh);
        else if (!m_zoomer->zoomRectIndex())
            m_dataFrame->setAxisAutoScale(QwtPlot::yLeft);
        if (!m_zoomer->zoomRectIndex())
            m_dataFrame->setAxisScale(QwtPlot::xBottom, 0, m_meas->width());
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
        print(m_printer, filter);
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
//	m_theApp->addMCPD(d.id(), d.ip());
	QMetaObject::invokeMethod(m_theApp, "addMCPD", Qt::BlockingQueuedConnection, Q_ARG(quint16, d.id()), Q_ARG(QString, d.ip()));
	init();
	m_theApp->setTimingSetup(d.id(), d.master(), d.terminate());
    }
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
    ModuleSetup d(m_theApp, this);
    d.exec();
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
//  d.setMCPD(devid_2->value());
    d.exec();
}

/*!
    \fn void MainWidget::setupMdll()

    opens the dialog to handle the MPSD Module setup
*/
void MainWidget::setupMdll(void)
{
    MdllSetup d(m_theApp, this);
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
//	settings.setValue("config/listfilepath", d.listFilePath());
//	settings.setValue("config/histfilepath", d.histFilePath());

        m_meas->setListfilepath(d.listFilePath());
        m_meas->setHistfilepath(d.histFilePath());
        m_meas->setConfigfilepath(d.configFilePath());
        m_meas->setRunId(d.lastRunId());
    }
}

/*!
    \fn void MainWidget::printPlot(void)

    opens a dialog to select the printer and if the ok button pressed printout the plot area widget
*/
void MainWidget::printPlot(void)
{
//  QPrinter printer(QPrinter::HighResolution);
    m_printer->setOutputFormat(QPrinter::NativeFormat);
    QPrintDialog dialog(m_printer);
    if(dialog.exec() == QDialog::Accepted)
    {
        QwtPlotPrintFilter filter;
        filter.setOptions(QwtPlotPrintFilter::PrintAll & ~QwtPlotPrintFilter::PrintBackground);
        print(m_printer, filter);
    }
}

/*!
    \fn void MainWidget::print(QPrinter *printer, QwtPlotPrintFilter &filter)

*/
void MainWidget::print(QPrinter *printer, QwtPlotPrintFilter &filter)
{
    QPen pen = m_curve[0]->pen();
    pen.setWidth(1);
    m_curve[0]->setPen(pen);
    m_dataFrame->print(*printer, filter);
    pen.setWidth(0);
    m_curve[0]->setPen(pen);
}

/*!
    \fn void MainWidget::quitContinue(void)

    asks for continue or abort in case of not initialized remote interface
*/
void MainWidget::quitContinue(void)
{
	if (QMessageBox::warning(this, tr("Remote control interface"), tr("Remote control interface is not initialized!<br>"
			"Do you want continue?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No)
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
}

/*!
   \fn void MainWidget::customEvent(QEvent *e)

   handles the customEvents sent via the remote interface
*/
void MainWidget::customEvent(QEvent *e)
{
    CommandEvent *event = dynamic_cast<CommandEvent*>(e);
    if(!event)
    {
        QWidget::customEvent(e);
        return;
    }
    else
    {
        CommandEvent::Command cmd = event->getCommand();
        QList<QVariant> args = event->getArgs();

        MultipleLoopApplication *app = dynamic_cast<MultipleLoopApplication*>(QApplication::instance());
        QMesyDAQDetectorInterface *interface = NULL;
        if(app)
	{
            interface = dynamic_cast<QMesyDAQDetectorInterface*>(app->getQtInterface());
        }

        switch(cmd)
	{
            case CommandEvent::C_START:
                clearMpsd->click();
            case CommandEvent::C_RESUME:
                if (!startStopButton->isChecked())
                        	startStopButton->animateClick();
                break;
            case CommandEvent::C_STOP:
                if (startStopButton->isChecked())
            			startStopButton->animateClick();
                break;
            case CommandEvent::C_CLEAR:
                clearAll->click();
                break;
            case CommandEvent::C_PRESELECTION:
                if(interface)
                {
                    double value(0);
                    if (timerPreset->isChecked())
                        value = timerPreset->value();
                    else if (eventsPreset->isChecked())
                        value = eventsPreset->value();
                    else if (monitor1Preset->isChecked())
                        value = monitor1Preset->value();
                    else if (monitor2Preset->isChecked())
                        value = monitor2Preset->value();
                    else if (monitor3Preset->isChecked())
                        value = monitor3Preset->value();
                    else if (monitor4Preset->isChecked())
                        value = monitor4Preset->value();
                    interface->postCommandToInterface(CommandEvent::C_PRESELECTION, QList<QVariant>() << value);
                }
                break;
            case CommandEvent::C_READ_DIFFRACTOGRAM:
                if(interface)
                {
                    QList<QVariant> retVal;
                    Spectrum *tmpSpectrum = m_meas->spectrum(Measurement::Diffractogram);
                    if (tmpSpectrum->width() > 0)
                    {
                        for (int i = 0; i < tmpSpectrum->width(); ++i)
                            retVal << tmpSpectrum->value(i);
                    }
                    else
                        retVal << m_meas->events();
                    interface->postCommandToInterface(CommandEvent::C_READ_DIFFRACTOGRAM, retVal);
                }
                break;
            case CommandEvent::C_READ_HISTOGRAM_SIZE:
                if (interface)
                {
                    QList<QVariant> retVal;
                    Histogram *tmpHistogram = m_meas->hist(Measurement::PositionHistogram);
                    retVal << tmpHistogram->height(); 	// width  (should be equal to number of MPSD inputs)
                    retVal << tmpHistogram->width(); 	// width  (should be equal to number of MPSD inputs)
//                  retVal << (m_meas->width() + 1);    // height (should be 960)
                    interface->postCommandToInterface(CommandEvent::C_READ_HISTOGRAM_SIZE, retVal);
                }
                break;
            case CommandEvent::C_READ_HISTOGRAM:
                if(interface)
                {
                    QList<QVariant> retVal;
                    QList<quint64>* tmpData = new QList<quint64>();
                    Histogram *tmpHistogram = m_meas->hist(Measurement::PositionHistogram);
                    if (tmpHistogram->height() > 0 && tmpHistogram->width() > 0)
                    {
                        // CARESS has it's x=0:y=0 position at top left corner
                        for (int y = tmpHistogram->width() - 1; y >= 0; --y)
                            for (int x = 0; x < tmpHistogram->height(); ++x)
                                tmpData->append(tmpHistogram->value(x, y));
                    }
                    else
                        tmpData->append(m_meas->events());
										// hack to transfer a QList<quint64> to QtInterface without to copy it
                    retVal << ((quint64)tmpData);
                    interface->postCommandToInterface(CommandEvent::C_READ_HISTOGRAM, retVal);
                }
                break;
            case CommandEvent::C_READ_SPECTROGRAM:
                if(interface)
                {
                    QList<QVariant> retVal;
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
                        tmpSpectrum = tmpHistogram->spectrum();
                    if (tmpSpectrum->width() > 0)
                    {
                        for (int x = 0; x < tmpSpectrum->width(); ++x)
                            retVal << tmpSpectrum->value(x);
                    }
                    else
                        retVal << m_meas->events();
                    interface->postCommandToInterface(CommandEvent::C_READ_SPECTROGRAM, retVal);
                }
                break;
            case CommandEvent::C_STATUS:
                if(interface)
                {
                    int i = startStopButton->isChecked();
                    interface->postCommandToInterface(CommandEvent::C_STATUS,QList<QVariant>() << i);
                }
                break;
            case CommandEvent::C_MAPCORRECTION: // mapping and correction data
                if (interface)
                {
                    MapCorrection *&pMap = m_meas->posHistMapCorrection();
                    if (!args.isEmpty())
                    {
                        MappedHistogram *&pHist = m_meas->posHistCorrected();
                        MapCorrection *pNewMap = dynamic_cast<MapCorrection*>((QObject*)args[0].toULongLong());
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
                        // query for current mapping and correction data
                        interface->postCommandToInterface(CommandEvent::C_MAPCORRECTION,QList<QVariant>() << ((quint64)pMap));
                }
                break;
            case CommandEvent::C_MAPPEDHISTOGRAM: // mapped and corrected position histogram
                if (interface)
                {
                    MappedHistogram*& pHist=m_meas->posHistCorrected();
                    interface->postCommandToInterface(CommandEvent::C_MAPPEDHISTOGRAM,QList<QVariant>() << ((quint64)pHist));
                }
                break;
            default :
                break;
        }

        if (!args.isEmpty())
	{
            switch(cmd)
            {
                case CommandEvent::C_READ_COUNTER:
                    if (interface)
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
                                value = m_meas->timer()/1000.0;
				break;
                        }
                        interface->postCommandToInterface(CommandEvent::C_READ_COUNTER,QList<QVariant>() << value);
                    }
                    break;
                case CommandEvent::C_SELECT_COUNTER:
                    switch (args[0].toInt())
                    {
                        case M1CT: 
				monitor1Preset->setChecked(true); 
				break;
                        case M2CT: 
				monitor2Preset->setChecked(true); 
				break;
                        case M3CT: 
				monitor3Preset->setChecked(true); 
				break;
                        case M4CT: 
				monitor4Preset->setChecked(true); 
				break;
                        case EVCT: 
				eventsPreset->setChecked(true);  
				break;
                        case TCT:  
				timerPreset->setChecked(true);  
				break;
                    }
                    break;
                case CommandEvent::C_SET_PRESELECTION:
                    if (timerPreset->isChecked())
                    {
                        timerPreset->setValue(args[0].toDouble());
                        m_meas->setPreset(TIMERID, quint64(timerPreset->value() * 1000), true);
                    }
                    else if (eventsPreset->isChecked())
                    {
                        eventsPreset->setValue(args[0].toInt());
                        m_meas->setPreset(EVID, eventsPreset->value(), true);
                    }
                    else if (monitor1Preset->isChecked())
                    {
                        monitor1Preset->setValue(args[0].toInt());
                        m_meas->setPreset(MON1ID, monitor1Preset->value(), true);
                    }
                    else if (monitor2Preset->isChecked())
                    {
                        monitor2Preset->setValue(args[0].toInt());
                        m_meas->setPreset(MON2ID, monitor2Preset->value(), true);
                    }
                    else if (monitor3Preset->isChecked())
                    {
                        monitor3Preset->setValue(args[0].toInt());
                        m_meas->setPreset(MON3ID, monitor3Preset->value(), true);
                    }
                    else if (monitor4Preset->isChecked())
                    {
                        monitor4Preset->setValue(args[0].toInt());
                        m_meas->setPreset(MON4ID, monitor4Preset->value(), true);
                    }
                    break;
                case CommandEvent::C_SET_LISTMODE:
                    acquireFile->setChecked(args[0].toBool());
                    break;
                case CommandEvent::C_SET_LISTHEADER:
                    if (interface)
                    {
			QByteArray header((const char*)args[0].toULongLong(),args[1].toInt());
			m_meas->setListFileHeader(header);
			interface->postCommandToInterface(CommandEvent::C_SET_LISTHEADER);
		    }
		    break;
		case CommandEvent::C_UPDATEMAINWIDGET:
		    if (args.count() >= 3)
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
		    }
		    break;
	        default:
		    break;
            }
        }
    }
}

void MainWidget::moduleHistogramSlot(quint8 id, bool set)
{
//	MSG_DEBUG << tr("MainWidget::moduleHistogramSlot %1 %2").arg(id).arg(set);
	m_theApp->setHistogram(devid_2->value(), id, set);
}

void MainWidget::moduleActiveSlot(quint8 id, bool set)
{
	m_theApp->setActive(devid_2->value(), id, set);
}
