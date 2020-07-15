#include <QCoreApplication>

#include "measurement.h"
#include "detector.h"
#include "qmlogging.h"

int main(int argc, char **argv)
{
	double measTime(10);
	QCoreApplication app(argc, argv);

	startLogging(NULL, NULL);

	Detector *detector = new Detector();
	Measurement *meas = new Measurement(detector, &app);

	QObject::connect(meas, SIGNAL(stopSignal()), &app, SLOT(quit()));

	app.setOrganizationName("MesyTec");
        app.setOrganizationDomain("mesytec.com");
        app.setApplicationName("QMesyDAQ");

// get last configuration file name
        QSettings settings(QSettings::IniFormat, QSettings::UserScope, app.organizationName(), app.applicationName());
        QString sFilename = settings.value("lastconfigfile", "mesycfg.mcfg").toString();

	meas->loadSetup(sFilename);

	meas->setTimerPreset(quint64(measTime * 1000), true);
//	meas->setEventCounterPreset(eventsPreset->presetValue(), true);

	meas->start();

	app.exec();
	return 0;
}
