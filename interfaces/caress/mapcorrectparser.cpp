/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009 by Jens Krüger <jens.krueger@frm2.tum.de>          *
 *   Copyright (C) 2011 by Lutz Rossa <rossa@helmholtz-berlin.de>          *
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

////////////////////////////////////////////////////////////////////////////
// $$HeadURL$$
//
// last change:
// $$Author$$
// $$Date$$
// revision $$Rev$$
////////////////////////////////////////////////////////////////////////////

// corr=id skip_first_values first_bin coded_length_array
// corre=id correction skip_first_values first_bin coded_length_array
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

#include <stdio.h>
#include <stdlib.h>
#include <QRegExp>
#include <QDateTime>
#include <QStringList>
#include <zlib.h>
#include "mapcorrectparser.h"

#ifdef DEBUGBUILD
#define DBG0(fmt) do { char now_s[64]; QDateTime now_q(QDateTime::currentDateTime()); \
  snprintf(now_s,ARRAY_SIZE(now_s),"%04d/%02d/%02d %02d:%02d:%02d.%03d",now_q.date().year(),now_q.date().month(), \
	   now_q.date().day(),now_q.time().hour(),now_q.time().minute(),now_q.time().second(),now_q.time().msec()); \
  now_s[ARRAY_SIZE(now_s)-1]='\0'; qDebug("[%s] %s(%d): " fmt,now_s,__FILE__,__LINE__); } while (0)
#define DBG(fmt,args...) do { char now_s[64]; QDateTime now_q(QDateTime::currentDateTime()); \
  snprintf(now_s,ARRAY_SIZE(now_s),"%04d/%02d/%02d %02d:%02d:%02d.%03d",now_q.date().year(),now_q.date().month(), \
	   now_q.date().day(),now_q.time().hour(),now_q.time().minute(),now_q.time().second(),now_q.time().msec()); \
  now_s[ARRAY_SIZE(now_s)-1]='\0'; qDebug("[%s] %s(%d): " fmt,now_s,__FILE__,__LINE__,args); } while (0)
#define CRITICAL0(fmt) do { char now_s[64]; QDateTime now_q(QDateTime::currentDateTime()); \
  snprintf(now_s,ARRAY_SIZE(now_s),"%04d/%02d/%02d %02d:%02d:%02d.%03d",now_q.date().year(),now_q.date().month(), \
	   now_q.date().day(),now_q.time().hour(),now_q.time().minute(),now_q.time().second(),now_q.time().msec()); \
  now_s[ARRAY_SIZE(now_s)-1]='\0'; qCritical("[%s] %s(%d): " fmt,now_s,__FILE__,__LINE__); } while (0)
#define CRITICAL(fmt,args...) do { char now_s[64]; QDateTime now_q(QDateTime::currentDateTime()); \
  snprintf(now_s,ARRAY_SIZE(now_s),"%04d/%02d/%02d %02d:%02d:%02d.%03d",now_q.date().year(),now_q.date().month(), \
	   now_q.date().day(),now_q.time().hour(),now_q.time().minute(),now_q.time().second(),now_q.time().msec()); \
  now_s[ARRAY_SIZE(now_s)-1]='\0'; qCritical("[%s] %s(%d): " fmt,now_s,__FILE__,__LINE__,args); } while (0)
#else
#define DBG0(fmt)             do {} while (0)
#define DBG(fmt,args...)      do {} while (0)
#define CRITICAL0(fmt)        do {} while (0)
#define CRITICAL(fmt,args...) do {} while (0)
#endif

#define ARRAY_SIZE(x) ((int)(sizeof(x)/sizeof((x)[0])))

class mapstorage {
public:
  mapstorage() : m_iTube(-1), m_fCorrection(1.0), m_iSkipFirst(0), m_iFirstBin(0) {}
  mapstorage(const mapstorage& src) : m_iTube(src.m_iTube), m_fCorrection(src.m_fCorrection),
    m_iSkipFirst(src.m_iSkipFirst), m_iFirstBin(src.m_iFirstBin), m_abyMapData(src.m_abyMapData) {}
  mapstorage& operator=(const mapstorage& src)
    { m_iTube=src.m_iTube; m_fCorrection=src.m_fCorrection; m_iSkipFirst=src.m_iSkipFirst;
      m_iFirstBin=src.m_iFirstBin; m_abyMapData=src.m_abyMapData; return *this; }

