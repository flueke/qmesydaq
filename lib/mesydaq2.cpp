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

#include <QDateTime>
#include <QSettings>
#include <QTextStream>
#include <QTimerEvent>
#include <QStringList>
#include <QDebug>
#include <QHash>
#include <QHostAddress>

#include "mdefines.h"
#include "mesydaq2.h"
#include "mcpd8.h"

/*!
    \fn Mesydaq2::Mesydaq2(QObject *parent)

    constructor

    \param parent Qt parent object
*/
Mesydaq2::Mesydaq2(QObject *parent)
	: MesydaqObject(parent) 
	, m_daq(IDLE)
	, m_runID(0)
	, m_acquireListfile(false)
	, m_listfilename("")
	, m_histfilename("")
	, m_listPath("/home")
	, m_histPath("/home")
	, m_configPath("/home")
	, m_timingwidth(1)
{
	protocol(tr("running on Qt %1").arg(qVersion()), NOTICE);
}

//! destructor
Mesydaq2::~Mesydaq2()
{
	foreach (MCPD8* value, m_mcpd)
		delete value;
	m_mcpd.clear();
}

//! \return number of received data packages for the whole setup
quint64 Mesydaq2::receivedData(void) 
{
	quint64 dataRxd = 0;
	foreach (MCPD8	*value, m_mcpd)
		dataRxd += value->receivedData();	
	return dataRxd;
}

//! \return number of received cmd answer packages for the whole setup
quint64 Mesydaq2::receivedCmds(void)
{
	quint64 cmdRxd = 0;
	foreach (MCPD8	*value, m_mcpd)
		cmdRxd += value->receivedCmds();	
	return cmdRxd;
}

//! \return number of sent cmd packages for the whole setup
quint64 Mesydaq2::sentCmds(void) 
{
	quint64 cmdTxd = 0;
	foreach (MCPD8	*value, m_mcpd)
		cmdTxd += value->sentCmds();	
	return cmdTxd;
}

/*!
    \fn Mesydaq2::time(void)

    time of the first master MCPD
    \todo at the moment not implemented, use instead the first MCPD

    \return the time of the MCPD
 */
quint64 Mesydaq2::time(void)
{
	quint64 ret(0);
	if (!m_mcpd.empty())
	{
		foreach (MCPD8	*value, m_mcpd)
			if (value->isMaster())
			{
				ret = value->time();
				break;
			}
	}
	return ret;
}

/*!
    \fn Mesydaq2::writeRegister(quint16 id, quint16 reg, quint16 val)
    
    writes a value to a register in a selected MCPD, if the MPCD with
    the id does not exist, the command will be ignored

    \param id number of the MCPD
    \param reg number of the register
    \param val new value
    \see readRegister
 */
void Mesydaq2::writeRegister(quint16 id, quint16 reg, quint16 val)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->writeRegister(reg, val);
}

/*!
    \fn Mesydaq2::isMaster(quint16 mod)
    \param mod number of the MCPD
    \return is the MCPD master or not 
*/
bool Mesydaq2::isMaster(quint16 mod)
{
	if (!m_mcpd.contains(mod))
		return false;
	return m_mcpd[mod]->isMaster();
}

/*!
    \fn Mesydaq2::isTerminated(quint16 mod)
    \param mod number of the MCPD
    \return is the MCPD terminated or not 
*/
bool Mesydaq2::isTerminated(quint16 mod)
{
	if (!m_mcpd.contains(mod))
		return false;
	if (isMaster(mod))
		return true;
	else
		return m_mcpd[mod]->isTerminated();
}

/*!
    \fn float Mesydaq2::getFirmware(quint16 id)
    
    gets the firmware version of a MCPD

    \param id number of the MCPD
    \return the firmware version as float value, if MCPD does not exist 0
*/
float Mesydaq2::getFirmware(quint16 id)
{
	if (!m_mcpd.contains(id))
		return 0;
	return m_mcpd[id]->version();
}

/*!
    \fn Mesydaq2::acqListfile(bool yesno)

    enables the writing of list mode file

    \param yesno enable/disable if true/false
 */
void Mesydaq2::acqListfile(bool yesno)
{
	m_acquireListfile = yesno;
    	protocol(tr("Listfile recording %1").arg((yesno ? "on" : "off")), NOTICE);
}

/*!
    \fn Mesydaq2::startedDaq(void)

    callback after the start was succeeded

    \see start
    \see stop
    \see stoppedDaq
 */
void Mesydaq2::startedDaq(void)
{
	if(m_acquireListfile && !m_datfile.isOpen())
	{
		m_datfile.setFileName(m_listfilename);
		m_datfile.open(QIODevice::WriteOnly);
		m_datStream.setDevice(&m_datfile);
		writeListfileHeader();
		writeHeaderSeparator();
	}
	m_daq = RUNNING;
	emit statusChanged("RUNNING");
	protocol("daq started", DEBUG);
}

/*!
    \fn Mesydaq2::stoppedDaq(void)

    callback after the aquisition was stopped

    \see start
    \see stop
    \see startedDaq
 */
void Mesydaq2::stoppedDaq(void)
{
	if(m_acquireListfile && m_datfile.isOpen())
	{
		writeClosingSignature();
		m_datfile.close();
	}
	m_daq = IDLE;
	emit statusChanged("IDLE");
	protocol("daq stopped", DEBUG);
}

/*!
    \fn Mesydaq2::addMCPD(quint16 id, QString ip, quint16 port, QString sourceIP)

    'adds' another MCPD to this class

    \param id the ID of the MCPD
    \param ip the IP address of the MCPD
    \param port the port number to send data and cmds
    \param sourceIP IP address to get data and cmd answers back
*/
void Mesydaq2::addMCPD(quint16 id, QString ip, quint16 port, QString sourceIP)
{
	if (m_mcpd.contains(id))
		return;
	m_mcpd[id] = new MCPD8(id, this, ip, port, sourceIP);
	connect(m_mcpd[id], SIGNAL(analyzeDataBuffer(DATA_PACKET &)), this, SLOT(analyzeBuffer(DATA_PACKET &)));
	connect(m_mcpd[id], SIGNAL(startedDaq()), this, SLOT(startedDaq()));
	connect(m_mcpd[id], SIGNAL(stoppedDaq()), this, SLOT(stoppedDaq()));
}

