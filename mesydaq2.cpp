/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009 by Jens Kr�ger <jens.krueger@frm2.tum.de>          *
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

// #include <QMainWindow>
#include <QMessageBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QFileDialog>
#include <QDateTime>
#include <QRadioButton>
#include <QDateTime>
#include <QPushButton>
#include <QSettings>

#include "mesydaq2.h"
#include "mainwidget.h"
#include "mdefines.h"
#include "histogram.h"
#include "mcpd8.h"
#include "measurement.h"
#include "corbathread.h"
#include "controlinterface.h"

Mesydaq2::Mesydaq2(QObject *parent)
	: QObject(parent) 
	, m_dataRxd(0)
	, m_cmdRxd(0)
	, m_cmdTxd(0)
	, m_hist(NULL)
	, m_daq(IDLE)
	, sep0(0x0000)
	, sep5(0x5555)
	, sepA(0xAAAA)
	, sepF(0xFFFF)
	, m_acquireListfile(false)
	, m_listfilename("")
	, m_histfilename("")
	, listPath("/home")
	, histPath("/home")
	, configPath("/home")
	, ovwList(false)
	, timingwidth(1)
	, initstep(0)
	, initialize(false)
{
#warning m_hist is not initialized
	meas = new Measurement(this);
	ct = new CorbaThread();
	cInt = new ControlInterface(this);
	
	connect(meas, SIGNAL(protocol(QString, quint8)), this, SLOT(protocol(QString, quint8)));
	connect(meas, SIGNAL(stop()), this, SLOT(stop()));
	connect(meas, SIGNAL(setCountlimit(quint8, ulong)), this, SLOT(setCountlimit(quint8, ulong)));
	connect(meas, SIGNAL(acqListfile(bool)), this, SLOT(acqListfile(bool)));
	
	initValues();
	initDevices();
	initTimers();
	initHardware();
	protocol(tr("running on Qt %1").arg(qVersion()));
#warning TODO	mainWin->displayMcpdSlot();
	dispMpsd();
#warning TODO	mainWin->dispFiledata();
#warning TODO	ct->initializeCorba(this, cInt);
}

Mesydaq2::~Mesydaq2()
{
	dispTimer->stop();
	if (dispTimer)
		delete dispTimer;
	dispTimer = NULL;
	for(quint8 i = 0; i < MCPDS; i++)
	{
		if (m_mcpd[i])
			delete m_mcpd[i];
		m_mcpd[i] = NULL;
	}
	if (m_hist)
		delete m_hist;
	m_hist = NULL;
}

/*!
    \fn Mesydaq2::initValues()
 */
void Mesydaq2::initValues()
{
	for (quint8 c = 0; c < 8; ++c)
		m_countLimit[c] = 0;
	statuscounter[0] = 0;
	statuscounter[1] = 0;
}

/*!
    \fn Mesydaq2::writeRegister(unsigned int id, unsigned int addr, unsigned int val)
 */
void Mesydaq2::writeRegister(quint16 id, quint16 addr, quint16 reg, quint16 val)
{
	m_mcpd[id]->writeRegister(addr, reg, val);
}

void Mesydaq2::analyzeBuffer(DATA_PACKET &pd, quint8 daq, Histogram &hist)
{
	if(daq == RUNNING)
	{
		quint8 	chan, mod, slot, trigId, dataId;
		quint8 	data0, data1, time, counter = 0;
		ulong 	data;
		quint32 i, j;
		quint64 var = 0;
		quint16 neutrons = 0;
		quint16 triggers = 0;
		quint64 htim = pd.time[0] + 0x10000ULL * pd.time[1] + 0x100000000ULL * pd.time[2],
			tim;
	
//		quint16 diff = pd.bufferNumber - m_lastBufnum;
//		if(diff > 1)
//			qDebug("Lost %d Buffers: current: %d, last: %d", diff, recBuf->bufferNumber, lastBufnum);

		quint16	id = pd.deviceId;
		m_lastBufnum = pd.bufferNumber;
		if(m_acquireListfile)
		{
			quint16 *pD = (quint16 *) &pd.bufferLength;
			unsigned int i;
			for(i = 0; i < pd.bufferLength; i++)
			{
				datStream << pD[i];
//				qDebug("%x", pD[i]);
			}
			writeBlockSeparator();
//			qDebug("------------------");
		}
		m_dataRxd++;
		qDebug("dataRxd : %ld", m_dataRxd);
		if(pd.bufferType == 0x0000 || pd.bufferType == 0x0001)
		{
			quint8 id = pd.deviceId;
// extract parameter values:
			for(i = 0; i < 4; i++)
			{
				var = 0;
				for(j = 0; j < 3; j++)
				{
					var *= 0x10000ULL;
					var += pd.param[i][2-j];
				}
				m_mcpd[id]->setParameter((unsigned char)i, var);
//				meas->setCounter(i, var);
			}		
// 			data length = (buffer length - header length) / (3 words per event) - 4 parameters.
 			quint32 datalen = (pd.bufferLength - pd.headerLength) / 3;
			for(i = 0; i < datalen; ++i, counter += 3)
			
			{
				tim = 0x10000 * (pd.data[counter + 1] & 0x0007) + pd.data[counter];
				tim += htim;
// not neutron event (counter, chopper, ...)
				if((pd.data[counter + 2] & 0x8000) == 0x8000)
				{
					triggers++;
					trigId = (pd.data[counter + 2] & 0x7000) / 0x1000;
					dataId = (pd.data[counter + 2] & 0x0F00) / 0x100;
					data = (pd.data[counter + 2] & 0x00FF) * 0x10000 + (pd.data[counter + 1] & 0xFFF8) / 8;
					time = (quint16) tim;
					// dispatch events:
					switch(dataId)
					{
						case MON1ID:
							if((m_countLimit[MON1ID] == 0) || (meas->mon1() < m_countLimit[MON1ID]))
								meas->incMon1();
							break;
						case MON2ID:
							if((m_countLimit[MON2ID] == 0) || (meas->mon2() < m_countLimit[MON2ID]))
								meas->incMon2();
							break;
						default:
							break;
					}
				}
// neutron event:
				else
				{
					neutrons++;
					mod = (pd.data[counter+2] & 0x7000) / 0x1000;
					slot = (pd.data[counter+2] & 0x0780) / 0x80;
					chan = 8 * 2 * mod + slot;
					data1 = (pd.data[counter+2] & 0x007F) * 8 + (pd.data[counter+1] & 0xE000) / 8192;
					data0 = (pd.data[counter+1] & 0x1FF8) / 8;
					if((m_countLimit[EVID] == 0) || (meas->events() < m_countLimit[EVID]))
					{
//						hist.incVal(chan, data0, data1, tim);
						meas->incEvents();
					}
				}
			}
		}
// now copy auxiliary counter values into operational counters
		meas->copyCounters();
	}
}

