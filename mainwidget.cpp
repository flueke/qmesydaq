/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann   *
 *   g.montermann@mesytec.com   *
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

#include <qwt_plot_curve.h>

#include <cmath>

#include "mainwidget.h"
#include "mdefines.h"
#include "mcpd8.h"
#include "mpsd8.h"
#include "measurement.h"
#include "mesydaq2.h"
#include "histogram.h"
#include "mesydaqdata.h"

MainWidget::MainWidget(Mesydaq2 *mesy, QWidget *parent)
	: QWidget(parent)
	, Ui_Mesydaq2MainWidget()
	, m_theApp(mesy)
	, m_dispMax(10)
	, m_dispRange(m_dispMax)
	, m_dispLen(0)
	, m_height(480)
{
	setupUi(this);
	
        QObject::connect(acqListfile, SIGNAL(toggled(bool)), m_theApp, SLOT(acqListfile(bool)));
        QObject::connect(allPulsersoffButton, SIGNAL(clicked()), m_theApp, SLOT(allPulserOff()));

	qDebug("comgain->isChecked() %d", comgain->isChecked());
	channelLabel->setHidden(true /* comgain->isChecked() */);
	channelLabel->setHidden(true /* comgain->isChecked() */);
	scanPeriSlot();
	m_pBuffer = (quint16 *) &m_cmdBuffer;
	width = 960;
	xstep = 1;
	ystep = 1;
	scale = 1;
	dispLog = false;
	dispThresh = false;
	dispLoThresh = 0;

	versionLabel->setText("QMesyDAQ " VERSION __DATE__);

	QRegExp ex("(([01]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])\\.){3}([01]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])"); 
	mcpdIPAddress->setValidator(new QRegExpValidator(ex, mcpdIPAddress));
	dataIPAddress->setValidator(new QRegExpValidator(ex, dataIPAddress));
	cmdIPAddress->setValidator(new QRegExpValidator(ex, cmdIPAddress));

	dataFrame->setAxisTitle(QwtPlot::xBottom, "channels");
	dataFrame->setAxisTitle(QwtPlot::yLeft, "counts");

	m_curve = new QwtPlotCurve("");
	m_curve->setStyle(QwtPlotCurve::Steps);
#if QT_VERSION >= 0x040000
	m_curve->setRenderHint(QwtPlotItem::RenderAntialiased);
#endif
	m_curve->setPen(QPen(Qt::black));
	m_curve->attach(dataFrame);
	m_data = new MesydaqData();
	m_curve->setData(*m_data);
}


MainWidget::~MainWidget()
{
}

void MainWidget::startStopSlot()
{
	if(startStopButton->isChecked())
	{
		// get timing binwidth
		m_theApp->setTimingwidth(timingBox->value());
		
		// get latest preset entry
		if(m_theApp->meas->isMaster(TCT))
			m_theApp->meas->setPreset(TCT, tPreset->value(), true);
		if(m_theApp->meas->isMaster(EVCT))
			m_theApp->meas->setPreset(EVCT, evPreset->value(), true);
		if(m_theApp->meas->isMaster(M1CT))
			m_theApp->meas->setPreset(M1CT, m1Preset->value(), true);
		if(m_theApp->meas->isMaster(M2CT))
			m_theApp->meas->setPreset(M2CT, m2Preset->value(), true);
		
		startStopButton->setText("Stop");
		// set device id to 0 -> will be filled by mesydaq for master
		m_theApp->start(); 
	}
	else
	{
		startStopButton->setText("Start");
		// set device idto 0 -> will be filled by mesydaq for master
		m_theApp->stop(); 
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
	m_theApp->protocol("set aux timer", 2);
	bool ok;
	quint16 compare = (quint16)compareAux->text().toInt(&ok, 0);
	m_theApp->setAuxTimer(mcpdId->value(), timer->value(), compare); 
}

void MainWidget::resetTimerSlot()
{
	quint16 id = mcpdId->value();
	m_theApp->protocol("reset timer", 2);
	m_theApp->setMasterClock(id, 0LL); 
}

void MainWidget::setTimingSlot()
{
	quint16 id = mcpdId->value();
	resetTimer->setEnabled(master->isChecked());
	m_theApp->protocol("set timing", 2);
	m_theApp->setTimingSetup(id, master->isChecked(), terminate->isChecked());
}

void MainWidget::setMcpdIdSlot()
{
	m_theApp->setId(mcpdId->value(), deviceId->value()); 
}

void MainWidget::setStreamSlot()
{
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
	
	quint16 pos;
	if(pulsLeft->isChecked())
		pos = 0;
	if(pulsRight->isChecked())
		pos = 1;
	if(pulsMid->isChecked())
		pos = 2;
	
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
#if 0	
	if(gainval < 0.5)
		gainval = 0.5;
	if(gainval > 1.88)
		gainval = 1.88;
#endif		
#warning TODO
	MPSD8	*ptrMPSD = m_theApp->m_mcpd[id]->m_mpsd[addr];
	m_theApp->setGain(id, addr, chan, ptrMPSD->calcGainpoti(gainval)); 
}

void MainWidget::setThresholdSlot()
{
	bool ok;
	quint16 id = (quint16) devid->value();	
	quint16 addr = module->value();
	quint16 thresh = threshold->text().toUInt(&ok, 0);

#warning TODO
	MPSD8	*ptrMPSD = m_theApp->m_mcpd[id]->m_mpsd[addr];
	m_theApp->setThreshold(id, addr, ptrMPSD->calcThreshpoti(thresh)); 
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
  	}
	m_theApp->setListfilename(name);
	dispFiledata();
}

