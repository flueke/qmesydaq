#include <QString>
#include <QDataStream>
#include <QTextStream>
#include <QFile>
#include <QDebug>
#include <QCoreApplication>

#include "structures.h"
#include "mdefines.h"
#include "logging.h"

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
