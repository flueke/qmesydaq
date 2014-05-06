#include "stdafx.h"

#if defined(_MSC_VER)
#include <QMutex>

int round(double value)
{
	if (value > 0)
		value += 0.5;
	else
		value -= 0.5;
	return (int)value;
}
void sleep(unsigned int s)
{
	QMutex tmpMutex;
	tmpMutex.lock();
	tmpMutex.tryLock(s*1000);
}
void usleep(unsigned int us)
{
	QMutex tmpMutex;
	tmpMutex.lock();
	tmpMutex.tryLock(us/1000);
}
#endif
