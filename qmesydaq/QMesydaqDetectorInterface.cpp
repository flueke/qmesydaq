/***************************************************************************
 *   Copyright (C) 2002 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2008 by Lutz Rossa <rossa@hmi.de>                       *
 *   Copyright (C) 2009-2010 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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

/*!
    constructor

    \param receiver
    \param parent
 */
QMesyDAQDetectorInterface::QMesyDAQDetectorInterface(QObject *receiver, QObject *parent)
    	: QtInterface(receiver, parent)
	, m_bDoLoop(true)
	, m_width(0)
	, m_height(0)
	, m_pObject(NULL)
	, m_status(0)
	, m_boolean(false)
{
}

/*!
    emits the start to the interface
 */
void QMesyDAQDetectorInterface::start()
{
        postCommand(CommandEvent::C_START);
}

/*!
    emits the stop to the interface
 */
void QMesyDAQDetectorInterface::stop()
{
        postCommand(CommandEvent::C_STOP);
}

/*!
    emits the clear to the interface
 */
void QMesyDAQDetectorInterface::clear()
{
        postCommand(CommandEvent::C_CLEAR);
}

/*!
    emits the resume to the interface
 */
void QMesyDAQDetectorInterface::resume()
{
        postCommand(CommandEvent::C_RESUME);
}

/*!
    initiate a read of the counter and return it

    \param id counter number

    \return counter value
 */
double QMesyDAQDetectorInterface::readCounter(int id)
{
	double r(0.0);
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_READ_COUNTER, QList<QVariant>() << id);
	r = m_counter;
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
	postCommand(CommandEvent::C_SELECT_COUNTER, QList<QVariant>() << id << bEnable);
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
	postCommand(CommandEvent::C_SELECT_COUNTER, QList<QVariant>() << id << bEnable << dblTarget);
}

/*!
    sets the preselection of the selected counter

    \param value preselection
 */
void QMesyDAQDetectorInterface::setPreSelection(double value)
{
        postCommand(CommandEvent::C_SET_PRESELECTION, QList<QVariant>() << value);
}

/*!
    sets the preselection of the counter id

    \param id id of the counter
    \param value preselection
 */
void QMesyDAQDetectorInterface::setPreSelection(int id, double value)
{
        postCommand(CommandEvent::C_SET_PRESELECTION, QList<QVariant>() << value << id);
}

/*!
    returns the preselection

    \return the preselection value of the preset counter
 */
double QMesyDAQDetectorInterface::preSelection()
{
	double r(0.0);
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_PRESELECTION);
	r = m_preSelection;
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
	double r(0.0);
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_PRESELECTION, QList<QVariant>() << id);
	r = m_preSelection;
	m_mutex.unlock();
	return r;
}

/*!
     return the size of the histogram 

     \param width
     \param height
 */
void QMesyDAQDetectorInterface::readHistogramSize(quint16 &width, quint16 &height)
{
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_READ_HISTOGRAM_SIZE);
	width = m_width;
	height = m_height;
	m_mutex.unlock();
}

/*!
    \return the histogram
 */
QList<quint64> QMesyDAQDetectorInterface::readHistogram()
{
	QList<quint64> r;
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_READ_HISTOGRAM);
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
int QMesyDAQDetectorInterface::status()
{
	int r(0);
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_STATUS);
	r = m_status;
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
			postCommand(CommandEvent::C_MAPCORRECTION,QList<QVariant>() << ((quint64)pMap));
		}
	}
	else if (pMap == NULL)
	{
		pMap = new MapCorrection(map);
		postCommand(CommandEvent::C_MAPCORRECTION,QList<QVariant>() << ((quint64)pMap));
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
    sets the histogram file name
    \param name histogram file name
 */
void QMesyDAQDetectorInterface::setHistogramFileName(const QString name)
{
	m_histFileName = name;
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
    enable/disable the listmode

    \param bEnable
 */
void QMesyDAQDetectorInterface::setListMode(bool bEnable)
{
	postCommand(CommandEvent::C_SET_LISTMODE,QList<QVariant>() << bEnable);
}

/*!
    return, if listmode is enabled or not

    \return is listmode enabled
 */
bool QMesyDAQDetectorInterface::getListMode()
{
	m_mutex.lock();
	postRequestCommand(CommandEvent::C_GET_LISTMODE);
	m_mutex.unlock();
	return m_boolean;
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
void QMesyDAQDetectorInterface::updateMainWidget(int iWidth, int iHeight, int iRunNo)
{
	postCommand(CommandEvent::C_UPDATEMAINWIDGET, QList<QVariant>() << iWidth << iHeight << iRunNo);
}

/*!

    \param sWidth
    \param sHeight
    \param sRunNo
 */
void QMesyDAQDetectorInterface::updateMainWidget(const QString& sWidth, const QString& sHeight, const QString& sRunNo)
{
	postCommand(CommandEvent::C_UPDATEMAINWIDGET, QList<QVariant>() << sWidth << sHeight << sRunNo);
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
					for (QList<QVariant>::const_iterator it = args.begin(); it != args.end(); ++it)
						m_values.push_back(it->toULongLong());
					m_eventReceived = true;
					break;
				case CommandEvent::C_READ_HISTOGRAM:
				{
					// hack to transfer a QList<quint64> to QtInterface without to copy it
					QList<quint64>* tmpData = (QList<quint64>*)args[0].toULongLong();
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
					m_eventReceived = true;
					break;
				case CommandEvent::C_COUNTER_SELECTED:
					m_counter = args[0].toUInt();
					m_eventReceived = true;
				default:
					break;
			}
		}
		else
		{
			switch (cmd)
			{
				case CommandEvent::C_QUIT:
					m_bDoLoop = false;
					break;
				case CommandEvent::C_SET_LISTHEADER:
					m_eventReceived = true;
					break;
				default:
					break;
			}
		}
	}
}

void QMesyDAQDetectorInterface::setRunID(const quint32 runid)
{
	postCommand(CommandEvent::C_SET_RUNID, QList<QVariant>() << runid);
}

void QMesyDAQDetectorInterface::setRunID(const quint32 runid, bool bAutoIncrement)
{
	postCommand(CommandEvent::C_SET_RUNID, QList<QVariant>() << runid << bAutoIncrement);
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
