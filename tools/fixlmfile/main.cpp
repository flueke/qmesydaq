#include <QCoreApplication>
#include <QTimer>
#include <QDebug>
#include <QStringList>

#include <iostream>
#include <mcpd8.h>
#include <mesydaq2.h>
#include <mdefines.h>
#include <logging.h>

void fixListfile(QString readfilename);

void version(void)
{
	qDebug() << "version : " << VERSION;
}

void help(const QString &program)
{
	qDebug() << program << ": [-v] [-h] list_mode_file";
	version();
}

int main(int argc, char **argv)
{
	DEBUGLEVEL = FATAL /* NOTICE */;

	QCoreApplication app(argc, argv);

	QStringList 	args = app.arguments();
	QString		fileName;
	if (args.size() > 1)
	{
		for (int i = 1; i < args.size(); ++i)
			if (args.at(i) == "-h")
			{
				help(argv[0]);
				return 1;
			}
			else if (args.at(i) == "-v")
			{
				version();
				return 1;
			}
			else
				fileName = args.at(i);
	}

	qDebug() << QObject::tr("Read file : %1 ").arg(fileName);

	fixListfile(fileName);

	QTimer::singleShot(50, &app, SLOT(quit()));

	app.exec();

	return 0;
}
