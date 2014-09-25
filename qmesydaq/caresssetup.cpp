/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2014 by Jens Krüger <jens.krueger@frm2.tum.de>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Module Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Module Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Module Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <QSettings>
#include <QApplication>
#include <QMessageBox>

#include "caresssetup.h"
#include "logging.h"

/*!
    constructor

    \param parent
 */
CARESSSetup::CARESSSetup(QWidget *parent)
	 : QDialog(parent)
{
	setupUi(this);

	QStringList asArguments;
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
	settings.beginGroup("CARESS");
	asArguments = settings.value("argumentlist").toStringList();
	settings.endGroup();

	QString sLine;
	for (int i = 0; i < asArguments.size(); ++i)
	{
		QString sPart(asArguments[i]);
		bool bNeedQuotes(sPart.isEmpty());
		for (int j = 0; j < sPart.size(); ++j)
		{
			QChar cPart(sPart[j]);
			switch (cPart.toLatin1())
			{
				case '\a': sPart.remove(j, 1); sPart.insert(j, QString("\\a")); break;
				case '\b': sPart.remove(j, 1); sPart.insert(j, QString("\\b")); break;
				case '\e': sPart.remove(j, 1); sPart.insert(j, QString("\\e")); break;
				case '\f': sPart.remove(j, 1); sPart.insert(j, QString("\\f")); break;
				case '\n': sPart.remove(j, 1); sPart.insert(j, QString("\\n")); break;
				case '\r': sPart.remove(j, 1); sPart.insert(j, QString("\\r")); break;
				case '\t': sPart.remove(j, 1); sPart.insert(j, QString("\\t")); break;
				case '\v': sPart.remove(j, 1); sPart.insert(j, QString("\\v")); break;
				case '\\':
				case '\'': sPart.insert(j++, '\\');                     break;
				case '\"': sPart.insert(j++, '\\'); bNeedQuotes = true; break;
				case ' ':                           bNeedQuotes = true; break;
				default:
				{
					ushort u(cPart.unicode());
					if (u < 32)
					{
						sPart.remove(j, 1);
						sPart.insert(j, QString("\\x%1").arg(u, 2, 16, QChar('0')));
					}
					break;
				}
			}
		}
		if (bNeedQuotes)
		{
			sPart.prepend('\"');
			sPart.append('\"');
		}
		if (!sLine.isEmpty())
			sLine.append(" ");
		sLine.append(sPart);
	}
	argumentlist->setText(sLine);
}

void CARESSSetup::accept(void)
{
	QString sLine(argumentlist->text());
	QStringList asArguments;
	bool bEscape(false),bEmpty(true);
	int iPos(0), iString(0);
	QString sError;

	for (int i = 0; i < sLine.size(); ++i, ++iPos)
	{
		if (bEscape)
		{
			bEscape = false;
			switch (sLine[i].toLatin1())
			{
				case '\\':
				case '\'':
				case '\"':
				case ' ': continue;
				case 'a': sLine[i] = '\a'; continue;
				case 'b': sLine[i] = '\b'; continue;
				case 'e': sLine[i] = '\e'; continue;
				case 'f': sLine[i] = '\f'; continue;
				case 'n': sLine[i] = '\n'; continue;
				case 'r': sLine[i] = '\r'; continue;
				case 't': sLine[i] = '\t'; continue;
				case 'v': sLine[i] = '\v'; continue;
				case 'u':
				case 'x':
				{
					int iLen((sLine[i] == 'u') ? 4 : 2);
					QString sPart(sLine.mid(i + 1, iLen).toUpper());
					if (sPart.size() < iLen) break;
					if (!sPart.contains(QRegExp("^[0-9A-F]+$"))) break;

					quint16 w(0);
					for (int j = 0; j < iLen; ++j)
					{
						char c(sPart[j].toLatin1() - '0');
						if (c > '9') c -= 'A' - '0' - 10;
						w <<= 4;
						w += c;
					}
					sLine.remove(i + 1, iLen);
					sLine[i] = QChar(w);
					continue;
				}
				default:
					break;
			}
			sError.append(QString("<li>The command line contains an invalid escape character at position %1.</li>").arg(iPos));
			break;
		}
		switch (sLine[i].toLatin1())
		{
			case '\\':
				sLine.remove(i--, 1);
				bEscape = true;
				bEmpty = false;
				break;
			case '\'':
				bEmpty = false;
				if (iString <= 0)
				{
					sLine.remove(i--, 1);
					iString = (!iString) ? -1 : 0;
				}
				break;
			case '\"':
				bEmpty = false;
				if (iString >= 0)
				{
					sLine.remove(i--, 1);
					iString = (!iString) ? 1 : 0;
				}
				break;
			case ' ':
			case '\t':
				if (!iString)
				{
					if (!bEmpty && i > 0)
						asArguments.append(sLine.left(i));
					sLine.remove(0, i + 1);
					i = -1;
					bEmpty = true;
				}
				break;
		}
	}
	if (!sLine.isEmpty())
		asArguments.append(sLine);
	if (!sError.isEmpty() || bEscape || iString != 0)
	{
		if (bEscape)
			sError.append("<li>The command line ends with an backslash (escape) character only.</li>");
		if (iString != 0)
			sError.append("<li>The command line ends does not end with a quote character.</li>");
		sError.prepend("<b>You entered an invalid command line:</b><ul>");
		sError.append("</ul>");
		QMessageBox::critical(this, "invalid command line", sError, QMessageBox::Ok);
		if (iPos > 0)
		{
			argumentlist->setCursorPosition(iPos);
			argumentlist->setFocus();
		}
		return;
	}

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
	settings.beginGroup("CARESS");
	settings.setValue("argumentlist", asArguments);
	settings.endGroup();
	QDialog::accept();
}
