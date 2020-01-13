/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009 by Jens Krüger <jens.krueger@frm2.tum.de>          *
 *   Copyright (C) 2011-2020 by Lutz Rossa <rossa@helmholtz-berlin.de>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <QRegExp>
#include <QDateTime>
#include <QStringList>
#include <QDebug>
#include <zlib.h>
#include "mapcorrectparser.h"
#include "qmlogging.h"

#define ARRAY_SIZE(x) ((int)(sizeof(x)/sizeof((x)[0])))

bool CaressHelper::zlib_zip(QByteArray abyIn, QByteArray &abyOut, bool bGzipHeader)
{
	const int CHUNK=16384;
	z_stream strm;
	int i;

	abyOut.clear();
	if (abyIn.isEmpty())
		return false;

	memset(&strm,0,sizeof(strm));
	strm.zalloc=Z_NULL;
	strm.zfree=Z_NULL;
	strm.opaque=Z_NULL;
	strm.next_in=Z_NULL;
	strm.avail_in=0;
	i=deflateInit2(&strm,Z_BEST_COMPRESSION/*level*/,Z_DEFLATED/*method*/,15+(bGzipHeader?16:0)/*windowBits+gzip-header*/,
				   9/*memLevel*/,Z_DEFAULT_STRATEGY/*strategy*/);
	if (i!=Z_OK)
		goto error_of_zlib_zip;
	if (bGzipHeader)
	{
		gz_header h;
		memset(&h,0,sizeof(h));
		h.text=Z_UNKNOWN;
		h.time=QDateTime::currentDateTime().toTime_t();
		h.xflags=9;
		h.os=255;
		i=deflateSetHeader(&strm,&h);
		if (i!=Z_OK)
			goto error_of_zlib_zip;
	}
	for (;;)
	{
		i=abyIn.size();
		if (i>=CHUNK) i=CHUNK;
		strm.avail_in=i;
		int iFlushFlag = (i==abyIn.size()) ? Z_FINISH : Z_NO_FLUSH;
		strm.next_in=(Bytef*)abyIn.constData();
		do
		{
			char buffer[CHUNK];
			strm.avail_out=CHUNK;
			strm.next_out=(Bytef*)(&buffer[0]);
			i=deflate(&strm,iFlushFlag);
			if (i==Z_STREAM_ERROR)
				goto error_of_zlib_zip;
			int iCompressed=CHUNK-strm.avail_out;
			if (iCompressed>0)
				abyOut+=QByteArray::fromRawData(buffer,iCompressed);
		} while (strm.avail_out==0);
		if (abyIn.size()<=CHUNK)
			break;
		abyIn.remove(0,CHUNK);
	}
	deflateEnd(&strm);
	return true;
error_of_zlib_zip:
	deflateEnd(&strm);
	return false;
}

/*!
 * \brief unpack zlib compressed data
 * \param [in]  abyIn   compressed data
 * \param [out] abyOut  uncompressed data
 * \return true, if successful
 */
bool CaressHelper::zlib_unzip(QByteArray abyIn, QByteArray& abyOut)
{
	z_stream strm;
	int i,j,iSize=1024;

	abyOut.clear();
	if (abyIn.isEmpty())
		return false;

	memset(&strm,0,sizeof(strm));
	strm.zalloc=Z_NULL;
	strm.zfree=Z_NULL;
	strm.opaque=Z_NULL;
	strm.next_in=Z_NULL;
	strm.avail_in=0;
	i=inflateInit(&strm);
	if (i!=Z_OK)
	{
		inflateEnd(&strm);
		return false;
	}

	strm.next_in=(Bytef*)abyIn.constData();
	strm.avail_in=abyIn.count();

	for (j=0;;)
	{
		if ((j+32)>=iSize) iSize+=32;
		abyOut.resize(iSize);
		strm.next_out=(Bytef*)(&(abyOut.data())[j]);
		strm.avail_out=abyOut.count()-j;
		i=inflate(&strm,Z_NO_FLUSH);
		switch (i)
		{
			case Z_NEED_DICT: i=Z_DATA_ERROR;
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				break;
			default:
				j+=iSize-strm.avail_out;
				if (strm.avail_out==0)
					continue;
		}
		abyOut.resize(j);
		break;
	}
	inflateEnd(&strm);
	if (i!=Z_OK || abyOut.isEmpty())
		return false;

	return true;
}