/*!
    \fn MainWidget::update(void)
 */
void MainWidget::update(void)
{
   	quint16 id = (quint16) paramId->value();	
	m_pstring.sprintf("%ld", m_theApp->m_dataRxd);
	dataRx->setText(m_pstring);
	m_pstring.sprintf("%ld", m_theApp->m_cmdTxd);
	cmdTx->setText(m_pstring);
	m_pstring.sprintf("%ld", m_theApp->m_cmdRxd);;
	cmdRx->setText(m_pstring);
	m_pstring = buildTimestring(m_theApp->m_headertime, true);
	hTimeText->setText(m_pstring);
	m_pstring = buildTimestring(m_theApp->meas->getMeastime(), false);
	mTimeText->setText(m_pstring);   
    
// parameter values for selected ID
	m_pstring.sprintf("%llu", m_theApp->m_mcpd[id]->getParameter(0));
	param0->setText(m_pstring);
	m_pstring.sprintf("%llu", m_theApp->m_mcpd[id]->getParameter(1));
	param1->setText(m_pstring);
	m_pstring.sprintf("%llu", m_theApp->m_mcpd[id]->getParameter(2));
	param2->setText(m_pstring);
	m_pstring.sprintf("%llu", m_theApp->m_mcpd[id]->getParameter(3));
	param3->setText(m_pstring);
	m_theApp->meas->calcMeanRates();
    
// measurement values counters and rates
	m_pstring.sprintf("%llu", m_theApp->meas->getCounter(EVCT));
	totalCounts->setText(m_pstring);
	m_pstring.sprintf("%llu", m_theApp->meas->getCounter(TCT));
	tSecsText->setText(m_pstring);
	m_pstring.sprintf("%lu", m_theApp->meas->getRate(EVCT));
	eventRate->setText(m_pstring);
	m_pstring.sprintf("%llu", m_theApp->meas->getCounter(M1CT));
	monitor1->setText(m_pstring);
	m_pstring.sprintf("%llu", m_theApp->meas->getCounter(M2CT));
	monitor2->setText(m_pstring);
	m_pstring.sprintf("%lu", m_theApp->meas->getRate(M1CT));
	monRate1->setText(m_pstring);
	m_pstring.sprintf("%lu", m_theApp->meas->getRate(M2CT));
	monRate2->setText(m_pstring);
}

/*!
    \fn MainWidget::buildTimestring(unsigned long nsec)
 */
QString MainWidget::buildTimestring(ulong timeval, bool nano)
{
// nsec = time in 100 nsecs
//-> usec = 
//->
	QString str;
	ulong nsec, val, sec, min, hr;
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
//		qDebug("%d %d %d", timeval, val, nsec);
	}
