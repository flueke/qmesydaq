/***************************************************************************
 *   Copyright (C) 2015-2019 by Lutz Rossa <rossa@helmholtz-berlin.de>     *
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

#ifndef __STREAMWRITER__1251BF59_C27F_4E56_85FE_2D0838FA281D__
#define __STREAMWRITER__1251BF59_C27F_4E56_85FE_2D0838FA281D__

#include <QDataStream>
#include "structures.h"
#include "libqmesydaq_global.h"

/**
 * \short Base class for streaming MCPD data into files
 *
 * This class acts like the QDataStream class for writing only,
 * but allows to overload functions. Additionally it counts the written bytes.
 *
 * \author Lutz Rossa <rossa@helmholtz-berlin.de>
*/
class LIBQMESYDAQ_EXPORT StreamWriter : public QObject
{
	Q_OBJECT

public:
	explicit StreamWriter(QIODevice *pDevice = NULL);
	explicit StreamWriter(const QString &szFile);
	virtual ~StreamWriter();

	virtual void setIODevice(QIODevice *pDevice = NULL);
	virtual bool setFile(const QString &szFile);
	virtual void close();

	virtual void setRunId(quint32 dwRunId);

	virtual QString fileName() const;
	virtual bool error() const;
	virtual void resetError();
	virtual bool isOpen() const;
	virtual QIODevice *iodevice() const;

	virtual StreamWriter &operator<<(qint8 i);
	virtual StreamWriter &operator<<(quint8 i);
	virtual StreamWriter &operator<<(qint16 i);
	virtual StreamWriter &operator<<(quint16 i);
	virtual StreamWriter &operator<<(qint32 i);
	virtual StreamWriter &operator<<(quint32 i);
	virtual StreamWriter &operator<<(qint64 i);
	virtual StreamWriter &operator<<(quint64 i);
	virtual StreamWriter &operator<<(bool i);
	virtual StreamWriter &operator<<(float i);
	virtual StreamWriter &operator<<(double i);
	virtual StreamWriter &operator<<(const char *str);
	virtual StreamWriter &operator<<(const DATA_PACKET *pPacket);

	virtual int writeRawData(const QString &str);
	virtual int writeRawData(const QByteArray &data);

	virtual StreamWriter &writeBytes(const char *pData, uint uLen);
	virtual int writeRawData(const char *pData, int iLen);

	virtual quint64 getWrittenBytes() const;

signals:
	void preCloseFile();                   //! triggered before closing a file
	void closedFile(QIODevice* pIODevice); //! triggered after closing a file
	void openedFile();                     //! triggered after opening a file
	void streamError();                    //! write error

protected:
	bool		m_bOwnDev;
	bool		m_bError;
	QIODevice	*m_pIODevice;
	quint64		m_qwBytesWritten;
};

/**
 * \short Base class for streaming MCPD data into files
 *
 * This class allows to use replacements like printf format strings
 * to specify a output files:
 *     %r  run number          (default 8 digits)
 *     %p  file part number (starting with 1, default 3 digits)
 *     %H  hour         (0..23, default 2 digits)
 *     %M  minute              (default 2 digits)
 *     %S  second              (default 2 digits)
 *     %d  day of month (1..31, default 2 digits)
 *     %m  month        (1..12, default 2 digits)
 *     %y  year                (default 4 digits)
 *     %%  the percent sign
 * Other formats are ignored and part of the file name
 * you are allowed to insert a field width (padded with '0'),
 * e.g. "%1p" uses only 1 digit part number
 *
 * \author Lutz Rossa <rossa@helmholtz-berlin.de>
*/
class LIBQMESYDAQ_EXPORT FilePartsStream : public StreamWriter
{
	Q_OBJECT

public:
	FilePartsStream(QIODevice *pDevice = NULL);
	virtual ~FilePartsStream();

	virtual void setMaxFileSize(quint64 qwMaxFileSize);
	virtual void setRunId(quint32 dwRunId);

	virtual void setIODevice(QIODevice *pDevice = NULL);
	virtual bool setFile(const QString &szFile);
	virtual int writeRawData(const char *pData, int iLen);

	virtual QString fileName() const;
	virtual quint32 part() const;

signals:
	void fileWrap(const QString &szNextFile); //! wrapped to next file

protected:
	virtual bool nextFile();

	bool		m_bAllowWrap;
	quint64		m_qwMaxFileSize;
	quint32		m_dwRunId;
	QString		m_szFileSpec;
	QString		m_szCurrentFile;
	quint32		m_dwCurrentPart;
};

#endif /*__STREAMWRITER__1251BF59_C27F_4E56_85FE_2D0838FA281D__*/
