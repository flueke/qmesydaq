/***************************************************************************
 *   Copyright (C) 2015 by Lutz Rossa <rossa@helmholtz-berlin.de>          *
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

#include "streamwriter.h"
#include <QDateTime>
#include <QFile>
#include "qendian.h"

/***************************************************************************
 * StreamWriter
 ***************************************************************************/
/*!
    constructor
    create a writer stream object

    \param pDevice existing IO device to write to
 */
StreamWriter::StreamWriter(QIODevice *pDevice)
	: m_bOwnDev(false)
	, m_bError(false)
	, m_pIODevice(NULL)
	, m_qwBytesWritten(0ULL)
{
	setIODevice(pDevice);
}

/*!
    constructor
    create a writer stream object

    \param szFile file name to write to (existing file will be overwritten)
 */
StreamWriter::StreamWriter(const QString &szFile)
	: m_bOwnDev(false)
	, m_bError(false)
	, m_pIODevice(NULL)
	, m_qwBytesWritten(0ULL)
{
	setFile(szFile);
}

/*!
    destructor
 */
StreamWriter::~StreamWriter()
{
	setIODevice(NULL);
}

/*!
    close the stream
 */
void StreamWriter::close()
{
	setIODevice(NULL);
}

/*!
    point writer stream to another object and reset written bytes

    \param pDevice IO device to write to
 */
void StreamWriter::setIODevice(QIODevice *pDevice)
{
	if (m_pIODevice)
	{
		emit preCloseFile();
		m_pIODevice->close();
		emit closedFile(m_pIODevice);
	}
	if (m_bOwnDev)
	{
		m_bOwnDev = false;
		delete m_pIODevice;
	}
	m_bError = false;
	m_pIODevice = pDevice;
	m_qwBytesWritten = 0ULL;
	if (m_pIODevice)
		emit openedFile();
}

/*!
    open a new file for writing and reset written bytes

    \param szFile file name to write to
 */
bool StreamWriter::setFile(const QString &szFile)
{
	QFile *pFile = new QFile(szFile);
	if (!pFile)
		return false;
	if (!pFile->open(QIODevice::WriteOnly))
	{
		delete pFile;
		return false;
	}
	if (m_pIODevice)
	{
		emit preCloseFile();
		m_pIODevice->close();
		emit closedFile(m_pIODevice);
	}
	if (m_bOwnDev)
		delete m_pIODevice;
	m_pIODevice = pFile;
	m_bOwnDev = true;
	m_qwBytesWritten = 0ULL;
	if (m_pIODevice)
		emit openedFile();
	return true;
}

/*!
    \return currently used file name
 */
QString StreamWriter::fileName() const
{
	QFile* pFile(dynamic_cast<QFile*>(m_pIODevice));
	if (pFile)
		return pFile->fileName();
	return QString::null;
}

/*!
    \return true, if a writing error occurred
 */
bool StreamWriter::error() const
{
	return m_bError;
}

/*!
    reset any previous writing error
 */
void StreamWriter::resetError()
{
	m_bError = false;
}

/*!
    \return if stream is ready to use
 */
bool StreamWriter::isOpen() const
{
	return (m_pIODevice);
}

/*!
    \return currently used QIODevice to stream to
 */
QIODevice *StreamWriter::iodevice() const
{
	return m_pIODevice;
}

/*!
    \param dwRunId run number
 */
void StreamWriter::setRunId(quint32 dwRunId)
{
	Q_UNUSED(dwRunId);
}

/*!
    write a signed 8 bit integer to stream, increment written bytes by 1
    \param i value
 */
StreamWriter &StreamWriter::operator<<(qint8 i)
{
	writeRawData(reinterpret_cast<const char*>(&i), sizeof(i));
	return *this;
}

/*!
    write an unsigned 8 bit integer to stream, increment written bytes by 1
    \param i value
 */
StreamWriter &StreamWriter::operator<<(quint8 i)
{
	writeRawData(reinterpret_cast<const char*>(&i), sizeof(i));
	return *this;
}

