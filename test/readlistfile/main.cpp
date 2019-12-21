#include <QCoreApplication>
#include <QTimer>
#include <QDebug>
#include <QStringList>

#include <iostream>
#include <mesydaq2.h>
#include <measurement.h>
#include <qmlogging.h>

void version(void)
{
	MSG_FATAL << "debug level : " << DEBUGLEVEL;
	MSG_FATAL << "version : " << VERSION;
}

void help(const QString &program)
{
	MSG_FATAL << program << ": [-v] [-h] list_mode_file";
	version();
}

int main(int argc, char **argv)
{

	QCoreApplication app(argc, argv);

	QStringList 	args = app.arguments();
	QString		fileName;
	startLogging(NULL, NULL);

	DEBUGLEVEL =  ERROR /* NOTICE */;

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

	MSG_ERROR << QObject::tr("Read file : %1 ").arg(fileName);

	Mesydaq2	m_mesy;
	Measurement 	m_meas(&m_mesy, NULL);
        m_meas.readListfile(fileName);

	QTimer::singleShot(50, &app, SLOT(quit()));

	app.exec();

	return 0;
}
