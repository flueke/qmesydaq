/***************************************************************************
 *   Copyright (C) 2011 by Lutz Rossa <rossa@helmholtz-berlin.de>          *
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

////////////////////////////////////////////////////////////////////////////
// $$HeadURL$$
//
// last change:
// $$Author$$
// $$Date$$
// revision $$Rev$$
////////////////////////////////////////////////////////////////////////////

#ifndef DATAREPEATER_H
#define DATAREPEATER_H

#include <QMutex>
#include <QList>
#include <QUdpSocket>


#include "libqmesydaq_global.h"

/**
 * \short this UDP socket sends the same data, which is written to a listmode file
 *
 * \author Lutz Rossa <rossa@helmholtz-berlin.de>
 */
class LIBQMESYDAQ_EXPORT DataRepeater : public QObject
{
  Q_OBJECT
public:
  //! default send interval for short datagrams
  static const int DEFAULTINTERVAL = 50;

  //! default UDP port to send to
  static const quint16 DEFAULTPORT = 54320;

  //! maximum datagram size (ethernet without fragmented IP datagrams)
  static const qint64 MAXDATAGRAMSIZE = 1450;

  explicit DataRepeater(QObject *parent, const QHostAddress &source, const QHostAddress &target = QHostAddress::LocalHost, quint16 port = DEFAULTPORT);
  explicit DataRepeater(QObject *parent, const QHostAddress &target = QHostAddress::LocalHost, quint16 port = DEFAULTPORT);
  explicit DataRepeater(const QHostAddress &source, const QHostAddress &target = QHostAddress::LocalHost, quint16 port = DEFAULTPORT);
  explicit DataRepeater(const QHostAddress &target = QHostAddress::Null, quint16 port = DEFAULTPORT);
  virtual ~DataRepeater();

  //! read, if class is enabled or not
  bool GetEnabled() const { return m_pSocket!=NULL; }

  //! read source host address
  QHostAddress GetSource() const { return m_Source; }

  //! read target host address
  QHostAddress GetTarget() const { return m_Target; }

  //! read target UDP port
  quint16 GetPort() const { return m_wPort; }

  //! enable or disable this class
  void SetEnabled(bool bEnable);

  //! set new source address (bind to interface): allowed is "Any", "LocalHost", IP address
  void SetSource(const QHostAddress &source = QHostAddress::Any);

  //! set new source address (bind to interface): allowed is "Any", "LocalHost", IP address or host name
  void SetSource(const QString &source);

  //! set new target address and UDP port: allowed is "LocalHost" (default), IP address
  void SetTarget(const QHostAddress &target = QHostAddress::LocalHost, quint16 port = DEFAULTPORT);

  //! set new target address and UDP port: allowed is "LocalHost", IP address or host name
  void SetTarget(const QString &target, quint16 port = DEFAULTPORT);

  //! store or send data to target (short data: store only and start timer)
  void WriteData(const QByteArray &data, bool doSend = false) { WriteData(data.data(),data.count(),doSend); }

  //! store or send data to target (short data: store only and start timer)
  void WriteData(const void* ptr, int length, bool doSend = false);

public slots:
  void readyRead();
  void timerEvent(QTimerEvent *);

protected:
  //! UDP socket
  QUdpSocket*  m_pSocket;

  //! source address
  QHostAddress m_Source;

  //! target address
  QHostAddress m_Target;

  //! target UDP port
  quint16      m_wPort;

  //! maximum datagram size
  int          m_iMaxDatagramSize;

  //! timer id for sending short datagrams
  int          m_iTimerId;

  //! mutex
  QMutex       m_Mutex;

  //! stored sending data
  QByteArray   m_abyTodo;

  //! UDP Package Counter (to check if one package losed)
  quint64 	m_globalPackageCounter;

  void InitSocket();
  void SendDatagram(bool bForce);
};

#endif // DATAREPEATER_H
