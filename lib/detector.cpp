/****************************************************************************
 *   Copyright (C) 2008-2015 by Gregor Montermann <g.montermann@mesytec.com>*
 *   Copyright (C) 2009-2020 by Jens Krüger <jens.krueger@frm2.tum.de>      *
 *   Copyright (C) 2011-2020 by Lutz Rossa <rossa@helmholtz-berlin.de>      *
 *                                                                          *
 *   This program is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 2 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program; if not, write to the                          *
 *   Free Software Foundation, Inc.,                                        *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.              *
 ****************************************************************************/
#include <algorithm>
#include <QThread>
#include <QStringList>
#include "mdefines.h"
#include "detector.h"
#include "datarepeater.h"
#include "streamwriter.h"
#include "mcpd8.h"
#include "qmlogging.h"
#include "stdafx.h"

/*!
    \fn Detector::Detector()

    constructor
*/
Detector::Detector()
	: m_pThread(NULL)
	, m_Running(DET_IDLE)
	, m_bRunAck(false)
	, m_acquireListfile(false)
	, m_autoSaveHistogram(false)
	, m_listfilename("")
	, m_pDatStream(NULL)
	, m_timingwidth(1)
	, m_bInsertHeaderLength(true)
	, m_starttime_msec(0)
	, m_runId(0)
	, m_bAutoIncRunId(true)
	, m_bWriteProtect(false)
	, m_setup(Mpsd)
	, m_detector(0)
	, m_histogramPos(QPoint(0, 0))
{
	MSG_NOTICE << tr("running on Qt %1").arg(qVersion());
	qRegisterMetaType<QSharedDataPointer<SD_PACKET> >("QSharedDataPointer<SD_PACKET>");
	qRegisterMetaType<QSharedDataPointer<EVENT_BUFFER> >("QSharedDataPointer<EVENT_BUFFER>");
	m_pThread = new QThread;
	moveToThread(m_pThread);
	connect(m_pThread, SIGNAL(finished()), this, SLOT(threadExit()), Qt::DirectConnection);
	m_pThread->start(QThread::TimeCriticalPriority);
	m_pDatSender = new DataRepeater;
	setStreamWriter(new StreamWriter);
}

//! destructor
Detector::~Detector()
{
	m_pThread->quit();
	m_pThread->wait();
	delete m_pThread;
	delete m_pDatStream;
	delete m_pDatSender;
}

void Detector::threadExit()
{
	foreach (MCPD8* value, m_mcpd)
		delete value;
	m_mcpd.clear();
}

//! \return number of missed data packages for the whole setup
quint64 Detector::missedData(void)
{
	quint64 dataMissed = 0;
	for (QHash<int, MCPD8 *>::iterator it = m_mcpd.begin(); it != m_mcpd.end(); ++it)
		dataMissed += it.value()->missedData();
	return dataMissed;
}

//! \return number of received data packages for the whole setup
quint64 Detector::receivedData(void)
{
	quint64 dataRxd = 0;
	for (QHash<int, MCPD8 *>::iterator it = m_mcpd.begin(); it != m_mcpd.end(); ++it)
		dataRxd += it.value()->receivedData();
	return dataRxd;
}

//! \return number of received cmd answer packages for the whole setup
quint64 Detector::receivedCmds(void)
{
	quint64 cmdRxd = 0;
	for (QHash<int, MCPD8 *>::iterator it = m_mcpd.begin(); it != m_mcpd.end(); ++it)
		cmdRxd += it.value()->receivedCmds();
	return cmdRxd;
}

//! \return number of sent cmd packages for the whole setup
quint64 Detector::sentCmds(void)
{
	quint64 cmdTxd = 0;
	for (QHash<int, MCPD8 *>::iterator it = m_mcpd.begin(); it != m_mcpd.end(); ++it)
		cmdTxd += it.value()->sentCmds();
	return cmdTxd;
}

/*!
    \fn Detector::time(void)

    time of the first master MCPD

    \return the time of the MCPD
 */
quint64 Detector::time(void)
{
	quint64 ret(0);
	if (!m_mcpd.empty())
	{
		for (QHash<int, MCPD8 *>::iterator it = m_mcpd.begin(); it != m_mcpd.end(); ++it)
			if (it.value()->isMaster())
			{
				ret = it.value()->time();
				break;
			}
	}
	return ret;
}

/*!
    \fn Detector::writeRegister(quint16 id, quint16 reg, quint16 val)

    writes a value to a register in a selected MCPD, if the MPCD with
    the id does not exist, the command will be ignored

    \param id number of the MCPD
    \param reg number of the register
    \param val new value
    \see readRegister
 */
void Detector::writeRegister(quint16 id, quint16 reg, quint16 val)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->writeRegister(reg, val);
}

/*!
    \fn Detector::isMaster(quint16 mod)
    \param mod number of the MCPD
    \return is the MCPD master or not
*/
bool Detector::isMaster(quint16 mod)
{
	if (!m_mcpd.contains(mod))
		return false;
	return m_mcpd[mod]->isMaster();
}

/*!
    \fn Detector::isExtsynced(quint16 mod)
    \param mod number of the MCPD
    \return is the MCPD external sync allowed or not
*/
bool Detector::isExtsynced(quint16 mod)
{
	if (!m_mcpd.contains(mod))
		return false;
	return m_mcpd[mod]->isExtsynced();
}

/*!
    \fn Detector::isTerminated(quint16 mod)
    \param mod number of the MCPD
    \return is the MCPD terminated or not
*/
bool Detector::isTerminated(quint16 mod)
{
	if (!m_mcpd.contains(mod))
		return false;
	if (isMaster(mod))
		return true;
	else
		return m_mcpd[mod]->isTerminated();
}

/*!
    \fn float Detector::getFirmware(quint16 id)

    gets the firmware version of a MCPD

    \param id number of the MCPD
    \return the firmware version as float value, if MCPD does not exist 0
*/
float Detector::getFirmware(quint16 id)
{
	if (!m_mcpd.contains(id))
		return 0;
	return m_mcpd[id]->version();
}

/*!
    \fn float Detector::getFpga(quint16 id)

    gets the FPGA version of a MCPD

    \param id number of the MCPD
    \return the FPGA version as float value, if MCPD does not exist 0
*/
float Detector::getFpga(quint16 id)
{
	if (!m_mcpd.contains(id))
		return 0;
	return m_mcpd[id]->fpgaVersion();
}
/*!
    \fn Detector::acqListfile(bool yesno)

    enables the writing of list mode file

    \param yesno enable/disable if true/false
 */
void Detector::acqListfile(bool yesno)
{
	m_acquireListfile = yesno;
	MSG_NOTICE << tr("Listfile recording %1").arg(yesno ? "on" : "off");
}

/*!
    \fn Detector::autoSaveHistogram(bool yesno)

    enables the writing of list mode file

    \param yesno enable/disable if true/false
 */
void Detector::autoSaveHistogram(bool yesno)
{
	m_autoSaveHistogram = yesno;
	MSG_NOTICE << tr("automatic histogram saving after stop data acquisition %1").arg(yesno ? "on" : "off");
}

/*!
    \fn Detector::startedDaq(void)

    callback after the start was succeeded

    \see start
    \see stop
    \see stoppedDaq
 */
void Detector::startedDaq(void)
{
	if (m_acquireListfile && !m_bRunAck)
	{
		if (!m_pDatStream)
			setStreamWriter(new StreamWriter);
		m_pDatStream->setFile(m_listfilename);
	}
	m_bRunAck = true;
	MSG_DEBUG << tr("daq started");
}

/*!
    \fn Detector::stoppedDaq(void)

    callback after the aquisition was stopped

    \see start
    \see stop
    \see startedDaq
 */
void Detector::stoppedDaq(void)
{
	m_pDatStream->close();
	m_bRunAck = false;
	MSG_DEBUG << tr("daq stopped");
}