/*!
    \fn Mesydaq2::writeListfileHeader(void)

    write the header of a listfile

    \see writeHeaderSeparator
    \see writeBlockSeparator
    \see writeClosingSignature
 */
void Mesydaq2::writeListfileHeader(void)
{
	if (m_datHeader.isEmpty())
	{
		QTextStream txtStr(&m_datfile);
		txtStr << QString("mesytec psd listmode data\n");
		txtStr << QString("header length: %1 lines \n").arg(2);
	}
	else
	{
		QByteArray header(m_datHeader);
		QByteArray lengthinfo;

		if (!header.endsWith('\n')) 
			header.append('\n');
		header.append("# offset (bytes) where binary tof data begins\n Data = ");
		int iLen = header.count();
		for (;;)
		{
			lengthinfo = QString("%1\n").arg(iLen).toLatin1();
			if ((header.count() + lengthinfo.count()) >= iLen) 
				break;
			++iLen;
		}
		header.append(lengthinfo);
		m_datfile.write(header);
	}
}


/*!
    \fn Mesydaq2::writeHeaderSeparator(void)

    write a header separator into a list mode file

    \see writeListfileHeader
    \see writeBlockSeparator
    \see writeClosingSignature
 */
void Mesydaq2::writeHeaderSeparator(void)
{
	m_datStream << sep0 << sep5 << sepA << sepF;
}


/*!
    \fn Mesydaq2::writeBlockSeparator(void)

    write a block separator into a list mode file
    
    \see writeListfileHeader
    \see writeHeaderSeparator
    \see writeClosingSignature
 */
void Mesydaq2::writeBlockSeparator(void)
{
	m_datStream << sep0 << sepF << sep5 << sepA;
}


/*!
    \fn Mesydaq2::writeClosingSignature(void)

    \see writeListfileHeader
    \see writeHeaderSeparator
    \see writeBlockSeparator
 */
void Mesydaq2::writeClosingSignature(void)
{
	m_datStream << sepF << sepA << sep5 << sep0;
}

/*!
    \fn Mesydaq2::width()

    the maximum number of bins over all modules

    \return number of bins
 */
quint16 Mesydaq2::width()
{
	quint16 bins(0);
	foreach(MCPD8 *value, m_mcpd) 
		if (value->bins() > bins)
			bins = value->bins();
	return bins;
}

quint16 Mesydaq2::height()
{
	QList<quint16> modList;
	quint16 n(0);
	foreach (MCPD8 *value, m_mcpd)
	{
		QList<quint16> tmpList = value->getHistogramList();
		foreach(quint16 h, tmpList)
			modList.append(value->getId() * 64 + h);
	}
	qSort(modList);
	if (!modList.isEmpty())
		n = modList.last() + 1;
	protocol(tr("Histogram height : %1").arg(n), INFO);
	return n;
}

/*!
    \fn Mesydaq2::isPulserOn()

    checks all MCPD's for a running pulser

    \return true if a running pulser found false otherwise
 */
bool Mesydaq2::isPulserOn()
{
	foreach(MCPD8 *value, m_mcpd) 
		if (value->isPulserOn())
			return true;
	return false;
}

/*!
    \fn Mesydaq2::isPulserOn(quint16 id)

    checks a MCPD with number id for a running pulser

    \param id number of the MCPD
    \return true if a running pulser found false otherwise
 */
bool Mesydaq2::isPulserOn(quint16 id)
{
	if (!m_mcpd.contains(id))
		return false;
	return m_mcpd[id]->isPulserOn();
}

/*!
    \fn Mesydaq2::isPulserOn(quint16 id, quint8 addr)

    checks a MPSD with number addr behind a MCPD with number id for a running pulser

    \param id number of the MCPD
    \param addr number of the MPSD behind the MCPD
    \return true if a running pulser found false otherwise
 */
bool Mesydaq2::isPulserOn(quint16 id, quint8 addr)
{
	if (!m_mcpd.contains(id))
		return false;
	return m_mcpd[id]->isPulserOn(addr);
}

/*!
    \fn Mesydaq2::scanPeriph(quint16 id)

    checks which MPSD's at the moment are connected to the MCPD

    \param id number of the MCPD
 */
void Mesydaq2::scanPeriph(quint16 id)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->readId();
}

/*!
    \fn Mesydaq2::saveSetup(const QString &name)

    Stores the setup in a file. This function stores INI files in format of
    "MesyDAQ" instead of "QMesyDAQ" using QSettings class (which is not easy
    human readable).

    Note: MesyDAQ INI file format is not used correctly, because the section
	  names are not unique: imagine you don't have a single MCPD-8 + MPSD-8 ...

    \param name file name
    \return true if successfully saved otherwise false
 */
