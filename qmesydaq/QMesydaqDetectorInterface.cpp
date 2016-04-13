/***************************************************************************
 *   Copyright (C) 2002 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2008-2014 by Lutz Rossa <rossa@hmi.de>                  *
 *   Copyright (C) 2009-2015 by Jens Krüger <jens.krueger@frm2.tum.de>     *
 *   Copyright (C) 2010 by Alexander Lenz <alexander.lenz@frm2.tum.de>     *
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
#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include "QMesydaqDetectorInterface.h"
#include "CommandEvent.h"
#include "LoopObject.h"
#include "mapcorrect.h"
#include "mappedhistogram.h"
#include "logging.h"

/*!
    constructor

    \param receiver
    \param parent
 */
QMesyDAQDetectorInterface::QMesyDAQDetectorInterface(QObject *receiver, QObject *parent)
	: QtInterface(receiver, parent)
	, m_bDoLoop(true)
	, m_preSelection(0.0)
	, m_counter(0.0)
	, m_width(0)
	, m_height(0)
	, m_pObject(NULL)
	, m_status(0)
	, m_boolean(false)
	, m_bWriteProtectFiles(false) // write protect data files
{
}

bool QMesyDAQDetectorInterface::doLoop() const
{
	return m_bDoLoop;
}

/*!
    emits the start to the interface
 */
void QMesyDAQDetectorInterface::start()
{
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_START);
	m_mutex.unlock();
}

/*!
    emits the stop to the interface
 */
void QMesyDAQDetectorInterface::stop()
{
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_STOP);
	m_mutex.unlock();
}

/*!
    emits the clear to the interface
 */
void QMesyDAQDetectorInterface::clear()
{
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_CLEAR);
	m_mutex.unlock();
}

/*!
    emits the resume to the interface
 */
void QMesyDAQDetectorInterface::resume()
{
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_RESUME);
	m_mutex.unlock();
}

/*!
    initiate a read of the counter and return it

    \param id counter number

    \return counter value
 */
double QMesyDAQDetectorInterface::readCounter(int id)
{
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_READ_COUNTER, QList<QVariant>() << id);
	double r = m_counter;
	m_mutex.unlock();
	return r;
}

/*!
    selects a counter for setting of preselection

    \param id counter number
    \param bEnable en-/disable counter
 */
void QMesyDAQDetectorInterface::selectCounter(int id, bool bEnable)
{
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_SELECT_COUNTER, QList<QVariant>() << id << bEnable);
	m_mutex.unlock();
}

bool QMesyDAQDetectorInterface::counterSelected(int id)
{
	bool r(false);
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_COUNTER_SELECTED, QList<QVariant>() << id);
	r = m_counter;
	m_mutex.unlock();
	return r;
}

/*!
    selects a counter for setting of preselection

    \param id counter number
    \param bEnable en-/disable counter
    \param dblTarget preselection value
 */
void QMesyDAQDetectorInterface::selectCounter(int id, bool bEnable, double dblTarget)
{
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_SELECT_COUNTER, QList<QVariant>() << id << bEnable << dblTarget);
	m_mutex.unlock();
}

/*!
    sets the preselection of the selected counter

    \param value preselection
 */
void QMesyDAQDetectorInterface::setPreSelection(double value)
{
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_SET_PRESELECTION, QList<QVariant>() << value);
	m_mutex.unlock();
}

/*!
    sets the preselection of the counter id

    \param id id of the counter
    \param value preselection
 */
void QMesyDAQDetectorInterface::setPreSelection(int id, double value)
{
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_SET_PRESELECTION, QList<QVariant>() << value << id);
	m_mutex.unlock();
}

/*!
    returns the preselection

    \return the preselection value of the preset counter
 */
double QMesyDAQDetectorInterface::preSelection()
{
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_PRESELECTION);
	double r = m_preSelection;
	m_mutex.unlock();
	return r;
}

/*!
    returns the preselection of the selected counter

    \param id counter id
    \return the preselection value of the preset counter
 */
double QMesyDAQDetectorInterface::preSelection(int id)
{
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_PRESELECTION, QList<QVariant>() << id);
	double r = m_preSelection;
	m_mutex.unlock();
	return r;
}

/*!
     return the size of the histogram

     \param width
     \param height
 */
QSize QMesyDAQDetectorInterface::readHistogramSize(int histo)
{
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_READ_HISTOGRAM_SIZE, QList<QVariant>() << histo);
	QSize s(m_width, m_height);
	m_mutex.unlock();
	return s;
}

/*!
    \return the histogram
 */
