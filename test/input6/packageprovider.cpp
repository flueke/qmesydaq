#include <iostream>

#include "packageprovider.h"


PackageProvider::PackageProvider()
{
	nd = NetworkDevice::create(54321);
}

int PackageProvider::run()
{
	t.start();
	for (int i = 0; i < 400000; ++i)
		emit bufferReceived(m_data);
	return t.elapsed();
}