bool Mesydaq2::saveSetup(const QString &name)
{
	if(name.isEmpty())
		m_configfile.setFile("mesycfg.mcfg");
	m_configfile.setFile(name);
  
	if(m_configfile.fileName().indexOf(".mcfg") == -1)
		m_configfile.setFile(m_configfile.absoluteFilePath().append(".mcfg"));

	m_lastConfiguration.EmptyFile(m_configfile.absoluteFilePath());
	saveSetup_helper(m_lastConfiguration,"MESYDAQ", -10, "comment", "QMesyDAQ configuration file");
	saveSetup_helper(m_lastConfiguration,"MESYDAQ", -10, "date", QDateTime::currentDateTime().toString(Qt::ISODate));
//	saveSetup_helper(m_lastConfiguration,"MESYDAQ", -10, "configPath", m_configPath);
	saveSetup_helper(m_lastConfiguration,"MESYDAQ", -10, "histogramPath", m_histPath);
	saveSetup_helper(m_lastConfiguration,"MESYDAQ", -10, "listfilePath", m_listPath);
	saveSetup_helper(m_lastConfiguration,"MESYDAQ", -10, "debugLevel", QString("%1").arg(DEBUGLEVEL));
	saveSetup_helper(m_lastConfiguration,"MESYDAQ", -10, "listmode", m_acquireListfile ? "true" : "false");
	m_lastConfiguration[m_lastConfiguration.FindSectionIndex("MESYDAQ")].AddComment(QString("QMesyDAQ configuration file, created %1").arg(QDateTime::currentDateTime().toString(Qt::ISODate)));

	int i = 0;
	foreach(MCPD8 *value, m_mcpd) 
	{
		CConfigSection mcpd8_section("MCPD", i);

		QString ip, dataip, cmdip;
		quint16 dataport, cmdport;
		value->getProtocol(ip, cmdip, cmdport, dataip, dataport);

		mcpd8_section.AddItem(CConfigItem("id", QString("%1").arg(i), 0));
		mcpd8_section.AddItem(CConfigItem("ipAddress", ip, 1));
		mcpd8_section.AddItem(CConfigItem("port", QString("%1").arg(cmdport), 2));
		if (!cmdip.isEmpty() && cmdip != "0.0.0.0")
		{
			mcpd8_section.AddItem(CConfigItem("cmdip", cmdip, 4));
			mcpd8_section.AddItem(CConfigItem("cmdport", QString("%1").arg(cmdport), 5));
		}
		if (!dataip.isEmpty() && dataip != "0.0.0.0")
		{
			mcpd8_section.AddItem(CConfigItem("dataip", dataip, 6));
			if (dataport != cmdport)
				mcpd8_section.AddItem(CConfigItem("dataport", QString("%1").arg(dataport), 7));
		}
		mcpd8_section.AddItem(CConfigItem("master", value->isMaster() ? "true" : "false", 8));
		mcpd8_section.AddItem(CConfigItem("terminate", value->isTerminated() ? "true" : "false", 9));
//		mcpd8_section.AddItem(CConfigItem("active", value->active() ? "true" : "false", 10));
//		mcpd8_section.AddItem(CConfigItem("histogram", value->histogram() ? "true" : "false", 11));

		for (int j =0; j < 4; ++j)
		{
			mcpd8_section.AddItem(CConfigItem(QString("auxtimer%1").arg(j), QString("%1").arg(value->getAuxTimer(j)), 200 + j));
			mcpd8_section.AddItem(CConfigItem(QString("paramsource%1").arg(j), QString("%1").arg(value->getParamSource(j)), 300 + j));
		}
		for (int j = 0; j < 8; ++j)
		{
			quint16 cells[2];
	    		value->getCounterCell(j, &cells[0]);
			CConfigItem item(QString("counterCell%1").arg(j), "", 100 + j);
			item.SetValueCount(2);
			item.SetLongValue(cells[0], CConfigItem::dec, 0);
			item.SetLongValue(cells[1], CConfigItem::dec, 1);
			mcpd8_section.AddItem(item);
		
//			if (value->getMpsdId(j))
			{
				CConfigSection mpsd_section("MODULE", 8 * (i + 1) + j);
				mpsd_section.AddItem(CConfigItem("id", QString("%1").arg( i * 8 + j), 0));
				for (int k = 0; k < 8; ++k)
				{
					mpsd_section.AddItem(CConfigItem(QString("gain%1").arg(k), QString("%1").arg(value->getGainPoti(j, k)), 10 + k));
					mpsd_section.AddItem(CConfigItem(QString("active%1").arg(k), value->active(j, k) ? "true" : "false", 30 + k));
					mpsd_section.AddItem(CConfigItem(QString("histogram%1").arg(k), value->histogram(j, k) ? "true" : "false", 40 + k));
				}
				mpsd_section.AddItem(CConfigItem("threshold", QString("%1").arg(value->getThreshold(j)), 20));
				m_lastConfiguration.AddSection(mpsd_section);
			}
		}
		m_lastConfiguration.AddSection(mcpd8_section);
		++i;
	}
	m_lastConfiguration.SaveFile();
	storeLastFile();
	return true;
}

/*!
 * \fn Mesydaq2::saveSetup_helper(CConfigFile& file, const QString& szSection, int iPriority, const QString& szItem, QString szValue)
 *
 * \brief store a single item into a section: generate non-existed sections; overwrite existing items or generate new ones
 * \param file reference to INI file in memory
 * \param szSection name of the section (existing or not)
 * \param iPriority priority for non-existing sections (possible change order of sections inside the INI file)
 * \param szItem name of the item (existing or not)
 * \param szValue value of the item
 */
void Mesydaq2::saveSetup_helper(CConfigFile& file, const QString& szSection, int iPriority, const QString& szItem, QString szValue)
{
	int i = file.FindSectionIndex(szSection);
	if (i < 0)
	{
		CConfigSection section(szSection,iPriority);
		i = file.AddSection(section);
	}

	CConfigSection &section = file[i];
	i = section.FindItemIndex(szItem);
	if (i < 0)
	{
		CConfigItem item(szItem, szValue);
		section.AddItem(item);
	}
	else
		section[i].SetValue(szValue);
}

void Mesydaq2::storeLastFile(void)
{
	QSettings settings("MesyTec", "QMesyDAQ");
	settings.setValue("lastconfigfile", getConfigfilename());
	settings.sync();
}

/*!
    \fn Mesydaq2::loadSetup(const QString &name)

    Loads the setup from a file. This function should be able to load
    "MesyDAQ" files and also "QMesyDAQ" files (using QSettings class which is
    not easy human readable).

    \note MesyDAQ INI file format is not used correctly, because the section
	  names are not unique: imagine you don't have a single MCPD-8 + MPSD-8 ...

    \param name file name
    \return true if successfully loaded otherwise false
 */