/*!
    write a signed 16 bit integer to stream, increment written bytes by 2
    \param i value
 */
StreamWriter &StreamWriter::operator<<(qint16 i)
{
#if Q_BYTE_ORDER != Q_BIG_ENDIAN
	i = qbswap(i);
#endif
	writeRawData(reinterpret_cast<const char*>(&i), sizeof(i));
	return *this;
}

/*!
    write an unsigned 16 bit integer to stream, increment written bytes by 2
    \param i value
 */
StreamWriter &StreamWriter::operator<<(quint16 i)
{
#if Q_BYTE_ORDER != Q_BIG_ENDIAN
	i = qbswap(i);
#endif
	writeRawData(reinterpret_cast<const char*>(&i), sizeof(i));
	return *this;
}

/*!
    write a signed 32 bit integer to stream, increment written bytes by 4
    \param i value
 */
StreamWriter &StreamWriter::operator<<(qint32 i)
{
#if Q_BYTE_ORDER != Q_BIG_ENDIAN
	i = qbswap(i);
#endif
	writeRawData(reinterpret_cast<const char*>(&i), sizeof(i));
	return *this;
}

/*!
    write an unsigned 32 bit integer to stream, increment written bytes by 4
    \param i value
 */
StreamWriter &StreamWriter::operator<<(quint32 i)
{
#if Q_BYTE_ORDER != Q_BIG_ENDIAN
	i = qbswap(i);
#endif
	writeRawData(reinterpret_cast<const char*>(&i), sizeof(i));
	return *this;
}

/*!
    write a signed 64 bit integer to stream, increment written bytes by 8
    \param i value
 */
StreamWriter &StreamWriter::operator<<(qint64 i)
{
#if Q_BYTE_ORDER != Q_BIG_ENDIAN
	i = qbswap(i);
#endif
	writeRawData(reinterpret_cast<const char*>(&i), sizeof(i));
	return *this;
}

/*!
    write an unsigned 64 bit integer to stream, increment written bytes by 8
    \param i value
 */
StreamWriter &StreamWriter::operator<<(quint64 i)
{
#if Q_BYTE_ORDER != Q_BIG_ENDIAN
	i = qbswap(i);
#endif
	writeRawData(reinterpret_cast<const char*>(&i), sizeof(i));
	return *this;
}

/*!
    write a boolean to stream, increment written bytes by 1
    \param i value
 */
StreamWriter &StreamWriter::operator<<(bool i)
{
	*this << (quint8)i;
	return *this;
}

/*!
    write a floating point number to stream as 64 bit IEEE double precision number, increment written bytes by 8
    \param i value
 */
StreamWriter &StreamWriter::operator<<(float i)
{
	*this << (double)i;
	return *this;
}

/*!
    write a floating point number to stream as 64 bit IEEE double precision number, increment written bytes by 8
    \param i value
 */
StreamWriter &StreamWriter::operator<<(double i)
{
	union
	{
		quint64 i;
		double d;
	} x;
	x.d = i;
	*this << x.i;
	return *this;
}

/*!
    write unsigned 32 bit length and unencoded string to stream, increment written bytes by 4 + length (without NUL character)
    \param str NUL terminated string
 */
StreamWriter &StreamWriter::operator<<(const char *str)
{
	uint uLen;
	if (m_bError || !m_pIODevice)
		return *this;

	if (!str)
	{
		str = "";
		uLen = 0;
	}
	else
		uLen = strlen(str);
	return writeBytes(str, uLen);
}

/*!
    write Mesytec MCPD data packet to stream, increment written bytes by length of packet
    \param pPacket MCPD data packet
 */
