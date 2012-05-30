#include "../../lib/mapcorrect.cpp"
#include "../../lib/histogram.cpp"

#include <iostream>

int main(int, char **)
{ 
	quint16	iSrcWidth(128),
		iSrcHeight(960),
		iDstWidth(128),
		iDstHeight(128);

	MapCorrection 	m;

	m.setNoMap();
	m.initialize(iSrcWidth, iSrcHeight, MapCorrection::OrientationUp, MapCorrection::CorrectSourcePixel);
	m.setMappedRect(QRect(0, 0, iDstWidth, iDstHeight));
	std::cout << "map is valid " << m.isValid() << std::endl;
// generate linear (default) mapping
  	for (int i = 0; i < iDstHeight; ++i)
	{
		int iStartY = (iSrcHeight * i) / iDstHeight;
		int iEndY   = (iSrcHeight * (i + 1)) / iDstHeight;
		for(int k = iStartY; k < iEndY; ++k)
			for (int j = 0; j < iDstWidth; ++j)
			{
				int iStartX = (iSrcWidth * j) / iDstWidth;
				int iEndX   = (iSrcWidth * (j + 1)) / iDstWidth;
				QPoint pt(j, i);
				while (iStartX < iEndX)
			  		m.map(QPoint(iStartX++, k), pt, 1.0);
			}
	}
	std::cout << "map is valid " << m.isValid() << std::endl;

	MappedHistogram	mh(&m);

	Histogram	h(iSrcWidth, iSrcHeight);
	Histogram	h2(iSrcWidth, iSrcHeight);
	std::cout << "total counts : " << h.getTotalCounts() << std::endl;

	std::cout << iSrcWidth << ", " << iSrcHeight << std::endl;
// ca. 12 million events
	for (int k = 0; k < 100; ++k)
	for (quint16 i = 0; i < iSrcWidth; ++i)
		for (quint16 j = 0; j < iSrcHeight; ++j)
		{
			mh.incVal(i, j);	// ratio is at the moment about 2.4 (2012/05/24)
			h.incVal(i, j);
			h2.incVal(i, j);
		}
	std::cout << "mapped" << std::endl;

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