/*!
    \fn Mesydaq2::acqListfile(bool yesno)
 */
void Mesydaq2::acqListfile(bool yesno)
{
	m_acquireListfile = yesno;
    	protocol(tr("Listfile recording %1").arg((yesno ? "on" : "off")), 1);
}

/*!
    \fn Mesydaq2::startedDaq(void)
 */
void Mesydaq2::startedDaq(void)
{
	meas->start(m_timeMsecs);
// maybe some remote control is interested?
   	cInt->completeCar();
	m_daq = RUNNING;
#warning TODO
//	mainWin->update();
//	mainWin->daqStatusLine->setText("RUNNING");
	protocol("daq started", 1);
}


/*!
    \fn Mesydaq2::stoppedDaq(void)
 */
void Mesydaq2::stoppedDaq(void)
{
	meas->stop(m_timeMsecs);
// maybe some remote control is interested?
   	cInt->completeCar();
	
	if(m_acquireListfile)
	{
		writeClosingSignature();
		datfile.close();
	}
	m_daq = IDLE;
#warning TODO
//	mainWin->update();
//	mainWin->daqStatusLine->setText("IDLE");
	protocol("daq stopped", 1);
}

/*!
    \fn Mesydaq2::startDaq(void)
 */
bool Mesydaq2::startDaq(void)
{
	// check for listfile
	if(m_acquireListfile)
	{
		if(meas->remoteStart())
		{
#warning TODO
//			listfilename = mainWin->listfilepath->text();
			m_listfilename.append("run%d.mdat");
			ovwList = true;
#warning TODO 
//			mainWin->listFilename->setText(listfilename);
		}
		else
		{
#warning TODO
#if 0
			if(mainWin->listFilename->text().isEmpty())
			{
				if(listfilename.isEmpty())
				{
					if(!getListfilename())
					{
						ovwList = false;
						return false;
					}
				}
				else
				{
					listfilename = mainWin->listFilename->text();
				}
			}
			else
				listfilename = mainWin->listFilename->text();
#endif
		}			
		// o.k. - now try to use it:
		datfile.setFileName(m_listfilename);
		// now check if existing:
		// listfile already existing? Warning!
   		if(QFile::exists(datfile.fileName()))
		{
   			qDebug("datfile exists");
   			if(!ovwList)
			{
   				qDebug("no overwrite");
#warning TODO
#if 0
				int answer = QMessageBox::warning(
						this, "Listfile Exists -- Overwrite File",
						QString("Overwrite existing listfile?"),
						"&Yes", "&No", QString::null, 1, 1 );
				qDebug("answer: %d", answer);
				if(answer == 1)
				{
					ovwList = false;
					return false;
				}
#endif
   			}
   		}
		// reset overwrite o.k. flag
		ovwList = false;
    	// o.k. - successfully retrieved a listfile
		datfile.open(QIODevice::WriteOnly);
		textStream.setDevice(&datfile);
  		datStream.setDevice(&datfile);
		writeListfileHeader();
		writeHeaderSeparator();
  	}
  	m_daq = STARTED;
#warning TODO 
//	mainWin->daqStatusLine->setText("STARTED");
	protocol("daq start",1);
	return true;
}