bool Mesydaq2::loadSetup(const QString &name)
{
	setConfigfilename(name);
	if (getConfigfilename().isEmpty())
		return false;

	CConfigSection *pSection=NULL;
	int 		i, nMcpd(0);
	QHash<int,int> 	hMCPDId2Pos, hMPSDId2Pos;
	QList<int>     	hMCPDPos2Id, hMPSDPos2Id;
	bool 		bQSettingsSpecial(false); // special INI format of QSettings class

	foreach (MCPD8* value, m_mcpd)
		delete value;
	m_mcpd.clear();

	protocol(tr("Reading configfile %1").arg(getConfigfilename()), NOTICE);

	m_lastConfiguration.LoadFile(getConfigfilename());

	pSection = &m_lastConfiguration[m_lastConfiguration.FindSectionIndex("MESYDAQ")];
	QString	home(getenv("HOME"));
//	m_configPath=loadSetup_helper(pSection,"configPath","/home");
	m_histPath = loadSetup_helper(pSection, "histogramPath", home);
	m_listPath = loadSetup_helper(pSection, "listfilePath", home);
	DEBUGLEVEL = loadSetup_helper(pSection, "debugLevel", QString("%1").arg(NOTICE)).toInt();
	do
	{
		QString sz = loadSetup_helper(pSection,"debugLevel",QString("%1").arg(NOTICE));
		bool bOK(false);
		i=sz.toInt(&bOK);
		if (bOK)
		  DEBUGLEVEL=i;
		else
		{
		  if (sz.contains("fatal",Qt::CaseInsensitive)) DEBUGLEVEL=FATAL;
		  else if (sz.contains("error",Qt::CaseInsensitive)) DEBUGLEVEL=ERROR;
		  else if (sz.contains("standard",Qt::CaseInsensitive) || sz.contains("warning",Qt::CaseInsensitive)) DEBUGLEVEL=WARNING;
		  else if (sz.contains("notice",Qt::CaseInsensitive)) DEBUGLEVEL=NOTICE;
		  else if (sz.contains("details",Qt::CaseInsensitive) || sz.contains("info",Qt::CaseInsensitive)) DEBUGLEVEL=INFO;
		  else if (sz.contains("debug",Qt::CaseInsensitive) || sz.contains("debug",Qt::CaseInsensitive)) DEBUGLEVEL=DEBUG;
		}
		m_acquireListfile = loadSetupBoolean(pSection, "listmode", true);
	} while (0);

	for (i = 0; i < m_lastConfiguration.GetSectionCount(); ++i)
	{
		pSection = &m_lastConfiguration[i];
		if (pSection->GetName().startsWith("MCPD"))
		{
			int iId = loadSetup_helper(pSection, "id", "-1").toInt();
			if (iId < 0 && nMcpd == 0 && pSection->FindItemIndex("0\\id") >= 0 && pSection->FindItemIndex("number") >= 0)
				bQSettingsSpecial = true;
			else if (iId < 0 && !bQSettingsSpecial)
			{
				qCritical(tr("found no or invalid MCPD id").toStdString().c_str());
				continue;
			}
			hMCPDId2Pos[iId] = i;
			hMCPDPos2Id.append(iId);
			++nMcpd;
		}
		if (pSection->GetName().startsWith("MPSD") || pSection->GetName() == "MODULE")
		{
			int iId = loadSetup_helper(pSection, "id", "-1").toInt();
			if (iId < 0 && !bQSettingsSpecial)
			{
				qCritical(tr("found no or invalid Module id").toStdString().c_str());
				continue;
			}
			hMPSDId2Pos[iId] = i;
			hMPSDPos2Id.append(iId);
		}
	}
	if (bQSettingsSpecial)
	{
		bQSettingsSpecial = false;
		pSection = &m_lastConfiguration[hMCPDId2Pos[hMCPDPos2Id[0]]];
		i = pSection->FindItemIndex("number");
		if (i >= 0)
		{
			long l = 0;
			if (pSection->GetItem(i).GetLongValue(l,0) != CConfigItem::ILLEGALVALUE && l > 0)
      			{
				bQSettingsSpecial = true;
				nMcpd = l;
			}
		}
  	}
	for (i = 0; i < nMcpd; ++i)
	{
		QString szPrefix;
		int iMCPDId;
		if (bQSettingsSpecial)
		{
			iMCPDId = i;
			szPrefix = QString("%1\\").arg(i);
		}
		else
		{
			iMCPDId = hMCPDPos2Id[i];
			pSection = &m_lastConfiguration[hMCPDId2Pos[iMCPDId]];
			szPrefix.clear();
		}

		QString IP = loadSetup_helper(pSection, QString("%1%2").arg(szPrefix).arg(bQSettingsSpecial ? "ip" : "ipAddress"), "192.168.168.121");
		quint16 port = loadSetup_helper(pSection, QString("%1port").arg(szPrefix), "54321").toUInt();
		QString cmdIP = loadSetup_helper(pSection, QString("%1cmdip").arg(szPrefix), "0.0.0.0");
		quint16 cmdPort = loadSetup_helper(pSection, QString("%1cmdport").arg(szPrefix), "0").toUInt();
		QString dataIP = loadSetup_helper(pSection, QString("%1dataip").arg(szPrefix), "0.0.0.0");
		quint16 dataPort = loadSetup_helper(pSection, QString("%1datatport").arg(szPrefix), "0").toUInt();

		do
		{
			QHostAddress cmd(cmdIP);
			if (cmd == QHostAddress::Any || cmd == QHostAddress::AnyIPv6)
			{
				cmdIP = "0.0.0.0";
				cmdPort = 0;
			}
			if (port == cmdPort)
				cmdPort = 0;
		} while (0);
		do
		{
			QHostAddress data(dataIP);
			if (data == QHostAddress::Any || data == QHostAddress::AnyIPv6)
			{
				dataIP = "0.0.0.0";
				dataPort = 0;
			}
			if (port == dataPort)
				dataPort = 0;
		} while (0);

		addMCPD(iMCPDId, IP, port > 0 ? port : cmdPort, cmdIP);
		
//		m_mcpd[iMCPDId]->setActive(loadSetupBoolean(pSection, szPrefix + "active", true));
//		m_mcpd[iMCPDId]->setHistogram(loadSetupBoolean(pSection, szPrefix + "histogram", true));
		for (int j = 0; j < 4; ++j)
		{
			setAuxTimer(iMCPDId, j, loadSetup_helper(pSection, QString("%1auxtimer%2").arg(szPrefix).arg(j), "0").toUInt());
			setParamSource(iMCPDId, j, loadSetup_helper(pSection, QString("%1paramsource%2").arg(szPrefix).arg(j), QString("%1").arg(j)).toUInt());
		}
		for (int j = 0; j < 8; ++j)
		{
			long cells[2] = {7, 22};
			int k = pSection->FindItemIndex(QString("countercell%1").arg(j));
			if (k >= 0)
			{
				CConfigItem &item = pSection->GetItem(k);
				if (item.GetLongValue(&cells[0], 0) != CConfigItem::SUCCESS) 
					cells[0] = 7;
				if (item.GetLongValue(&cells[1], 1) != CConfigItem::SUCCESS) 
					cells[1] = 22;
      			}
			setCounterCell(iMCPDId, j, cells[0], cells[1]);
		}
		
		setTimingSetup(iMCPDId, loadSetupBoolean(pSection, szPrefix + "master", true), loadSetupBoolean(pSection, szPrefix + "terminate", true));
		
		for (int j = 0; j < 8; ++j)
		{
			if (getMpsdId(iMCPDId, j))
			{
				CConfigSection *pMPSD;
				quint8 	gains[8],
					threshold;
				bool 	comgain(true);
				int 	iMPSDId(i * 8 + j);

				if (bQSettingsSpecial)
				{
					pMPSD = &m_lastConfiguration[hMPSDId2Pos.begin().value()];
					for (int k = 0; k < 8; ++k)
						gains[k] = loadSetup_helper(pMPSD, QString("%1\\gains\\%2\\gain").arg(iMPSDId).arg(k), "92").toUInt();
					threshold = loadSetup_helper(pMPSD, QString("%1threshold").arg(iMPSDId), "22").toUInt();
				}
				else
				{
					pMPSD = &m_lastConfiguration[hMPSDId2Pos[iMPSDId]];
					for (int k = 0; k < 8; ++k)
					{
						gains[k] = loadSetup_helper(pMPSD, QString("gain%1").arg(k), "92").toUInt();
						m_mcpd[iMCPDId]->setActive(j, k, loadSetupBoolean(pMPSD, QString("active%1").arg(k), true));
						m_mcpd[iMCPDId]->setHistogram(j, k, loadSetupBoolean(pMPSD, QString("histogram%1").arg(k), true));
					}
					threshold = loadSetup_helper(pMPSD, "threshold", "22").toUInt();
				}
				for (int k = 0; k < 8; ++k)
					if (gains[0] != gains[k])
	  				{
	    					comgain = false;
						break;
					}
				if (comgain)
					setGain(iMCPDId, j, 8, gains[0]);
				else
					for (int k = 0; k < 8; ++k)
						setGain(iMCPDId, j, k, gains[k]);
				setThreshold(iMCPDId, j, threshold);
			}
		}
		setProtocol(iMCPDId, QString("0.0.0.0"), dataIP, dataPort, cmdIP, cmdPort);
	}

// scan connected MCPDs
	quint16 p = 0;
	foreach(MCPD8 *value, m_mcpd)
		p += value->numModules();

	protocol(tr("%1 MCPD-8 and %2 Modules found").arg(nMcpd).arg(p),NOTICE);
	storeLastFile();
	return true;
}