QList<quint64> QMesyDAQDetectorInterface::readHistogram(int histo)
{
	QList<quint64> r;
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_READ_HISTOGRAM, QList<QVariant>() << histo);
	r = m_values;
	m_mutex.unlock();
	return r;
}

/*!
    \return the 'diffractogram' as 64bit values
 */
QList<quint64> QMesyDAQDetectorInterface::readDiffractogram()
{
	QList<quint64> r;
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_READ_DIFFRACTOGRAM);
	r = m_values;
	m_mutex.unlock();
	return r;
}

/*!
    \param iSpectrogram number of tube
    \return a spectrogram of a tube
 */
QList<quint64> QMesyDAQDetectorInterface::readSpectrogram(int iSpectrogram/*=-1*/)
{
	QList<quint64> r;
	m_mutex.lock();
	if (iSpectrogram >= 0)
		postRequestCommand(CommandEvent::C_READ_SPECTROGRAM, QList<QVariant>() << iSpectrogram);
	else
		postRequestCommand(CommandEvent::C_READ_SPECTROGRAM);
	r = m_values;
	m_mutex.unlock();
	return r;
}

/*!
    \return the current status of the detector
 */
int QMesyDAQDetectorInterface::status(bool* pbRunAck /*= NULL*/)
{
	int r(0);
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_STATUS);
	r = m_status;
	if (pbRunAck != NULL)
		*pbRunAck = m_boolean;
	m_mutex.unlock();
	return r;
}

/*!
    \return the correction map
 */
const MapCorrection* QMesyDAQDetectorInterface::getMappingCorrection()
{
	MapCorrection* pResult(NULL);
	m_mutex.lock();
	m_pObject = NULL;
	postRequestCommand(CommandEvent::C_MAPCORRECTION);
	pResult = dynamic_cast<MapCorrection*>(m_pObject);
	m_mutex.unlock();
	return pResult;
}

/*!
    sets the correction map
    \param map
 */
void QMesyDAQDetectorInterface::setMappingCorrection(const MapCorrection& map)
{
	if (!map.isValid())
		return;
	MapCorrection *pMap(NULL);
	m_mutex.lock();
	m_pObject = NULL;
	postRequestCommand(CommandEvent::C_MAPCORRECTION);
	pMap = dynamic_cast<MapCorrection*>(m_pObject);
	if (map.isNoMap())
	{
		if (pMap != NULL)
		{
			pMap = NULL;
			postRequestCommand(CommandEvent::C_MAPCORRECTION,QList<QVariant>() << ((quint64)pMap));
		}
	}
	else if (pMap == NULL)
	{
		pMap = new MapCorrection(map);
		postRequestCommand(CommandEvent::C_MAPCORRECTION,QList<QVariant>() << ((quint64)pMap));
	}
	else
		(*pMap) = map;
	m_mutex.unlock();
}

/*!
    \return the histogram multiplied by the correction factors
 */
const MappedHistogram *QMesyDAQDetectorInterface::getMappedHistogram()
{
	MappedHistogram *pResult(NULL);
	m_mutex.lock();
	m_pObject = NULL;
	postRequestCommand(CommandEvent::C_MAPPEDHISTOGRAM);
	pResult = dynamic_cast<MappedHistogram*>(m_pObject);
	m_mutex.unlock();
	return pResult;
}

/*!
    \return name of the histogram data file
 */
QString QMesyDAQDetectorInterface::getHistogramFileName(void) const
{
	return m_histFileName;
}

/*!
    sets the histogram file name
    \param name histogram file name
 */
void QMesyDAQDetectorInterface::setHistogramFileName(const QString name)
{
	m_histFileName = name;
}

/*!
    \return name of the list mode data file
 */
QString QMesyDAQDetectorInterface::getListFileName(void) const
{
	return m_listFileName;
}

/*!
    sets the listmode file name
    \param name listmode file name
 */
void QMesyDAQDetectorInterface::setListFileName(const QString name)
{
	m_listFileName = name;
}

/*!
    enable/disable the histogram file save

    \param bEnable
 */
void QMesyDAQDetectorInterface::setHistogramMode(bool bEnable)
{
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_SET_SAVEHISTOGRAM, QList<QVariant>() << bEnable);
	m_mutex.unlock();
}

/*!
    return, if listmode is enabled or not

    \return is listmode enabled
 */
bool QMesyDAQDetectorInterface::getHistogramMode(void)
{
	bool bHistogramActive;

	m_mutex.lock();
	postRequestCommand(CommandEvent::C_GET_SAVEHISTOGRAM);
	bHistogramActive = m_boolean;
	m_mutex.unlock();
	return bHistogramActive;
}