/*!
    \fn Detector::addMCPD(quint8 byId, QString szMcpdIp, quint16 wPort, QString szHostIp)

    'adds' another MCPD to this class

    \param byId      the ID of the MCPD
    \param szMcpdIp  the IP address of the MCPD
    \param wPort     the port number to send cmd (and data if no explicit data port is given)
    \param szDatatIp the IP address of the data sending MCPD (optional)
    \param wDataPort the port number of the data sending port of the MCPD (optional)
    \param szHostIp  IP address to get data and cmd answers back
*/
void Detector::addMCPD(quint8 byId, QString szMcpdIp, quint16 wPort, QString szDataIp, quint16 wDataPort, QString szHostIp)
{
	if (m_mcpd.contains(byId))
		return;
	MCPD8 *tmp = new MCPD8(byId, szMcpdIp, wPort, szDataIp, wDataPort, szHostIp);
	if (tmp)
	{
		connect(tmp, SIGNAL(analyzeDataBuffer(QSharedDataPointer<SD_PACKET>)), this, SLOT(dumpListmode(QSharedDataPointer<SD_PACKET>)));
		connect(tmp, SIGNAL(startedDaq()), this, SLOT(startedDaq()));
		connect(tmp, SIGNAL(stoppedDaq()), this, SLOT(stoppedDaq()));
		connect(tmp, SIGNAL(headerTimeChanged(quint64)), this, SLOT(setHeadertime(quint64)));
		connect(tmp, SIGNAL(lostSync(quint16, bool)), this, SLOT(lostSync(quint16, bool)));
		connect(tmp, SIGNAL(newDataBuffer(QSharedDataPointer<EVENT_BUFFER>)), this, SLOT(rearrangeEvents(QSharedDataPointer<EVENT_BUFFER>)));

		m_mcpd.insert(byId, tmp);
	}
}

/*!
    \fn Detector::writeListfileHeader(void)

    write the header of a listfile

    \see writeHeaderSeparator
    \see writeBlockSeparator
    \see writeClosingSignature
 */
void Detector::writeListfileHeader(void)
{
	if (m_datHeader.isEmpty())
	{
		m_pDatStream->writeRawData(QString("mesytec psd listmode data\n"));
		m_pDatStream->writeRawData(QString("header length: %1 lines \n").arg(2));

		if ((m_Running == DET_RUNNING) && !m_bRunAck)
			m_pDatSender->WriteData(QByteArray("DATA\n"));
	}
	else
	{
		QByteArray header1("");
		QByteArray lengthinfo;

		if (m_bInsertHeaderLength)
		{
			QByteArray header2(m_datHeader);
			if (!header2.endsWith('\n'))
				header2.append('\n');
			header1.append("DATA = ");

			int iLen = header1.count() + header2.count();
			do
			{
				++iLen;
				lengthinfo = QString("%1\n").arg(iLen).toLatin1();
			} while ((header1.count() + header2.count() + lengthinfo.count()) > iLen);
			header1.append(lengthinfo);
			header1.append(header2);
		}
		else
			header1 = m_datHeader;
		if (m_pDatStream->isOpen())
			m_pDatStream->writeRawData(header1);
		if ((m_Running == DET_RUNNING) && !m_bRunAck)
			m_pDatSender->WriteData(header1);
	}
}


/*!
    \fn Detector::writeHeaderSeparator(void)

    write a header separator into a list mode file

    \see writeListfileHeader
    \see writeBlockSeparator
    \see writeClosingSignature
 */
void Detector::writeHeaderSeparator(void)
{
	//const unsigned short awBuffer[] = {sep0, sep5, sepA, sepF};
	if (m_pDatStream->isOpen())
		*m_pDatStream << sep0 << sep5 << sepA << sepF;
	//if ((m_Running == DET_RUNNING) && !m_bRunAck)
	//	m_pDatSender->WriteData(&awBuffer[0], sizeof(awBuffer));
}


/*!
    \fn Detector::writeBlockSeparator(void)

    write a block separator into a list mode file

    \see writeListfileHeader
    \see writeHeaderSeparator
    \see writeClosingSignature
 */
void Detector::writeBlockSeparator(void)
{
	const unsigned short awBuffer[] = {sep0, sepF, sep5, sepA};
	if (m_pDatStream->isOpen())
		*m_pDatStream << sep0 << sepF << sep5 << sepA;
	m_pDatSender->WriteData(&awBuffer[0], sizeof(awBuffer));
}


/*!
    \fn Detector::writeClosingSignature(void)

    \see writeListfileHeader
    \see writeHeaderSeparator
    \see writeBlockSeparator
 */
void Detector::writeClosingSignature(void)
{
	const unsigned short awBuffer[] = {sepF, sepA, sep5, sep0};
	if (m_pDatStream->isOpen())
		*m_pDatStream << sepF << sepA << sep5 << sep0;
	if ((m_Running == DET_IDLE) && m_bRunAck)
		m_pDatSender->WriteData(&awBuffer[0], sizeof(awBuffer), true);
}

/*!
    \fn Detector::writeProtectFile(QIODevice* pIODevice)
 */
void Detector::writeProtectFile(QIODevice* pIODevice)
{
	if (m_bWriteProtect)
	{
		QFile* pFile(dynamic_cast<QFile*>(pIODevice));
		if (pFile)
			pFile->setPermissions(pFile->permissions() & (~(QFile::WriteOwner|QFile::WriteUser|QFile::WriteGroup|QFile::WriteOther)));
	}
}

/*!
    \fn QSize Detector::size(void)

    \return the 'size' of the detector in 'channels'
 */
QSize Detector::size(void)
{
	return QSize(width(), height());
}

/*!
    \fn quint16 Detector::height()

    the maximum number of bins over all modules

    \return number of bins
 */
quint16 Detector::height(void)
{
	quint16 bins(0);
	for (QHash<int, MCPD8 *>::iterator it = m_mcpd.begin(); it != m_mcpd.end(); ++it)
		if (it.value()->bins() > bins)
			bins = it.value()->bins();
	return bins ? bins : 960;
}

/*!
    \fn quint16 Detector::width()

    the maximum number of channel over all modules

    \return maximum number of channel
 */
quint16 Detector::width(void)
{
	m_tubeMapping.clear();
	for (QHash<int, MCPD8 *>::iterator it = m_mcpd.begin(); it != m_mcpd.end(); ++it)
	{
		QMap<quint16, quint16> tmpList = it.value()->getTubeMapping();
		quint16 lastTube = !m_tubeMapping.empty() ? m_tubeMapping.values().last() + 1 : 0;
		quint16 lastKey = it.key() * (*it)->maxwidth();
		for (QMap<quint16, quint16>::iterator jt = tmpList.begin(); jt != tmpList.end(); ++jt)
			m_tubeMapping.insert(lastKey + jt.key(), lastTube + jt.value());
	}
	quint16 w(m_tubeMapping.size() - m_tubeMapping.count(0xFFFF));
	MSG_NOTICE << tr("Found %1 tubes to histogram (%2 - %3)").arg(w).arg(m_tubeMapping.size()).arg(m_tubeMapping.count(0xFFFF));
	MSG_INFO << m_tubeMapping;
	return w;
}

/*!
    \fn Detector::isPulserOn()

    checks all MCPD's for a running pulser

    \return true if a running pulser found false otherwise
 */
bool Detector::isPulserOn()
{
	for (QHash<int, MCPD8 *>::iterator it = m_mcpd.begin(); it != m_mcpd.end(); ++it)
		if (it.value()->isPulserOn())
			return true;
	return false;
}

/*!
    \fn Detector::isPulserOn(quint16 id)

    checks a MCPD with number id for a running pulser

    \param id number of the MCPD
    \return true if a running pulser found false otherwise
 */
bool Detector::isPulserOn(quint16 id)
{
	if (!m_mcpd.contains(id))
		return false;
	return m_mcpd[id]->isPulserOn();
}

/*!
    \fn Detector::isPulserOn(quint16 id, quint8 addr)

    checks a MPSD with number addr behind a MCPD with number id for a running pulser

    \param id number of the MCPD
    \param addr number of the MPSD behind the MCPD
    \return true if a running pulser found false otherwise
 */
bool Detector::isPulserOn(quint16 id, quint8 addr)
{
	if (!m_mcpd.contains(id))
		return false;
	return m_mcpd[id]->isPulserOn(addr);
}

/*!
    \fn Detector::scanPeriph(quint16 id)

    checks which MPSD's at the moment are connected to the MCPD

    \param id number of the MCPD
 */
void Detector::scanPeriph(quint16 id)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->scanPeriph();
}

/*!
    \fn Detector::saveSetup(QSettings &settings)

    Stores the setup in a file. This function stores INI files in format of
    "MesyDAQ" instead of "QMesyDAQ" using QSettings class (which is not easy
    human readable).

    Note: MesyDAQ INI file format is not used correctly, because the section
	  names are not unique: imagine you don't have a single MCPD-8 + MPSD-8 ...

    \param settings
    \return true if successfully saved otherwise false
 */
