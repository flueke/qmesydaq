#include "mesydaqdata.h"

MesydaqData::MesydaqData()
	: QwtData()
	, d_size(1)
{
	m_data.clear();
	for (quint32 i = 0; i <= d_size; ++i)
		m_data.append(i * i);
}


QwtData *MesydaqData::copy() const
{
	return new MesydaqData();
}

double MesydaqData::x(size_t i) const
{
	return i;
}

double MesydaqData::y(size_t i) const
{
	return m_data[i];
}

void MesydaqData::setData(ulong *data, quint32 len)
{
	m_data.clear();
	for (quint32 i = 0; i < len; ++i)
		m_data.append(data[i]);
	d_size = len;
}

quint32 MesydaqData::max(void)
{
	quint32 m = 0;
	for(int i = 0; i < m_data.size(); ++i)
		if (m_data.value(i, 0) > m)
			m = m_data.value(i, 0);
	return m;
}