/*!
    \fn Mesydaq2::stopDaq(void)
 */
void Mesydaq2::stopDaq(void)
{
	m_daq = STOPPED;
#warning TODO
//	mainWin->daqStatusLine->setText("STOPPED");
	protocol("daq stop", 1);
}


/*!
    \fn Mesydaq2::protocol(QString str, unsigned char level)
 */
void Mesydaq2::protocol(QString str, quint8 level)
{
	if(level <= DEBUGLEVEL)
	{
		QDateTime datetime;
		QString datestring = datetime.currentDateTime().toString("hh:mm:ss.zzz");
		str.prepend(" - ");
		str.prepend(datestring);
		qDebug("[%d] %s", level, str.toStdString().c_str());
	}
}


/*!
    \fn Mesydaq2::initDevices(void)
 */
void Mesydaq2::initDevices(void)
{
	for(quint8 i = 0; i < MCPDS; i++)
		m_mcpd[i] = new MCPD8(i, this);
}


/*!
    \fn Mesydaq2::initTimers(void)
 */
void Mesydaq2::initTimers(void)
{
// display refresh timer
	dispTimer = new QTimer(this);
	connect(dispTimer, SIGNAL(timeout()), this, SLOT(dispTime()));
	dispTimer->start(1000);
	
// central dispatch timer
	theTimer = new QTimer(this);
	connect(theTimer, SIGNAL(timeout()), this, SLOT(centralDispatch()));
	theTimer->start(1);
	dispatchLevel = 0;
	for(quint8 c = 0; c < 10; c++)
		dispatch[c] = 0;

// communication timeout timer
	commTimer = new QTimer(this);
	connect(commTimer, SIGNAL(timeout()), this, SLOT(commTimeout()));
	commTimer->setSingleShot(true);
	commId = 99;
}


/*!
    \fn Mesydaq2::dispTime()
 */
void Mesydaq2::dispTime()
{
	protocol("disp time reached", 3);
	draw();
#warning TODO	mainWin->update();
}


/*!
    \fn Mesydaq2::clearAllHist(void)
 */
void Mesydaq2::clearAllHist(void)
{
	if (m_hist)
		m_hist->clear();
}


/*!
    \fn Mesydaq2::clearChanHist(unsigned long chan)
 */
void Mesydaq2::clearChanHist(unsigned long chan)
{
	if (m_hist)
		m_hist->clear(chan);
}


/*!
    \fn Mesydaq2::writeListfileHeader(void)
 */
void Mesydaq2::writeListfileHeader(void)
{
	QString str;
	str. sprintf("mesytec psd listmode data\n");
	textStream << str;
	str. sprintf("header length: %d lines \n", 2);
	textStream << str;		
}


/*!
    \fn Mesydaq2::writeHeaderSeparator(void)
 */
void Mesydaq2::writeHeaderSeparator(void)
{
	datStream << sep0 << sep5 << sepA << sepF;
}


/*!
    \fn Mesydaq2::writeBlockSeparator(void)
 */
void Mesydaq2::writeBlockSeparator(void)
{
	datStream << sep0 << sepF << sep5 << sepA;
}


/*!
    \fn Mesydaq2::writeClosingSignature(void)
 */
void Mesydaq2::writeClosingSignature(void)
{
	datStream << sepF << sepA << sep5 << sep0;
}


/*!
    \fn Mesydaq2::readListfile(QString readfilename)
 */
void Mesydaq2::readListfile(QString readfilename)
{
	QDataStream datStream;
	QTextStream textStream;
	QFile datfile;
	QString str;
	bool ok = false;
	unsigned short sep1, sep2, sep3, sep4;
    
	datfile.setFileName(readfilename);
	datfile.open(QIODevice::ReadOnly);
	datStream.setDevice(&datfile);
	textStream.setDevice(&datfile);

	unsigned int blocks = 0;
	unsigned int bcount = 0;

	qDebug("readListfile");
	str = textStream.readLine();
	qDebug(str.toStdString().c_str());
	str = textStream.readLine();
	qDebug(str.toStdString().c_str());
	datStream >> sep1 >> sep2 >> sep3 >> sep4;
	if((sep1 == sep0) && (sep2 == sep5) && (sep3 == sepA) && (sep4 == sepF))
	{
		ok = true;
	}
	while(ok)
	{
		datStream >> sep1 >> sep2 >> sep3 >> sep4;
// check for closing signature:
		if((sep1 == sepF) && (sep2 == sepA) && (sep3 == sep5) && (sep4 == sep0))
		{
			qDebug("EOF reached after %d buffers", blocks);
			break;
		}
		DATA_PACKET 	dataBuf;
		dataBuf.bufferLength = sep1;
		dataBuf.bufferType = sep2;
		dataBuf.headerLength = sep3;
		dataBuf.bufferNumber = sep4;
		if(dataBuf.bufferLength > 729)
		{
			qDebug("erroneous length: %d - aborting", dataBuf.bufferLength);
			datStream >> sep1 >> sep2 >> sep3 >> sep4;
			qDebug("Separator: %x %x %x %x", sep1, sep2, sep3, sep4);
			break;
		}
		quint16 *pD = (quint16 *)&dataBuf.bufferLength;
		for(int i = 4; i < dataBuf.bufferLength; i++)
			datStream >> pD[i];
// hand over data buffer for processing
		analyzeBuffer(dataBuf, m_daq, *m_hist);
// increment local counters
		blocks++;
		bcount++;
// check for next separator:
		datStream >> sep1 >> sep2 >> sep3 >> sep4;
//		qDebug("Separator: %x %x %x %x", sep1, sep2, sep3, sep4);
		if((sep1 == sep0) && (sep2 == sepF) && (sep3 == sep5) && (sep4 == sepA))
		{
			ok = true;
		}
		else
		{
			qDebug("File structure error - read aborted after %d buffers", blocks);
			ok = false;
		}
		if(bcount == 1000)
		{
			bcount = 0;
			draw();
//			kapp->processEvents();
		}  
	}	
	datfile.close();
	draw();
}


