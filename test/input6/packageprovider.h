#include <QObject>
#include <QTime>

#include "structures.h"
#include <networkdevice.h>


class PackageProvider : public QObject
{
	Q_OBJECT
	struct DATA_PACKET	m_data;
	QTime			t;
public:
	PackageProvider();
	int run();

signals:
	void bufferReceived(const DATA_PACKET &);

private:
	NetworkDevice	*nd = NetworkDevice::create(54321);
};
