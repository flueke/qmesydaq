#include <QElapsedTimer>
#include <QHostInfo>
#include <QDateTime>

#include <unistd.h>
#include <mcpd8.h>
#include <mdefines.h>
#include <logging.h>

#include "testapplication.h"


int main(int argc, char **argv)
{
	QElapsedTimer eTimer;

	eTimer.invalidate();

	TestApplication app(argc, argv);

	MSG_ERROR << QHostInfo::fromName("taco6.ictrl.frm2").addresses().first();

	qDebug() << DEBUGLEVEL;

	app.stop();
	sleep(1);
	app.clear();
	app.start();
	eTimer.start();

	app.exec();
	app.result(eTimer);

	return 0;
}