/*!
    \fn Mesydaq2::draw(void)
 */
void Mesydaq2::draw(void)
{
//	quint8 id = mainWin->getDispId();
#warning TODO
#if 0
	if(mainWin->dispAll->isChecked())
	{
		if(mainWin->dispAllPos->isChecked())
		{
//			hist[id]->copyLine(CHANNELS-2, dispBuf);
//			mainWin->setData(dispBuf, 960, hist[id]->max(CHANNELS-2));
		}
		else
		{
//			hist[id]->copyLine(CHANNELS-1, dispBuf);
//			mainWin->setData(dispBuf, 960, hist[id]->max(CHANNELS-1));
		}
	}
	else
	{
		if(mainWin->specialBox->isChecked())
		{
//			hist[id]->copyLine(CHANNELS, dispBuf);
//			mainWin->setData(dispBuf, 960, hist[id]->max(CHANNELS));
		}
		else
		{
			quint32 chan = mainWin->dispMcpd->value() * 128;
			chan += mainWin->dispMpsd->value() * 16;
			chan += mainWin->dispChan->value();
//			hist[id]->copyLine(chan, dispBuf);
//			mainWin->setData(dispBuf, 960, hist[id]->max(chan));
		}
	}
	mainWin->draw();
#endif
}

bool Mesydaq2::isPulserOn()
{
	for (quint8 i = 0; i < MCPDS; ++i)
		if (m_mcpd[i]->isPulserOn())
			return true;
	return false;
}

bool Mesydaq2::isPulserOn(quint16 id)
{
	return m_mcpd[id]->isPulserOn();
}

bool Mesydaq2::isPulserOn(quint16 id, quint8 addr)
{
	return m_mcpd[id]->isPulserOn(addr);
}

/*!
    \fn Mesydaq2::scanPeriph(unsigned short id)
 */
void Mesydaq2::scanPeriph(quint16 id)
{
	m_mcpd[id]->readId();
}


/*!
    \fn Mesydaq2::dispMpsd(void)
 */
void Mesydaq2::dispMpsd(void)
{
#warning TODO	mainWin->displayMpsdSlot();
}


/*!
    \fn Mesydaq2::initHardware(void)
 */
void Mesydaq2::initHardware(void)
{
// starts the init procedure:
// 1) check all MCPD
// 2) check all MPSD: read ID, read Registers
// setup MCPDs
// setup MPSDs
	
// set init values:
	pstring.sprintf("initializing hardware");
	protocol(pstring, 1);
	 
// scan connected MCPDs
	quint8 p = 0;
	for(quint8 c = 0; c < MCPDS; c++)
	{
		m_mcpd[c]->readId();
		for(quint8 i = 0; i < 8; ++i)
			if(m_mcpd[c]->getMpsdId(i))
				p++;
	}
	
	pstring.sprintf("%d mpsd-8 found", p);
	protocol(pstring, 1);
	
// initialize all connected hardware modules
// try to read standard config file
	protocol("load setup ?");
	if(!loadSetup(false))
	{
		// initialize MPSD-8 by default values.
		for(quint8 c = 0; c < MCPDS; c++)
			for (int i = 0; i < 8; ++i)
				if(m_mcpd[c]->getMpsdId(i))
					initMpsd(c * 8 + i);
	}
	initMcpd(0);
}


/*!
    \fn Mesydaq2::saveSetup(void)
 */
bool Mesydaq2::saveSetup(void)
{
	QString name;
#warning TODO
#if 0
	name = QFileDialog::getSaveFileName(this, "Save Config File...", configPath, "mesydaq config files (*.mcfg);;all files (*.*)"); 
#endif
	if(name.isEmpty())
		return false;
  
	m_configfilename = name;
	int i = m_configfilename.indexOf(".mcfg");
	if(i == -1)
		m_configfilename.append(".mcfg");

	QDateTime dateTime;
	dateTime=QDateTime::currentDateTime();

	QSettings settings(m_configfilename, QSettings::IniFormat);
	settings.setValue("general/date", dateTime);
	settings.setValue("general/Config Path", configPath);
	settings.setValue("general/Histogram Path", histPath);
	settings.setValue("general/Listfile Path", listPath);
	for (int i = 0; i < MCPDS; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			if (m_mcpd[i]->getMpsdId(j))
			{
				QString str;
				str.sprintf("MPSD-8 #%d/", i);
				settings.beginWriteArray(str + "gains");
				for(int k = 0; k < 8; k++)
				{
					settings.setArrayIndex(k);
					settings.setValue("gain", m_mcpd[i]->getGain(j, k));
				}
				settings.endArray();
				settings.setValue(str + "threshold", m_mcpd[i]->getThreshold(j));
			}
		}
	}