StreamWriter &StreamWriter::operator<<(const DATA_PACKET *pPacket)
{
	if (m_bError || !m_pIODevice || !pPacket || !pPacket->bufferLength)
		return *this;

#if Q_BYTE_ORDER == Q_BIG_ENDIAN
	writeRawData(reinterpret_cast<const char*>(&pPacket->bufferLength), pPacket->bufferLength * sizeof(quint16));
#else
	DATA_PACKET tmpPacket;
	const quint16* pSrc=reinterpret_cast<const quint16*>(&pPacket->bufferLength);
	quint16* pDst=reinterpret_cast<quint16*>(&tmpPacket.bufferLength);
	for (quint16 wLen(pPacket->bufferLength); wLen; --wLen)
		*pDst++ = qbswap(*pSrc++);
	writeRawData(reinterpret_cast<const char*>(&tmpPacket.bufferLength), pPacket->bufferLength * sizeof(quint16));
#endif
	return *this;
}

/*!
    write unsigned 32 bit length and unencoded string to stream, increment written bytes by 4 + length
    \param pData array of data bytes
    \param uLen  length of array
 */
StreamWriter &StreamWriter::writeBytes(const char *pData, uint uLen)
{
	*this << (quint32)uLen;
	if (uLen)
		writeRawData(pData, uLen);
	return *this;
}

/*!
    write ASCII data to stream; on success, increment written bytes by length of converted string
    \param str input string
 */
int StreamWriter::writeRawData(const QString &str)
{
	return writeRawData(str.toAscii());
}

/*!
    write unencoded data to stream; on success, increment written bytes by array size
    \param data array of data bytes
 */
int StreamWriter::writeRawData(const QByteArray &data)
{
	return writeRawData(data.constData(), data.size());
}

/*!
    write unencoded data to stream; on success, increment written bytes by iLen
    \param pData array of data bytes
    \param iLen  length of array
 */
int StreamWriter::writeRawData(const char *pData, int iLen)
{
	if (m_bError || !m_pIODevice)
		return -1;

	int iResult = m_pIODevice->write(pData, iLen);
	if (iResult == iLen)
		m_qwBytesWritten += iResult;
	else
	{
		m_bError = true;
		emit streamError();
	}
	return iResult;
}

/*!
    \return number of written bytes
 */
quint64 StreamWriter::getWrittenBytes() const
{
	return m_qwBytesWritten;
}

/***************************************************************************
 * FilePartsStream
 ***************************************************************************/
/*!
    constructor
    create a writer stream object

    \param pDevice existing IO device to write to
 */
FilePartsStream::FilePartsStream(QIODevice *pDevice)
	: StreamWriter(pDevice)
	, m_bAllowWrap(true)
	, m_qwMaxFileSize(0ULL)
	, m_dwRunId(0UL)
	, m_dwCurrentPart(0UL)
{
}

/*!
    destructor
 */
FilePartsStream::~FilePartsStream()
{
}

/*!
    set information used for trigger next file part

    \param qwMaxFileSize maximum file size
 */
void FilePartsStream::setMaxFileSize(quint64 qwMaxFileSize)
{
	m_qwMaxFileSize = qwMaxFileSize;
}

/*!
    set information used for generating file names

    \param dwRunId run number
 */
void FilePartsStream::setRunId(quint32 dwRunId)
{
	m_dwRunId = dwRunId;
}

/*!
    \return currently used file name
 */
QString FilePartsStream::fileName() const
{
	return m_szCurrentFile;
}

/*!
    \return current file part
 */
quint32 FilePartsStream::part() const
{
	return m_dwCurrentPart;
}

/*!
    point writer stream to another object and disable file parts

    \param pDevice IO device to write to
 */
void FilePartsStream::setIODevice(QIODevice *pDevice)
{
	m_bAllowWrap = true;
	m_dwCurrentPart = 0UL;
	m_qwMaxFileSize = 0ULL;
	m_szFileSpec.clear();
	m_szCurrentFile.clear();
	StreamWriter::setIODevice(pDevice);
}

