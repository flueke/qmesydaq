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

#include "datarepeater.h"
#include <QUdpSocket>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QTimerEvent>

//! if defined use a connected UDP socket, else send datagrams directly
#define USE_CONNECTED_UDP

/*!
  \brief constructor
  \param parent  parent object
  \param source  source address: allowed is "Any" (default), "LocalHost", IP address
  \param target  target address: allowed is "LocalHost" (default), "Broadcast", IP address
  \param port    target UDP port (default=54320)
*/
DataRepeater::DataRepeater(QObject *parent, const QHostAddress &source, const QHostAddress &target, quint16 port)
  : QObject(parent), m_pSocket(NULL), m_Source(source), m_Target(target), m_wPort(port), m_iMaxDatagramSize(MAXDATAGRAMSIZE), m_iTimerId(0)
{
  InitSocket();
}

/*!
  \brief constructor
  \param parent  parent object
  \param target  target address: allowed is "LocalHost" (default), "Broadcast", IP address
  \param port    target UDP port (default=54320)
*/
DataRepeater::DataRepeater(QObject *parent, const QHostAddress &target, quint16 port)
  : QObject(parent), m_pSocket(NULL), m_Source(QHostAddress::Any), m_Target(target), m_wPort(port), m_iMaxDatagramSize(MAXDATAGRAMSIZE), m_iTimerId(0)
{
  InitSocket();
}

/*!
  \brief constructor
  \param source  source address: allowed is "Any" (default), "LocalHost", IP address
  \param target  target address: allowed is "LocalHost" (default), "Broadcast", IP address
  \param port    target UDP port (default=54320)
*/
DataRepeater::DataRepeater(const QHostAddress &source, const QHostAddress &target, quint16 port) :
  QObject(), m_pSocket(NULL), m_Source(source), m_Target(target), m_wPort(port), m_iMaxDatagramSize(MAXDATAGRAMSIZE), m_iTimerId(0)
{
  InitSocket();
}

/*!
  \brief default constructor
  \param target  target address: allowed is "LocalHost" (default), "Broadcast", IP address
  \param port    target UDP port (default=54320)
*/
DataRepeater::DataRepeater(const QHostAddress &target, quint16 port) :
  QObject(), m_pSocket(NULL), m_Source(QHostAddress::Any), m_Target(target), m_wPort(port), m_iMaxDatagramSize(MAXDATAGRAMSIZE), m_iTimerId(0)
{
  if (target.isNull())
    m_Target=QHostAddress::LocalHost;
  else
    InitSocket();
}

//! destructor
DataRepeater::~DataRepeater()
{
  delete m_pSocket;
}

//! initialize this class, create+bind an UDP socket
void DataRepeater::InitSocket()
{
  QMutexLocker lock(&m_Mutex);
  m_abyTodo.clear();
  if (m_iTimerId!=0)
  {
    killTimer(m_iTimerId);
    m_iTimerId=0;
  }
  if (m_pSocket!=NULL)
  {
    m_pSocket->disconnect(this);
    delete m_pSocket;
  }
  if (m_wPort<2) m_wPort=DEFAULTPORT;
  m_pSocket=new QUdpSocket;
  if (m_pSocket==NULL)
    return;

#ifdef USE_CONNECTED_UDP
  m_pSocket->connectToHost(m_Target,m_wPort,QUdpSocket::WriteOnly);
#else
  if (m_pSocket->bind(m_Source,0,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint))
#endif
    if (connect(m_pSocket,SIGNAL(readyRead()),this,SLOT(readyRead())))
      return;

  delete m_pSocket;
  m_pSocket=NULL;
}

/*!
  \brief enable or disable this class
  \param bEnable flag to enable this class
*/
void DataRepeater::SetEnabled(bool bEnable)
{
  if (bEnable && m_pSocket==NULL)
    InitSocket();
  else if (!bEnable && m_pSocket!=NULL)
  {
    m_Mutex.lock();
    delete m_pSocket;
    m_pSocket=NULL;
    m_Mutex.unlock();
  }
}

/*!
  \brief set new source address and bind to interface
  \param source  source address: allowed is "Any" (default), "LocalHost", IP address
*/
void DataRepeater::SetSource(const QHostAddress &source)
{
  // check source address against local host addresses
  if (!QNetworkInterface::allAddresses().contains(source) || source.isNull())
    m_Source=QHostAddress::Any;
  else
    m_Source=source;
  InitSocket();
}

/*!
  \brief set new source address and bind to interface
  \param source  source address: string IP address or host name
*/
void DataRepeater::SetSource(const QString &source)
{
  QHostAddress s(QHostAddress::Any);
  if (!s.setAddress(source))
  {
    // resolve host name into IP address
    QHostInfo i=QHostInfo::fromName(source);
    if (!i.addresses().isEmpty())
      s=i.addresses().first();
    else
      s=QHostAddress::Any;
  }
  SetSource(s);
}