#if 0
	QFile f(configfilename);
	if ( f.open(QIODevice::WriteOnly) ) {    // file opened successfully
		QTextStream t( &f );        // use a text stream
		QString s;
		// Title
		t << "mesydaq configuration file   ";
		t << dateTime.toString("dd.MM.yy") << " " << dateTime.toString("hh.mm.ss");
		t << '\r' << '\n';
		t << '\r' << '\n';
		// default paths
		t << "Config Path";
		t << '\r' << '\n';
		t << configPath;
		t << '\r' << '\n';
		t << "Histogram Path";
		t << '\r' << '\n';
		t << histPath;
		t << '\r' << '\n';
		t << "Listfile Path";
		t << '\r' << '\n';
		t << listPath;
		t << '\r' << '\n';
		t << "Setup:";
		t << '\r' << '\n';
/*	    t << "MCPD-8 settings";
    	t << '\r' << '\n';
	    t << '\r' << '\n';
		t << "MCPD-8 #" << 0 << ":";
		t << '\r' << '\n';
*/    	
    	t << "MPSD-8 settings";
	    t << '\r' << '\n';
    	t << '\r' << '\n';
		for(int i=0; i<8*MCPDS; i++){
		if(myMpsd[i]->getMpsdId()){
			t << "MPSD-8 #" << i << ":";
			t << '\r' << '\n';
			t << "gains:";
			t << '\r' << '\n';
			for(int j=0; j<8; j++){
			t << myMpsd[i]->getGainpoti(j,0);
			t << '\r' << '\n';
			}
			t << "threshold:";
			t << '\r' << '\n';
			t << myMpsd[i]->getThreshpoti(0);
			t << '\r' << '\n';
		}
    }
  }
  f.close();
#endif
	return true;
}



/*!
    \fn Mesydaq2::loadSetup(void)
 */
bool Mesydaq2::loadSetup(bool ask)
{
	QString name;

#warning TODO
#if 0
	if(ask)
		name = QFileDialog::getOpenFileName(this, "Load Config File...", configPath, "mesydaq config files (*.mcfg);;all files (*.*)");
	else
		name.sprintf("mesycfg.mcfg");
#endif
  
	if(name.isEmpty())
		return false;
  
	m_configfilename = name;
  
	if(!ask)
		pstring = "Reading standard configfile ";
	else
		pstring = "Reading configfile ";
	pstring += m_configfilename;
	protocol(pstring, 1);

	QSettings settings(m_configfilename, QSettings::IniFormat);

	configPath = settings.value("general/Config Path", "/home").toString();
	histPath = settings.value("general/Histogram Path", "/home").toString();
	listPath = settings.value("general/Listfile Path", "/home").toString();
	for (int i = 0; i < MCPDS; ++i)
	{
		protocol(tr("mcpd #%1").arg(i));
		for (int j = 0; j < 8; ++j)
		{
			QString str;
			str.sprintf("MPSD-8 #%d", j);
			qDebug("number of groups %d", settings.childGroups().size());
			if (settings.childGroups().contains(str))
			{
				settings.beginGroup(str);
				protocol("init " + str);
				int size = settings.beginReadArray(str + "gains");
				for (int l = 0; l < size; ++l)
				{
					settings.setArrayIndex(l);
					quint8 gain = settings.value("gain", 128).toUInt() & 0xff;
					m_mcpd[i]->setGain(j, l, gain);
				}
				settings.endArray();
				quint8 thresh = settings.value(str + "threshold", 10).toUInt() & 0xff;
				m_mcpd[i]->setThreshold(j, thresh);
				settings.endGroup();
			}
		}
	}
#if 0
	QFile f(configfilename);

	QString s, str;


	if (f.open(QIODevice::ReadOnly)) 
	{    // file opened successfully
		QTextStream t( &f );        // use a text stream

// now read from stream
    
// Headline + empty line
		s = t.readLine();
		s = t.readLine();

//  Headline + configPath
		s = t.readLine();
		configPath = t.readLine();

// Headline + histPath
		s = t.readLine();
		histPath = t.readLine();

// Headline + listPath
		s = t.readLine();
		listPath = t.readLine();
	
// three headlines
		s = t.readLine();
		s = t.readLine();
		s = t.readLine();

// now the MPSD-8, if any...
		s = t.readLine();
		char charpos = s.indexOf('#', 0);

// loop through existing entries
		while(charpos > 6)
		{
			// module address: next to # sign
			QString as = s.right(s.length()-charpos-1);
			as = as.left(1);
			unsigned char modaddr = as.toUShort();
			// one title row
			s = t.readLine();
			// 9 gain poti values
			if(myMpsd[modaddr]->getMpsdId() > 0)
			{
				for(unsigned char i = 0; i < 8; i++)
				{
					s = t.readLine();
					myMpsd[modaddr]->setGain(i, (unsigned char)s.toUShort(), 1);
				}
			}
			// one title row
			s = t.readLine();
			// threshold poti
			s = t.readLine();
			if(myMpsd[modaddr]->getMpsdId() > 0)
			{
				myMpsd[modaddr]->setThreshpoti((unsigned char)s.toUShort(), 1);
			}
			if(myMpsd[modaddr]->getMpsdId() > 0)
			{
				initMpsd(modaddr);
			}	
			// further entries?
			s = t.readLine();
			qDebug(s.toStdString().c_str());
			charpos = s.indexOf('#', 0);
		}
	
		f.close();
		mainWin->dispFiledata();
		return true;
	}
  	else
	{
		pstring.sprintf("ERROR: opening configfile '");
		pstring.append(name);
		pstring.append("' failed");
		protocol(pstring, 1);
		return false;
	}
#endif
	return true;
}