bool Detector::saveSetup(QSettings &settings)
{
	settings.beginGroup("MESYDAQ");
	settings.setValue("listmode", m_acquireListfile ? "true" : "false");
	settings.setValue("autosavehistogram", m_autoSaveHistogram ? "true" : "false");
	settings.setValue("repeatersource", m_pDatSender->GetSource().toString());
	settings.setValue("repeatertarget", m_pDatSender->GetTarget().toString());
	settings.setValue("repeaterport", m_pDatSender->GetPort());
	settings.setValue("repeaterenable", m_pDatSender->GetEnabled() ? "true" : "false");
	settings.endGroup();

	int i = 0;
	foreach(MCPD8 *value, m_mcpd)
	{
		QString mcpdName = QString("MCPD-%1").arg(i);

		settings.beginGroup(mcpdName);
		QString ip, dataip, cmdip;
		quint16 dataport, cmdport;
		value->getProtocol(ip, cmdip, cmdport, dataip, dataport);

		settings.setValue("id", i);
		settings.setValue("ipAddress", ip);
		settings.setValue("port", cmdport);

		if (!dataip.isEmpty() && dataip != "0.0.0.0" && dataip != ip)
		{
			settings.setValue("dataip", dataip);
			if (dataport != cmdport)
				settings.setValue("dataport", dataport);
		}
		settings.setValue("master", value->isMaster() ? "true" : "false");
		settings.setValue("detector", m_detector);
		if (value->type() != TYPE_MWPCHR) // no terminate/sync, timers, parameter sources, and counter cells
		{
			settings.setValue("terminate", value->isTerminated() ? "true" : "false");
			settings.setValue("extsync", value->isExtsynced() ? "true" : "false");

			for (int j = 0; j < 4; ++j)
			{
				settings.setValue(QString("auxtimer%1").arg(j), value->getAuxTimer(j));
				settings.setValue(QString("paramsource%1").arg(j), value->getParamSource(j));
			}
			for (int j = 0; j < 8; ++j)
			{
				quint16 cells[2];
				value->getCounterCell(j, &cells[0]);
				settings.setValue(QString("counterCell%1").arg(j), QString("%1 %2").arg(cells[0]).arg(cells[1]));
			}
		}
		settings.endGroup();

		// Module part
		for (int j = 0; j < 8; ++j)
		{
			QString moduleName;
			int moduleID = value->getModuleId(j);
			switch (moduleID)
			{
				case TYPE_NOMODULE:
					break;
				case TYPE_MWPCHR:
					if (j == 0)
					{
						settings.beginGroup(QString("MDLL-%1").arg(i));
						settings.setValue("id", i);
						settings.setValue("active", value->active());
						settings.setValue("histogram", value->histogram());
						settings.endGroup();
					}
					break;
				case TYPE_MDLL:
					if (j == 0)
					{
						settings.beginGroup(QString("MDLL-%1").arg(i));
						settings.setValue("id", i);
						settings.setValue("active", value->active());
						settings.setValue("histogram", value->histogram());

						settings.setValue("threshX", value->getMdllThreshold(0));
						settings.setValue("threshY", value->getMdllThreshold(1));
						settings.setValue("threshA", value->getMdllThreshold(2));

						settings.setValue("shiftX", value->getMdllSpectrum(0));
						settings.setValue("shiftY", value->getMdllSpectrum(1));
						settings.setValue("scaleX", value->getMdllSpectrum(2));
						settings.setValue("scaleY", value->getMdllSpectrum(3));

						settings.setValue("tWinXLo", value->getMdllTimingWindow(0));
						settings.setValue("tWinXHi", value->getMdllTimingWindow(1));
						settings.setValue("tWinYLo", value->getMdllTimingWindow(2));
						settings.setValue("tWinYHi", value->getMdllTimingWindow(3));

						settings.setValue("eWinLo", value->getMdllEnergyWindow(0));
						settings.setValue("eWinHi", value->getMdllEnergyWindow(1));

						settings.setValue("dataSet", value->getMdllDataset());

						settings.endGroup();
					}
					break;
				default:
					moduleName = QString("MODULE-%1").arg(8 * i + j);

					settings.beginGroup(moduleName);
					settings.setValue("id", i * 8 + j);
					settings.setValue("ampmode", value->getMode(j));
					settings.setValue("threshold", value->getThreshold(j));
					int modules = value->getChannels(j);
					for (int k = 0; k < modules; ++k)
					{
						settings.setValue(QString("gain%1").arg(k), value->getGainPoti(j, k));
						MSG_NOTICE << tr("%1 %2 ").arg(j).arg(k) << value->active(j, k);
					}
					for (int k = 0; k < modules; ++k)
					{
						settings.setValue(QString("active%1").arg(k), value->active(j, k) ? "true" : "false");
						settings.setValue(QString("histogram%1").arg(k), value->histogram(j, k) ? "true" : "false");
						MSG_NOTICE << tr("%1 %2 ").arg(j).arg(k) << value->histogram(j, k);
					}
					settings.endGroup();
					break;
			}
		}
		++i;
	}
	return true;
}

/*!
    \fn Detector::loadSetup(QSettings &settings)

    Loads the setup from a file. This function should be able to load
    "MesyDAQ" files and also "QMesyDAQ" files (using QSettings class which is
    not easy human readable).

    \note MesyDAQ INI file format is not used correctly, because the section
	  names are not unique: imagine you don't have a single MCPD-8 + MPSD-8 ...

    \param settings
    \return true if successfully loaded otherwise false
 */
