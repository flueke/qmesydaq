#if 0
mdll.cpp: In member function 'void Mdll::setMdll(_MDLL_SETTINGS*, bool)':
mdll.cpp:268: error: invalid use of incomplete type 'struct mesydaq3'
mdll.h:32: error: forward declaration of 'struct mesydaq3'
mdll.cpp: In member function 'void Mdll::setThreshold()':
mdll.cpp:351: error: invalid use of incomplete type 'struct mesydaq3'
mdll.h:32: error: forward declaration of 'struct mesydaq3'
mdll.cpp: In member function 'void Mdll::setSpectrum()':
mdll.cpp:365: error: invalid use of incomplete type 'struct mesydaq3'
mdll.h:32: error: forward declaration of 'struct mesydaq3'
mdll.cpp: In member function 'void Mdll::setDatareg()':
mdll.cpp:412: error: invalid use of incomplete type 'struct mesydaq3'
mdll.h:32: error: forward declaration of 'struct mesydaq3'
mdll.cpp: In member function 'void Mdll::setAcqset()':
mdll.cpp:428: error: invalid use of incomplete type 'struct mesydaq3'
mdll.h:32: error: forward declaration of 'struct mesydaq3'
mdll.cpp: In member function 'void Mdll::setEnergy()':
mdll.cpp:442: error: invalid use of incomplete type 'struct mesydaq3'
mdll.h:32: error: forward declaration of 'struct mesydaq3'
mdll.cpp: In member function 'void Mdll::setCountercells()':
mdll.cpp:456: error: invalid use of incomplete type 'struct mesydaq3'
mdll.h:32: error: forward declaration of 'struct mesydaq3'
mdll.cpp: In member function 'void Mdll::setAuxtimers()':
mdll.cpp:470: error: invalid use of incomplete type 'struct mesydaq3'
mdll.h:32: error: forward declaration of 'struct mesydaq3'
mdll.cpp: In member function 'void Mdll::setParams()':
mdll.cpp:484: error: invalid use of incomplete type 'struct mesydaq3'
mdll.h:32: error: forward declaration of 'struct mesydaq3'
mdll.cpp: In member function 'void Mdll::setTiming()':
mdll.cpp:503: error: invalid use of incomplete type 'struct mesydaq3'
mdll.h:32: error: forward declaration of 'struct mesydaq3'
mdll.cpp: In member function 'void Mdll::serialize(QFile*)':
mdll.cpp:539: error: invalid use of incomplete type 'struct mesydaq3'
mdll.h:32: error: forward declaration of 'struct mesydaq3'
#endif

#ifndef MESYDAQ3_H
#define MESYDAQ3_H

class NetworkDevice;

class LIBQMESYDAQ_EXPORT mesydaq3
{
public:
	void sendBuffer(quint8 id, quint8 len){}

	NetworkDevice *netDev;
};

#endif