/*!
    \fn Mesydaq2::commTimeout()
 */
void Mesydaq2::commTimeout()
{
	m_mcpd[commId]->communicate(false);
	qDebug("timeout while waiting for cmd answer from ID: %d", commId);
}


/*!
    \fn Mesydaq2::centralDispatch()
 */
void Mesydaq2::centralDispatch()
{
	if(cInt->caressTaskPending() && (!cInt->asyncTaskPending()))
    		cInt->caressTask();
    	
	dispatch[0]++;
	if(dispatch[0] == 8)
	{
		dispatch[0] = 0;
		meas->calcRates();
	}
	
	dispatch[1]++;
	if(dispatch[1] == 50)
	{
//		checkMcpd(0);
	}
	if(dispatch[1] == 60)
	{
		//if(meas->isOk() == 0)
			//meas->setOnline(false);
		dispatch[1] = 0;
	}
	
	dispatchLevel ++;
	if(dispatchLevel == 100)
	{
    		dispatchLevel = 0;
    	}
}


/*!
    \fn Mesydaq2::initMpsd(unsigned char id)
 */
void Mesydaq2::initMpsd(unsigned char id)
{
	quint8 	start = 8,
		stop = 9;
	
#warning TODO
#if 0
	// gains:
	if(!myMpsd[id]->comGain())
	{
		// iterate through all channels
		start = 0;
		stop = 8;
	}
	for (quint8 c = start; c < stop; ++c)
		m_mcpd[id]->setGain(c, myMpsd[id]->getGainpoti(c, 1));
#endif
	
// threshold:
	m_mcpd[0]->setThreshold(id, m_mcpd[0]->getThreshold(id));

// pulser
	m_mcpd[0]->setPulser(id, 0, 2, 50, false);

// mode
	m_mcpd[0]->setMode(id, false);
	
// now set tx capabilities, if id == 105
	if(m_mcpd[0]->getMpsdId(id) == 105)
	{
		// write register 1
		m_mcpd[0]->writePeriReg(id, 1, 4);
	}
}


/*!
    \fn Mesydaq2::initMcpd(unsigned char id)
 */
void Mesydaq2::initMcpd(quint8 id)
{
	m_mcpd[id]->init();
}


/*!
    \fn Mesydaq2::allPulserOff()
 */
void Mesydaq2::allPulserOff()
{
// send pulser off to all connected MPSD
	for(quint32 i = 0; i < MCPDS; i++)
		for(quint32 j = 0; j < 8; j++)
			if(m_mcpd[i]->getMpsdId(j))
				m_mcpd[i]->setPulser(j, 0, 2, 40, 0);
}

/*!
    \fn Mesydaq2::setTimingwidth(unsigned char width)
 */
void Mesydaq2::setTimingwidth(quint8 width)
{
	if(width < 48)
		timingwidth = width;
	else
    		timingwidth = 48;
	if (m_hist)
   		m_hist->setWidth(timingwidth); 
}


/*!
    \fn Mesydaq2::readPeriReg(unsigned short id, unsigned short mod, unsigned short reg)
 */
quint16 Mesydaq2::readPeriReg(quint16 id, quint16 mod, quint16 reg)
{
	return m_mcpd[id]->readPeriReg(mod, reg);
}


/*!
    \fn Mesydaq2::writePeriReg(unsigned short id, unsigned short mod, unsigned short reg, unsigned short val)
 */
void Mesydaq2::writePeriReg(quint16 id, quint16 mod, quint16 reg, quint16 val)
{
	m_mcpd[id]->writePeriReg(mod, reg, val);
}


/*!
    \fn Mesydaq2::checkListfilename(void)
 */
bool Mesydaq2::checkListfilename(void)
{
// name already defined?
// no - try ot get one...
#warning TODO
	datfile.setFileName(m_listfilename);
	return m_listfilename.isEmpty();
}