bool Detector::loadSetup(QSettings &settings)
{
	int		nMcpd(0);
	bool		bOK(false);

	foreach (MCPD8* value, m_mcpd)
		delete value;
	m_mcpd.clear();

	settings.beginGroup("MESYDAQ");
	m_acquireListfile = settings.value("listmode", "true").toBool();
	m_autoSaveHistogram = settings.value("autosavehistogram", "false").toBool();
	m_pDatSender->SetSource(settings.value("repeatersource", "").toString());
	m_pDatSender->SetTarget(settings.value("repeatertarget", "").toString(),
				settings.value("repeaterport", m_pDatSender->DEFAULTPORT).toUInt());
	m_pDatSender->SetEnabled(settings.value("repeaterenable", "false").toBool());
	settings.endGroup();

	QStringList mcpdList = settings.childGroups().filter("MCPD");
	for (int i = 0; i < mcpdList.size(); ++i)
	{
		settings.beginGroup(mcpdList[i]);
		int iId = settings.value("id", "-1").toInt();
		if (iId < 0)
		{
			MSG_FATAL << tr("found no or invalid MCPD id");
			continue;
		}
		++nMcpd;

		QString IP = settings.value("ipAddress", "192.168.168.121").toString();
		quint16 port = settings.value("port", "54321").toUInt();
		QString cmdIP = settings.value("cmdip", "0.0.0.0").toString();
		quint16 cmdPort = settings.value("cmdport", "0").toUInt();
		QString dataIP = settings.value("dataip", "0.0.0.0").toString();
		quint16 dataPort = settings.value("dataport", "0").toUInt();
		m_detector = settings.value("detector", "0").toInt();
		m_histogramPos = settings.value("histogrampos", QPoint(0, 0)).toPoint();


		QHostAddress cmd(cmdIP);
		if (
#if QT_VERSION < 0x050000
		    cmd == QHostAddress::Any ||
#else
		    cmd == QHostAddress::AnyIPv4 ||
#endif
		    cmd == QHostAddress::AnyIPv6 || IP == cmdIP)
		{
			cmdIP = "0.0.0.0";
			cmdPort = 0;
		}
		if (port == cmdPort)
			cmdPort = 0;

		QHostAddress data(dataIP);
		if (
#if QT_VERSION < 0x050000
		    data == QHostAddress::Any ||
#else
		    data == QHostAddress::AnyIPv4 ||
#endif
		    data == QHostAddress::AnyIPv6 || IP == dataIP)
		{
			dataIP = "0.0.0.0";
//			dataPort = 0;
		}
		if (port == dataPort)
			dataPort = 0;

#if 1
		addMCPD(iId, IP, port > 0 ? port : cmdPort, dataIP, dataPort, cmdIP);
#else
		QMetaObject::invokeMethod(this, "addMCPD", Qt::BlockingQueuedConnection,
					  Q_ARG(quint16, iId),
					  Q_ARG(QString, IP), Q_ARG(quint16, port > 0 ? port : cmdPort),
					  Q_ARG(QString, dataIP), Q_ARG(quint16, dataPort),
					  Q_ARG(QString, cmdIP));
#endif
		MCPD8 *pMCPD = m_mcpd[iId];
		if (dynamic_cast<MCPD *>(pMCPD) != NULL && pMCPD->isInitialized())
		{
			for (int j = 0; j < 4; ++j)
			{
				setAuxTimer(iId, j, settings.value(QString("auxtimer%1").arg(j), "0").toUInt());
				setParamSource(iId, j, settings.value(QString("paramsource%1").arg(j), QString("%1").arg(j)).toUInt());
			}

//			pMCPD->setActive(loadSetupBoolean(pSection, szPrefix + "active", true));
//			pMCPD->setHistogram(loadSetupBoolean(pSection, szPrefix + "histogram", true));

			for (int j = 0; j < 8; ++j)
			{
				long cells[2] = {7, 22};
				QStringList k = settings.value(QString("countercell%1").arg(j), "7 22").toString().split(QRegExp("\\s+"));
				for(int m = 0; m < k.size() && m < 2; ++m)
				{
					int tmp = k[m].toInt(&bOK);
					if (bOK)
						cells[m] = tmp;
				}
				setCounterCell(iId, j, cells[0], cells[1]);
			}
			setTimingSetup(iId, settings.value("master", "true").toBool(), settings.value("terminate", "true").toBool(),
				settings.value("extsync", "false").toBool());
			setProtocol(iId, QString("0.0.0.0"), dataIP, dataPort, cmdIP, cmdPort);
		}
		else
			MSG_FATAL << tr("MCPD id %1 with address %2 was not correctly initialized").arg(iId).arg(IP);
		settings.endGroup();
	}

	QStringList moduleList = settings.childGroups().filter("MODULE") + settings.childGroups().filter("MPSD");
	for (int i = 0; i < moduleList.size(); ++i)
	{
		settings.beginGroup(moduleList[i]);

		int iId = settings.value("id", "-1").toInt();
		if (iId < 0)
		{
			MSG_FATAL << tr("found no or invalid Module id");
			continue;
		}
		int iMCPDId = iId / 8;
		int j = iId % 8;
		quint8	gains[16],
			threshold,
			channels(8);
		bool	comgain(true);
		bool	ampmode(false);

		if (m_mcpd.contains(iMCPDId) && dynamic_cast<MCPD *>(m_mcpd[iMCPDId]) != NULL && m_mcpd[iMCPDId]->isInitialized())
		{
			int moduleID = getModuleId(iMCPDId, j);
			switch (moduleID)
			{
				case TYPE_MDLL:
					break;
				case TYPE_MSTD16:
					channels = 16;
					ampmode = true;
					/* no break */
				case TYPE_NOMODULE:	// If there is no hardware module found it will be assumed, it should be a missed MPSD-8
				default:
					for (int k = 0; k < channels; ++k)
						gains[k] = settings.value(QString("gain%1").arg(k), "92").toUInt();
					for (int k = 0; k < channels; ++k)
					{
						setActive(iMCPDId, j, k, settings.value(QString("active%1").arg(k), moduleID == TYPE_NOMODULE ? "false" : "true").toBool());
						setHistogram(iMCPDId, j, k, settings.value(QString("histogram%1").arg(k), "true").toBool());
					}

					threshold = settings.value("threshold", "22").toUInt();
					ampmode = settings.value("ampmode", ampmode ? "true" : "false").toBool();

					for (int k = 0; k < channels; ++k)
						if (gains[0] != gains[k])
						{
							comgain = false;
							break;
						}
					if (comgain)
						setGain(iMCPDId, j, channels, gains[0]);
					else
						for (int k = 0; k < channels; ++k)
							setGain(iMCPDId, j, k, gains[k]);
					setThreshold(iMCPDId, j, threshold);
					setMode(iMCPDId, j, ampmode);
					break;
			}
		}
		settings.endGroup();
	}

	moduleList = settings.childGroups().filter("MDLL");
	for (int i = 0; i < moduleList.size(); ++i)
	{
		settings.beginGroup(moduleList[i]);

		int iMCPDId = settings.value("id", "-1").toInt();
		if (iMCPDId < 0)
		{
			MSG_FATAL << tr("found no or invalid Module id");
			continue;
		}

		if (dynamic_cast<MCPD *>(m_mcpd[iMCPDId]) != NULL && m_mcpd[iMCPDId]->isInitialized())
		{
			if (getModuleId(iMCPDId, 0) == TYPE_MDLL)
			{
				quint8	thresh[3],
					shift[2],
					scale[2],
					dataset,
					ewindow[2];
				quint16 twindow[4];

				thresh[0] = settings.value(QString("threshX"), "20").toUInt();
				thresh[1] = settings.value(QString("threshY"), "20").toUInt();
				thresh[2] = settings.value(QString("threshA"), "20").toUInt();
				setMdllThresholds(iMCPDId, thresh[0], thresh[1], thresh[2]);

				shift[0] = settings.value(QString("shiftX"), "100").toUInt();
				shift[1] = settings.value(QString("shiftY"), "100").toUInt();
				scale[0] = settings.value(QString("scaleX"), "48").toUInt();
				scale[1] = settings.value(QString("scaleY"), "48").toUInt();
				setMdllSpectrum(iMCPDId, shift[0], shift[1], scale[0], scale[1]);

				twindow[0] = settings.value(QString("tWinXLo"), "100").toUInt();
				twindow[1] = settings.value(QString("tWinXHi"), "1000").toUInt();
				twindow[2] = settings.value(QString("tWinYLo"), "100").toUInt();
				twindow[3] = settings.value(QString("tWinYHi"), "1000").toUInt();
				setMdllTimingWindow(iMCPDId, twindow[0], twindow[1], twindow[2], twindow[3]);

				ewindow[0] = settings.value(QString("eWinLo"), "20").toUInt();
				ewindow[1] = settings.value(QString("eWinHi"), "240").toUInt();
				setMdllEnergyWindow(iMCPDId, ewindow[0], ewindow[1]);

				dataset = settings.value(QString("dataset"), "0").toUInt();
				setMdllDataset(iMCPDId, dataset);
			}
			setActive(iMCPDId, iMCPDId, settings.value(QString("active"), "true").toBool());
			setHistogram(iMCPDId, iMCPDId, settings.value(QString("histogram"), "true").toBool());
		}
		settings.endGroup();
	}

// scan connected MCPDs
	quint16 p(0);
	foreach(MCPD8 *value, m_mcpd)
	{
		Q_ASSERT_X(value != NULL, "Detector::loadSetup", "one of the MCPD's is NULL");
		p += value->numModules();
	}
	MSG_INFO << tr("%1 MCPD-8 and %2 Modules found").arg(nMcpd).arg(p);
	return true;
}

/*!
    \fn Detector::timerEvent(QTimerEvent *event)

    callback for the timer
 */
void Detector::timerEvent(QTimerEvent * /* event */)
{
#if 0
	if (event->timerId() == m_checkTimer)
		checkMcpd(0);
#endif
}

/*!
    \fn Detector::initMcpd(quint8 id)

    initializes a MCPD

    \todo is this function really needed

    \param id number of the MCPD
 */
void Detector::initMcpd(quint8 id)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->init();
}


/*!
    \fn Detector::allPulserOff()

    switches all pulsers off
 */
void Detector::allPulserOff()
{
// send pulser off to all connected MPSD
	for (QHash<int, MCPD8 *>::iterator it = m_mcpd.begin(); it != m_mcpd.end(); ++it)
		for(quint8 j = 0; j < 8; j++)
			if(it.value()->getModuleId(j))
				it.value()->setPulser(j, 0, 2, 40, false);
}

/*!
    \fn Detector::setTimingwidth(quint8 width)

    defines a timing width ????

    \todo not really implemented

    \param width ????
 */
void Detector::setTimingwidth(quint8 width)
{
	m_timingwidth = width;
	if(width > 48)
		m_timingwidth = 48;
#if defined (_MSC_VER)
#	pragma message("ToDo")
#else
#	warning TODO
#endif
//! \todo set the timing width of the histogram
#if 0
	if (m_hist)
		m_hist->setWidth(m_timingwidth);
#endif
}

/*!
    \fn quint16 Detector::capabilities(quint16 id, const bool cached)

    reads the capabilities of the central module.

    \param id number of the MCPD
    \return the central module capabilities
 */
quint16 Detector::capabilities(quint16 id, const bool cached)
{
	if (m_mcpd.empty() || !m_mcpd.contains(id))
		return 0;
	return m_mcpd[id]->capabilities(cached);
}

/*!
 * \fn quint16 Detector::getTxMode(quint16 id)
 * reads the current set transmission mod of the central module
 *
 *  \param id number of the MCPD
 *  \return the central module transmission mode
 */