// hours = val / 3600 (s/h)
	hr = val / 3600;
// remaining seconds:
	val -= hr * 3600;
// minutes:
	min = val / 60;
// remaining seconds:
	sec = val - (min * 60);
//	qDebug("%x - %x - %x", m_theApp->ht[2], m_theApp->ht[1], m_theApp->ht[0]);
//	qDebug("%lu %lu %lu %lu %lu", nsecs, hr, min, sec, nsec);
	str.sprintf("%2lu:%2lu:%2lu", hr, min, sec);
	return str;
}


/*!
    \fn MainWidget::setData(unsigned long * data, unsigned int len)
 */
void MainWidget::setData(ulong *data, quint32 len, ulong max)
{
	qDebug("setData, len : %d, max: %ld", len, max);
	ulong m = max * 1.1;
	m_pDispBuffer = data;
	m_dispLen = len;
	m_dispMax = 10;
	m_dispRange = m_dispMax;
	scale = 1;

	m_data->setData(data, len);
	qDebug("setData max : %u", m_data->max());
	m_curve->setData(*m_data);
    
// reduce data in case of threshold settings:
	if(dispThresh)
	{
		for(quint32 i = 0; i < len; i++)
		{
//    			if(m_pDispBuffer[i] > dispLoThresh && m_pDispBuffer[i] < dispHiThresh)
//    				m_pDispBuffer[i] -= dispLoThresh;
//    			else
//			{ 
				if(m_pDispBuffer[i] < dispLoThresh)
					m_pDispBuffer[i] = 0;
				if(m_pDispBuffer[i] > dispHiThresh)
					m_pDispBuffer[i] = dispHiThresh;
//	    		}
    		}
    	}
// check for upper threshold
	if(dispThresh)
	{
		if(dispHiThresh < max)
		{
			m_dispMax = dispHiThresh;
//	    		m_dispRange = m_dispMax - dispLoThresh;
	    		m_dispRange = m_dispMax;
	    	}
	    	else
		{
		    // calculate maximum
			while(m > m_dispMax)
				m_dispMax *= 1.3;    	
			m_dispRange = m_dispMax;
		}
   	}
	else
	{
	    // calculate maximum
		while(m > m_dispMax)
			m_dispMax *= 1.3;    	
		m_dispRange = m_dispMax;
	}
}

/*!
    \fn MainWidget::draw(void)
 */
void MainWidget::draw(void)
{
//	qDebug("draw()");
	dispId = dispMcpd->value();
	dataFrame->replot();
	drawOpData();
//	displayMpsdSlot();
}

void MainWidget::clearAllSlot()
{
	m_theApp->clearAllHist();
}

void MainWidget::clearMcpdSlot()
{
	quint32 start = dispMcpd->value() * 64;
	for(quint32 i = start; i < start + 64; i++)
		m_theApp->clearChanHist(i);
	m_theApp->draw();
}

void MainWidget::clearMpsdSlot()
{
	quint32 start = dispMpsd->value() * 8 + dispMcpd->value() * 64;
//	qDebug("clearMpsd: %d", start);
	for(quint32 i = start; i < start + 8; i++)
		m_theApp->clearChanHist(i);
	m_theApp->draw();
}

void MainWidget::clearChanSlot()
{
	ulong chan = dispChan->value() + dispMpsd->value() * 8 + dispMcpd->value() * 64;
//	qDebug("clearChan: %d",chan);
	m_theApp->clearChanHist(chan);
	m_theApp->draw();
}


void MainWidget::replayListfileSlot()
{
	QString name = QFileDialog::getOpenFileName(this, "Load...", m_theApp->getListfilepath(), "mesydaq data files (*.mdat);;all files (*.*);;really all files (*)");
	if(!name.isEmpty())
		m_theApp->readListfile(name);
}

void MainWidget::setRunIdSlot()
{
	quint16 runid = (quint16) devid->value();	

	m_theApp->m_mcpd[0]->setRunId(runid); 
	m_pstring.sprintf("Set run ID to %d", runid);	
	m_theApp->protocol(m_pstring, 2);
}

/*!
    \fn MainWidget::dispMcpdSlot(void)
 */
