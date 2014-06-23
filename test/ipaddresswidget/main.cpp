#include "ipaddresswidget.h"
#include <QApplication>

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	IPAddressWidget	mainWin("192.168.168.5");

	mainWin.setAddress("168..122.a");

	mainWin.show();

	return app.exec();
}