  int m_iTube;
  float m_fCorrection;
  int m_iSkipFirst;
  int m_iFirstBin;
  QByteArray m_abyMapData;
};

MapCorrection parseCaressMapCorrection(const char* pMapping, int iLength, int iSrcWidth, int iSrcHeight, int iDstWidth, int iDstHeight)
{
  QString mapping=QString::fromLatin1(pMapping,iLength);
  return parseCaressMapCorrection(mapping,iSrcWidth,iSrcHeight,iDstWidth,iDstHeight);
}

MapCorrection parseCaressMapCorrection(const QString& mapping, int iSrcWidth, int iSrcHeight, int iDstWidth, int iDstHeight)
{
  MapCorrection result;
  const QChar* pStart=mapping.constData();
  const QChar* pEOL;
  const QChar* pEnd=pStart+mapping.count();
  const QChar* p;
  QList<mapstorage> aMappings;
  int i,j,iLine;
  bool bDetSection=true;

  result.setNoMap();
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

    if (*pStart=='#' || *pStart==';') // ignore comments
      continue;

    // parse section name
    if (p==NULL && (pStart+1)<pEOL && *pStart=='[' && *(pEOL-1)==']')
    {
      bDetSection=(QString::fromRawData(pStart,pEOL-pStart).compare("[DET_]",Qt::CaseInsensitive)==0);
      continue;
    }

    // ignore section other than "[DET_]"
    if (!bDetSection)
      continue;

    QString item=QString::fromRawData(pStart,p-pStart);
    item.remove(QRegExp("^[ \t]+"));
    item.remove(QRegExp("[ \t]+$"));
    if (item.isEmpty())
    {
      DBG("mapping and correction: ignoring invalid line %d",iLine);
      continue;
    }
    ++p;

    QString value=QString::fromRawData(p,pEOL-p);
    value.remove(QRegExp("^[ \t]+"));
    value.remove(QRegExp("[ \t]+$"));
    ++pEOL;

    QStringList tmp(value.split(QRegExp("[, \t]+")));
    if (!item.compare("corr",Qt::CaseInsensitive))
    {
      if (tmp.count()<4)
      {
	DBG("mapping and correction: ignoring invalid line %d",iLine);
	continue;
      }
      bCorrE=false;
    }
    else if (!item.compare("corre",Qt::CaseInsensitive))
    {
      if (tmp.count()<5)
      {
	DBG("mapping and correction: ignoring invalid line %d",iLine);
	continue;
      }
      bCorrE=true;
    }
    else if (!item.compare("corrz",Qt::CaseInsensitive))
    {
      // zlib compressed + base64/mime coded
      QByteArray abyIn=QByteArray::fromBase64(tmp.last().toLatin1());
      QByteArray abyOut;
      z_stream strm;
      int iSize=1024;

      if (abyIn.isEmpty())
      {
	DBG("mapping and correction: ignoring invalid line %d",iLine);
	continue;
      }

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
	DBG("mapping and correction: ignoring invalid line %d",iLine);
	continue;
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
      {
	DBG("mapping and correction: ignoring invalid line %d",iLine);
	continue;
      }

      tmp.last()=QString::fromLatin1(abyOut.constData(),abyOut.count());
      bCorrE=(tmp.count()>4);
    }
    else
    {
      DBG("mapping and correction: ignoring invalid line %d",iLine);
      continue;
    }

    // corr=id skip_first_values first_bin coded_length_array
    // corre=id correction skip_first_values first_bin coded_length_array
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
    // additional line like "corre" named "corrz": the "coded_length_array" is
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
      DBG("mapping: increasing height from %d to %d",iDstHeight,s.m_iTube);
      iDstHeight=s.m_iTube;
    }
    if (iDstWidth<iTubeChannels)
    {
      DBG("mapping: increasing width from %d to %d",iDstWidth,iTubeChannels);
      iDstWidth=iTubeChannels;
    }

    int iPos=0,iMin=0,iMax=aMappings.count()-1;
    bool bInsert=true;
    while (iMin<=iMax)
    {
      iPos=(iMin+iMax)/2;
      if (aMappings[iPos].m_iTube==s.m_iTube)
      {
	DBG("overwriting mapping for tube %d",s.m_iTube);
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
    CRITICAL("mapped size is greater than source:  source=%d*%d  mapped=%d*%d",iSrcWidth,iSrcHeight,iDstWidth,iDstHeight);
    Q_ASSERT(iSrcWidth<=iDstWidth && iSrcHeight<=iDstHeight);
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