/*!
 * \fn QString Mesydaq2::loadSetup_helper(CConfigSection* pSection, const QString& szItem, const QString& szDefault)
 *
 * \brief read a single item from a section of a INI file in memory
 * \param pSection section of the INI file in memory
 * \param szDefault default value, if items does not exist
 * \return value of the item or default (on errors)
 */
QString Mesydaq2::loadSetup_helper(CConfigSection* pSection, const QString& szItem, const QString& szDefault)
{
	int i = pSection->FindItemIndex(szItem);
	QString szResult;
	if (i < 0)
		return szDefault;
	if (!pSection->GetItem(i).GetValue(szResult))
		return szDefault;
	return szResult;
}

/*! 
 * \fn bool Mesydaq2::loadSetupBoolean(CConfigSection* pSection, const QString& szItem, const bool)
 *
 * \brief read a boolean item from a setction of an INI file in memory
 * 
 * \param pSection section of the INI file in memory
 * \param szDefault default value, if items does not exist
 * \return boolean value of the item or the default (if not found)
 */
bool Mesydaq2::loadSetupBoolean(CConfigSection* pSection, const QString& szItem, const bool bDefault)
{
	bool result(bDefault);

	QString sz = loadSetup_helper(pSection, szItem, bDefault ? "1" : "0");
	bool bOK(false);
	result = (sz.toInt(&bOK) != 0);
	if (!bOK) 
		result = !sz.contains("false", Qt::CaseInsensitive)  && !sz.contains("no", Qt::CaseInsensitive);
	return result;
}

/*!
    \fn Mesydaq2::timerEvent(QTimerEvent *event)

    callback for the timer
 */
void Mesydaq2::timerEvent(QTimerEvent * /* event */)
{
#warning TODO if(cInt->caressTaskPending() && (!cInt->asyncTaskPending()))
#warning TODO 		cInt->caressTask();
//! \todo CARESS binding and checks for the hardware
#if 0
	if (event->timerId() == m_checkTimer)	
		checkMcpd(0);
#endif
}

/*!
    \fn Mesydaq2::initMcpd(quint8 id)

    initializes a MCPD

    \todo is this function really needed

    \param id number of the MCPD
 */
void Mesydaq2::initMcpd(quint8 id)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->init();
}


/*!
    \fn Mesydaq2::allPulserOff()

    switches all pulsers off
 */
void Mesydaq2::allPulserOff()
{
// send pulser off to all connected MPSD
	foreach(MCPD8 *value, m_mcpd)
		for(quint8 j = 0; j < 8; j++)
			if(value->getMpsdId(j))
				value->setPulser(j, 0, 2, 40, 0);
}

/*!
    \fn Mesydaq2::setTimingwidth(quint8 width)

    defines a timing width ????
 
    \todo not really implemented

    \param width ????
 */
void Mesydaq2::setTimingwidth(quint8 width)
{
	m_timingwidth = width;
	if(width > 48)
    		m_timingwidth = 48;
#warning TODO
//! \todo set the timing width of the histogram
#if 0
	if (m_hist)
   		m_hist->setWidth(m_timingwidth); 
#endif
}


/*!
    \fn Mesydaq2::readPeriReg(quint16 id, quint16 mod, quint16 reg)

    reads the content of a register in a module

    \param id number of the MCPD
    \param mod number of the module
    \param reg number of the register
    \return content of the register
    \see writePeriReg
 
 */
quint16 Mesydaq2::readPeriReg(quint16 id, quint16 mod, quint16 reg)
{
	if (m_mcpd.empty())
		return 0;
	return m_mcpd[id]->readPeriReg(mod, reg);
}

/*!
    \fn Mesydaq2::writePeriReg(quint16 id, quint16 mod, quint16 reg, quint16 val)

    writes a value into a module register

    \param id number of the MCPD
    \param mod number of the module
    \param reg number of the register
    \param val new value
    \see readPeriReg
 */
void Mesydaq2::writePeriReg(quint16 id, quint16 mod, quint16 reg, quint16 val)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->writePeriReg(mod, reg, val);
}

// command shortcuts for simple operations:
/*!
    \fn Mesydaq2::start(void)

    starts a data acquisition
 */
