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

#ifndef MESYDAQ_DETECTOR_INTERFACE_H
#define MESYDAQ_DETECTOR_INTERFACE_H

#include <QMutex>
#include <QSize>
#include "QtInterface.h"
#include "CommandEvent.h"

class MapCorrection;
class MappedHistogram;
class StreamWriter;

/*!
    \class QMesyDAQDetectorInterface

    \short This class handles the detector remote interface

    \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class QMesyDAQDetectorInterface : public QtInterface
{
	Q_OBJECT

	//! the name of the list mode data file
	Q_PROPERTY(QString m_listFileName READ getListFileName WRITE setListFileName)

	//! the name of the histogram data file
	Q_PROPERTY(QString m_histFileName READ getHistogramFileName WRITE setHistogramFileName)

	//! is the event loop running
	Q_PROPERTY(bool m_bDoLoop READ doLoop)

	//! the preselection value
	Q_PROPERTY(double m_preSelection READ preSelection WRITE setPreSelection)

public:
	QMesyDAQDetectorInterface(QObject *receiver = 0, QObject *parent = 0);

	//! \return whether looping
	bool doLoop() const;

	void start();
	void stop();
	void clear();
	void resume();
	double readCounter(int id);
	void selectCounter(int id, bool bEnable = true);
	void selectCounter(int id, bool bEnable, double dblTarget);
	bool counterSelected(int id);

	void setPreSelection(double);
	void setPreSelection(int id, double);
	double preSelection();
	double preSelection(int id);

	QSize readHistogramSize(int = 0);
	QList<quint64> readHistogram(int = 0);
	QList<quint64> readDiffractogram();
	QList<quint64> readSpectrogram(int iSpectrogram = -1);
	int status(bool *pbRunAck = NULL);

	const MapCorrection* getMappingCorrection();
	void setMappingCorrection(const MapCorrection& map);
	const MappedHistogram* getMappedHistogram();

	bool getListMode(bool *pbWriteProtect = NULL);
	void setListMode(bool bEnable, bool bWriteProtection = false);

	bool getHistogramMode(void);
	void setHistogramMode(bool bEnable);

	QString getListFileName(void) const;
	void setListFileName(const QString name);

	//! \brief store header for list mode file
	void setListFileHeader(const void* pData, int iLength, bool bInsertHeaderLength);

	QString getHistogramFileName(void) const;
	void setHistogramFileName(const QString name);

	void updateMainWidget(int iWidth, int iHeight, int iRunNo, const QString& sActive);
	void updateMainWidget(const QString& sWidth, const QString& sHeight, const QString& sRunNo, const QString& sActive);

	void loadConfigurationFile(const QString &confFile);
	QString getConfigurationFileName(void);

	/*!
	 * Sets the run ID
	 *
	 * \param runid
	 */
	void setRunID(const quint32 runid);
	void setRunID(const quint32 runid, bool bAutoIncrement);

	//! returns the current runid
	quint32 getRunID(bool *pbAutoIncrement = NULL);

	//! set stream writer class to write list mode files to
	void setStreamWriter(StreamWriter *pStreamWriter);

	//! return QMesyDAQ and library version text
	QString getVersionText();

	//! initialises the QMesyDAQ (loads the current set config file again)
	void init();

protected:
	void customEvent(QEvent *);

private:
	//! mutex to lock the access
	mutable QMutex		m_mutex;

	bool                    m_bDoLoop;

	//! preselection value
	double			m_preSelection;

	//! counter value
	double			m_counter;

	//! with
	quint16			m_width;

	//! height
	quint16			m_height;

	//! stores the current data set
	QList<quint64>		m_values;

	//! ????
	QObject			*m_pObject;

	//! stores the current state
	int			m_status;

	//! name of the current listmode file
	QString			m_listFileName;

	//! name of the current histogram file
	QString			m_histFileName;

	//! QMesyDAQ and library version
	QString			m_sVersion;

	//! current run ID
	quint32			m_runid;

	//! is listmode enabled / auto increment run number
	bool			m_boolean;

	//! write protect data files?
	bool			m_bWriteProtectFiles;

	//! name of the configuration file
	QString			m_configFile;
};

#endif // MESYDAQDETECTORQTINTERFACE_H
