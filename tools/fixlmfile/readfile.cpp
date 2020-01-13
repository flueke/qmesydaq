/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2020 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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

#include <QString>
#include <QDataStream>
#include <QTextStream>
#include <QFile>
#include <QDebug>
#include <QCoreApplication>

#include "structures.h"
#include "mdefines.h"
#include "qmlogging.h"

static QString key = "mesytec psd listmode data";

void fixListfile(QString readfilename)
{
	QFile 		datfile;

	datfile.setFileName(readfilename);
	datfile.open(QIODevice::ReadOnly);

	QTextStream 	textStream;
	textStream.setDevice(&datfile);

	QChar		c;
    
	QFile	outFile;
	outFile.setFileName(readfilename + ".cp");
	QTextStream	outStream;

	for ( ; !textStream.atEnd(); )
	{
	
		textStream >> c;
		if (c == key[0])
		{
			QString 	str("");
			quint64		startpos = textStream.pos() - 1;
			for (int i = 0; i < key.length() && c == key[i]; ++i)
			{
				str.append(c);
				textStream >> c;
			}
			if (str == key)
			{
				MSG_ERROR << "found " << str << " at " << startpos;
				if (startpos > 0)
				{
					MSG_ERROR << "would open " << readfilename + ".cp";
					outFile.open(QIODevice::WriteOnly);
					outStream.setDevice(&outFile);
					outStream << str;
				}
			}
		}
		if (outFile.isOpen())
			outStream << c;
	} 
	datfile.close();
	outFile.close();
}