/*!
    enable/disable the listmode

    \param bEnable
 */
void QMesyDAQDetectorInterface::setListMode(bool bEnable, bool bWriteProtection)
{
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_SET_LISTMODE,QList<QVariant>() << bEnable << bWriteProtection);
	m_mutex.unlock();
}

/*!
    return, if listmode is enabled or not

    \return is listmode enabled
 */
bool QMesyDAQDetectorInterface::getListMode(bool *pbWriteProtect)
{
	bool bListmodeActive;

	m_mutex.lock();
	postRequestCommand(CommandEvent::C_GET_LISTMODE);
	bListmodeActive = m_boolean;
	if (pbWriteProtect != NULL)
		*pbWriteProtect = m_bWriteProtectFiles;
	m_mutex.unlock();
	return bListmodeActive;
}

void QMesyDAQDetectorInterface::setListFileHeader(const void* pData, int iLength, bool bInsertHeaderLength)
{
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_SET_LISTHEADER,QList<QVariant>() << ((quint64)pData) << iLength << bInsertHeaderLength);
	m_mutex.unlock();
}

/*!

    \param iWidth
    \param iHeight
    \param iRunNo
 */
void QMesyDAQDetectorInterface::updateMainWidget(int iWidth, int iHeight, int iRunNo, const QString& sActive)
{
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_UPDATEMAINWIDGET, QList<QVariant>() << iWidth << iHeight << iRunNo << sActive);
	m_mutex.unlock();
}

/*!

    \param sWidth
    \param sHeight
    \param sRunNo
 */
void QMesyDAQDetectorInterface::updateMainWidget(const QString& sWidth, const QString& sHeight, const QString& sRunNo, const QString& sActive)
{
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_UPDATEMAINWIDGET, QList<QVariant>() << sWidth << sHeight << sRunNo << sActive);
	m_mutex.unlock();
}

void QMesyDAQDetectorInterface::setRunID(const quint32 runid)
{
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_SET_RUNID, QList<QVariant>() << runid);
	m_mutex.unlock();
}

void QMesyDAQDetectorInterface::setRunID(const quint32 runid, bool bAutoIncrement)
{
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_SET_RUNID, QList<QVariant>() << runid << bAutoIncrement);
	m_mutex.unlock();
}

quint32 QMesyDAQDetectorInterface::getRunID(bool *pbAutoIncrement)
{
	quint32 r(0.0);
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_GET_RUNID);
	r = m_runid;
	if (pbAutoIncrement)
		*pbAutoIncrement = m_boolean;
	m_mutex.unlock();
	return r;
}

QString QMesyDAQDetectorInterface::getVersionText()
{
	QString sVersion;
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_VERSIONTEXT);
	sVersion = m_sVersion;
	m_mutex.unlock();
	return sVersion;
}

void QMesyDAQDetectorInterface::init(void)
{
// possible in the future add a name of a different config file
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_INIT, QList<QVariant>());
	m_mutex.unlock();
}

void QMesyDAQDetectorInterface::loadConfigurationFile(const QString &confFile)
{
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_SET_CONFIGFILE, QList<QVariant>() << confFile);
	m_mutex.unlock();
}

QString QMesyDAQDetectorInterface::getConfigurationFileName(void)
{
	QString configFile;
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_GET_CONFIGFILE);
	configFile = m_configFile;
	m_mutex.unlock();
	return configFile;
}

void QMesyDAQDetectorInterface::loadCalibrationFile(const QString &calibrationFile)
{
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_SET_CALIBRATIONFILE, QList<QVariant>() << calibrationFile);
	m_mutex.unlock();
}

QString QMesyDAQDetectorInterface::getCalibrationFileName(void)
{
	QString calibrationFile;
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_GET_CALIBRATIONFILE);
	calibrationFile = m_calibrationFile;
	m_mutex.unlock();
	return calibrationFile;
}

void QMesyDAQDetectorInterface::setStreamWriter(StreamWriter *pStreamWriter)
{
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_SET_STREAMWRITER, QList<QVariant>() << ((quint64)pStreamWriter));
	m_mutex.unlock();
}

/*!
    \fn void QMesyDAQDetectorInterface::customEvent(QEvent *e)

    handles the custom events

    \param e custom event structure
 */