void MainWidget::displayMcpdSlot(int)
{
	QString str;
	quint16 values[4];
// retrieve displayed ID
	quint8 id = mcpdId->value();
     
// now get and display parameters:
    
// get cell parameters
	m_theApp->m_mcpd[id]->getCounterCell(cellSource->currentIndex(), values);
	cellTrigger->setCurrentIndex(values[0]);
	cellCompare->setValue(values[1]);
    
// get parameter settings
	paramSource->setCurrentIndex(m_theApp->m_mcpd[id]->getParamSource(param->value()));

// get timer settings
	str.sprintf("%x", m_theApp->m_mcpd[id]->getAuxTimer(timer->value()));
	compareAux->setText(str); 
	
// get stream setting
//	statusStream->setChecked(m_theApp->myMcpd[id]->getStream());	
}


/*!
    \fn MainWidget::dispMpsdSlot(void)
 */
void MainWidget::displayMpsdSlot(int)
{
	QString dstr;
    
// retrieve displayed ID
	quint8 id = mcpdId->value();
	quint8 mod = module->value();
    
// Status display:
	if(m_theApp->getMpsdId(id, 0))
		status0->setText(tr("%1").arg(m_theApp->getMpsdId(id, 0)));
	else
		status0->setText("-");		
	if(m_theApp->getMpsdId(id, 1))
		status1->setText(tr("%1").arg(m_theApp->getMpsdId(id, 1)));
	else
		status1->setText("-");		
	if(m_theApp->getMpsdId(id, 2))
		status2->setText(tr("%1").arg(m_theApp->getMpsdId(id, 2)));
	else
		status2->setText("-");		
	if(m_theApp->getMpsdId(id, 3))
		status3->setText(tr("%1").arg(m_theApp->getMpsdId(id, 3)));
	else
		status3->setText("-");		
	if(m_theApp->getMpsdId(id, 4))
		status4->setText(tr("%1").arg(m_theApp->getMpsdId(id, 4)));
	else
		status4->setText("-");		
	if(m_theApp->getMpsdId(id, 5))
		status5->setText(tr("%1").arg(m_theApp->getMpsdId(id, 5)));
	else
		status5->setText("-");		
	if(m_theApp->getMpsdId(id, 6))
		status6->setText(tr("%1").arg(m_theApp->getMpsdId(id, 6)));
	else
		status6->setText("-");		
	if(m_theApp->getMpsdId(id, 7))
		status7->setText(tr("%1").arg(m_theApp->getMpsdId(id, 7)));
	else
		status7->setText("-");		
		
// gain:
	quint8 chan = channel->value();
	gain->setText(tr("%1").arg(double(m_theApp->getGain(id, mod, chan)), 4, 'f', 2));	
	
// threshold:
	threshold->setText(tr("%1").arg(m_theApp->getThreshold(id, mod)));
		
// pulser:  on/off
	if(m_theApp->m_mcpd[id]->m_mpsd[mod]->isPulserOn())
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
	pulsChan->setValue((int)m_theApp->m_mcpd[id]->m_mpsd[mod]->getPulsChan());
// amplitude
	dstr.sprintf("%3d", m_theApp->m_mcpd[id]->m_mpsd[mod]->getPulsAmp(0));
//	pulsAmp->setValue((int)m_theApp->m_mpsd[8*id+mod]->getPulsAmp());
// position
	switch(m_theApp->m_mcpd[id]->m_mpsd[mod]->getPulsPos())
	{
		case 0:
			pulsLeft->setChecked(true);
			break;
		case 1:
			pulsRight->setChecked(true);
			break;
		case 2:
			pulsMid->setChecked(true);
			break;
	}
// mode
	if(m_theApp->m_mcpd[id]->getMode(mod))
		amp->setChecked(true);
	else
		Ui_Mesydaq2MainWidget::pos->setChecked(true);
}



void MainWidget::scanPeriSlot()
{
	m_theApp->scanPeriph((quint16)devid->value());
	displayMpsdSlot();
}

void MainWidget::setModeSlot(qint32 mode)
{
	m_theApp->setMode(devid->value(), module->value(), mode); 
}

