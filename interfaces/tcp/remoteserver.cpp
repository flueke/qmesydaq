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
#include "remoteserver.h"
#include "logging.h"

/*!
    constructor 

    \param parent 
 */
RemoteSocket::RemoteSocket(QObject *parent)
	: QTcpSocket(parent)
{
	connect(this, SIGNAL(readyRead()), this, SLOT(readClient()));
}

/*!
    \fn void RemoteSocket::sendClient(const QString &val)

    callback for sending data to the client

    \param val data to be sent
 */
void RemoteSocket::sendClient(const QString &val)
{
	write(val.toAscii());
}

/*!
    \fn void RemoteSocket::readClient(void)

    callback to read a line from the client.
  
    It only reads data from the client if a line is complete (including the 
    EOL).
 */
void RemoteSocket::readClient(void)
{
	if (canReadLine())
	{
		QString line = readLine();
		emit gotLine(line);
	}
}

/*! 
    constructor

    \param parent
 */
RemoteServer::RemoteServer(QObject *parent) 
	: QTcpServer(parent)
	, m_socket(NULL)
{
	setMaxPendingConnections(1);
}

/*!
    \fn void RemoteServer::sendAnswer(const QString &val)
    
    callback to send a string to the client
   
    \param val answer string
 */
void RemoteServer::sendAnswer(const QString &val)
{
	emit sendClient(val);
}

/*!
    \fn void RemoteServer::incomingConnection(int socketId)

    overwritten callback in case of incoming connection

    \param socketId the new socket
 */
void RemoteServer::incomingConnection(int socketId)
{
	if (m_socket)
		destroySocket();
	m_socket = new RemoteSocket(this);
	m_socket->setSocketDescriptor(socketId);
	connect(m_socket, SIGNAL(gotLine(const QString &)), this, SLOT(parseInput(const QString &)));
	connect(this, SIGNAL(sendClient(const QString &)), m_socket, SLOT(sendClient(const QString &)));
	connect(m_socket, SIGNAL(disconnected()), this, SLOT(destroySocket()));
}

/*!
    \fn void RemoteServer::destroySocket()

    destroys the existing connection
 */
void RemoteServer::destroySocket()
{
	if (m_socket)
	{
		m_socket->disconnectFromHost();
		m_socket->deleteLater();
	}
	m_socket = NULL;
}

/*!
    \fn void RemoteServer::parseInput(const QString &line)

    callback to parse the input line an react on the data
   
    add the moment are the following commands implemented
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

    for actions and settings it returns the strings MESYDAQ OK" or "MESYDAQ NOTOK"
    depending on the input data. If all was fine the the OK is sent otherwise NOTOK.
     
    \param line input line to parse
*/
void RemoteServer::parseInput(const QString &line)
{
	QString tmp(line);

	bool	syntaxOk(true);

	if (tmp.endsWith("\n"))
		tmp.remove(tmp.length() - 1, 1);
	if (tmp.endsWith("\r"))
		tmp.remove(tmp.length() - 1, 1);
	MSG_DEBUG << "parseInput : " << tmp.toLocal8Bit().constData();
		
	QStringList l = tmp.split(' ', QString::SkipEmptyParts);
	if (l.length() > 1 && l.at(0) == "MESYDAQ")
	{
		QString cmd = l.at(1);
		MSG_DEBUG << "command : " << cmd.toLocal8Bit().constData();

		if (cmd == "START")
		{
			emit start();
		}
		else if (cmd == "STOP")
		{
			emit stop();
		}
		else if (cmd == "CLEAR")
		{
			emit clear();
		}
		else if (cmd == "MONITOR1")
		{
			if (l.length() > 2)
			{
				quint32 val = l.at(2).toULong();
				emit monitor1(val);
			}
			else
				syntaxOk = false;
		}
		else if (cmd == "MONITOR2")
		{
			if (l.length() > 2)
			{
				quint32 val = l.at(2).toULong();
				emit monitor2(val);
			}
			else
				syntaxOk = false;
		}
		else if (cmd == "TIMER")
		{
			if (l.length() > 2)
			{
				quint32 val = l.at(2).toULong();
				emit timer(val);
			}
			else
				syntaxOk = false;
		}
		else if (cmd == "EVENT")
		{
			if (l.length() > 2)
			{
				quint32 val = l.at(2).toULong();
				emit event(val);
			}
			else
				syntaxOk = false;
		}
		else if (cmd == "RESET")
		{
			emit reset();
		}
		else if (cmd == "STATUS")
		{
			emit status();
			return;
		}
		else if (cmd == "GET")
		{
			if (l.length() > 2)
			{
				QString val = l.at(2);
				if (val == "HISTOGRAM")
				{
					emit histogram();
					return;
				}
				else if (val == "TIMER")
				{
					emit timer();
					return;
				}	
				else if (val == "EVENT")
				{
					emit event();
					return;
				}
				else if (val == "MONITOR1")
				{
					emit monitor1();
					return;
				}
				else if (val == "MONITOR2")
				{
					emit monitor2();
					return;
				}
				else
					syntaxOk = false;
			}
			else
				syntaxOk = false;
		}
		else
		{
			syntaxOk = false;
			MSG_DEBUG << "unknown command : %s" << cmd.toLocal8Bit().constData();
		}
	}
	else
		syntaxOk = false;
	emit sendClient(syntaxOk ? "MESYDAQ OK\n" : "MESYDAQ NOTOK\n");
}