quint16 Detector::getTxMode(quint16 id, const bool cached)
{
	if (m_mcpd.empty() || !m_mcpd.contains(id))
		return 0;
	return m_mcpd[id]->getTxMode(cached);
}

/*!
 * \fn quint16 Detector::getTxMode(quint16 id, quint8 mod)
 * reads the current set transmission mod of the central module
 *
 *  \param id number of the MCPD
 *  \return the central module transmission mode
 */
quint16 Detector::getTxMode(quint16 id, quint8 mod)
{
	if (m_mcpd.empty() || !m_mcpd.contains(id))
		return 0;
	return m_mcpd[id]->getTxMode(mod);
}

/*!
    \fn Detector::readPeriReg(quint16 id, quint16 mod, quint16 reg)

    reads the content of a register in a module

    \param id number of the MCPD
    \param mod number of the module
    \param reg number of the register
    \return content of the register
    \see writePeriReg

 */
quint16 Detector::readPeriReg(quint16 id, quint16 mod, quint16 reg)
{
	if (m_mcpd.empty())
		return 0;
	return m_mcpd[id]->readPeriReg(mod, reg);
}

/*!
    \fn Detector::writePeriReg(quint16 id, quint16 mod, quint16 reg, quint16 val)

    writes a value into a module register

    \param id number of the MCPD
    \param mod number of the module
    \param reg number of the register
    \param val new value
    \see readPeriReg
 */
void Detector::writePeriReg(quint16 id, quint16 mod, quint16 reg, quint16 val)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->writePeriReg(mod, reg, val);
}

// command shortcuts for simple operations:
/*!
    \fn Detector::start(void)

    starts a data acquisition
 */
void Detector::start(void)
{
	MSG_NOTICE << tr("remote start");
// start the slaves first
	for (QHash<int, MCPD8 *>::iterator it = m_mcpd.begin(); it != m_mcpd.end(); ++it)
		if (!it.value()->isMaster())
			it.value()->start();
// start the masters
	for (QHash<int, MCPD8 *>::iterator it = m_mcpd.begin(); it != m_mcpd.end(); ++it)
		if (it.value()->isMaster())
			it.value()->start();
	m_Running = DET_RUNNING;
	m_starttime_msec = time();
	emit statusChanged("STARTED");
}

/*!
    \fn Detector::stop(void)

    stops a data acquisition
 */
void Detector::stop(void)
{
	MSG_NOTICE << tr("remote stop");
	emit statusChanged("STOPPED");
// Stop the masters first
	for (QHash<int, MCPD8 *>::iterator it = m_mcpd.begin(); it != m_mcpd.end(); ++it)
		if (it.value()->isMaster())
			it.value()->stop();
// Stop the slaves
	for (QHash<int, MCPD8 *>::iterator it = m_mcpd.begin(); it != m_mcpd.end(); ++it)
		if (!it.value()->isMaster())
			it.value()->stop();
	m_Running = DET_IDLE;
	emit statusChanged("IDLE");
}

/*!
    \fn Detector::cont(void)

    continues a data acquisition
 */
void Detector::cont(void)
{
	MSG_NOTICE << tr("remote cont");
// continue the slaves first
	for (QHash<int, MCPD8 *>::iterator it = m_mcpd.begin(); it != m_mcpd.end(); ++it)
		if (!it.value()->isMaster())
			it.value()->cont();
// continue the masters
	for (QHash<int, MCPD8 *>::iterator it = m_mcpd.begin(); it != m_mcpd.end(); ++it)
		if (it.value()->isMaster())
			it.value()->cont();
	emit statusChanged("STARTED");
}

/*!
    \fn Detector::reset(void)

    resets the MCPD's

    \todo implement me
 */
void Detector::reset(void)
{
	MSG_NOTICE << tr("remote reset");
	for (QHash<int, MCPD8 *>::iterator it = m_mcpd.begin(); it != m_mcpd.end(); ++it)
		it.value()->reset();
}

/*!
    \fn Detector::checkMcpd(quint8 device)

    rescan all MCPD's for the connected modules

    \return true if successfully finished otherwise false
 */
bool Detector::checkMcpd(quint8 /* device */)
{
	quint8 c(0);
	for (QHash<int, MCPD8 *>::iterator it = m_mcpd.begin(); it != m_mcpd.end(); ++it)
		(void)it.value(), scanPeriph(c++);
	return true;
}

/*!
    \fn Detector::setProtocol(const quint16 id, const QString &mcpdIP, const QString &dataIP, const quint16 dataPort, const QString &cmdIP, const quint16 cmdPort)

    configures a MCPD for the communication it will set the IP address of the module, the IP address and ports of the data and command sink

    \param id number of the MCPD
    \param mcpdIP new IP address of the module
    \param dataIP IP address to which data packets should be send (if 0.0.0.0 the sender will be receive them)
    \param dataPort port number for data packets (if 0 the port number won't be changed)
    \param cmdIP IP address to which cmd answer packets should be send (if 0.0.0.0 the sender will be receive them)
    \param cmdPort port number for cmd answer packets (if 0 the port number won't be changed)
    \see getProtocol
 */
void Detector::setProtocol(const quint16 id, const QString &mcpdIP, const QString &dataIP, quint16 dataPort, const QString &cmdIP, quint16 cmdPort)
{
	// NOTE: the MWPCHR module type does not need set the communication parameters
	if (m_mcpd.contains(id) && m_mcpd[id]->getModuleId(0) != TYPE_MWPCHR)
		m_mcpd[id]->setProtocol(mcpdIP, dataIP, dataPort, cmdIP, cmdPort);
}

/*!
    \fn void Detector::getProtocol(const quint16 id, QString &mcpdIP, QString &dataIP, quint16 &dataPort, QString &cmdIP, quint16 &cmdPort) const

    gets the configured parameters from the MCPD with the ID id.

    \param id number of the MCPD
    \param mcpdIP new IP address of the module
    \param dataIP IP address to which data packets should be send (if 0.0.0.0 the sender will be receive them)
    \param dataPort port number for data packets (if 0 the port number won't be changed)
    \param cmdIP IP address to which cmd answer packets should be send (if 0.0.0.0 the sender will be receive them)
    \param cmdPort port number for cmd answer packets (if 0 the port number won't be changed)
    \see setProtocol
 */
void Detector::getProtocol(const quint16 id, QString &mcpdIP, QString &dataIP, quint16 &dataPort, QString &cmdIP, quint16 &cmdPort) const
{
	if (m_mcpd.contains(id))
		 m_mcpd[id]->getProtocol(mcpdIP, cmdIP, cmdPort, dataIP, dataPort);
}

/*!
    \fn Detector::getMode(const quint16 id, quint8 addr)

    get the mode: amplitude or position

    \param id number of the MCPD
    \param addr module number
    \see setMode
 */
bool Detector::getMode(const quint16 id, quint8 addr)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getMode(addr);
	return false;
}

/*!
    \fn Detector::setMode(const quint16 id, quint8 addr, bool mode)

    set the mode to amplitude or position

    \param id number of the MCPD
    \param addr number of the module
    \param mode if true amplitude mode otherwise position mode
    \see getMode
*/
void Detector::setMode(const quint16 id, quint8 addr, bool mode)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setMode(addr, mode);
}

/*!
    \fn Detector::setPulser(const quint16 id, quint8 addr, quint8 chan, quint8 pos, quint8 amp, bool onoff)

    set the pulser of the module to a position, channel, and amplitude
    and switch it on or off

    \param id number of the MCPD
    \param addr number of the module
    \param chan number of the channel of the module
    \param pos set the position to left, middle or right of the 'tube'
    \param amp the amplitude of a test pulse (event)
    \param onoff true the pulser will be switch on, otherwise off
    \see isPulserOn
 */
void Detector::setPulser(const quint16 id, quint8 addr, quint8 channel, quint8 position, quint8 amp, bool onoff)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setPulser(addr, channel, position, amp, onoff);
}

/*!
    \fn Detector::setCounterCell(quint16 id, quint16 source, quint16 trigger, quint16 compare)

    map the counter cell

    \param id number of the MCPD
    \param source source of the counter
    \param trigger trigger level
    \param compare ????
    \see getCounterCell
 */
void Detector::setCounterCell(quint16 id, quint16 source, quint16 trigger, quint16 compare)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setCounterCell(source, trigger, compare);
}

/*!
    \fn Detector::setParamSource(quint16 id, quint16 param, quint16 source)

    set the source of a parameter

    \param id number of the MCPD
    \param param number of the parameter
    \param source number of source
    \see getParamSource
 */
