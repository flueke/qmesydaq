/***************************************************************************
 *   Copyright (C) 2002 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2011 by David Schulz <david.schulz@helmholtz-berlin.de> *
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

//	authors:	David Schulz, Lutz Rossa
//	last change: $$Author$$
//	          on $$Date$$
//	revision $$Rev$$
//
//	$$HeadURL$$
//
//	Implementation for common logging via qDebug functions.

#include <QCoreApplication>
#include <QStringList>
#include <QDateTime>
#include <QFile>
#include <QDebug>
#include <iostream>
#include "logging.h"

// maximum length of logging data before word wrapping
//#define MAX_LOGLINE_LENGTH 80

// global factor for timeouts, watchdog time slots
double g_dblTimeoutFactor=1.0;

// log file
static QFile g_LogFile;

// containing time-informations for the last log entry
static QTime	lastlogTime;
static QDate	lastlogDate;
static QString lastlogTimeString;
static QString lastlogDateString;

//	show which debugging options are enabled
static bool g_bUseLogfile=false;
static bool g_bUseTimestamp=true;
static bool g_bUseSourcefile=true;
int DEBUGLEVEL=3;

//==============================================================================
// writes a message with date and time to the given file
static QString createMessage(const char *msg, char cLogLevel)
{
	QString retStr = "";

	while (!g_bUseSourcefile)
	{
		// strip source file name and line number from message
		const char* p=msg;
		while (*p!='\0' && *p!='(') ++p;
		if (*p++!='(') break;
		while (*p!='\0' && *p>='0' && *p<='9') ++p;
		if (p[0]!=')' || p[1]!=':' || p[2]!=' ') break;
		msg=p+3;
		break;
	}

	if (g_bUseTimestamp)
	{
		QTime currentTime = QTime::currentTime();
		QDate currentDate = QDate::currentDate();

		if (lastlogTime != currentTime)
		{
			lastlogTime = currentTime;
			lastlogTimeString = currentTime.toString("hh:mm:ss.zzz");
		}
		if (lastlogDate != currentDate)
		{
			lastlogDate = currentDate;
			lastlogDateString = currentDate.toString("yyyy/dd/MM");
		}

		retStr.append(lastlogDateString);
		retStr.append(' ');
		retStr.append(lastlogTimeString);
#if defined(MAX_LOGLINE_LENGTH) && MAX_LOGLINE_LENGTH>=40
		retStr.append(':');
	}

  QString qmsg(msg);
  QStringList list = qmsg.split(" ",QString::SkipEmptyParts);
  QString part("");
  bool first = true;
  foreach (QString word, list)
  {
    if (word.size()+part.size()<MAX_LOGLINE_LENGTH)
    {
      part.append(word + " ");
    }
    else
    {
      part.append("\r\n");
      if (first)
        retStr.insert(MAX_LOGLINE_LENGTH,part);
      else
        retStr.insert(retStr.size()+MAX_LOGLINE_LENGTH,part);
      part = word + " ";
      first = false;
    }
  }
  if (first)
		retStr.insert(24,part);
  else
		retStr.insert(retStr.size()+24,part);
#else /* !defined(MAX_LOGLINE_LENGTH) || MAX_LOGLINE_LENGTH<40 */
		retStr.insert(24,msg);
	}
	else
		retStr=msg;
#endif /* MAX_LOGLINE_LENGTH */
  retStr.append("\r\n");

	if (cLogLevel!='\0' && (g_bUseTimestamp || g_bUseSourcefile))
	{
		retStr.prepend("] ");
		retStr.prepend(cLogLevel);
		retStr.prepend('[');
	}

  return retStr;
}