void MainWidget::saveSetupSlot()
{
	m_theApp->saveSetup();
}

void MainWidget::restoreSetupSlot()
{
	m_theApp->loadSetup(true);
}


void MainWidget::redrawSlot()
{
	draw();
}


/*!
    \fn MainWidget::processDispData()
 */
void MainWidget::processDispData()
{
    /// @todo implement me
}

void MainWidget::applyThreshSlot()
{
	bool ok;
	if(useThresh->isChecked()){
		dispThresh = true;
		dispHiThresh = hiLim->text().toUInt(&ok, 0);
		dispLoThresh = loLim->text().toUInt(&ok, 0);
		qDebug("lo: %ld, hi: %ld", dispLoThresh, dispHiThresh);
	}
	else
		dispThresh = false;		
}

void MainWidget::linlogSlot()
{
	if(log->isChecked()){
		dispLog = true;
		qDebug("log");
	}
	else{
		dispLog = false;
		qDebug("lin");
	}
}


/*!
    \fn MainWidget::drawOpData()
 */
void MainWidget::drawOpData()
{
	float meansigma[2] = {0.0, 0.0};
	
// display mean and sigma:
#if 0
	if(dispAll->isChecked())
	{
		if(dispAllPos->isChecked())
			m_theApp->hist[dispId]->getMean(CHANNELS+1, meansigma);
		else
			m_theApp->hist[dispId]->getMean(CHANNELS+2, meansigma);
	}
	else{
		if(specialBox->isChecked())
			m_theApp->hist[dispId]->getMean(CHANNELS, meansigma);
		else
			m_theApp->hist[dispId]->getMean(dispMpsd->value()*8*2 + dispChan->value(), meansigma);
	}
#endif	
	meanText->setText(tr("%1").arg(meansigma[0], 3, 'f', 1));
	sigmaText->setText(tr("%1").arg(meansigma[1], 3, 'f', 1));
// pulser warning
	if(m_theApp->isPulserOn())
		pulserWarning->setText("<p align=\"center\">PULSER ON!</p>");
	else
		pulserWarning->setText("");
}


void MainWidget::writeRegisterSlot()
{
	bool ok;
	quint16 id = (quint16) devid->value();	
	quint16 addr = module->value();
	quint16 reg = registerSelect->value();
	quint16 val = registerValue->text().toUInt(&ok, 0);
	m_theApp->writeRegister(id, addr, reg, val);
}

void MainWidget::readRegisterSlot()
{
	quint16 id = (quint16) devid->value();	
	quint16 addr = module->value();
	quint16 reg = registerSelect->value();
	m_theApp->readPeriReg(id, addr, reg);
}

void MainWidget::selectConfigpathSlot()
{
	QString name = QFileDialog::getExistingDirectory((QWidget *)this, tr("Select Config File Path..."), m_theApp->getConfigfilepath());
	if(!name.isEmpty())
		m_theApp->setConfigfilepath(name);
	dispFiledata();
}


void MainWidget::selectHistpathSlot()
{
	QString name = QFileDialog::getExistingDirectory((QWidget *)this, tr("Select Histogram File  Path..."), m_theApp->getHistfilepath());
	if(!name.isEmpty())
		m_theApp->setHistfilepath(name);
	dispFiledata();
}


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
    \fn MainWidget::getDispId(void)
 */
quint8 MainWidget::getDispId(void)
{
	return dispMcpd->value();
}


void MainWidget::writeHistSlot()
{
	QString name = QFileDialog::getSaveFileName(this, tr("Write Histogram..."), m_theApp->getHistfilepath(), 
				"mesydaq histogram files (*.mtxt);;all files (*.*)");
  	if(!name.isEmpty())
	{
    		int i = name.indexOf(".mtxt");
		if(i == -1)
			name.append(".mtxt");
 		m_theApp->setHistfilename(name);
		m_theApp->writeHistograms();
  	}
}


void MainWidget::ePresetSlot(bool pr)
{
	if(pr)
	{
		m1PresetButton->setChecked(false);
		m2PresetButton->setChecked(false);
		tPresetButton->setChecked(false);
		m1Preset->setEnabled(false);
		m2Preset->setEnabled(false);
		tPreset->setEnabled(false);
	}
	evPreset->setEnabled(pr);
	m_theApp->meas->setPreset(EVCT, evPreset->value(), pr);
}