/*!
  \brief parse a mapping from CARESS
  \param[in] pMapping   textual mapping line from CARESS
  \param[in] iLength    length of textual mapping line
  \param[in] iSrcWidth  width of source histogram
  \param[in] iSrcHeight height of source histogram
  \param[in] iDstSidth  width of mapped histogram
  \param[in] iDstHeight height of mapped histogram
  \return new MapCorrection which may be used with QMesyDAQ
 */
MapCorrection CaressMapCorrection::parseCaressMapCorrection(const char* pMapping, int iLength, int iSrcWidth, int iSrcHeight, int iDstWidth, int iDstHeight)
{
	QString mapping=QString::fromLatin1(pMapping,iLength);
	return parseCaressMapCorrection(mapping,iSrcWidth,iSrcHeight,iDstWidth,iDstHeight);
}

/*!
  \brief parse a mapping from CARESS
  \param[in] mapping    textual mapping line from CARESS
  \param[in] iSrcWidth  width of source histogram
  \param[in] iSrcHeight height of source histogram
  \param[in] iDstWidth  width of mapped histogram
  \param[in] iDstHeight height of mapped histogram
  \return new MapCorrection which may be used with QMesyDAQ
 */
MapCorrection CaressMapCorrectionDefault::parseCaressMapCorrection(const QString& mapping, int iSrcWidth, int iSrcHeight, int iDstWidth, int iDstHeight)
{
	MapCorrection result;
	const QChar* pStart=mapping.constData();
	const QChar* pEOL;
	const QChar* pEnd=pStart+mapping.count();
	QList<mapstorage> aMappings;
	int i,j,iLine;
	bool bDetSection=true;

	result.setNoMap();
	if (iSrcWidth<1 || iSrcHeight<1) return result;
	for (iLine=1; pStart<pEnd; ++iLine, pStart=pEOL+1)
	{
		bool bCorrE=false;
		const QChar* p=NULL;
		for (pEOL=pStart; pEOL<pEnd; ++pEOL)
		{
			if (*pEOL=='\r' || *pEOL=='\n') break;
			if (p==NULL && *pEOL=='=') p=pEOL;
		}

		if (pStart>=pEOL) // ignore empty lines
			continue;

		if (*pStart=='#' || *pStart==';') // ignore comments
			continue;

		// parse section name
		if (p==NULL && (pStart+1)<pEOL && *pStart=='[' && *(pEOL-1)==']')
		{
			bDetSection=(QString::fromRawData(pStart,pEOL-pStart).compare("[DET_]",Qt::CaseInsensitive)==0);
			continue;
		}

		// ignore section other than "[DET_]" or not a <name>=<value> pair
		if (!bDetSection || p==NULL)
			continue;

		QString item=QString::fromRawData(pStart,p-pStart);
		item.remove(QRegExp("^[ \t]+"));
		item.remove(QRegExp("[ \t]+$"));
		if (item.isEmpty())
		{
			MSG_DEBUG << "mapping and correction: ignoring invalid line " << iLine;
			continue;
		}
		++p;

		QString value=QString::fromRawData(p,pEOL-p);
		value.remove(QRegExp("^[ \t]+"));
		value.remove(QRegExp("[ \t]+$"));

		QStringList tmp(value.split(QRegExp("[, \t]+")));
		if (!item.compare("corr",Qt::CaseInsensitive))
		{
			if (tmp.count()<4)
			{
				MSG_DEBUG << "mapping and correction: ignoring invalid line " << iLine;
				continue;
			}
			bCorrE=false;
		}
		else if (!item.compare("core",Qt::CaseInsensitive))
		{
			if (tmp.count()<5)
			{
				MSG_DEBUG << "mapping and correction: ignoring invalid line " << iLine;
				continue;
			}
			bCorrE=true;
		}
		else if (!item.compare("corz",Qt::CaseInsensitive))
		{
			// zlib compressed + base64/mime coded
			QByteArray abyIn=QByteArray::fromBase64(tmp.last().toLatin1());
			QByteArray abyOut;
			if (!CaressHelper::zlib_unzip(abyIn,abyOut))
			{
				MSG_DEBUG << "mapping and correction: ignoring invalid line " << iLine;
				continue;
			}
			tmp.last()=QString::fromLatin1(abyOut.constData(),abyOut.count());
			bCorrE=(tmp.count()>4);
		}
		else
		{
			MSG_DEBUG << "mapping and correction: ignoring invalid line " << iLine;
			continue;
		}

		// corr=id skip_first_values first_bin coded_length_array
		// core=id correction skip_first_values first_bin coded_length_array
		// coded_length_array: every character [0-9A-Za-z] is a length (0...61) without
		//                     separating blanks maps to how number of destination channels
		// (0..9,A..Z,a..z are for bin width values 0..9,10..35,36..61)
		//
		// We could describe bins by
		// skip_first_values number_of_first_bin lenght_bin0 length_bin1 ...
		// like
		// corr=13 5 10 2 2 3 2
		// expanded to a mapping vector for tube 13
		// raw channel                   10
		//  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14
		// mapped to
		// -1 -1 -1 -1 -1 10 10 11 11 12 12 12 13 13 -1 -1 -1 ...
		//
		// additional line like "corr/core" named "corz": the "coded_length_array" is
		// libz-compressed + base64(mime)-coded data

		mapstorage s;
		s.m_iTube=tmp.takeFirst().toInt();
		if (bCorrE)
			s.m_fCorrection=tmp.takeFirst().toDouble();
		else
			s.m_fCorrection=1.0;
		s.m_iSkipFirst=tmp.takeFirst().toInt();
		s.m_iFirstBin=tmp.takeFirst().toInt();
		if (tmp.count()==1)
		{
			// coded_length_array: without separators
			QString line(tmp.takeFirst());
			for (i=0; i<line.count(); ++i)
			{
				quint8 v=0;
				char c=line.at(i).toAscii();
				if (c>='0' && c<='9') v=c-'0'; else
					if (c>='A' && c<='Z') v=c-'A'+10; else
						if (c>='a' && c<='z') v=c-'a'+36;
				s.m_abyMapData.append(v);
			}
		}
		else
		{
			// length_array with separators
			while (!tmp.isEmpty())
				s.m_abyMapData.append(tmp.takeFirst().toInt());
		}

		int iTubeChannels=s.m_iSkipFirst;
		for (i=0; i<s.m_abyMapData.count(); ++i)
			iTubeChannels+=s.m_abyMapData[i];

		if (iDstHeight<s.m_iTube)
		{
			MSG_DEBUG << "mapping: increasing height from " << iDstHeight << " to " << s.m_iTube;
			iDstHeight=s.m_iTube;
		}
		if (iDstWidth<iTubeChannels)
		{
			MSG_DEBUG << "mapping: increasing width from " << iDstWidth << " to " << iTubeChannels;
			iDstWidth=iTubeChannels;
		}

		int iPos=0,iMin=0,iMax=aMappings.count()-1;
		bool bInsert=true;
		while (iMin<=iMax)
		{
			iPos=(iMin+iMax)/2;
			if (aMappings[iPos].m_iTube==s.m_iTube)
			{
				MSG_DEBUG << "overwriting mapping for tube " << s.m_iTube;
				aMappings[iPos]=s;
				bInsert=false;
				break;
			}
			if (aMappings[iPos].m_iTube<s.m_iTube)
				iMax=iPos-1;
			else
				iMin=++iPos;
		}
		if (bInsert)
			aMappings.insert(iPos,s);
	}
	if (iSrcWidth>iDstWidth || iSrcHeight>iDstHeight)
	{
		MSG_ERROR << "mapped size is greater than source:  source=" << iSrcWidth << '*' << iSrcHeight << "  mapped=" << iDstWidth << '*' << iDstHeight;
		return result;
	}

	// store size of mapped data
	result.initialize(iSrcWidth,iSrcHeight,MapCorrection::OrientationUpRev,MapCorrection::CorrectSourcePixel);
	result.setMappedRect(QRect(0,0,iDstWidth,iDstHeight));

	for (i=0; i<iSrcHeight; ++i)
	{
		mapstorage* pMap=NULL;
		for (j=aMappings.count()-1; j>=0; --j)
		{
			if (aMappings[j].m_iTube==i)
			{
				pMap=&aMappings[j];
				break;
			}
		}
		if (pMap!=NULL)
		{
			// fill mapping with read data
			int iSrcX=pMap->m_iFirstBin;
			int iDstX=pMap->m_iSkipFirst;
			for (j=0; j<pMap->m_abyMapData.count(); ++j)
			{
				QPoint pt(iDstX++,i);
				for (int iCount=pMap->m_abyMapData[j]; iCount>0; --iCount)
					result.map(QPoint(iSrcX++,i),pt,pMap->m_fCorrection);
			}
		}
		else
		{
			// generate linear (default) mapping
			for (j=0; j<iDstWidth; ++j)
			{
				int iStartX=(iSrcWidth*j)/iDstWidth;
				int iEndX=(iSrcWidth*(j+1))/iDstWidth;
				QPoint pt(j,i);
				while (iStartX<iEndX)
					result.map(QPoint(iStartX++,i),pt,1.0);
			}
		}
	}
	return result;
}

