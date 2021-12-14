// #include "detector.h"
#include <QSettings>
#include <QDebug>
#include <QStringList>

int main(int argc, char **argv)
{
	QString settingsFile("/home/jkrueger/data/mno.mcfg");

//	Detector *detector = new Detector();
//	detector->loadSetup("/home/jkrueger/data/ghi.mcfg");

	if (argc > 1)
		settingsFile = QString(argv[1]);

	QSettings settings(settingsFile, QSettings::IniFormat);

	QStringList tmp = settings.childGroups();
	qDebug() << tmp;

	QStringList mesydaq = tmp.filter("MESYDAQ");
	for (int i = 0; i < mesydaq.size(); ++i)
	{
		QString group = mesydaq[i];
		settings.beginGroup(group);
		qDebug() << group;
		qDebug() << settings.childKeys();
		settings.endGroup();
	}

	QStringList mcpd = tmp.filter("MCPD");
	for (int i = 0; i < mcpd.size(); ++i)
	{
		QString group = mcpd[i];
		settings.beginGroup(group);
		qDebug() << group;
		qDebug() << settings.childKeys();
		qDebug() << "master " << settings.value("master", "false").toBool();
//		settings.beginGroup("0");
		qDebug() << "master " << settings.value("0/master", "false").toString();
//		settings.endGroup();
		settings.endGroup();
	}
	QStringList mpsd = tmp.filter("MODULE") + tmp.filter("MPSD");
	for (int i = 0; i < mpsd.size(); ++i)
	{
		QString group = mpsd[i];
		settings.beginGroup(group);
		qDebug() << group;
//		qDebug() << settings.childKeys();
		settings.endGroup();
	}
//	delete mesy;
	return 0;
}