void MainWidget::tPresetSlot(bool pr)
{
	if(pr)
	{
		m1PresetButton->setChecked(false);
		m2PresetButton->setChecked(false);
		ePresetButton->setChecked(false);
		m1Preset->setEnabled(false);
		m2Preset->setEnabled(false);
		evPreset->setEnabled(false);
	}
	tPreset->setEnabled(pr);
	m_theApp->meas->setPreset(TCT, tPreset->value(), pr);
}


void MainWidget::m1PresetSlot(bool pr)
{
	if(pr)
	{
		ePresetButton->setChecked(false);
		m2PresetButton->setChecked(false);
		tPresetButton->setChecked(false);
		m2Preset->setEnabled(false);
		evPreset->setEnabled(false);
		tPreset->setEnabled(false);
	}
	m1Preset->setEnabled(pr);
	m_theApp->meas->setPreset(M1CT, m1Preset->value(), pr);
}


void MainWidget::m2PresetSlot(bool pr)
{
	if(pr)
	{
		m1PresetButton->setChecked(false);
		ePresetButton->setChecked(false);
		tPresetButton->setChecked(false);
		m1Preset->setEnabled(false);
		evPreset->setEnabled(false);
		tPreset->setEnabled(false);
	}
	m2Preset->setEnabled(pr);
	m_theApp->meas->setPreset(M2CT, m2Preset->value(), pr);
}


/*!
    \fn MainWidget::updatePresets(void)
 */
void MainWidget::updatePresets(void)
{
// presets
	evPreset->setValue(m_theApp->meas->getPreset(EVCT));
	tPreset->setValue(m_theApp->meas->getPreset(TCT));
	m1Preset->setValue(m_theApp->meas->getPreset(M1CT));
	m2Preset->setValue(m_theApp->meas->getPreset(M2CT));
    
// check for master preset counter
	ePresetButton->setChecked(m_theApp->meas->isMaster(EVCT));
    	m1PresetButton->setChecked(m_theApp->meas->isMaster(M1CT));
    	m2PresetButton->setChecked(m_theApp->meas->isMaster(M2CT));
    	tPresetButton->setChecked(m_theApp->meas->isMaster(TCT));
   
// Caress values
	m_pstring.sprintf("%d", m_theApp->meas->getCarHeight());
	caressHeight->setText(m_pstring);
	m_pstring.sprintf("%d", m_theApp->meas->getCarWidth());
	caressWidth->setText(m_pstring);
	m_pstring.sprintf("%d", m_theApp->meas->getRun());
	caressRun->setText(m_pstring);
}

void MainWidget::tResetSlot()
{
	m_theApp->meas->clearCounter(TCT);
	update();
}

void MainWidget::eResetSlot()
{
	m_theApp->meas->clearCounter(EVCT);
	update();
}

void MainWidget::m1ResetSlot()
{
	m_theApp->meas->clearCounter(M1CT);
	update();
}

void MainWidget::m2ResetSlot()
{
	m_theApp->meas->clearCounter(M2CT);
	update();
}



/*!
    \fn MainWidget::updateCaress(void)
 */
void MainWidget::updateCaress(void)
{
	QString str;
	str.sprintf("%d", m_theApp->meas->getCarWidth());
	caressWidth->setText(str);
	str.sprintf("%d", m_theApp->meas->getCarHeight());
	caressHeight->setText(str);
	str.sprintf("%d", m_theApp->meas->getRun());
	caressRun->setText(str);
}

/*!
    \fn MainWidget::saveConfigSlot(void)
 */
void MainWidget::saveConfigSlot(void)
{
}

void MainWidget::mpsdCheck(int mod)
{
	m_theApp->protocol(tr("MainWidget::mpsdCheck() : module %1").arg(mod), 1);
	quint8 id = mcpdId->value();
	if (m_theApp->m_mcpd[id]->getMpsdId(mod))
		displayMpsdSlot(mod);
	else if (mod < 7)
		module->stepUp();
	else
		module->stepBy(-7);
}