/*!
    \fn Mesydaq2::writeHistograms()
 */
void Mesydaq2::writeHistograms()
{
	if(m_histfilename.isEmpty())
		return;

	QFile f;
	f.setFileName(m_histfilename);
	if (f.open(QIODevice::WriteOnly)) 
	{    // file opened successfully
		QTextStream t( &f );        // use a text stream
		// Title
		t << "mesydaq Histogram File    " << QDateTime::currentDateTime().toString("dd.MM.yy  hh:mm:ss") << '\r' << '\n';
		t.flush();
		if (m_hist)
    			m_hist->writeHistogram(&f);
		f.close();
	}
}

// command shortcuts for simple operations:
/*!
    \fn Mesydaq2::start(void)
 */
void Mesydaq2::start(void)
{
   	protocol("remote start", 1);
#warning TODO
	m_mcpd[0]->start();
//	mainWin->updateCaress();
}

/*!
    \fn Mesydaq2::stop(void)
 */
void Mesydaq2::stop(void)
{
#warning TODO
	m_mcpd[0]->stop();
//	mainWin->updateCaress();
   	protocol("remote stop", 1);
}

/*!
    \fn Mesydaq2::continue(void)
 */
void Mesydaq2::cont(void)
{
    /// @todo implement me
}


/*!
    \fn Mesydaq2::reset(void)
 */
void Mesydaq2::reset(void)
{
    /// @todo implement me
}


/*!
    \fn Mesydaq2::checkMcpd(unsigned char device)
 */
bool Mesydaq2::checkMcpd(quint8 /* device */)
{
	for(quint8 c = 0; c < MCPDS; c++)
		scanPeriph(c);    	
	return true;
}

/*!
    \fn dataCruncher::setLimit(unsigned long lim)
 */
void Mesydaq2::setLimit(quint8 cNum, ulong lim)
{
	m_countLimit[cNum] = lim;
}

/*!
    \fn Mesydaq2::setCountlimit(unsigned char cNum, unsigned long lim)
 */
void Mesydaq2::setCountlimit(quint8 cNum, ulong lim)
{
	switch(cNum)
	{
		case M1CT:
    			setLimit(MON1ID, lim);
    			break;
		case M2CT:
    			setLimit(MON2ID, lim);
    			break;    	
		case EVCT:
    			setLimit(EVID, lim);
    			break;
    		default:
    		break;
	}
}


/*!
    \fn Mesydaq2::copyData(unsigned int line, unsigned long * data)
 */
void Mesydaq2::copyData(quint32 line, ulong *data)
{
	if (m_hist)
		m_hist->copyLine(line, data);
}

void Mesydaq2::setProtocol(const quint16 id, const QString &mcpdIP, const QString dataIP, const qint16 dataPort, const QString cmdIP, const qint16 cmdPort)
{
	m_mcpd[id]->setProtocol(mcpdIP, dataIP, dataPort, cmdIP, cmdPort);
}	

void Mesydaq2::setMode(const quint16 id, quint16 addr, bool mode)
{
	m_mcpd[id]->setMode(addr, mode);
}

void Mesydaq2::setPulser(const quint16 id, quint16 addr, quint8 channel, quint8 position, quint8 amp, bool onoff)
{
	m_mcpd[id]->setPulser(addr, channel, position, amp, onoff);
}

void Mesydaq2::setCounterCell(quint16 id, quint16 source, quint16 trigger, quint16 compare)
{
	m_mcpd[id]->setCounterCell(source, trigger, compare);
}

void Mesydaq2::setParamSource(quint16 id, quint16 param, quint16 source)
{
	m_mcpd[id]->setParamSource(param, source);
}

void Mesydaq2::setAuxTimer(quint16 id, quint16 tim, quint16 val)
{
	m_mcpd[id]->setAuxTimer(tim, val);
}

void Mesydaq2::setMasterClock(quint16 id, quint64 val)
{
	m_mcpd[id]->setMasterClock(val);
}

void Mesydaq2::setTimingSetup(quint16 id, bool master, bool sync)
{
	m_mcpd[id]->setTimingSetup(master, sync);
}

void Mesydaq2::setId(quint16 id, quint8 mcpdid)
{
	m_mcpd[id]->setId(mcpdid);
}

void Mesydaq2::setGain(quint16 id, quint16 addr, quint8 channel, quint8 gain)
{
	m_mcpd[id]->setGain(addr, channel, gain);
}

void Mesydaq2::setGain(quint16 id, quint16 addr, quint8 channel, float gain)
{
	m_mcpd[id]->setGain(addr, channel, gain);
}

void Mesydaq2::setThreshold(quint16 id, quint16 addr, quint8 thresh)
{
	m_mcpd[id]->setThreshold(addr, thresh);
}

quint8 Mesydaq2::getMpsdId(quint16 id, quint8 addr)
{
	return m_mcpd[id]->getMpsdId(addr);
}

quint8 Mesydaq2::getGain(quint16 id, quint16 addr, quint8 chan)
{
	return m_mcpd[id]->getGain(addr, chan);
}

