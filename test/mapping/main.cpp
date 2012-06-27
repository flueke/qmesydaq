#include "../../lib/mapcorrect.cpp"
#include "../../lib/mappedhistogram.cpp"
#include "../../lib/histogram.cpp"

#include <iostream>

int main(int, char **)
{ 
	quint16	iSrcWidth(128),
		iSrcHeight(960),
		iDstWidth(128),
		iDstHeight(128);

// generate linear (default) mapping
	LinearMapCorrection 	m(QSize(iSrcWidth, iSrcHeight), QSize(iDstWidth, iDstHeight));

	std::cout << "map is valid " << m.isValid() << std::endl;

	MappedHistogram	mh(&m);

	Histogram	h(iSrcWidth, iSrcHeight);
	Histogram	h2(iSrcWidth, iSrcHeight);
	std::cout << "total counts : " << h.getTotalCounts() << std::endl;

	std::cout << iSrcWidth << ", " << iSrcHeight << std::endl;
// ca. 12 million events
	int counts(0);
	for (int k = 0; k < 100; ++k)
		for (quint16 i = 0; i < iSrcWidth; ++i)
			for (quint16 j = 0; j < iSrcHeight; ++j)
			{
				mh.incVal(i, j);	// ratio is at the moment about 2.4 (2012/05/24)
				h.incVal(i, j);
				h2.incVal(i, j);
				++counts;
			}
	std::cout << counts << " events mapped" << std::endl;

	std::cout << "total counts : " << mh.getTotalCounts() << std::endl;
	std::cout << "total counts (corrected) : " << mh.getCorrectedTotalCounts() << std::endl;

	std::cout << "value(0,0) " << mh.value(0, 0) << std::endl;
	std::cout << "value(0,1) " << mh.value(0, 1) << std::endl;

	mh.incVal(0, 0);
	std::cout << "total counts : " << mh.getTotalCounts() << std::endl;
	std::cout << "value(0,0) " << mh.value(0, 0) << std::endl;

	h.setValue(0, 0, 10);

	std::cout << "total counts : " << h.getTotalCounts() << std::endl;

	std::cout << "total counts (corrected) : " << mh.getCorrectedTotalCounts() << std::endl;
}
