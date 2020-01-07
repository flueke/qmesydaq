/***************************************************************************
 *   Copyright (C) 2002 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2008 by Lutz Rossa <rossa@hmi.de>                       *
 *   Copyright (C) 2009-2019 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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
#ifndef TCPLOOP_H
#define TCPLOOP_H

#include <QList>
#include <QString>

#include <LoopObject.h>

class QtInterface;
class RemoteServer;
class QMesyDAQDetectorInterface;
class QThread;
class QSettings;

#if 0
#	include "measurement.h"
#else
//! Defines the histogram type
enum HistogramType {
	PositionHistogram = 0,		//!< raw position histogram
	AmplitudeHistogram,		//!< raw amplitude histogram
	CorrectedPositionHistogram,	//!< corrected position histogram
};
#endif

/**
  \short TCP loop to manange incoming TCP requests

   This class gets its information from the TCP section of the QMesyDAQ settings
   file

   \code
   [TCP]
   personal='personal name of the TACO server'
   device='TACO device name which has to be created'
   \endcode

   \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class TCPLoop : public LoopObject
{
	Q_OBJECT
public:
	//! constructor
	TCPLoop(QtInterface *interface = 0);

	//! destructor
	virtual ~TCPLoop();

	QString version(void);

private slots:
	void help(void);
	void start(void);
	void stop(void);
	void clear(void);
	void reset(void);
	void presetTimer(const quint32);
	void presetMonitor(const quint8, const quint32);
	void presetEvent(const quint32);
	void status(void);
	void histogram(void);
	void timer(void);
	void event(void);
	void monitor(const quint8);
	void histogram(bool);
	void histogram(const QString &);
	void listmode(void);
	void listmode(bool);
	void listmode(const QString &);
	void histogramFile(void);
	void histogramType(void);
	void histogramType(const QString &);
	void histogramSave(void);
	void listmodeFile(void);

protected:
	//! thread  loop
	void runLoop();

private:
	QString formatHistogram(quint32 width, quint32 height, const QList<quint64> histo);

	QString incNumber(const QString &val);

	void sendOnOff(bool);

private:
	//! TCP server object
	RemoteServer			*m_server;

	//! TCP server port
	quint16				m_tcpport;

	//! The Qt interface
	QMesyDAQDetectorInterface	*m_interface;

	QString				m_listFilename;

	QString				m_histFilename;

	QString				m_binnedFilename;

	quint32				m_runid;

	quint32				m_histo;

	QThread				*m_thread;

	QSettings			*m_settings;
};

#endif // TACOLOOP_H

