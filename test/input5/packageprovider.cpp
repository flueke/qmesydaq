#include <iostream>

#include "packageprovider.h"


PackageProvider::PackageProvider()
{
	m_data.bufferLength = 750;
	m_data.bufferType = 1;
	m_data.headerLength = 21;
	m_data.bufferNumber = 1;
	m_data.runID = 1;
	m_data.deviceStatus = 0;
	m_data.deviceId = 0;
	for (int i = 0; i < 3; ++i)
		m_data.time[0] = 0;
	// m_data.param[4][3];
	// m_data.data[750];
	for (int i = 0; i < 729; ++i)
		m_data.data[i] = 0;
	std::cerr << sizeof(m_data) << std::endl;
}

int PackageProvider::run()
{
	t.start();
	for (int i = 0; i < 400000; ++i)
		emit bufferReceived(m_data);
	return t.elapsed();
}