void QMesyDAQDetectorInterface::customEvent(QEvent *e)
{
	CommandEvent *event = dynamic_cast<CommandEvent*>(e);
	if (!event)
	{
		QtInterface::customEvent(e);
		return;
	}
	else
	{
		CommandEvent::Command cmd = event->getCommand();
		QList<QVariant> args = event->getArgs();

		if (!args.isEmpty())
		{
			switch(cmd)
			{
				case CommandEvent::C_PRESELECTION:
					m_preSelection = args[0].toDouble();
					m_eventReceived = true;
					break;
				case CommandEvent::C_READ_DIFFRACTOGRAM:
				case CommandEvent::C_READ_SPECTROGRAM:
					m_values.clear();
					foreach (const QVariant &v, args)
						m_values.push_back(v.toULongLong());
					m_eventReceived = true;
					break;
				case CommandEvent::C_READ_HISTOGRAM:
				{
					// hack to transfer a QList<quint64> to QtInterface without to copy it
					QList<quint64> *tmpData = (QList<quint64>*)args[0].toULongLong();
					if (tmpData != NULL)
					{
						m_values = *tmpData;
						delete tmpData;
					}
					else
						m_values.clear();
					m_eventReceived = true;
					break;
				}
				case CommandEvent::C_READ_HISTOGRAM_SIZE:
				{
					int i(0);
					for (QList<QVariant>::const_iterator it = args.begin(); it != args.end(); ++it, ++i)
					{
						switch (i)
						{
							case 0:
								m_width = it->toUInt();
								m_height = 0;
								break;
							case 1:
								m_height = it->toUInt();
								break;
							default: break;
						}
					}
					m_eventReceived = true;
					break;
				}
				case CommandEvent::C_STATUS:
					m_status = args[0].toInt();
					m_eventReceived = true;
					if (args.size() > 1)
						m_boolean = args[1].toBool();
					else
						m_boolean = (m_status != 0);
					break;
				case CommandEvent::C_READ_COUNTER:
					m_counter = args[0].toDouble();
					m_eventReceived = true;
					break;
				case CommandEvent::C_MAPPEDHISTOGRAM:
				case CommandEvent::C_MAPCORRECTION:
					m_pObject = (QObject*)args[0].toULongLong();
					m_eventReceived = true;
					break;
				case CommandEvent::C_GET_RUNID:
					m_runid = args[0].toUInt();
					if (args.size() > 1)
						m_boolean = args[1].toBool();
					else
						m_boolean = true;
					m_eventReceived = true;
					break;
				case CommandEvent::C_GET_LISTMODE:
					m_boolean = args[0].toBool();
					if (args.size() > 1)
						m_bWriteProtectFiles = args[1].toBool();
					else
						m_bWriteProtectFiles = false;
					m_eventReceived = true;
					break;
				case CommandEvent::C_GET_SAVEHISTOGRAM:
					m_boolean = args[0].toBool();
					m_eventReceived = true;
					break;
				case CommandEvent::C_COUNTER_SELECTED:
					m_counter = args[0].toUInt();
					m_eventReceived = true;
					break;
				case CommandEvent::C_VERSIONTEXT:
					m_sVersion = args[0].toString();
					m_eventReceived = true;
					break;
				case CommandEvent::C_GET_CALIBRATIONFILE:
					m_calibrationFile = args[0].toString();
					m_eventReceived = true;
					break;
				case CommandEvent::C_GET_CONFIGFILE:
					m_configFile = args[0].toString();
					m_eventReceived = true;
					break;
				default:
					MSG_DEBUG << tr("ignoring invalid interface answer %1").arg(cmd) << args;
					break;
			}
		}
		else
		{
			switch (cmd)
			{
				case CommandEvent::C_VERSIONTEXT:
					m_sVersion = QString("QMesyDAQ version unknown");
					m_eventReceived = true;
					break;
				case CommandEvent::C_QUIT:
					m_bDoLoop = false;
					break;
				case CommandEvent::C_INIT:
				case CommandEvent::C_START:
				case CommandEvent::C_STOP:
				case CommandEvent::C_CLEAR:
				case CommandEvent::C_RESUME:
				case CommandEvent::C_SET_CONFIGFILE:
				case CommandEvent::C_SET_CALIBRATIONFILE:
				case CommandEvent::C_SET_PRESELECTION:
				case CommandEvent::C_SELECT_COUNTER:
				case CommandEvent::C_SET_LISTMODE:
				case CommandEvent::C_SET_SAVEHISTOGRAM:
				case CommandEvent::C_UPDATEMAINWIDGET:
				case CommandEvent::C_SET_RUNID:
				case CommandEvent::C_SET_LISTHEADER:
				case CommandEvent::C_SET_STREAMWRITER:
					m_eventReceived = true;
					break;
				default:
					MSG_DEBUG << tr("ignoring invalid interface answer %1").arg(cmd) << args;
					break;
			}
		}
	}
}