/*!
  \brief parse a mapping from CARESS
  \param[in] mapping    textual mapping line from CARESS
  \param[in] iSrcWidth  width of source histogram
  \param[in] iSrcHeight height of source histogram
  \param[in] iDstWidth  width of mapped histogram
  \param[in] iDstHeight height of mapped histogram
  \return new MapCorrection which may be used with QMesyDAQ
 */
MapCorrection CaressMapCorrectionV4::parseCaressMapCorrection(const QString& mapping, int iSrcWidth, int iSrcHeight, int iDstWidth, int iDstHeight)
{
	MapCorrection result;
//	QList<mapstorage> aMappings;

	result.setNoMap();
#if 0
	QHash<QString,QString> hsData;
	bool bDetSection=false;
	int i,j,iLine;
	const QChar* pEOL;
	const QChar* p;
	const QChar* pStart=mapping.constData();
	const QChar* pEnd=pStart+mapping.count();
	if (iSrcWidth<1 || iSrcHeight<1) return result;

	for (iLine=1; pStart<pEnd; ++iLine, pStart=pEOL+1)
	{
		bool bCorrE=false;
		p=NULL;
		for (pEOL=pStart; pEOL<pEnd; ++pEOL)
		{
			if (*pEOL=='\r' || *pEOL=='\n') break;
			if (p==NULL && *pEOL=='=') p=pEOL;
		}

		if (pStart>=pEOL) // ignore empty lines
			continue;

		if (*pStart=='#' || *pStart==';' || *pStart=='*') // ignore comments
			continue;

		if (p==NULL) // ignore other lines
			continue;

		QString item=QString::fromRawData(pStart,p-pStart);
		item.remove(QRegExp("^[ \t]+"));
		item.remove(QRegExp("[ \t]+$"));
		if (item.isEmpty())
		{
			MSG_DEBUG << "mapping and correction: ignoring invalid line " << iLine;
			continue;
		}
		++p;

		QString value=QString::fromRawData(p,pEOL-p);
		value.remove(QRegExp("^[ \t]+"));
		value.remove(QRegExp("[ \t]+$"));

		if (item.compare("Type",Qt::CaseInsensitive)==0 ||
			item.compare("Version",Qt::CaseInsensitive)==0 ||
			item.compare("SizeX",Qt::CaseInsensitive)==0 ||
			item.compare("SizeY",Qt::CaseInsensitive)==0)
			hsData[item.toLower()]=value;

		if (item.compare("EndOfPreHeader",Qt::CaseInsensitive)==0 ||
			item.compare("EndOfMapping",Qt::CaseInsensitive)==0)
			bDetSection=false;

		if (item.compare("Type",Qt::CaseInsensitive)==0)
		{
			if ((value.compare("HZB-V4-Listmode",Qt::CaseInsensitive)==0 && iType==CARESSMAP_V4_LISTMODE) ||
				(value.compare("HZB-V4-CARESS",Qt::CaseInsensitive)==0 && iType==CARESSMAP_V4_CARESS))
				bDetSection=true;
			else
			{
				MSG_WARNING << "invalid V4 mapping type" << value;
				bDetSection=false;
				hsData.clear();
			}
		}

		// ignore other data
		if (!bDetSection || p==NULL)
			continue;
		if (!item.startsWith("Tube",Qt::CaseInsensitive))
			continue;
		if (!hsData.contains("version") || !hsData.contains("sizex") || !hsData.contains("sizey") ||
			hsData.value("version").toInt()!=1 || hsData.value("sizex").toInt()!=iDstWidth ||
			hsData.value("sizey").toInt()!=iDstHeight)
		{
			MSG_WARNING << "missing/invalid info in V4 mapping before tube definitions";
			bDetSection=false;
			hsData.clear();
			continue;
		}

// Type=HZB-V4-Listmode
// Version=1
// SizeX=1024
// SizeY=112
// * TrueY (0..111), MCPD8 (0..1), ModID (0..6), SlotID (0..7), Start(mm), Step(mm/pixel), MIME-String
// Tube0=   111, 0, 0, 0, 173.85689064, 0.68850152, eNrlwklagQEAAFDHMAtJA0VKRVFppJDMSe5/id/Oqrez870XhIKtCW9EIAoxikMCkpCCPUhDBrKwDzk4gDwcwhEcwwkUoAindAYlKMM5VOACLqEKV3ANN1CDOtzCHTSgCff0AI/Qgid4hhd4hTd4hzZ04AM+oQs96MMXDOgbhjCCMUxgCjOYww8s4BeW8AerfwU7YA3/lGSb
// Tube1=   110, 0, 0, 1, 175.31668672, 0.68752480, eNrlwklagQEAAFDHMAtJA0VKRVFppJDMSe5/id/Oqrez870XhIKtCG9EIAoxiFMCkpCCPUhDBrKwDzk4gDwcwhEdwwkUoAincAYlKMM5VOACLqEKV3QNN1CDOtzCHTSgCffwAI/Qgid4phd4hTd4hzZ04AM+oQs96MMXDOCbhjCCMUxgCjOYww8s4BeW8AerfwU7YQ2Z7WPV
// Tube2=   109, 0, 0, 2, 175.54665274, 0.68630572, eNrlwklagQEAAFDHMAtJA0VKRVFppJDMSe5/id/Oqrez870XhIKtCG9EIAoxiEMCkpSCPUhDBrKwDzk4gDwc0hEcwwkUoAincAYlKMM5VOgCLqEKV3ANN1CDOtzCHTSoCffwAI/Qgid4hhd4hTd6hzZ04AM+oQs96MMXDOCbhjCCMUxgCjOYww8s4BeW9AerfwQ7YA2xEmMS
// Tube3=   108, 0, 0, 3, 175.22534637, 0.68615242, eNrdwklagQEAAFDHMAtJA0VKRVFppJDMSe5/id/Gsrez8r0XhIItCW9EKAoxiEMCkpCCPUhDBrK0Dzk4gDwcwhEcwwkUoEincAYlKMM5VOACLqEKV3BNN1CDOtzCHTSgCffwAI/Ugid4hhd4hTd4hzZ04IM+oQs96MMXDOAbhjCCMUxoCjOYww8s4BeW8AerfwU7bw1p7GMw
// Tube4=   107, 0, 0, 4, 176.13016385, 0.68557234, eNrlwklagQEAAFDHMAtJA0VKRVFppJDMSe5/id/Oqrez870XhIItCG9EIAoxikMCkpCCPUhDBrKwDzk6gDwcwhEcwwkUoAindAYlKMM5VOACLqEKV3QNN1CDOtzCHTSgCffwQI/Qgid4hhd4hTd4hzZ14AM+oQs96MMXDOAbhjSCMUxgCjOYww8s4JeW8AerfwQ7YA1JQmJE
// Tube5=   106, 0, 0, 5, 175.64076959, 0.68583750, eNrdwklagQEAAFDHMAtJA0VKRVFppJDMSe5/id/Oqrez8r0XhIKtCG9EIAoxiEMCkpSCPUhDBrKwDzk4gDwc0hEcwwkUoAincAYlKMM5VeACLqEKV3ANN1CDOtzSHTSgCffwAI/Qgid4hhd6hTd4hzZ04AM+oQs96sMXDOAbhjCCMUxgCjOaww8s4BeW8AerfwU7bw3+/2Jl
// Tube6=   105, 0, 0, 6, 175.42846641, 0.68619188, eNrdwklagQEAAFDHMAtJA0VKRVFppJDMSe5/id/Oqrez8r0XhIKtCG9EIAoxiFMCkpCCPUhDBrKwDzk4oDwcwhEcwwkUoAincAYlKNM5VOACLqEKV3ANN1CDOt3CHTSgCffwAI/Qgid4hhd6hTd4hzZ04AM+oQs96NMXDOAbhjCCMUxgCjOY0w8s4BeW8AerfwU7bw3eL2Jh
// Tube7=   104, 0, 0, 7, 173.82512883, 0.68591958, eNrdwklagQEAAFDHMAtJA0VKRVFppJDMSe5/id/Oqrez8r0XhIItCm9EIAoxiEMCkpCCPUhDhrKwDzk4gDwcwhEcwwkUqAincAYlKMM5VOACLqFKV3ANN1CDOtzCHTSgCff0AI/Qgid4hhd4hTd4hzZ14AM+oQs96MMXDOAbhjSCMUxgCjOYww8s4BeW9AerfwQ7bQ2bJGMi
// Tube8=   103, 0, 1, 0, 175.93056304, 0.68506321, eNrdwklagQEAAFDHMAtJA0VKRVFppJDMSe5/id/Oqrez8r0XhIKtCG9EIAoxiEMCkpCCPUhTBrKwDzk4gDwcwhEc0wkUoAincAYlKMM5VeACLqEKV3ANN1CDOt3CHTSgCffwAI/Qgid6hhd4hTd4hzZ04IM+oQs96MMXDOAbhjCiMUxgCjOYww8s4JeW8AerfwQ7bw0CwGJd
// Tube9=   102, 0, 1, 1, 175.48527476, 0.68478457, eNrdwklagQEAAFDHMAtJA0VKRVFppJDMSe5/id/Oqrez8r0XhIKtCG9EIAoxikMCkpCCPUhDBrK0Dzk4gDwcwhEcwwkUqAincAYlKMM5VOCCLqEKV3ANN1CDOtzSHTSgCffwAI/Qgid4phd4hTd4hzZ04AM+qQs96MMXDOAbhjCiMUxgCjOYww8s4JeW8AerfwQ7bg2EKmGC
// Tube10=  101, 0, 1, 2, 175.56079544, 0.68330013, eNrdwklagQEAAFDHMAtJA0VKRVFppJDMSe5/id/Oqrez8r0XhIKtCG9EIAoxikMCkpCCPUhDhrKwDzk4gDwc0hEcwwkUoAincEYlKMM5VOACLqFKV3ANN1CDOtzCHTWgCffwAI/Qgid6hhd4hTd4hzZ14AM+oQs96MMXDeAbhjCCMUxgSjOYww8s4BeW8EerfwQ7bA2jpmAD
// Tube11=  100, 0, 1, 3, 175.88322585, 0.68255498, eNrdwklagQEAAFDHMAtJA0VKRVFppJDMSe5/id/Oqrez8r0XhIKtCG9EIAoxiEOCkpCCPUhDBrK0Dzk4gDwcwhEc0wkUoAincAYlKsM5VOACLqFKV3ANN1CDOtzSHTSgCffwAI/Qoid4hhd4hTd4pzZ04AM+oQs96sMXDOAbhjCiMUxgCjOYww8s6BeW8AerfwU7aw0pql9f
// ...
// Tube13=   98, 0, 1, 5, 190.64806660, 0.68065643, eNrlwllagQEAAMCOIQlZk6IoW2inFZE9y/0v8XvryZygbyY4CCj05xDCdAQROIYoxCgOJ5CAJKQgTRnIwink4IzycA4XUIAiXcIVlKAM13QDFahCDerUgFtoQgvadAf38ACP8ETP8AId6MIrvNE7fMAnfEGP+jCAbxjCiH5gDBOYwozmsIAl/MKK1rCB7V7BP7cDr1ddDg==
// Tube14=   97, 0, 1, 6,  68.92051198, 0.93769047, eNrdwjNyQAEUAMC0sW3btm3b9v2rnyZF5u0NMrtJSvJH6q80pCMDmUEWspGDXOQhHwVBIYpQjBKUogzlQQUqUYVq1KAWdUE9GtCIJjSjJWhFG9rRgU50oTvoQS/60I8BDGIoGMYIRjGGcUxgMpjCNGYwiznMBwtYxBKWsYJVrAXr2MAmtrCNHewGe9jHAQ5xhGOcBKc4wzkucImr4Bo3uMUd7vGAx+AJz3jBK97wjo/gE1/4DpJ/7Qdg4Eq1
// Tube15=   96, 0, 1, 7,  66.41857335, 0.94118533, eNrdwjN2RQEUAMC0sW3+2LZt2+b+q5cm1Z0d5MwkaQnS/2QgM8hCNnKQi7wgHwUoRBGKURKUogzlqEBlUIVq1KAWdagPGtCIJjSjJUihFW1oRwc6gy50owe96Av6MYBBDGEYI8EoxjCOCUwGU5jGDGYxh/lgAYtYwjJWglWsYR0b2MRWsI0d7GIP+8EBDnGEY5zgNDjDOS5wiavgGje4xR3u8RA84gnPeMFr8IZ3fOATX/gOfpD8a7/FSk01
// ...
// Tube35=   76, 0, 4, 3,  68.59219802, 0.93369810, eNrdwjNyQAEUAMC0sW3btm3b9v2rnyZF5u0NMrtJSvJH6q80pCMDmchCNnKQi7wgHwUoRBGKUYJSlKE8qEAlqlCNGtSiDvVoCBrRhGa0oBVtaEcHOtEVdKMHvehDPwYwiCEMByMYxRjGMYFJTGEaM8Es5jCPBSxiCctYwWqwhnVsYBNb2MYOdrEX7OMAhzjCMU5wijOcBxe4xBWucYNb3OEeD8EjnvCMF7ziDe/4wGfwhW8k/9AP2BxG+Q==
// Tube36=   75, 0, 4, 4,  -5.77237632, 1.08900432, eNrdwUNyAAEAAMHYtm3btm3b5vM3p6Rq5gnpDkKCv79CMUzDMUIjNQqjNUZjMU7jMUETNQmTNUVTMU3TMUMzNQuzNUdzMU/zsUALtQiLtURLsUzLsUIrtQqrtQZrtU7rsUEbtQmbtQVbtU3bsUM7tQu7tQd7tU/7cUAHdQiHdQRHdUzHcUIndQqndQZndU7ncUEXcUmXdQVXdU3XcUM3cUu3dQd3dU/38UAP8UiP9QRP9UzP8UIv8Uqv9QZv9U7v8UEf8Umf9QVf9Q3f9UM/8Uu/NfiXfwBrREVi
// ...
// Tube55=   56, 0, 6, 7, -10.73607154, 1.09586639, eNrdwUNiAAEAwMDatm3btm3b5vO3px6S/qAzQUjw569QDcNwjcBIjdJojNFYjNN4TNBETcJkTcFUTcN0zdBMzNJszNFczNN8LcBCLcJiLcFSLdNyrNBKrNJqrNFarcN6bcBGbcJmbdFWbNN27NBO7cJu7cFe7cN+HdBBHNJhHNFRHNNxncBJncJpncFZndN5XNBFXNJlXNFVXcN13cBN3cJt3dFd3NN9PNBDPNJjPcFTPcNzvcBLvdJrvNFbvNN7fNBHfcJnfcFXfdN3/NBP/NJvDP7tHwQ2Sak=
// Tube56=   55, 1, 0, 0,  -6.06349984, 1.08825770, eNrdwUNyAAEAAMHYtm3btm3b5vM3p6Rq5gnpDkKCv79CNQzDNQIjNUqjMUZjNQ7jNQETNUmTMUVTNQ3TNQMzNUuzMUdzNQ/ztQALtUiLsURLtQzLtQIrtUqrsUZrtQ7rtQEbtUmbsUVbtQ3btQM7tUu7sUd7tQ/7dQAHdUiHcURHdQzHdUIncUqncUZndQ7ndUEXcUmXcUVXdQ3XdUM3cUu3cUd3dQ/39UAP8UiP8URP9QzP9UIv8Uqv8UZv9Q7v9UEf8Umf8UVf9Q3f9UM/8Uu/MfiXfwBkUUUd
// ...
// Tube75=   36, 1, 2, 3,  -4.79221838, 1.08712195, eNrdwUNiAAEAwMDatm3btm3b5vO3l/aQPKEzQUjw+08ohmm4RmCkRmk0xmgsxmm8JmCiJmkypmiqpmG6ZmCmZmk25miu5mG+FmghFmkxlmiplmG5VmglVmk11mit1mG9NmgjNmmztmCrtmG7dmgndmm39mCv9mk/DuggDumwjuCojuk4TuikTuG0zuCszuk8LuiiLuGyrugqruk6buimbuG27ugu7uk+HuihHuGxnugpnum5XuClXuG13ugt3um9PuCjPukzvugrvum7fuCnfuk3Bv/yDyHLRHA=
// Tube76=   35, 1, 2, 4,  66.46801777, 0.93913468, eNrdwjN2RQEUAMC0sW3+2LZt2+b+q5cm1Z0d5MwkaQnS/2QgM8hCNnKQizzkBwUoRBGKUYLSoAzlqEAlqlAd1KAWdahHAxqDJjSjBSm0oi1oRwc60YVu9KA36EM/BjCIIQwHIxjFGMYxgclgCtOYwSzmMB8sYBFLWMYKVoM1rGMDm9jCNnaCXexhHwc4xFFwjBOc4gznuAgucYVr3OAWd8E9HvCIJzzjJXjFG97xgU98Bd/4QfJv/QKXVku0
// ...
// Tube93=   18, 1, 4, 5,  67.55520144, 0.93736525, eNrdwkNyQAEUALBua9u2bdu27fuvfned6csNOkmSkvyR+istSEcGMpGFbOQgN8hDPgpQiCIUoyQoRRnKUYFKVAXVqEEt6lCPBjQGTWhGC1rRhnZ0BJ3oQjd60Is+9AcDGMQQhjGCUYwF45jAJKYwjRnMBnOYxwIWsYRlrASrWMM6NrCJLWwHO9jFHvZxgMPgCMc4wSnOcI6L4BJXuMYNbnGH++ABj3jCM17wirfgHR/4xBe+g+Tf+gFFkEnn
// Tube94=   17, 1, 4, 6, 186.80396023, 0.68232838, eNrlwllWQQEAAFDLMAtJA0VKRVFppJDMSfa/iefPV/cc/869QSjYSngjAlGIQRwSlIQU7EEaMpClfcjBAeThEI7oGE6gAEU4hTMqQRnOoQIXcElVuIJruIEa1OGW7qABTbiHB3ikFjzBM7zAK7zRO7ShAx
// ...
// Tube110=   1, 1, 6, 6, 187.82938961, 0.68230510, eNrlwllWQQEAAFDLMAtJA0VKRVFppJDMSfa/iefPV/dYgHNvEAq2Cm9EIEoxiEMCkpCCPUpDBrKwDzk4oDwcwhEcwwkUqAincAYlKMM5VeACLqEKV3BNN1CDOtzCHTSoCffwAI/Qgid4phd4hTd4hzZ06AM+oQs96MMXDeAbhjCCMUxoCjOYww8s4JeW8AerfwU7bA24Bl6R
// Tube111=   0, 1, 6, 7, 187.70750324, 0.68273367, eNrlwllWQQEAAFDLMAtJA0VKRVFppJDMSfa/iefHZ/cc/869QSjYQngjQlGIQRwSkIQU7UEaMpCFfcjBAeXhEI7gGE6gQEU4hTMoQRnOoUIXcAlVuIJruKEa1OEW7qABTbqHB3iEFjzBM7zQK7zBO7ShAx/0CV3oQR++YEDfMIQRjGECU5jRHH5gAb+whD9a/SPYaWt8LGAf
// EndOfPreHeader
		mapstorage s;
		bool bOK=false;
		s.m_iTube=item.remove(0,4).trimmed().toInt(&bOK);
		if (!bOK || s.m_iTube<0 || s.m_iTube>=iSrcHeight)
		{
			MSG_ERROR << "invalid mapping in line" << iLine;
			return result;
		}
//		aMappings.append(
#warning "TODO: V4"
	} // for (iLine=1; pStart<pEnd; ++iLine, pStart=pEOL+1)

	if (iSrcWidth>iDstWidth || iSrcHeight>iDstHeight)
	{
		MSG_ERROR << "mapped size is greater than source:  source=" << iSrcWidth << '*' << iSrcHeight << "  mapped=" << iDstWidth << '*' << iDstHeight;
		return result;
	}

	// store size of mapped data
	result.initialize(iSrcWidth,iSrcHeight,MapCorrection::OrientationUpRev,MapCorrection::CorrectSourcePixel);
	result.setMappedRect(QRect(0,0,iDstWidth,iDstHeight));

	for (i=0; i<iSrcHeight; ++i)
	{
		mapstorage* pMap=NULL;
		for (j=aMappings.count()-1; j>=0; --j)
		{
			if (aMappings[j].m_iTube==i)
			{
				pMap=&aMappings[j];
				break;
			}
		}
		if (pMap!=NULL)
		{
			// fill mapping with read data
			int iSrcX=pMap->m_iFirstBin;
			int iDstX=pMap->m_iSkipFirst;
			for (j=0; j<pMap->m_abyMapData.count(); ++j)
			{
				QPoint pt(iDstX++,i);
				for (int iCount=pMap->m_abyMapData[j]; iCount>0; --iCount)
					result.map(QPoint(iSrcX++,i),pt,pMap->m_fCorrection);
			}
		}
		else
		{
			// generate linear (default) mapping
			for (j=0; j<iDstWidth; ++j)
			{
				int iStartX=(iSrcWidth*j)/iDstWidth;
				int iEndX=(iSrcWidth*(j+1))/iDstWidth;
				QPoint pt(j,i);
				while (iStartX<iEndX)
					result.map(QPoint(iStartX++,i),pt,1.0);
			}
		}
	}
#endif
	return result;
}