void Mesydaq2::start(void)
{
   	protocol("remote start", NOTICE);
	foreach(MCPD8 *it, m_mcpd)
		if (!it->isMaster())
			it->start();
	foreach(MCPD8 *it, m_mcpd)
		if (it->isMaster())
			it->start();
	emit statusChanged("STARTED");
}

/*!
    \fn Mesydaq2::stop(void)

    stops a data acquisition
 */
void Mesydaq2::stop(void)
{
   	protocol("remote stop", NOTICE);
	foreach(MCPD8 *it, m_mcpd)
		if (it->isMaster())
			it->stop();
	foreach(MCPD8 *it, m_mcpd)
		if (!it->isMaster())
			it->stop();
	emit statusChanged("STOPPED");
}

/*!
    \fn Mesydaq2::cont(void)

    continues a data acquisition
 */
void Mesydaq2::cont(void)
{
	protocol("remote cont", NOTICE);
	foreach(MCPD8 *it, m_mcpd)
		if (!it->isMaster())
			it->cont();
	foreach(MCPD8 *it, m_mcpd)
		if (it->isMaster())
			it->cont();
	emit statusChanged("STARTED");
}

/*!
    \fn Mesydaq2::reset(void)

    resets the MCPD's

    \todo implement me
 */
void Mesydaq2::reset(void)
{
}

/*!
    \fn Mesydaq2::checkMcpd(quint8 device)
    
    rescan all MCPD's for the connected modules

    \return true if successfully finished otherwise false
 */
bool Mesydaq2::checkMcpd(quint8 /* device */)
{
	quint8 c(0);
	foreach(MCPD8 *value, m_mcpd)
		scanPeriph(c++);    	
	return true;
}

/*!
    \fn Mesydaq2::setProtocol(const quint16 id, const QString &mcpdIP, const QString &dataIP, const quint16 dataPort, const QString &cmdIP, const quint16 cmdPort)

    configures a MCPD for the communication it will set the IP address of the module, the IP address and ports of the data and command sink

    \param id number of the MCPD
    \param mcpdIP new IP address of the module
    \param dataIP IP address to which data packets should be send (if 0.0.0.0 the sender will be receive them)
    \param dataPort port number for data packets (if 0 the port number won't be changed)
    \param cmdIP IP address to which cmd answer packets should be send (if 0.0.0.0 the sender will be receive them)
    \param cmdPort port number for cmd answer packets (if 0 the port number won't be changed)
    \see getProtocol
 */
void Mesydaq2::setProtocol(const quint16 id, const QString &mcpdIP, const QString &dataIP, quint16 dataPort, const QString &cmdIP, quint16 cmdPort)
{
	m_mcpd[id]->setProtocol(mcpdIP, dataIP, dataPort, cmdIP, cmdPort);
}	

/*!
    \fn Mesydaq2::getMode(const quint16 id, quint8 addr)

    get the mode: amplitude or position

    \param id number of the MCPD
    \param addr module number
    \see setMode
 */
bool Mesydaq2::getMode(const quint16 id, quint8 addr)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getMode(addr);
	return false;
}

/*!
    \fn Mesydaq2::setMode(const quint16 id, quint8 addr, bool mode)

    set the mode to amplitude or position

    \param id number of the MCPD
    \param addr number of the module
    \param mode if true amplitude mode otherwise position mode
    \see getMode
*/
void Mesydaq2::setMode(const quint16 id, quint8 addr, bool mode)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setMode(addr, mode);
}

/*!
    \fn Mesydaq2::setPulser(const quint16 id, quint8 addr, quint8 chan, quint8 pos, quint8 amp, bool onoff)

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
void Mesydaq2::setPulser(const quint16 id, quint8 addr, quint8 channel, quint8 position, quint8 amp, bool onoff)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setPulser(addr, channel, position, amp, onoff);
}

/*!
    \fn Mesydaq2::setCounterCell(quint16 id, quint16 source, quint16 trigger, quint16 compare)

    map the counter cell

    \param id number of the MCPD
    \param source source of the counter
    \param trigger trigger level
    \param compare ????
    \see getCounterCell
 */
void Mesydaq2::setCounterCell(quint16 id, quint16 source, quint16 trigger, quint16 compare)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setCounterCell(source, trigger, compare);
}

/*!
    \fn Mesydaq2::setParamSource(quint16 id, quint16 param, quint16 source)

    set the source of a parameter

    \param id number of the MCPD
    \param param number of the parameter
    \param source number of source
    \see getParamSource
 */
void Mesydaq2::setParamSource(quint16 id, quint16 param, quint16 source)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setParamSource(param, source);
}

/*!
    \fn Mesydaq2::setAuxTimer(quint16 id, quint16 tim, quint16 val)

    sets the auxiliary timer to a new value

    \param id number of the MCPD
    \param tim number of the timer
    \param val new timer value
    \see getAuxTimer
 */
void Mesydaq2::setAuxTimer(quint16 id, quint16 tim, quint16 val)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setAuxTimer(tim, val);
}

/*!
    \fn Mesydaq2::setMasterClock(quint16 id, quint64 val)

    sets the master clock to a new value

    \todo check for the masters

    \param id number of the MCPD
    \param val new clock value
 */
void Mesydaq2::setMasterClock(quint16 id, quint64 val)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setMasterClock(val);
}

/*!
    \fn Mesydaq2::setTimingSetup(quint16 id, bool master, bool term)

    sets the communication parameters between the MCPD's

    \param id number of the MCPD
    \param master is this MCPD master or not
    \param term should the MCPD synchronization bus terminated or not
 */
void Mesydaq2::setTimingSetup(quint16 id, bool master, bool term)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setTimingSetup(master, term);
}

/*!
    \fn Mesydaq2::setId(quint16 id, quint8 mcpdid)

    sets the id of the MCPD

    \param id number of the MCPD
    \param mcpdid the new ID of the MCPD
    \see getId
 */
void Mesydaq2::setId(quint16 id, quint8 mcpdid)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setId(mcpdid);
}

/*!
    \fn Mesydaq2::setGain(quint16 id, quint8 addr, quint8 chan, quint8 gainval)

    sets the gain to a poti value

    \param id number of the MCPD
    \param addr number of the module
    \param chan channel number of the module
    \param gainval poti value of the gain
    \see getGain
 */