void Detector::setParamSource(quint16 id, quint16 param, quint16 source)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setParamSource(param, source);
}

/*!
    \fn Detector::setAuxTimer(quint16 id, quint16 tim, quint16 val)

    sets the auxiliary timer to a new value

    \param id number of the MCPD
    \param tim number of the timer
    \param val new timer value
    \see getAuxTimer
 */
void Detector::setAuxTimer(quint16 id, quint16 tim, quint16 val)
{
	MSG_INFO << tr("set aux timer : ID = %1, timer = %2, interval = %3").arg(id).arg(tim).arg(val);
	if (m_mcpd.contains(id))
		m_mcpd[id]->setAuxTimer(tim, val);
}

/*!
    \fn Detector::setMasterClock(quint16 id, quint64 val)

    sets the master clock to a new value

    \todo check for the masters

    \param id number of the MCPD
    \param val new clock value
 */
void Detector::setMasterClock(quint16 id, quint64 val)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setMasterClock(val);
}

/*!
    \fn Detector::setTimingSetup(quint16 id, bool master, bool term, bool extsync)

    sets the communication parameters between the MCPD's

    \param id number of the MCPD
    \param master is this MCPD master or not
    \param term should the MCPD synchronization bus terminated or not
    \param extsync is external synchronization bus enabled or not (omly master)
 */
void Detector::setTimingSetup(quint16 id, bool master, bool term, bool extsync)
{
	// MWPCHR does not have set timing setup
	if (m_mcpd.contains(id) && m_mcpd[id]->getModuleId(0) != TYPE_MWPCHR)
		m_mcpd[id]->setTimingSetup(master, term, extsync);
}

/*!
    \fn Detector::setId(quint16 id, quint8 mcpdid)

    sets the id of the MCPD

    \param id number of the MCPD
    \param mcpdid the new ID of the MCPD
    \see getId
 */
void Detector::setId(quint16 id, quint8 mcpdid)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setId(mcpdid);
}

/*!
    \fn Detector::setGain(quint16 id, quint8 addr, quint8 chan, quint8 gainval)

    sets the gain to a poti value

    \param id number of the MCPD
    \param addr number of the module
    \param chan channel number of the module
    \param gainval poti value of the gain
    \see getGain
 */
void Detector::setGain(quint16 id, quint8 addr, quint8 channel, quint8 gain)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setGain(addr, channel, gain);
}

/*!
    \overload Detector::setGain(quint16 id, quint8 addr, quint8 chan, float gainval)

    the gain value will be set as a user value

    \param id number of the MCPD
    \param addr number of the module
    \param chan channel number of the module
    \param gainval user value of the gain
    \see getGain
 */
void Detector::setGain(quint16 id, quint8 addr, quint8 chan, float gain)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setGain(addr, chan, gain);
}

/*!
    \fn Detector::setThreshold(quint16 id, quint8 addr, quint8 thresh)

    set the threshold value as poti value

    \param id number of the MCPD
    \param addr number of the module
    \param thresh threshold value as poti value
    \see getThresh
 */
void Detector::setThreshold(quint16 id, quint8 addr, quint8 thresh)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setThreshold(addr, thresh);
}

/*!
    \overload Detector::setThreshold(quint16 id, quint8 addr, quint16 thresh)

    set the threshold value

    \param id number of the MCPD
    \param addr number of the module
    \param thresh threshold value
    \see getThresh
*/
void Detector::setThreshold(quint16 id, quint8 addr, quint16 thresh)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setThreshold(addr, thresh);
}

/*!
    \fn void Detector::setMdllThresholds(quint16 id, quint8 threshX, quint8 threshY, quint8 threshA)

    \param id
    \param threshX
    \param threshY
    \param threshA
 */
void Detector::setMdllThresholds(quint16 id, quint8 threshX, quint8 threshY, quint8 threshA)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setMdllThresholds(threshX, threshY, threshA);
}

/*!
    \fn void Detector::setMdllSpectrum(quint16 id, quint8 shiftX, quint8 shiftY, quint8 scaleX, quint8 scaleY)

    \param id
    \param shiftX
    \param shiftY
    \param scaleX
    \param scaleY
 */
void Detector::setMdllSpectrum(quint16 id, quint8 shiftX, quint8 shiftY, quint8 scaleX, quint8 scaleY)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setMdllSpectrum(shiftX, shiftY, scaleX, scaleY);
}

/*!
     \fn void Detector::setMdllDataset(quint16 id, quint8 set)

     \param id
     \param set
 */
void Detector::setMdllDataset(quint16 id, quint8 set)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setMdllDataset(set);
}

/*!
    \fn void Detector::setMdllTimingWindow(quint16 id, quint16 xlo, quint16 xhi, quint16 ylo, quint16 yhi)

    \param id
    \param xlo
    \param xhi
    \param ylo
    \param yhi
 */
void Detector::setMdllTimingWindow(quint16 id, quint16 xlo, quint16 xhi, quint16 ylo, quint16 yhi)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setMdllTimingWindow(xlo, xhi, ylo, yhi);
}

/*!
    \fn void Detector::setMdllEnergyWindow(quint16 id, quint8 elo, quint8 ehi)

    \param id
    \param elo
    \param ehi
 */
void Detector::setMdllEnergyWindow(quint16 id, quint8 elo, quint8 ehi)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setMdllEnergyWindow(elo, ehi);
}

/*!
    \fn quint8 Detector::getModuleId(quint16 id, quint8 addr)

    get the detected ID of the MPSD. If MPSD not exists it will return 0.

    \param id number of the MCPD
    \param addr module number
    \return module ID (type)
    \see readId
 */
quint8 Detector::getModuleId(quint16 id, quint8 addr)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getModuleId(addr);
	return 0;
}

/*!
    \fn quint8 Detector::getChannels(quint16 id, quint8 addr)

    get the channel count of the MPSD. If MPSD not exists it will return 0.

    \param id number of the MCPD
    \param addr module number
    \return channel count
 */
quint8 Detector::getChannels(quint16 id, quint8 addr)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getChannels(addr);
	return 0;
}

/*!
    \fn quint8 Detector::getMdllDataset(quint16 id)

    get dataset setting of the MDLL. E, X, Y / E, tX, tY

    \param id number of the MCPD
    \return dataset
 */
quint8 Detector::getMdllDataset(quint16 id)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getMdllDataset();
	return 0;
}

/*!
    \fn quint16 Detector::getMdllTimingWindow(quint16 id, quint8 val)

    get one of four timing window borders of the MDLL.

    \param id number of the MCPD
    \param val requested value: txlo, txhi, tylo, tyhi (0...3)
    \return border value
 */
quint16 Detector::getMdllTimingWindow(quint16 id, quint8 val)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getMdllTimingWindow(val);
	return 0;
}

/*!
    \fn quint8 Detector::getMdllEnergyWindow(quint16 id, quint8 val)

    get one of two energy window borders of the MDLL.

    \param id number of the MCPD
    \param val requested value: elo, ehi (0/1)
    \return border value
 */
quint8 Detector::getMdllEnergyWindow(quint16 id, quint8 val)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getMdllEnergyWindow(val);
	return 0;
}

/*!
    \fn quint8 Detector::getMdllSpectrum(quint16 id, quint8 val)

    get one of four spectrum parameters of the MDLL.

    \param id number of the MCPD
    \param val requested value: shiftX, shiftY, scaleX, scaleY (0...3)
    \return spectrum value
 */
quint8 Detector::getMdllSpectrum(quint16 id, quint8 val)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getMdllSpectrum(val);
	return 0;
}

/*!
    \fn quint8 Detector::getMdllThresholds(quint16 id, quint8 val)

    get one of three CFD thresholds of the MDLL.

    \param id number of the MCPD
    \param val requested value: threshX, threshY, threshA (0...2)
    \return threshold value
 */
quint8 Detector::getMdllThresholds(quint16 id, quint8 val)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getMdllThreshold(val);
	return 0;
}

/*!
    \fn quint8 Detector::getMdllPulser(quint16 id, quint8 val)

    get one of three pulser parameters of the MDLL.

    \param id number of the MCPD
    \param val requested value: On, Amplitude, Position (0...2)
    \return pulser parameter
 */
quint8 Detector::getMdllPulser(quint16 id, quint8 val)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getMdllPulser(val);
	return 0;
}

/*!
    \fn bool Detector::online(quint16 id, quint8 addr)

    returns whether the module is detected and online or not

    \param id number of the MCPD
    \param addr module number
    \return true or false
 */
bool Detector::online(quint16 id, quint8 addr)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->online(addr);
	return false;
}

