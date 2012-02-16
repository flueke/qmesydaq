/***************************************************************************
 *   Copyright (C) 2002 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2011-2012 by Lutz Rossa <rossa@helmholtz-berlin.de>     *
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

#define MSG_FATAL    qDebug().nospace() << '0' << QString("%1(%2): ").arg(__FILE__).arg(__LINE__).toLocal8Bit().constData()
#define MSG_ERROR    qDebug().nospace() << '1' << QString("%1(%2): ").arg(__FILE__).arg(__LINE__).toLocal8Bit().constData()
#define MSG_WARNING  qDebug().nospace() << '2' << QString("%1(%2): ").arg(__FILE__).arg(__LINE__).toLocal8Bit().constData()
#define MSG_NOTICE   qDebug().nospace() << '3' << QString("%1(%2): ").arg(__FILE__).arg(__LINE__).toLocal8Bit().constData()
#define MSG_INFO     qDebug().nospace() << '4' << QString("%1(%2): ").arg(__FILE__).arg(__LINE__).toLocal8Bit().constData()
#define MSG_DEBUG    qDebug().nospace() << '5' << QString("%1(%2): ").arg(__FILE__).arg(__LINE__).toLocal8Bit().constData()

#define MSG_QCRITICAL qCritical().nospace() << QString("%1(%2): ").arg(__FILE__).arg(__LINE__).toLocal8Bit().constData()
#define MSG_QWARNING  qWarning().nospace() << QString("%1(%2): ").arg(__FILE__).arg(__LINE__).toLocal8Bit().constData()

// which debugging options are enabled
extern int DEBUGLEVEL;

// start logging (parse command line parameters from QCoreApplication)
void startLogging(const char* szShortUsage, const char* szLongUsage);

#endif /* __LOGGING_H__6D7BA1B3_11A3_4533_B63D_C7416EEDF845__ */
