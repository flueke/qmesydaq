/***************************************************************************
 *   Copyright (C) 2002 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2011-2014 by Lutz Rossa <rossa@helmholtz-berlin.de>     *
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

//	author:	Lutz Rossa
//	last change: $$Author$$
//	          on $$Date$$
//	revision $$Rev$$
//
//	$$HeadURL$$
//
//	Definition of common logging functions.

#ifndef __LOGGING_H__6D7BA1B3_11A3_4533_B63D_C7416EEDF845__
#define __LOGGING_H__6D7BA1B3_11A3_4533_B63D_C7416EEDF845__

#include <QDebug>

#include "libqmesydaq_global.h"

#define MSG_FATAL    qDebug().nospace() << QString("0 %1(%2): ").arg(__FILE__).arg(__LINE__).toLocal8Bit().constData()
#define MSG_ERROR    qDebug().nospace() << QString("1 %1(%2): ").arg(__FILE__).arg(__LINE__).toLocal8Bit().constData()
#define MSG_WARNING  qDebug().nospace() << QString("2 %1(%2): ").arg(__FILE__).arg(__LINE__).toLocal8Bit().constData()
#define MSG_NOTICE   qDebug().nospace() << QString("3 %1(%2): ").arg(__FILE__).arg(__LINE__).toLocal8Bit().constData()
#define MSG_INFO     qDebug().nospace() << QString("4 %1(%2): ").arg(__FILE__).arg(__LINE__).toLocal8Bit().constData()
#define MSG_DEBUG    qDebug().nospace() << QString("5 %1(%2): ").arg(__FILE__).arg(__LINE__).toLocal8Bit().constData()

#define MSG_QCRITICAL qCritical().nospace() << QString("%1(%2): ").arg(__FILE__).arg(__LINE__).toLocal8Bit().constData()
#define MSG_QWARNING  qWarning().nospace() << QString("%1(%2): ").arg(__FILE__).arg(__LINE__).toLocal8Bit().constData()

// which debugging options are enabled
extern int DEBUGLEVEL;

// start logging (parse command line parameters from QCoreApplication)
void LIBQMESYDAQ_EXPORT startLogging(const char* szShortUsage, const char* szLongUsage);

QByteArray LIBQMESYDAQ_EXPORT HexDump(const void* pData, int iLength);

#endif /* __LOGGING_H__6D7BA1B3_11A3_4533_B63D_C7416EEDF845__ */
