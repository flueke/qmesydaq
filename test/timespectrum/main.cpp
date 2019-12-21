#include "histogram.h"

#include <QApplication>
#include <QStringList>
#include <QTimer>

#include "qmlogging.h"
#include "spectrum.h"

#include <iostream>

void version(void)
{
	std::cout << "version : " << VERSION << std::endl;
}

int main(int argc, char **argv)
{
	QCoreApplication app(argc, argv);
	startLogging("[-v]", "  -v           display version info");

	DEBUGLEVEL = WARNING;

	QStringList 	args = app.arguments();

	if (args.size() > 1)
		for (int i = 1; i < args.size(); ++i)
			if (args.at(i) == "-v")
			{
				version();
				return 1;
			}

	TimeSpectrum *tSpectrum = new TimeSpectrum(1, 32);

	for (int i = 0; i < 65536; ++i)
		tSpectrum->incVal(i & 0x3F);

	for (int i = 0; i < 32; ++i)
		std::cout << tSpectrum->value(0) << " ";
	std::cout << std::endl;

	delete tSpectrum;

	return 0;
}
