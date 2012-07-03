#include "histogram.h"

#include <QApplication>
#include <QStringList>
#include <QTimer>

#include "logging.h"
#include "measurement.h"
#include "mesydaq2.h"

#include "mapcorrect.h"

#include <iostream>

void version(void)
{
	std::cout << "version : " << VERSION << std::endl;
}

extern Histogram	*m_posHist;
extern Histogram	*m_ampHist;
extern Spectrum		*m_diffractogram;

int main(int argc, char **argv)
{
	QCoreApplication app(argc, argv);
	startLogging("[-v] calibration_file", "  -v           display version info");

	DEBUGLEVEL = WARNING;

	QStringList 	args = app.arguments();
	QString		fileName;

	if (args.size() > 1)
	{
		for (int i = 1; i < args.size(); ++i)
			if (args.at(i) == "-v")
			{
				version();
				return 1;
			}
			else
				fileName = args.at(i);

		MSG_FATAL << QObject::tr("Read file : %1 ").arg(fileName);

		Measurement *meas = new Measurement(new Mesydaq2());
		
		meas->readCalibration(fileName);
		MapCorrection *map = meas->posHistMapCorrection();
		for (int j = 0; j < 960; ++j)
		{
			std::cout << j << " ";
			for (int i = 0; i < 128; ++i)
			{
				int k, l;
				float	f;
				map->getMap(i, j, k, l, f);
				std::cout << "(" << i << ")" << l << " " << f;
			}
			std::cout << std::endl;
		}
	}
	else
//		help(argv[0]);
	return 0;
}