//==================================================================================================================
// new Qt message handler for logging and debugging the qtMessages
// messages will go to the logfile
static void messageToFile(QtMsgType type, const char *msg)
{
	char cLogLevel='\0';
  int iLogLevel;
  switch (type)
  {
		case QtDebugMsg:
			iLogLevel=3;
			if (msg[0]>='0' && msg[0]<='9')
			{
				cLogLevel=msg[0];
				iLogLevel=cLogLevel-'0';
				++msg;
			}
			break;
    case QtWarningMsg:  iLogLevel=2; break;
    case QtCriticalMsg: iLogLevel=1; break;
    case QtFatalMsg:    iLogLevel=0; break;
  }
	if (iLogLevel<=DEBUGLEVEL)
  {
		QString logline=createMessage(msg,cLogLevel);
		std::cerr << logline.toStdString();

    if (g_bUseLogfile)
    {
      if (g_LogFile.open(QFile::WriteOnly | QFile::Append))
      {
        g_LogFile.write(logline.toLocal8Bit());
        g_LogFile.close();
      }
    }
  }  
}

//==================================================================================================================
// start logging (parse command line parameters from QCoreApplication)
void startLogging(const char* szShortUsage, const char* szLongUsage)
{
  QStringList args=QCoreApplication::instance()->arguments();

  g_bUseLogfile=false;
	DEBUGLEVEL=1;

  qInstallMsgHandler(messageToFile);
  for (int i=1; i<args.size(); ++i)
  {
		QString szArgument=args[i], szParameter;
		bool bSeparatedParameter=false;
		if (szArgument.indexOf('=')>=0)
		{
			int iPos=szArgument.indexOf('=');
			szParameter=szArgument.mid(iPos);
			szArgument.remove(iPos,szArgument.size()-iPos);
		}
		else
		{
			szParameter=args[i];
			bSeparatedParameter=true;
		}

		if (szArgument.startsWith("--"))
			szArgument.remove(0,1);
		if (szArgument=="-l" || szArgument=="-log")
		{
			g_LogFile.setFileName(szParameter);
			if (!g_LogFile.open(QFile::WriteOnly | QFile::Append))
			{
				std::cerr << "cannot append to file for logging " << szParameter.toStdString() << std::endl;
				exit(1);
			}
			else
			{
				g_LogFile.close();
				g_bUseLogfile=true;
				MSG_DEBUG << "logfile found or created: " << szParameter;
			}
			if (bSeparatedParameter) ++i;
		}
		else if (szArgument=="-d" || szArgument=="-debug")
		{
			bool bOK=false;
			int iLogLevel=szParameter.toInt(&bOK);
			if (bOK) DEBUGLEVEL=iLogLevel;
			if (bSeparatedParameter) ++i;
			MSG_DEBUG << "logging level " << DEBUGLEVEL;
		}
		else if (szArgument=="-nt" || szArgument.indexOf(QRegExp("-no.?time"))>=0)
		{
			g_bUseTimestamp=false;
		}
		else if (szArgument=="-ns" || szArgument.indexOf(QRegExp("-no.?source"))>=0)
		{
			g_bUseSourcefile=false;
		}
		else if (szArgument=="-h" || szArgument=="-help" || szArgument=="-?")
		{
			std::cout << args.first().split('/').last().toStdString() << " [-l=<logfile> | --log=<logfile] [-d|--debug|-d=<level>|--debug=<level>]" << std::endl
								<< "         [-nt|--no-timestamp] [-ns|--no-source]" << std::endl;
			if (szShortUsage!=NULL && szShortUsage[0]!='\0') std::cout << "         " << szShortUsage << std::endl;
			std::cout << "  -h --help    this help text" << std::endl
								<< "  -l --log     write messages to this log file" << std::endl
								<< "  -d=<level> --debug=<level>" << std::endl
								<< "  -d --debug   set logging level: 0=fatal errors only, 1=errors, 2=warnings," << std::endl
								<< "               3=notices (default), 4=info messages, 5=debug messages" << std::endl
								<< "  -nt --no-timestamps" << std::endl
								<< "               do not print timestamps" << std::endl
								<< "  -ns --no-source" << std::endl
								<< "               do not print source file names" << std::endl;
			if (szLongUsage!=NULL && szLongUsage[0]!='\0') std::cout << szLongUsage << std::endl;
			exit(0);
		}
  }
}