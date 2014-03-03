/***************************************************************************
 *   Copyright (C) 2011-2013 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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
#ifndef REMOTE_SERVER_H
#define REMOTE_SERVER_H

#include <QDebug>

#include <QTcpServer>
#include <QTcpSocket>
#include <QStringList>

class RemoteSocket : public QTcpSocket
{
Q_OBJECT
public:
	RemoteSocket(QObject *parent = 0);

public slots:
	void sendClient(const QString &val);

private slots:
	void readClient(void);

signals:
	void gotLine(const QString &);
};

class RemoteServer : public QTcpServer
{
Q_OBJECT
public:
	RemoteServer(QObject *parent = 0);

public slots:
	void sendAnswer(const QString &val);

protected:
	void incomingConnection(int socketId);

signals :
	void start(void);

	void stop(void);

	void clear(void);

	void reset(void);

	void timer(const quint32);

	void timer(void);

	void event(const quint32);

	void event(void);

	void monitor(const quint8, const quint32);

	void monitor(const quint8);

	void status(void);

	void histogram(void);

	void sendClient(const QString &);

private slots:
	void destroySocket();

	/**
		actions:
			MESYDAQ [START|STOP|CLEAR|RESET]
		sets:
			MESYDAQ [TIMER|EVENT|MONITOR1|MONITOR2] PRESETVAL
		queries:
			MESYDAQ STATUS -> MESYDAQ [STARTED|STOPPED]
			MESYDAQ GET HISTOGRAM -> Histogram in Mesytec histogram format
			MESYDAQ GET TIMER -> timer
			MESYDAQ GET EVENT -> events over all
			MESYDAQ GET MONITOR[1|2] -> monitors
	*/
	void parseInput(const QString &line);

private:
	RemoteSocket 	*m_socket;

};

#endif // REMOTE_SERVER_H