void Mesydaq2::setGain(quint16 id, quint8 addr, quint8 channel, quint8 gain)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setGain(addr, channel, gain);
}

/*!
    \overload Mesydaq2::setGain(quint16 id, quint8 addr, quint8 chan, float gainval)

    the gain value will be set as a user value

    \param id number of the MCPD
    \param addr number of the module
    \param chan channel number of the module
    \param gainval user value of the gain
    \see getGain
 */
void Mesydaq2::setGain(quint16 id, quint8 addr, quint8 chan, float gain)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setGain(addr, chan, gain);
}

/*!
    \fn Mesydaq2::setThreshold(quint16 id, quint8 addr, quint8 thresh)

    set the threshold value as poti value

    \param id number of the MCPD
    \param addr number of the module
    \param thresh threshold value as poti value
    \see getThresh
 */
void Mesydaq2::setThreshold(quint16 id, quint8 addr, quint8 thresh)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setThreshold(addr, thresh);
}

/*!
    \overload Mesydaq2::setThreshold(quint16 id, quint8 addr, quint16 thresh)

    set the threshold value 

    \param id number of the MCPD
    \param addr number of the module
    \param thresh threshold value 
    \see getThresh
*/
void Mesydaq2::setThreshold(quint16 id, quint8 addr, quint16 thresh)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->setThreshold(addr, thresh);
}

/*!
    \fn quint8 Mesydaq2::getMpsdId(quint16 id, quint8 addr)

    get the detected ID of the MPSD. If MPSD not exists it will return 0.

    \param id number of the MCPD
    \param addr module number
    \return module ID (type)
    \see readId
 */
quint8 Mesydaq2::getMpsdId(quint16 id, quint8 addr)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getMpsdId(addr);
	return 0;
}

/*!
    \fn bool Mesydaq2::online(quint16 id, quint8 addr)

    returns whether the module is detected and online or not

    \param id number of the MCPD
    \param addr module number
    \return true or false
 */
bool Mesydaq2::online(quint16 id, quint8 addr)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->online(addr);
	return false;
}

/*!
    \fn Mesydaq2::getMpsdType(quint16 id, quint8 addr)

    get the detected ID of the MPSD. If MPSD not exists it will return 0.
    the value is a human readable value.

    \param id number of the MCPD
    \param addr module number
    \return module ID (type)
    \see readId
 */
QString Mesydaq2::getMpsdType(quint16 id, quint8 addr)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getMpsdType(addr);
	return "-";
}

/*!
    \fn float Mesydaq2::getMpsdVersion(quint16 id, quint8 addr)

    get the detected version of the MPSD. If MPSD not exists it will return 0.

    \param id number of the MCPD
    \param addr module number
    \return module ID (type)
    \see readId
*/
float Mesydaq2::getMpsdVersion(quint16 id, quint8 addr)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->version(addr);
	return 0.0;
}

/*!
    \fn Mesydaq2::getPulsChan(quint16 id, quint8 addr)

    gets the current set channel of the pulser of a module

    \param id number of the MCPD
    \param addr module number
    \return the pulser channel
    \see setPulser
    \see getPulsAmp
    \see getPulsPos
 */
quint8 Mesydaq2::getPulsChan(quint16 id, quint8 addr)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getPulsChan(addr);
	return 0;
}

/*!
    \fn Mesydaq2::getPulsAmp(quint16 id, quint8 addr)

    gets the current set pulser amplitude of a module

    \param id number of the MCPD
    \param addr module number
    \return the pulser amplitude
    \see setPulser
    \see getPulsPos
    \see getPulsChan
 */
quint8 Mesydaq2::getPulsAmp(quint16 id, quint8 addr)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getPulsAmp(addr);
	return 0;
}

/*!
    \fn Mesydaq2::getPulsPos(quint16 id, quint8 addr)
    gets the current set pulser position of a module

    \param id number of the MCPD
    \param addr module number
    \return the pulser position
    \see setPulser
    \see getPulsAmp
    \see getPulsChan
 */
quint8 Mesydaq2::getPulsPos(quint16 id, quint8 addr)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getPulsPos(addr);
	return 0;
}

/*!
    \fn Mesydaq2::getCounterCell(quint16 id, quint8 cell, quint16 *celldata)

    celldata[0] = trig, celldata[1] = comp

    \param id number of the MCPD
    \param cell cell number
    \param celldata return data
    \see setCounterCell
 */
void Mesydaq2::getCounterCell(quint16 id, quint8 cell, quint16 *celldata)
{
	if (m_mcpd.contains(id))
		m_mcpd[id]->getCounterCell(cell, celldata);
}

/*!
    \fn Mesydaq2::getParamSource(quint16 id, quint16 param)

    get the source of parameter param

    \param id number of the MCPD
    \param param the parameter number
    \return source of the parameter
    \see setParamSource
 */
quint16 Mesydaq2::getParamSource(quint16 id, quint16 param)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getParamSource(param);
	return 0;
}
	
/*!
    \fn Mesydaq2::getAuxTimer(quint16 id, quint16 timer)

    get the value of auxiliary counter

    \param id number of the MCPD
    \param timer number of the timer
    \return counter value
    \see setAuxTimer
 */
quint16 Mesydaq2::getAuxTimer(quint16 id, quint16 timer)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getAuxTimer(timer);
	return 0;
}

/*!
    \fn Mesydaq2::getParameter(quint16 id, quint16 param)

    gets the value of the parameter number param

    \param id number of the MCPD
    \param param parameter number
    \return parameter value
    \see setParameter
 */
quint64 Mesydaq2::getParameter(quint16 id, quint16 param)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getParameter(param);
	return 0;
}

/*!
    \fn Mesydaq2::getGain(quint16 id, quint8 addr,  quint8 chan)

    gets the currently set gain value for a special module and channel

    if the channel number is greater 7 than all channels of the module
    will be set

    \param id number of the MCPD
    \param addr number of the module
    \param chan number of the channel of the module
    \return poti value of the gain
    \see setGain
 */
float Mesydaq2::getGain(quint16 id, quint8 addr, quint8 chan)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getGainVal(addr, chan);
	qDebug() << "getGain not found id " << id;
	return 0;
}

/*!
    \fn Mesydaq2::getThreshold(quint16 id, quint8 addr)

    get the threshold value as poti value

    \param id number of the MCPD
    \param addr module number
    \return the threshold as poti value
    \see setThreshold
 */