quint8 Mesydaq2::getThreshold(quint16 id, quint16 addr)
{
	return m_mcpd[id]->getThreshold(addr);
}

void Mesydaq2::setHistfilename(QString name) 
{
	m_histfilename = name;
	if(m_histfilename.indexOf(".mtxt") == -1)
		m_histfilename.append(".mtxt");
}
     
/*!
    \fn Mesydaq2::analyzeBuffer(DATA_PACKET &pd, quint8 daq)
 */
void Mesydaq2::analyzeBuffer(DATA_PACKET &pd)
{
	if(m_daq == RUNNING)
	{
		quint16 time, counter = 0;
		ulong 	data;
		quint32 i, j;
		quint16 neutrons = 0;
		quint16 triggers = 0;
		quint64 htim = pd.time[0] + quint64(pd.time[1]) << 16 + quint64(pd.time[2]) << 32,
			tim;
		quint16 mod = pd.deviceId;	
		quint16 diff = pd.bufferNumber - m_lastBufnum;
		if(diff > 1)
			protocol(tr("Lost %1 Buffers: current: %2, last: %3").arg(diff).arg(pd.bufferNumber).arg(m_lastBufnum));
		m_lastBufnum = pd.bufferNumber;
		if(m_acquireListfile)
		{
#if 0
			quint16 *pD = (quint16 *) &pd.bufferLength;
			unsigned int i;
			for(i = 0; i < pd.bufferLength; i++)
			{
				datStream << pD[i];
//				qDebug("%x", pD[i]);
			}
			writeBlockSeparator();
//			qDebug("------------------");
#endif
		}
		protocol(tr("dataRxd : %1").arg(++m_dataRxd));
		protocol(tr("buffer : length : %1").arg(pd.bufferLength));
		if(pd.bufferType < 0x0002) 
		{
			quint8 id = pd.deviceId;
// extract parameter values:
			for(i = 0; i < 4; i++)
			{
				quint64 var = 0;
				for(j = 0; j < 3; j++)
				{
					var <<= 16;
					var |= pd.param[i][2-j];
				}
				m_mcpd[mod]->setParameter((unsigned char)i, var);
			}		
// 			data length = (buffer length - header length) / (3 words per event) - 4 parameters.
 			quint32 datalen = (pd.bufferLength - pd.headerLength) / 3;
			protocol(tr("%1 data").arg(datalen));
			quint8 status = pd.deviceStatus;
			protocol(tr("running %1").arg(status & 1)); 
			QChar c('0');
			for(i = 0; i < datalen; ++i, counter += 3)
			{
				tim = pd.data[counter + 1] & 0x7;
				tim <<= 16;
				tim += pd.data[counter];
//				protocol(tr("time : %1 (%2 %3)").arg(tim).arg(pd.data[counter + 1] & 0x7).arg(pd.data[counter])); //, 8, 16, c));
				tim += htim;
// id stands for the trigId and modId depending on the package type
				quint8 id = (pd.data[counter + 2] >> 12) & 0x7;
//				ulong delta = tim - m_lastTime;
//				m_lastTime = tim;
// not neutron event (counter, chopper, ...)
//				protocol(tr("%1 %2 %3").arg(pd.data[counter + 2],4,16,c).arg(pd.data[counter + 1],4,16,c).arg(pd.data[counter], 4, 16,c));
				if((pd.data[counter + 2] & 0x8000))
				{
					triggers++;
					quint8 dataId = (pd.data[counter + 2] >> 8) & 0x0F;
					data = (pd.data[counter + 2] & 0xFF) << 13 + (pd.data[counter + 1] >> 3) & 0x7FFF;
					time = (quint16)tim;
#warning TODO 				emit incEvents(tim, id, dataId, data);
//					protocol(tr("Trigger : %1 id %2 data %3").arg(triggers).arg(id).arg(dataId));
				}
// neutron event:
				else
				{
					neutrons++;
					quint8 slotId = (pd.data[counter + 2] >> 7) & 0x1F;
					quint8 chan = (id << 3) + slotId;
					quint16 amp(0), 
						pos(0);
					if (m_mcpd[mod]->getMpsdId(id) == 103)
					{
						if (m_mcpd[mod]->getMode(id))
							amp = (pd.data[counter+1] >> 3) & 0x3FF;
						else
							pos = (pd.data[counter+1] >> 3) & 0x3FF;
					}
					else
					{
						amp = (pd.data[counter+2] & 0x7F) << 3 + (pd.data[counter+1] >> 13) & 0x7;
						pos = (pd.data[counter+1] >> 3) & 0x3FF;
					}
					if (m_hist)
					{
						m_hist->incVal(chan, pos, tim);
#warning TODO 					m_hist->incVal(chan, pos, tim);
					}
//					protocol(tr("Neutron : %1 id %2 slot %3 pos 0x%4 amp 0x%5").arg(neutrons).arg(id).arg(slotId).arg(pos, 4, 16, c).arg(amp, 4, 16, c));
				}
			}
		}
	}
}

