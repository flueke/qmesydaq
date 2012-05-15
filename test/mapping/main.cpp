#include <mapcorrect.h>
#include <histogram.h>

#include <iostream>

int main(int, char **)
{ 
	quint16	iSrcWidth(128),
		iSrcHeight(960),
		iDstWidth(128),
		iDstHeight(128);

	MapCorrection 	m;

	Histogram	h(iSrcWidth, iSrcHeight);

	std::cout << iSrcWidth << ", " << iSrcHeight << std::endl;
	for (quint16 i = 0; i < iSrcWidth; ++i)
		for (quint16 j = 0; j < iSrcHeight; ++j)
		{
			h.setValue(i, j, 1);
		}

	std::cout << "total counts : " << h.getTotalCounts() << std::endl;

	m.setNoMap();
	
	m.initialize(128, 960, MapCorrection::OrientationUp, MapCorrection::CorrectSourcePixel);

	m.setMappedRect(QRect(0, 0, 128, 128));

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

	std::cout << "total counts : " << h.getTotalCounts() << std::endl;

	MappedHistogram	mh(&m, &h);

	std::cout << "mapped" << std::endl;

	std::cout << "total counts : " << mh.getTotalCounts() << std::endl;
	std::cout << "total counts (corrected) : " << mh.getCorrectedTotalCounts() << std::endl;

	std::cout << "value(0,0) " << mh.value(0, 0) << std::endl;
	std::cout << "value(0,1) " << mh.value(0, 1) << std::endl;

	h.setValue(0, 0, 10);

	std::cout << "total counts : " << h.getTotalCounts() << std::endl;

	std::cout << "total counts : " << mh.getTotalCounts() << std::endl;
	std::cout << "total counts (corrected) : " << mh.getCorrectedTotalCounts() << std::endl;

	mh.setMapCorrection(&m, &h);

	std::cout << "total counts : " << mh.getTotalCounts() << std::endl;
	std::cout << "total counts (corrected) : " << mh.getCorrectedTotalCounts() << std::endl;
}