/*!
  \brief set new target address and UDP port
  \param target  target address: allowed is "LocalHost" (default), "Broadcast", IP address
  \param port    target UDP port (default=54320)
*/
void DataRepeater::SetTarget(const QHostAddress &target, quint16 port)
{
  if (target.isNull())
  {
    m_Mutex.lock();
    m_Target=QHostAddress::LocalHost;
    if (m_pSocket!=NULL)
    {
      delete m_pSocket;
      m_pSocket=NULL;
    }
    m_wPort=port;
    m_Mutex.unlock();
    return;
  }
  else
    m_Target=target;
  if (m_wPort!=port)
  {
    m_wPort=port;
    InitSocket();
  }
#ifdef USE_CONNECTED_UDP
  else
  {
    m_pSocket->disconnectFromHost();
    m_pSocket->connectToHost(m_Target,m_wPort,QUdpSocket::WriteOnly);
  }
#endif
}

/*!
  \brief set new target address and UDP port
  \param target  target address: string IP address or host name
  \param port    target UDP port (default=54320)
*/
void DataRepeater::SetTarget(const QString &target, quint16 port)
{
  QHostAddress t(QHostAddress::LocalHost);
  if (!t.setAddress(target))
  {
    // resolve host name into IP address
    QHostInfo i=QHostInfo::fromName(target);
    if (!i.addresses().isEmpty())
      t=i.addresses().first();
    else
      t=QHostAddress::LocalHost;
  }
  SetTarget(QHostAddress(target),port);
}

/*!
  \brief store or send data to target (short data: store only and start timer)
  \param ptr     pointer to data
  \param length  data length in bytes
*/
void DataRepeater::WriteData(const void* ptr, int length)
{
  const char* pData=(const char*)ptr;
  if (m_pSocket==NULL || pData==NULL || length<1) return;
  m_Mutex.lock();
  m_abyTodo.append((const char*)ptr,length);
  if (m_iTimerId==0)
  {
    if (m_abyTodo.count()>=m_iMaxDatagramSize)
      SendDatagram(false);
    if (m_abyTodo.count()>0)
      m_iTimerId=startTimer(DEFAULTINTERVAL);
  }
  m_Mutex.unlock();
}

/*!
  \brief timer slot for sending short datagrams
  \param event   Qt event data structure
*/
void DataRepeater::timerEvent(QTimerEvent *event)
{
  if (event==NULL) return;
  int id=event->timerId();
  if (id!=0 && id==m_iTimerId)
  {
    m_Mutex.lock();
    if (m_abyTodo.count()>0)
      SendDatagram(true);
    killTimer(id);
    m_iTimerId=0;
    m_Mutex.unlock();
  }
  else
    killTimer(id);
}

/*!
  \brief real data sending function, which splits much data into smaller datagrams
  \param bForce  false=do not send short datagrams, true=send also short datagrams
*/
void DataRepeater::SendDatagram(bool bForce)
{
  int iLength=m_abyTodo.count();
  if (m_pSocket==NULL)
  {
    if (iLength>0)
      m_abyTodo.clear();
    return;
  }
  while (iLength>0)
  {
    int len=iLength;
    if (len>m_iMaxDatagramSize)
      len=m_iMaxDatagramSize;
#ifdef USE_CONNECTED_UDP
    if (m_pSocket->write(m_abyTodo.data(),len)<0)
#else
    if (m_pSocket->writeDatagram(m_abyTodo.data(),len,m_Target,m_wPort)<0)
#endif
    {
      if (m_pSocket->error()==QAbstractSocket::DatagramTooLargeError && m_iMaxDatagramSize>=16)
      {
	m_iMaxDatagramSize>>=1;
	continue;
      }
      break;
    }
    m_abyTodo.remove(0,len);
    iLength-=len;
    if (!bForce && iLength<m_iMaxDatagramSize) break;
  }
}

//! an UDP socket is able to receive data; discard it!
void DataRepeater::readyRead()
{
  // discard read datagrams
  char buffer[65536];
#ifdef USE_CONNECTED_UDP
  for (;;)
  {
    qint64 i=m_pSocket->bytesAvailable();
    if (i<1) break;
    if (i>(qint64)sizeof(buffer)) i=(qint64)sizeof(buffer);
    m_pSocket->read(&buffer[0],i);
  }
#else
  while (m_pSocket->hasPendingDatagrams())
    m_pSocket->readDatagram(&buffer[0],sizeof(buffer));
#endif
}