quint8 Mesydaq2::getThreshold(quint16 id, quint8 addr)
{
	if (m_mcpd.contains(id))
		return m_mcpd[id]->getThreshold(addr);
	return 0;
}

/*!
    \fn Mesydaq2::setRunId(quint16 runid)

    sets the run ID of the measurement

    \param runid the new run ID
    \return true if operation was succesful or not
    \see getRunId
 */
void Mesydaq2::setRunId(quint16 runid)
{
	foreach (MCPD8* value, m_mcpd)
		if (value->isMaster())
			value->setRunId(runid); 
	m_runID = runid;
	protocol(tr("Mesydaq2::setRunId(%1)").arg(m_runID), NOTICE);
}

void Mesydaq2::setHistfilename(QString name) 
{
	m_histfilename = name;
	if(m_histfilename.indexOf(".mtxt") == -1)
		m_histfilename.append(".mtxt");
}
     
/*!
    \fn Mesydaq2::analyzeBuffer(DATA_PACKET &pd)

    callback to analyze input data packet

    \param pd data packet
 */
void Mesydaq2::analyzeBuffer(DATA_PACKET &pd)
{
	protocol(tr("Mesydaq2::analyzeBuffer(): %1").arg(m_daq), DEBUG);
	if (m_daq == RUNNING)
	{
		quint16 mod = pd.deviceId;
		m_runID = pd.runID;
 		quint32 datalen = (pd.bufferLength - pd.headerLength) / 3;
		for(quint32 i = 0, counter = 0; i < datalen; ++i, counter += 3)
		{
			if(!(pd.data[counter + 2] & TRIGGEREVENTTYPE))
			{
				quint8 slotId = (pd.data[counter + 2] >> 7) & 0x1F;
				quint8 id = (pd.data[counter + 2] >> 12) & 0x7;
				if (getMpsdId(mod, slotId) == TYPE_MPSD8 && getMode(mod, id)) // amplitude mode
				{
					// put the amplitude to the new format position
					quint16 amp = (pd.data[counter + 1] >> 3) & 0x3FF;
					pd.data[counter + 2] &= 0xFF80;	// clear amp and pos field
					pd.data[counter + 1] &= 0x0007;	
					pd.data[counter + 2] |= (amp >> 3);
					pd.data[counter + 1] |= ((amp & 0x7) << 13);
				}
			}
		}
		if(m_acquireListfile)
		{
			quint16 *pD = (quint16 *)&pd;
			if (pd.bufferLength == 0)
			{
				protocol(tr("BUFFER with length 0"), ERROR);
				return;
			}
#if 0
			if (pd.bufferLength == 21)
				return;
#endif
			if (pd.bufferLength > sizeof(DATA_PACKET) / 2)
			{
				protocol(tr("BUFFER with length ").arg(pd.bufferLength), ERROR);
				return;
			}
			m_datStream << pd.bufferLength;
			m_datStream << pd.bufferType;
			m_datStream << pd.headerLength;
			m_datStream << pd.bufferNumber;
			for(quint16 i = 4; i < pd.bufferLength; i++)
				m_datStream << pD[i];
			writeBlockSeparator();
//			qDebug("------------------");
		}
		protocol(tr("buffer : length : %1 type : %2").arg(pd.bufferLength).arg(pd.bufferType), DEBUG);
		if(pd.bufferType < 0x0002) 
		{
// extract parameter values:
			for(quint8 i = 0; i < 4; i++)
			{
				quint64 var = 0;
				for(quint8 j = 0; j < 3; j++)
				{
					var <<= 16;
					var |= pd.param[i][2 - j];
				}
				m_mcpd[mod]->setParameter(i, var);
			}
			emit analyzeDataBuffer(pd);
		}
	}
}

void Mesydaq2::setConfigfilename(const QString &name)
{
	if (name.isEmpty())
		m_configfile.setFile("mesycfg.mcfg");
	else
		m_configfile.setFile(name);
}

QString Mesydaq2::getConfigfilename(void) 
{
	if (m_configfile.exists())
		return m_configfile.absoluteFilePath();
	else
		return QString("");
}

/*!
    \fn bool Mesydaq2::active(quint16 mid, quint16 id, quint16 chan)
    
    \param mid MCPD id
    \param id MPSD id
    \param chan channel id
    \return true or false
 */
bool Mesydaq2::active(quint16 mid, quint16 id, quint16 chan)
{
	if (m_mcpd.contains(mid))
		return m_mcpd[mid]->active(id, chan);
	return false;
}

/*!
    \fn bool Mesydaq2::active(quint16 mid, quint16 id)
    
    \param mid MCPD id
    \param id MPSD id
    \return true or false
 */
bool Mesydaq2::active(quint16 mid, quint16 id)
{
	if (m_mcpd.contains(mid))
		return m_mcpd[mid]->active(id);
	return false;
}

/*!
    \fn bool Mesydaq2::histogram(quint16, quint16, quint16 chan)
    \param mid MCPD id
    \param id MPSD id
    \param chan channel id
    \return true or false
 */
bool Mesydaq2::histogram(quint16 mid, quint16 id, quint16 chan)
{
	if (m_mcpd.contains(mid))
		return m_mcpd[mid]->histogram(id, chan);
	return false;
}

/*!
    \fn bool Mesydaq2::histogram(quint16, quint16)
    \param mid MCPD mid
    \param id MPSD id
    \return true or false
 */
bool Mesydaq2::histogram(quint16 mid, quint16 id)
{
	if (m_mcpd.contains(mid))
		return m_mcpd[mid]->histogram(id);
	return false;
}

/*!
    \fn void Mesydaq2::setActive(quint16 mcpd, quint16 mpsd, bool set)

    \param mcpd
    \param mpsd
    \param set
 */
void Mesydaq2::setActive(quint16 mcpd, quint16 mpsd, bool set)
{
	if (m_mcpd.contains(mcpd))
		m_mcpd[mcpd]->setActive(mpsd, set);
}

/*!
    \fn void Mesydaq2::setHistogram(quint16 mcpd, quint16 mpsd, bool set)

    \param mcpd
    \param mpsd
    \param set
 */
void Mesydaq2::setHistogram(quint16 mcpd, quint16 mpsd, bool set)
{
	if (m_mcpd.contains(mcpd))
		m_mcpd[mcpd]->setHistogram(mpsd, set);
}