/*!
    open a new file for writing

    you may use replacements like printf format strings
    to specify a output files:
        %r  run number          (default 8 digits)
        %p  file part number (starting with 1, default 3 digits)
        %H  hour         (0..23, default 2 digits)
        %M  minute              (default 2 digits)
        %S  second              (default 2 digits)
        %d  day of month (1..31, default 2 digits)
        %m  month        (1..12, default 2 digits)
        %y  year                (default 4 digits)
        %%  the percent sign
    Other formats are ignored and part of the file name
    you are allowed to insert a field width (padded with '0'),
    e.g. "%1p" uses only 1 digit part number

    \param szFile file name or spec to write to
 */
bool FilePartsStream::setFile(const QString &szFile)
{
	m_bAllowWrap = true;
	m_dwCurrentPart = 0UL;
	m_szFileSpec = szFile;
	m_szCurrentFile.clear();
	return nextFile();
}

/*!
    open next file part
 */
bool FilePartsStream::nextFile()
{
	m_szCurrentFile = m_szFileSpec;
	if (!m_szCurrentFile.isEmpty())
	{
		int iStart(0);
		bool bEscape(false);
		QDateTime tNow(QDateTime::currentDateTime());
		QString szDigits;

		++m_dwCurrentPart;
		for (int i = 0; i < m_szCurrentFile.size(); ++i)
		{
			QChar c(m_szCurrentFile[i]);
			if (bEscape)
			{
				quint32 dwValue(0);
				int iDefaultDigits(1);
				if (c.isDigit())
				{
					szDigits.append(c);
					continue;
				}

				bool bOk(true);
				int iDigits(szDigits.toInt(&bOk));
				szDigits.clear();
				bEscape = false;

				switch (c.toAscii())
				{
					case '%':
						m_szCurrentFile.remove(i, 1);
						continue;
					case 'r':
							iDefaultDigits = 8;
						dwValue = m_dwRunId;
						break;
					case 'p':
						iDefaultDigits = 3;
						dwValue = m_dwCurrentPart;
						break;
					case 'H':
						iDefaultDigits = 2;
						dwValue = tNow.time().hour();
						break;
					case 'M':
						iDefaultDigits = 2;
						dwValue = tNow.time().minute();
						break;
					case 'S':
						iDefaultDigits = 2;
						dwValue = tNow.time().second();
						break;
					case 'd':
						iDefaultDigits = 2;
						dwValue = tNow.date().day();
						break;
					case 'm':
						iDefaultDigits = 2;
						dwValue = tNow.date().month();
						break;
					case 'y':
						iDefaultDigits = 4;
						if (bOk && iDigits > 0 && iDigits < 4)
							dwValue = tNow.date().year() % 100;
						else
							dwValue = tNow.date().year();
						break;
					default:
						continue;
				}

				if (!bOk || iDigits < 1)
					iDigits = iDefaultDigits;
				m_szCurrentFile.replace(iStart, i - iStart + 1, QString("%1").arg(dwValue, iDigits, 10, QChar('0')));
			}
			else if (c == '%')
			{
				bEscape = true;
				iStart = i;
			}
		}

		if (m_szCurrentFile == m_szFileSpec)
			// file name has no option for a part number, disable parts
			m_qwMaxFileSize = 0ULL;
	}

	bool bAllowWrap(m_bAllowWrap);
	bool bResult(true);
	m_bAllowWrap = false;

	if (StreamWriter::setFile(m_szCurrentFile))
		emit fileWrap(m_szCurrentFile);
	else
		bResult = false;

	m_bAllowWrap = bAllowWrap;
	return bResult;
}

/*!
    write unencoded data to stream; handle maximum file size
    \param pData array of data bytes
    \param iLen  length of array
 */
int FilePartsStream::writeRawData(const char *pData, int iLen)
{
	if (iLen < 0)
		return -1;
	if (!pData || !iLen)
		return 0;
	if (m_bAllowWrap && m_qwMaxFileSize != 0ULL)
	{
		quint64 qwBytesWritten(getWrittenBytes());
		if (qwBytesWritten != 0ULL && (qwBytesWritten + (quint64)iLen) > m_qwMaxFileSize)
			nextFile();
	}
	return StreamWriter::writeRawData(pData, iLen);
}
