#ifndef MESYDAQ_DATA_H
#define MESYDAQ_DATA_H

#include <QVector>

#include <qwt_data.h>

class MesydaqData : public QwtData
{
public:
	MesydaqData();

	virtual QwtData *copy() const;

	virtual size_t size() const {return d_size;}

	virtual double x(size_t i) const;

	virtual double y(size_t i) const;

	void setData(ulong *, quint32);

	quint32 max(void);

private:
	size_t 		d_size;
	QVector<ulong>	m_data;
	
};

#endif