/*!
    \fn Detector::getModuleType(quint16 id, quint8 addr)

    get the detected ID of the MPSD. If MPSD not exists it will return 0.
    the value is a human readable value.

    \param id number of the MCPD
    \param addr module number
    \return module ID (type)
    \see readId
 */
QString Detector::getModuleType(quint16 id, quint8 addr)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getModuleType(addr);
	return "-";
}

/*!
    \fn float Detector::getModuleVersion(quint16 id, quint8 addr)

    get the detected version of the MPSD. If MPSD not exists it will return 0.

    \param id number of the MCPD
    \param addr module number
    \return module ID (type)
    \see readId
*/
float Detector::getModuleVersion(quint16 id, quint8 addr)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->version(addr);
	return 0.0;
}

/*!
    \fn Detector::getPulsChan(quint16 id, quint8 addr)

    gets the current set channel of the pulser of a module

    \param id number of the MCPD
    \param addr module number
    \return the pulser channel
    \see setPulser
    \see getPulsAmp
    \see getPulsPos
 */
quint8 Detector::getPulsChan(quint16 id, quint8 addr)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getPulsChan(addr);
	return 0;
}

/*!
    \fn Detector::getPulsAmp(quint16 id, quint8 addr)

    gets the current set pulser amplitude of a module

    \param id number of the MCPD
    \param addr module number
    \return the pulser amplitude
    \see setPulser
    \see getPulsPos
    \see getPulsChan
 */
quint8 Detector::getPulsAmp(quint16 id, quint8 addr)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getPulsAmp(addr);
	return 0;
}

/*!
    \fn Detector::getPulsPos(quint16 id, quint8 addr)
    gets the current set pulser position of a module

    \param id number of the MCPD
    \param addr module number
    \return the pulser position
    \see setPulser
    \see getPulsAmp
    \see getPulsChan
 */
quint8 Detector::getPulsPos(quint16 id, quint8 addr)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getPulsPos(addr);
	return 0;
}

/*!
    \fn Detector::getCounterCell(quint16 id, quint8 cell, quint16 *celldata)

    celldata[0] = trig, celldata[1] = comp

    \param id number of the MCPD
    \param cell cell number
    \param celldata return data
    \see setCounterCell
 */
void Detector::getCounterCell(quint16 id, quint8 cell, quint16 *celldata)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->getCounterCell(cell, celldata);
}

/*!
    \fn Detector::getParamSource(quint16 id, quint16 param)

    get the source of parameter param

    \param id number of the MCPD
    \param param the parameter number
    \return source of the parameter
    \see setParamSource
 */
quint16 Detector::getParamSource(quint16 id, quint16 param)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getParamSource(param);
	return 0;
}

/*!
    \fn Detector::getAuxTimer(quint16 id, quint16 timer)

    get the value of auxiliary counter

    \param id number of the MCPD
    \param timer number of the timer
    \return counter value
    \see setAuxTimer
 */
quint16 Detector::getAuxTimer(quint16 id, quint16 timer)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getAuxTimer(timer);
	return 0;
}

/*!
    \fn Detector::getParameter(quint16 id, quint16 param)

    gets the value of the parameter number param

    \param id number of the MCPD
    \param param parameter number
    \return parameter value
    \see setParameter
 */
quint64 Detector::getParameter(quint16 id, quint16 param)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getParameter(param);
	return 0;
}

/*!
    \fn Detector::getGain(quint16 id, quint8 addr,  quint8 chan)

    gets the currently set gain value for a special module and channel

    if the channel number is greater 7 or 15 than all channels of the module
    will be set (MSTD-16 has 16 channels, others have 8 channels)

    \param id number of the MCPD
    \param addr number of the module
    \param chan number of the channel of the module
    \return poti value of the gain
    \see setGain
 */
float Detector::getGain(quint16 id, quint8 addr, quint8 chan)
{
	if (m_mcpd.contains(id))
#if 0
		return m_mcpd[id]->getGainVal(addr, chan);
#else
		return m_mcpd[id]->getGainPoti(addr, chan);
#endif
	MSG_DEBUG << tr("getGain not found id %1").arg(id);
	return 0;
}

/*!
    \fn Detector::getThreshold(quint16 id, quint8 addr)

    get the threshold value as poti value

    \param id number of the MCPD
    \param addr module number
    \return the threshold as poti value
    \see setThreshold
 */
quint8 Detector::getThreshold(quint16 id, quint8 addr)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getThreshold(addr);
	return 0;
}

quint32 Detector::runId(void)
{
	return m_runId;
#if 0
	//! this is dangerous because at startup the mcpd's not set
	//! and the configuration will not be reloaded
	foreach (MCPD8* value, m_mcpd)
		if (value->isMaster())
			return value->getRunId();
#endif
}
/*!
    \fn Detector::setRunId(quint32 runid)

    sets the run ID of the measurement

    \param runid the new run ID
    \return true if operation was succesful or not
    \see getRunId
 */
void Detector::setRunId(quint32 runid)
{
	MSG_NOTICE << tr("Detector::setRunId(%1)").arg(runid);
	m_runId = runid;
	m_pDatStream->setRunId(m_runId);
	for (QHash<int, MCPD8 *>::iterator it = m_mcpd.begin(); it != m_mcpd.end(); ++it)
		if (it.value()->isMaster())
		{
			it.value()->setRunId(m_runId);
			return;
		}
}

/*!
    \fn Detector::dumpListmode(QSharedDataPointer<SD_PACKET> pPacket)

    callback to dump the data into a list mode file

    \param pPacket data packet
 */
void Detector::dumpListmode(QSharedDataPointer<SD_PACKET> pPacket)
{
	if (m_Running == DET_RUNNING)
	{
		const DATA_PACKET *dp = &pPacket.constData()->dp;
		if(m_acquireListfile)
		{
			*m_pDatStream << dp;
		}
		m_pDatSender->WriteData(&dp->bufferLength, dp->bufferLength, true);
		writeBlockSeparator();
	}
}

/*!
    \fn Detector::rearrangeEvents(EVENT_BUFFER evb)

    callback to rearrange the positions

    \param evb event packet
 */
void Detector::rearrangeEvents(QSharedDataPointer<EVENT_BUFFER> evb)
{
	if (m_Running == DET_IDLE)
	{
		MSG_ERROR << tr("DROP DATA PACKET: no data acquisition is running!");
		MSG_ERROR << tr("device id: %1").arg(evb->id);
		return;
	}
	if (m_mcpd.contains(evb->id))
	{
		quint32 xshift = idOffset(evb->id).x();
		quint32 yshift = 0 /* idMapping(evb->id).y() * m_mcpd[evb->id]->height() */;
#if 0
		if (evb->id > 0)
			MSG_ERROR << tr("X shift: %1").arg(xshift);
#endif
		for (QVector<EVENT>::iterator it = evb->events.begin(); it != evb->events.end(); ++it)
		{
			if (m_starttime_msec > (it->timestamp / 10000))
				MSG_FATAL << tr("OLD EVENT : %1 < %2").arg(it->timestamp / 10000).arg(m_starttime_msec);
			if (!it->trigger)
			{
				it->ev_neutron.x += xshift;
				it->ev_neutron.y += yshift;
			}
		}
	}
	emit newDataBuffer(evb);
}

/*!
    \fn Detector::idOffset(const quint8 id) const

    Calculates the x,y position of the module origin in the histogram.

    The histogram number and the position are given via the configuration file.

    \param id MCPD id
 */
QPoint Detector::idOffset(const quint8 id) const
{
	// TODO: the id of the module has to be mapped
	if (m_mcpd.contains(id))
	{
		int offset(0);
		// MSG_ERROR << tr("MCPD type %1: %2").arg(id).arg(m_mcpd[id]->type());
		switch (m_mcpd[id]->type())
		{
			case TYPE_MWPCHR:
			case TYPE_MDLL:
				for (int i = 0; i < id; ++i)
					offset += m_mcpd[i]->width();
				return QPoint(offset, 0);
			default:
				for (quint8 i = 0; i < id; ++i)
					offset += m_mcpd[i]->width();
				return QPoint(offset, 0);
		}
	}
	return QPoint(0, 0);
}

/*!
    \fn bool Detector::active(quint16 mid, quint16 id, quint16 chan)

    \param mid MCPD id
    \param id MPSD id
    \param chan channel id
    \return true or false
 */
bool Detector::active(quint16 mid, quint16 id, quint16 chan)
{
	if (m_mcpd.contains(mid))
		return m_mcpd[mid]->active(id, chan);
	return false;
}

/*!
    \fn bool Detector::active(quint16 mid, quint16 id)

    \param mid MCPD id
    \param id MPSD id
    \return true or false
 */
bool Detector::active(quint16 mid, quint16 id)
{
	if (m_mcpd.contains(mid))
		return m_mcpd[mid]->active(id);
	return false;
}

/*!
    \fn bool Detector::histogram(quint16 mid, quint16 id, quint16 chan)
    \param mid MCPD id
    \param id MPSD id
    \param chan channel id
    \return true or false
 */
bool Detector::histogram(quint16 mid, quint16 id, quint16 chan)
{
	bool result(false);
	if (m_mcpd.contains(mid))
		result = m_mcpd[mid]->histogram(id, chan);
	return result;
}

/*!
    \fn bool Detector::histogram(quint16, quint16)
    \param mid MCPD mid
    \param id MPSD id
    \return true or false
 */
bool Detector::histogram(quint16 mid, quint16 id)
{
	if (m_mcpd.contains(mid))
		return m_mcpd[mid]->histogram(id);
	return false;
}

/*!
    \fn void Detector::setActive(quint16 mcpd, quint16 mpsd, bool set)

    \param mcpd
    \param mpsd
    \param set
 */
void Detector::setActive(quint16 mcpd, quint16 mpsd, bool set)
{
	if (m_mcpd.contains(mcpd))
		m_mcpd[mcpd]->setActive(mpsd, set);
}

/*!
    \fn void Detector::setActive(quint16 mcpd, quint16 mpsd, quint8 channel, bool set)

    \param mcpd
    \param mpsd
    \param channel
    \param set
 */
void Detector::setActive(quint16 mcpd, quint16 mpsd, quint8 channel, bool set)
{
	if (m_mcpd.contains(mcpd))
		m_mcpd[mcpd]->setActive(mpsd, channel, set);
}

/*!
    \fn void Detector::setHistogram(quint16 mcpd, quint16 mpsd, bool hist)

    \param mcpd
    \param mpsd
    \param hist
 */
void Detector::setHistogram(quint16 mcpd, quint16 mpsd, bool hist)
{
	if (m_mcpd.contains(mcpd))
		m_mcpd[mcpd]->setHistogram(mpsd, hist);
}

/*!
    \fn void Detector::setHistogram(quint16 mcpd, quint16 mpsd, quint8 channel, bool hist)

    \param mcpd
    \param mpsd
    \param channel
    \param hist
 */
void Detector::setHistogram(quint16 mcpd, quint16 mpsd, quint8 channel, bool hist)
{
	if (m_mcpd.contains(mcpd))
		m_mcpd[mcpd]->setHistogram(mpsd, channel, hist);
}

QString Detector::libVersion(void) const
{
	return QString(VERSION);
}

float Detector::version(quint16 mcpd, quint8 mpsd)
{
	if (m_mcpd.contains(mcpd))
		return m_mcpd[mcpd]->version(mpsd);
	return 0.0;
}

/*! \brief store header for list mode file
 *
 * \param header the complete header text
 * \param bInsertHeaderLength add the length of the header or not in the header itself
 */
void Detector::setListFileHeader(const QByteArray& header, bool bInsertHeaderLength)
{
	m_datHeader = header;
	m_bInsertHeaderLength = bInsertHeaderLength;
}

/*! \returns return acquisition status
 *
 * \param pbAck return acknowledged acquisition status of hardware
 * \return return acquisition status
 */
bool Detector::status(bool* pbAck /*= NULL*/) const
{
	if (pbAck != NULL)
		*pbAck = m_bRunAck;
	return m_Running == DET_RUNNING;
}

/*!
 *
 * \returns the tube mapping vector
 */
QMap<quint16, quint16>& Detector::getTubeMapping()
{
	if (m_tubeMapping.isEmpty())
		width();
	return m_tubeMapping;
}

QList<int> Detector::mcpdId(void)
{
	MSG_INFO << "Detector::mcpdId : " << m_mcpd.size();
	QList<int> st = m_mcpd.keys();
    std::sort(std::begin(st), std::end(st));
	return st;
}

QList<int> Detector::mpsdId(const int id)
{
	QList<int> modList;
	if (m_mcpd.contains(id))
		modList = m_mcpd[id]->mpsdId();
	return modList;
}

QList<int> Detector::histogrammedId(const int id)
{
	QList<int> modList;
	if (m_mcpd.contains(id))
		modList = m_mcpd[id]->histogrammedId();
	return modList;
}

QList<int> Detector::channelId(const int id, const int mod)
{
	QList<int> channelList;
	if (m_mcpd.contains(id))
		if (m_mcpd[id]->getModuleId(mod))
			channelList = m_mcpd[id]->channelId(mod);
	return channelList;
}

quint16 Detector::startChannel(quint16 id)
{
	quint16 start(0);
	for (quint16 i = 0; i < id; ++i)
		start += width(i);
	return start;
}

quint16 Detector::width(quint16 id, quint16 end)
{
	quint16 size(0);
	for (int i = 0; i < end; ++i)
		size += getChannels(id, i);
	return size;
}

void Detector::lostSync(quint16 id, bool bLost)
{
	MSG_NOTICE << tr("MODULE : %1 %2").arg(id).arg(bLost ? "lost sync" : "resynchronized");
	emit syncLost(id, bLost);
}

bool Detector::isSynced() const
{
	foreach (const MCPD8 *pMCPD, m_mcpd)
		if (!pMCPD->isSynced())
			return false;
	return true;
}

void Detector::setStreamWriter(StreamWriter* pStreamWriter)
{
	if (m_pDatStream != NULL)
	{
		disconnect(m_pDatStream, SIGNAL(preCloseFile()), this, SLOT(writeClosingSignature()));
		disconnect(m_pDatStream, SIGNAL(closedFile(QIODevice*)), this, SLOT(writeProtectFile(QIODevice*)));
		disconnect(m_pDatStream, SIGNAL(openedFile()), this, SLOT(writeListfileHeader()));
		disconnect(m_pDatStream, SIGNAL(openedFile()), this, SLOT(writeHeaderSeparator()));
		delete m_pDatStream;
	}
	m_pDatStream = pStreamWriter;
	if (m_pDatStream != NULL)
	{
		connect(m_pDatStream, SIGNAL(preCloseFile()), this, SLOT(writeClosingSignature()), Qt::DirectConnection);
		connect(m_pDatStream, SIGNAL(closedFile(QIODevice*)), this, SLOT(writeProtectFile(QIODevice*)), Qt::DirectConnection);
		connect(m_pDatStream, SIGNAL(openedFile()), this, SLOT(writeListfileHeader()), Qt::DirectConnection);
		connect(m_pDatStream, SIGNAL(openedFile()), this, SLOT(writeHeaderSeparator()), Qt::DirectConnection);
	}
}

void Detector::setListfilename(const QString &name)
{
	m_listfilename = name;
}

QString Detector::getListfilename() const
{
	return m_listfilename;
}

const QByteArray& Detector::getListFileHeader() const
{
	return m_datHeader;
}

bool Detector::getAutoIncRunId() const
{
	return m_bAutoIncRunId;
}

void Detector::setAutoIncRunId(bool b)
{
	m_bAutoIncRunId = b;
}

bool Detector::getWriteProtection() const
{
	return m_bWriteProtect;
}

void Detector::setWriteProtection(bool b)
{
	m_bWriteProtect = b;
}

quint16 Detector::numMCPD(void) const
{
	return m_mcpd.size();
}

bool Detector::acqListfile() const
{
	return m_acquireListfile;
}

bool Detector::autoSaveHistogram() const
{
	return m_autoSaveHistogram;
}

void Detector::setHeadertime(quint64 ht)
{
	emit headerTimeChanged(ht);
	emit newCmdPackageReceived();
}

void Detector::setSetupType(const Setup val)
{
	m_setup = val;
}

Setup Detector::setupType() const
{
	return m_setup;
}

void Detector::replayPacket(QSharedDataPointer<SD_PACKET> pPacket)
{
	const DATA_PACKET *dp = &pPacket.constData()->dp;
	quint16 module = dp->deviceId;
	if (m_mcpd.contains(module))
	{
		MSG_INFO << tr("replay packet of module %1 (%2)").arg(module).arg(m_mcpd[module]->type());
		m_Running = DET_REPLAY;
		m_mcpd[module]->analyzeBuffer(pPacket);
		m_Running = DET_REPLAY;
	}
	else
		MSG_ERROR << tr("ignoring packet of module %1").arg(module);
}
