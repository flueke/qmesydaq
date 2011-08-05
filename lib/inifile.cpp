/***************************************************************************
 *   Copyright (C) 2004-2011 by Lutz Rossa <rossa@helmholtz-berlin.de>     *
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
/*****************************************************************************
 * $$HeadURL$$
 *
 * file:            inifile.cpp
 * authors:         Lutz Rossa <rossa@helmholtz-berlin.de>
 *
 * last changed by: $$Author$$
 * last update:     $$Date$$
 * revision:        $$Rev$$
 ****************************************************************************/
#include "inifile.h"
#include <math.h>

//********************************************************************
//********************************************************************
// CConfigItem
//********************************************************************
//********************************************************************

// Standardkonstruktor
CConfigItem::CConfigItem(const QString& Name, const QString& Value, int Position, bool HasLineBefore)
{
  m_aszComments.clear();
  m_aszValues.clear();
  m_bHasLineBefore=HasLineBefore;
  m_szName=Name;
  m_iPosition=Position;
  m_pSection=NULL;

  if (!Value.isEmpty())
    SetValue(Value);
}

// Kopierkonstruktor
CConfigItem::CConfigItem(const CConfigItem &src) : QObject(), m_pSection(NULL)
{
  *this=src;
}

// interner Konstruktor
CConfigItem::CConfigItem(CConfigSection *pSection)
{
  m_aszComments.clear();
  m_aszValues.clear();
  m_bHasLineBefore=false;
  m_szName.clear();
  m_iPosition=0x7FFFFFFF;
  m_pSection=pSection;
}

// Destruktor
CConfigItem::~CConfigItem()
{
  EmptyItem();
}

// erlaubt Kopiervorgänge des C++-Datentyps "CConfigItem"
CConfigItem& CConfigItem::operator=(const CConfigItem &src)
{
  m_aszComments=src.m_aszComments;
  m_aszValues=src.m_aszValues;
  m_bHasLineBefore=src.m_bHasLineBefore;
  m_szName=src.m_szName;
  m_iPosition=src.m_iPosition;
//m_pSection wird nicht verändert
  return *this;
}

// gesamten Inhalt des Items löschen
void CConfigItem::EmptyItem()
{
  m_aszComments.clear();
  m_aszValues.clear();
}

// gesamten Wertestring lesen
bool CConfigItem::GetValue(QString *Value) const
{
  if (!Value) return false;

  Value->clear();
  for (int iCount=0; iCount<m_aszValues.count(); iCount++)
  {
    QString szValueItem(m_aszValues[iCount]);
    QString szTemp;
    bool bExtendedString=false;

    for (int iPos=0; iPos<szValueItem.count(); iPos++)
    {
      char c=szValueItem[iPos].toLatin1();
      switch (c)
      {
#ifdef CONFIG_USE_C_SYNTAX
	case '\r': szTemp+="\\r";  bExtendedString=true; break;
	case '\n': szTemp+="\\n";  bExtendedString=true; break;
	case '\t': szTemp+="\\t";  bExtendedString=true; break;
	case '\f': szTemp+="\\f";  bExtendedString=true; break;
	case '\\': szTemp+="\\\\"; bExtendedString=true; break;
	case ',':
#endif
	case ';':
	case '[':
	case ']':
	case ' ': szTemp+=c; bExtendedString=true; break;
	default:
	  if (((c>=0) && (c<32)) || (c=='\"'))
#ifdef CONFIG_USE_C_SYNTAX
	  {
	    QString szTemp2;
	    szTemp2.sprintf("\\x%02x",c);
	    szTemp+=szTemp2;
	    bExtendedString=true;
	  }
#else
	    szTemp+='_';
#endif
	  else
	    szTemp+=c;
	  break;
      }
    }

    if (bExtendedString)
    {
      szTemp.insert(0,'\"');
      szTemp+='\"';
    }

    if (iCount>0) *Value+=" ";
    *Value+=szTemp;
  }

  return true;
}

// gesamten Wertestring lesen
bool CConfigItem::GetValue(char *ValueBuffer, int Length) const
{
  QString szValue;

  if ((!ValueBuffer) || (!GetValue(&szValue))) return false;
  if (szValue.count()>=Length) return false;

  strcpy(ValueBuffer,szValue.toLatin1().constData());
  return true;
}

// einzelnen Teilwert als String lesen
CConfigItem::STATUSINFO CConfigItem::GetStringValue(QString* Value, int Index) const
{
  if (!Value) return ILLEGALVALUE;

  if ((Index>=0) && (Index<m_aszValues.count()))
  {
    *Value=m_aszValues[Index];
    return Value->isEmpty()?EMPTYVALUE:SUCCESS;
  }
  if (Index==0) // wenn kein Wert gespeichert wurde, einen virtuellen Leerstring zurückgeben
  {
    Value->clear();
    return EMPTYVALUE;
  }
  return ILLEGALVALUE;
}

// einzelnen Teilwert als String lesen
CConfigItem::STATUSINFO CConfigItem::GetStringValue(char *ValueBuffer, int Length, int Index) const
{
  if ((!ValueBuffer) || (Length<2)) return ILLEGALVALUE;

  QString temp;
  STATUSINFO iRet=GetStringValue(&temp,Index);
  if (iRet==SUCCESS || iRet==EMPTYVALUE)
  {
    if (temp.count()+1>Length) temp.remove(Length,temp.count()-Length);
    strcpy(ValueBuffer,temp.toLatin1().constData());
    if (temp.isEmpty()) iRet=EMPTYVALUE;
  }
  return iRet;
}

// einzelnen Teilwert als BOOLEAN lesen
CConfigItem::STATUSINFO CConfigItem::GetBooleanValue(bool *Value, int Index) const
{
  QString szValue;

  if (!Value) return ILLEGALVALUE;
  register STATUSINFO iRet=GetStringValue(&szValue,Index);
  if (iRet!=SUCCESS) return iRet;

  szValue.remove(QRegExp("^[ \t]+"));
  szValue.remove(QRegExp("[ \t]+$"));
  szValue=szValue.toLower();

  if ((szValue=="true") || (szValue=="yes"))
  {
    *Value=true;
    return SUCCESS;
  }
  if ((szValue=="false") || (szValue=="no"))
  {
    *Value=false;
    return SUCCESS;
  }

  return ILLEGALVALUE;
}

// einzelnen Teilwert als LONG lesen
CConfigItem::STATUSINFO CConfigItem::GetLongValue(long *Value, int Index) const
{
  double dblValue;
  STATUSINFO iRet=GetDoubleValue(&dblValue,Index);
  if (iRet!=SUCCESS) return iRet;
  if ((dblValue>=-2147483648.0) && (dblValue<2147483648.0) && (floor(dblValue)==dblValue))
  {
    *Value=(long)dblValue;
    return SUCCESS;
  }
  else
    return ILLEGALVALUE;
}

// einzelnen Teilwert als U_LONG lesen
CConfigItem::STATUSINFO CConfigItem::GetULongValue(unsigned long *Value, int Index) const
{
  double dblValue;
  STATUSINFO iRet=GetDoubleValue(&dblValue,Index);
  if (iRet!=SUCCESS) return iRet;
  if ((dblValue>=0.0) && (dblValue<4294967296.0) && (floor(dblValue)==dblValue))
  {
    *Value=(unsigned long)dblValue;
    return SUCCESS;
  }
  else
    return ILLEGALVALUE;
}

// einzelnen Teilwert als DOUBLE lesen
CConfigItem::STATUSINFO CConfigItem::GetDoubleValue(double *Value, int Index) const
{
  QString szValue;

  if (!Value) return ILLEGALVALUE;
  register STATUSINFO iRet=GetStringValue(&szValue,Index);
  if (iRet!=SUCCESS) return iRet;

  szValue.remove(QRegExp("^[ \t]+"));
  szValue.remove(QRegExp("[ \t]+$"));

  bool bOK=false;
  double dblValue=szValue.toDouble(&bOK);
  if (bOK)
  {
    *Value=dblValue;
    iRet=SUCCESS;
  }
  else
    iRet=szValue.isEmpty() ? EMPTYVALUE : ILLEGALVALUE;

  return iRet;
}

// Name des Elements setzen
void CConfigItem::SetName(const QString& Name)
{
  QString szName(Name);
  szName.remove(QRegExp("^[ \t]+"));
  szName.remove(QRegExp("[ \t]+$"));
  m_szName=szName;
}

// gesamten Wertestring schreiben
void CConfigItem::SetValue(const QString& Value)
{
  if (!Value.isEmpty())
  {
    bool bString=false;
    int iStartPos=0;

    m_aszValues.clear();

    QString szValue(Value);
    szValue+=' ';
    for (int iPos=0; iPos<szValue.count(); iPos++)
    {
      switch (szValue[iPos].toLatin1())
      {
	case '\"': // Strings können durch Anführungszeichen eingeschlossen sein (bei Sonderzeichen)
	  bString=!bString;
	  break;
	case ' ': // Teilwerte werden durch Space getrennt angegeben
	  if (!bString)
	  {
	    QString Temp(szValue.mid(iStartPos,iPos-iStartPos));

	    // führende und nachfolgende Leerzeichen abtrennen
	    Temp.remove(QRegExp("^[ \t]+"));
	    Temp.remove(QRegExp("[ \t]+$"));

	    // führende und nachfolgende Anführungszeichen abtrennen
	    if (Temp.left(1)==QString("\""))
	    {
	      Temp.remove(0,1);
	      if (Temp.right(1)==QString("\"")) Temp.remove(Temp.count()-1,1);
	    }

#ifdef CONFIG_USE_C_SYNTAX
	    int iPos=-1;
	    for (;;)
	    {
	      iPos=Temp.indexOf(QChar('\\'),iPos+1);
	      if ((iPos<0) || (iPos+1>=Temp.count())) break;

	      QChar ReplaceChar=Temp[iPos+1];
	      switch (ReplaceChar.toLatin1())
	      {
		case 'x': case 'X': // hexadezimale Angabe
		{
		  char c1=Temp[iPos+2].toLatin1();
		  if (c1>='a') c1-=0x20;
		  if (c1>='A') c1-=7;
		  c1-='0';
		  char c2=Temp[iPos+3].toLatin1();
		  if (c2>='a') c2-=0x20;
		  if (c2>='A') c2-=7;
		  c2-='0';

//                if ((c1>=0) && (c1<16) && (c2>=0) && (c2<16))
		  if ((!(c1 & 0xF0)) && (!(c2 & 0xF0)))
		  {
		    Temp[iPos]=QChar((c1<<4)+c2);
		    Temp.remove(iPos+1,3);
		  }
		  continue;
		}
		case 'r': ReplaceChar='\r'; break; // carriage return
		case 'n': ReplaceChar='\n'; break; // line feed
		case 't': ReplaceChar='\t'; break; // tabulator
		case 'f': ReplaceChar='\f'; break; // form feed
	      }

	      // ersetze Zeichen durch vordefiniertes Zeichen bzw.
	      // löschen den Backslash davor
	      Temp[iPos]=ReplaceChar;
	      Temp.remove(iPos+1,1);
	    }
#endif

	    m_aszValues.append(Temp);
	  }
	  iStartPos=iPos+1;
	  break;
      }
    }
  }
  else
    m_aszValues.clear();
}

// einzelnen Teilwert als String schreiben, Index==-1: neuen Teilwert anhängen
CConfigItem::STATUSINFO CConfigItem::SetStringValue(const QString& Value, int Index)
{
  int iCount=m_aszValues.count();
  bool bAppend=false;
  if (((Index>=-1) && (Index<iCount)) || (Index==0))
  {
    if (Index==-1) Index=iCount;
    if (m_aszValues.count()<Index)
    {
      bAppend=true;
      m_aszValues.reserve(Index+1);
    }
#ifndef CONFIG_USE_C_SYNTAX
    QString szTemp(Value);
    for (int iPos=0; iPos<szTemp.count(); iPos++)
      if ((szTemp[iPos]>=0) && (szTemp[iPos]<32))
	szTemp[iPos]=QChar('_');
    if (bAppend)
    {
      while (m_aszValues.count()<Index) m_aszValues.append(QString(""));
      m_aszValues.append(szTemp);
    }
    else
      m_aszValues[Index]=szTemp;
    return szTemp.isEmpty()?EMPTYVALUE:SUCCESS;
#else
    if (bAppend)
    {
      while (m_aszValues.count()<Index) m_aszValues.append(QString(""));
      m_aszValues.append(Value);
    }
    else
      m_aszValues[Index]=Value;
    return Value.isEmpty()?EMPTYVALUE:SUCCESS;
#endif
  }
  return ILLEGALVALUE;
}

// einzelnen Teilwert als BOOLEAN schreiben (dec,hex,bin,oct)
// Index=-1: neuen Teilwert anhängen
CConfigItem::STATUSINFO CConfigItem::SetBooleanValue(bool Value, int Index)
{
  if (Value)
    return SetStringValue("true",Index);
  else
    return SetStringValue("false",Index);
}

// einzelnen Teilwert als LONG schreiben (dec,hex,bin,oct)
CConfigItem::STATUSINFO CConfigItem::SetLongValue(long Value, CConfigItem::LONGVALUEMODE Mode, int Index)
{
  QString szTemp;
  switch (Mode)
  {
    //////////////////////////////////////////////////////////////////////
    // dezimale Schreibweise
    //////////////////////////////////////////////////////////////////////
    case CConfigItem::dec:
      szTemp.sprintf("%ld",Value);
      break;

    //////////////////////////////////////////////////////////////////////
    // hexadezimale Schreibweise (mit mindestens 2-8 Stellen)
    //////////////////////////////////////////////////////////////////////
    case CConfigItem::hex:
    case CConfigItem::hex1:
      szTemp.sprintf("0x%lx",Value);
      break;

    case CConfigItem::hex2:
    case CConfigItem::hex3:
    case CConfigItem::hex4:
    case CConfigItem::hex5:
    case CConfigItem::hex6:
    case CConfigItem::hex7:
    case CConfigItem::hex8:
    {
      int iLen;
      switch (Mode)
      {
	// case CConfigItem::hex2
	default:               iLen=2; break;
	case CConfigItem::hex3: iLen=3; break;
	case CConfigItem::hex4: iLen=4; break;
	case CConfigItem::hex5: iLen=5; break;
	case CConfigItem::hex6: iLen=6; break;
	case CConfigItem::hex7: iLen=7; break;
	case CConfigItem::hex8: iLen=8; break;
      }
      szTemp.sprintf("0x%0*lx",iLen,Value);
      break;
    }

    //////////////////////////////////////////////////////////////////////
    // binäre Schreibweise (mit mindestens 2-8,12,16,20,24,28,32 Stellen)
    //////////////////////////////////////////////////////////////////////
    case CConfigItem::bin:
    case CConfigItem::bin1:
    case CConfigItem::bin2:
    case CConfigItem::bin3:
    case CConfigItem::bin4:
    case CConfigItem::bin5:
    case CConfigItem::bin6:
    case CConfigItem::bin7:
    case CConfigItem::bin8:
    case CConfigItem::bin12:
    case CConfigItem::bin16:
    case CConfigItem::bin20:
    case CConfigItem::bin24:
    case CConfigItem::bin28:
    case CConfigItem::bin32:
    {
      int iPos;
      szTemp.resize(32);
      for (iPos=31; iPos>=0; iPos--)
      {
	szTemp[iPos]=QChar((char)('0'+(Value & 1)));
	Value>>=1;
      }

      switch (Mode)
      {
	// case CConfigItem::bin1:
	default:                 iPos=1; break;
	case CConfigItem::bin2:  iPos=2; break;
	case CConfigItem::bin3:  iPos=3; break;
	case CConfigItem::bin4:  iPos=4; break;
	case CConfigItem::bin5:  iPos=5; break;
	case CConfigItem::bin6:  iPos=6; break;
	case CConfigItem::bin7:  iPos=7; break;
	case CConfigItem::bin8:  iPos=8; break;
	case CConfigItem::bin12: iPos=12; break;
	case CConfigItem::bin16: iPos=16; break;
	case CConfigItem::bin20: iPos=20; break;
	case CConfigItem::bin24: iPos=24; break;
	case CConfigItem::bin28: iPos=28; break;
	case CConfigItem::bin32: iPos=32; break;
      }

      while ((szTemp.count()>iPos) && (szTemp.left(1)==QString("0"))) szTemp.remove(0,1);
      szTemp.insert(0,"0b");
      break;
    }

    //////////////////////////////////////////////////////////////////////
    // oktale Schreibweise (mit mindestens 2-9,12 Stellen)
    //////////////////////////////////////////////////////////////////////
    case CConfigItem::oct:
    case CConfigItem::oct1:
      szTemp.sprintf("0o%lo",Value);
      break;

    case CConfigItem::oct2:
    case CConfigItem::oct3:
    case CConfigItem::oct4:
    case CConfigItem::oct5:
    case CConfigItem::oct6:
    case CConfigItem::oct7:
    case CConfigItem::oct8:
    case CConfigItem::oct9:
    case CConfigItem::oct12:
    {
      int iLen;
      switch (Mode)
      {
	// case CConfigItem::oct2:
	default:                 iLen=2; break;
	case CConfigItem::oct3:  iLen=3; break;
	case CConfigItem::oct4:  iLen=4; break;
	case CConfigItem::oct5:  iLen=5; break;
	case CConfigItem::oct6:  iLen=6; break;
	case CConfigItem::oct7:  iLen=7; break;
	case CConfigItem::oct8:  iLen=8; break;
	case CConfigItem::oct9:  iLen=9; break;
	case CConfigItem::oct12: iLen=12; break;
      }
      szTemp.sprintf("0o%0*lo",iLen,Value);
      break;
    }
  }
  return SetStringValue(szTemp,Index);
}

// einzelnen Teilwert als DOUBLE schreiben (ggf. technische Schreibweise)
CConfigItem::STATUSINFO CConfigItem::SetDoubleValue(double Value, bool bTechnical, int Index)
{
  // Exponent zur Basis 10 ermitteln
  int iExp=0;
  if ((Value<=-10.0) || (Value>=10.0))
  {
    double fTemp=Value;
    if (fTemp<0) fTemp=-fTemp;
    for (;fTemp>=10.0; iExp++) fTemp/=10.0;
  } else
  if ((Value>-1.0) && (Value<1.0) && (Value!=0.0))
  {
    double fTemp=Value;
    if (fTemp<0) fTemp=-fTemp;
    for (;fTemp<1.0; iExp--) fTemp*=10.0;
  }

  QString szTemp;

  if (bTechnical) // technische Schreibweise mit Exponent, der durch 3 teilbar ist
  {
    if (iExp<0) iExp-=2;
    szTemp=Dbl2Str(Value,3*(iExp/3));
  }
  else // nicht technische Schreibweise
  {
    if ((iExp<-5) || (iExp>10))
      szTemp=Dbl2Str(Value,iExp); // Schreibweise mit Exponent
    else
    {
      szTemp.sprintf("%0.14lf",Value); // Schreibweise ohne Exponent
      if (szTemp.indexOf('.')>=0)
      {
	szTemp.remove(QRegExp("0+$"));
	if (szTemp.right(1)==QString(".")) szTemp.remove(szTemp.count()-1,1);
      }
    }
  }

  return SetStringValue(szTemp,Index);
}

// Umwandlung einer DOUBLE-Zahl in Exponentenschreibweise
QString CConfigItem::Dbl2Str(double fValue, int iExponent)
{
  int iPos;

  // Mantisse auf gewünschten Exponent anpassen
  if (iExponent>0)
  {
    double fDivider=1.0;
    for (iPos=0; iPos<iExponent; iPos++) fDivider*=10.0;
    fValue/=fDivider; // gewünschten Exponent "abtrennen"
  } else
  if (iExponent<0)
  {
    double fFactor=1.0;
    for (iPos=0; iPos>iExponent; iPos--) fFactor*=10.0;
    fValue*=fFactor; // gewünschten Exponent "abtrennen"
  }

  // maximale Anzahl der Nachkommastellen festlegen
  if ((fValue<=-100.0) || (fValue>=100.0))
    iPos=12;
  else
    if ((fValue<=-10.0) || (fValue>=10.0))
      iPos=13;
    else
      iPos=14;

  // Mantisse schreiben
  char szTempPtr[50];
  iPos=sprintf(szTempPtr,"%0.*lf",iPos,fValue);

  // auf nötige Anzahl an Nachkommastellen reduzieren
  for (;szTempPtr[iPos-1]=='0';iPos--);
  if (szTempPtr[iPos-1]=='.') iPos--;

  // Exponent schreiben
  iPos+=sprintf(&szTempPtr[iPos],"e%+d",iExponent);

  // korrekte Länge des Strings setzen
  return QString::fromLatin1(szTempPtr,iPos);
}

// Anzahl der Teilwerte festlegen
void CConfigItem::SetValueCount(int Count)
{
  if (Count<0) Count=0;
  m_aszValues.reserve(Count);
  while (m_aszValues.count()<Count)
    m_aszValues.append(QString(""));
  while (m_aszValues.count()>Count)
    m_aszValues.removeLast();
}

// eine Kommentarzeile hinzufügen, die vor das Element gesetzt wird
int CConfigItem::AddComment(const QString& Comment)
{
  QString szTemp(Comment);
  szTemp.remove(QRegExp("^[ \t]+"));
  QChar c(' ');
  if (!szTemp.isEmpty()) c=szTemp.at(0);
  switch (c.toLatin1())
  {
    case ';': // System-Kommentarzeile
    case '#': // Anwender-Kommentarzeile
    {
      szTemp=Comment;
      int iPos=szTemp.indexOf(QRegExp("[;#]"));
      if (szTemp.mid(iPos+1,1)!=QString(" ")) szTemp.insert(iPos+1,' ');
      break;
    }
    default:  // kein Typ angegeben, es wird eine Anwender-Kommentarzeile angenommen
      szTemp=QString("# ")+Comment;
      break;
  }
  m_aszComments.append(szTemp);
  return m_aszComments.count()-1;
}

// eine Kommentarzeile lesen
QString& CConfigItem::GetComment(int Index)
{
  if ((Index>=0) && (Index<m_aszComments.count()))
    return m_aszComments[Index];
  else
  {
    static QString szTempComment;
    szTempComment.clear();
    return szTempComment;
  }
}

void CConfigItem::RemoveComment(int Index)
{
  if ((Index>=0) && (Index<m_aszComments.count()))
    m_aszComments.removeAt(Index);
}

void CConfigItem::ConfigSectionPtr(CConfigSection *pSection)
{
  m_pSection=pSection;
}

//********************************************************************
//********************************************************************
// CConfigSection
//********************************************************************
//********************************************************************

// Standardkonstruktor
CConfigSection::CConfigSection(const QString& Name, int Position)
  : QObject()
{
  m_aszComments.clear();
  m_aItems.clear();
  m_szName=Name;
  m_iPosition=Position;
  m_bHasLineBefore=true;
  m_pFile=NULL;
}

// Kopierkonstruktor
CConfigSection::CConfigSection(const CConfigSection &src)
  : QObject(), m_pFile(NULL)
{
  *this=src;
}

// interner Konstruktor
CConfigSection::CConfigSection(CConfigFile *pFile)
  : QObject()
{
  m_aszComments.clear();
  m_bHasLineBefore=false;
  m_aItems.clear();
  m_szName.clear();
  m_iPosition=0x7FFFFFFF;
  m_pFile=pFile;
}

// Destruktor
CConfigSection::~CConfigSection()
{
  m_aszComments.clear();
  for (int iPos=0; iPos<m_aItems.count(); iPos++)
    delete m_aItems[iPos];
  m_aItems.clear();
}

// erlaubt Kopiervorgänge des C++-Datentyps "CConfigSection"
CConfigSection& CConfigSection::operator=(const CConfigSection &src)
{
  int iPos;

  m_szName=src.m_szName;
  m_aszComments=src.m_aszComments;
  m_iPosition=src.m_iPosition;
  m_bHasLineBefore=src.m_bHasLineBefore;
//m_pFile wird nicht verändert

  for (iPos=0; iPos<m_aItems.count(); iPos++)
    delete m_aItems[iPos];
  m_aItems.clear();
  m_aItems.reserve(src.m_aItems.count());
  for (iPos=0; iPos<src.m_aItems.count(); iPos++)
  {
    CConfigItem *pItem=new CConfigItem(this);
    *pItem=*(src.m_aItems[iPos]);
    m_aItems.append(pItem);
  }

  return *this;
}

// eine Referenz zu einem Element der Sektion ermitteln
CConfigItem& CConfigSection::operator[](int Index) const
{
  if ((Index>=0) && (Index<m_aItems.count()))
    return *(m_aItems[Index]);
  else
  {
    static CConfigItem DummyItem;
    return DummyItem;
  }
}

// suche Index für vorhandenes oder neu zu erstellendes Item
bool CConfigSection::FindIndex(const QString& szName, int &iIndex) const
{
  int i;
  for (i=0; i<m_aItems.count(); ++i)
  {
    if (m_aItems[i]->GetName().compare(szName,Qt::CaseInsensitive)==0)
    {
      iIndex=i;
      return true; // gefunden
    }
  }
  iIndex=i;
  return false; // nicht gefunden
}

// ein Element zur Sektion hinzufügen
int CConfigSection::AddItem(const CConfigItem &Item)
{
  // neues Element einfügen
  CConfigItem *pItem=new CConfigItem(Item);
  int i;

  // wenn das Item hinten angefügt werden soll, finde die nächst spätere Position
  if (pItem->m_iPosition>=0x7FFFFFFF)
  {
    int iPosition=0x40000000;
    for (i=0; i<m_aItems.count(); i++)
    {
      CConfigItem* pTmpItem=m_aItems[i];
      if (pTmpItem->m_iPosition>=iPosition) iPosition=pTmpItem->m_iPosition+1;
    }
    pItem->m_iPosition=iPosition;
  }
  m_aItems.append(pItem);
  for (i=0; i<m_aItems.count(); i++)
    m_aItems[i]->ConfigSectionPtr(this);
  return i-1; // das ist der Index, des neuen Elements
}

// ein Element der Sektion anhand des Namens suchen
int CConfigSection::FindItemIndex(const QString& Name) const
{
  int iIndex;
  if (FindIndex(Name,iIndex)) return iIndex; // gefunden
  return -1;
}

// das Element löschen, auf das sich der Index bezieht
void CConfigSection::RemoveItem(int Index)
{
  if ((Index>=0) && (Index<m_aItems.count()))
  {
    delete m_aItems[Index];
    m_aItems.removeAt(Index);
  }
}

// eine Kommentarzeile hinzufügen, die vor den Sektionsnamen gesetzt wird
int CConfigSection::AddComment(const QString& Comment)
{
  QString szTemp(Comment);
  szTemp.remove(QRegExp("^[ \t]+"));
  QChar c(' ');
  if (!szTemp.isEmpty()) c=szTemp.at(0);
  switch (c.toLatin1())
  {
    case ';': // System-Kommentarzeile
    case '#': // Anwender-Kommentarzeile
    {
      szTemp=Comment;
      int iPos=szTemp.indexOf(QRegExp("[;#]"));
      if (szTemp.mid(iPos+1,1)!=QString(" ")) szTemp.insert(iPos+1,' ');
      break;
    }
    default:  // kein Typ angegeben, es wird eine Anwender-Kommentarzeile angenommen
      szTemp=QString("# ")+Comment;
      break;
  }
  m_aszComments.append(szTemp);
  return m_aszComments.count()-1;
}

// eine Kommentarzeile lesen
QString& CConfigSection::GetComment(int Index)
{
  if ((Index>=0) && (Index<m_aszComments.count()))
    return m_aszComments[Index];
  else
  {
    static QString szTempComment;
    szTempComment.clear();
    return szTempComment;
  }
}

void CConfigSection::RemoveComment(int Index)
{
  if ((Index>=0) && (Index<m_aszComments.count()))
    m_aszComments.removeAt(Index);
}

// Name des Elements setzen
void CConfigSection::SetName(const QString& Name)
{
  QString szName(Name);
  szName.remove(QRegExp("^[ \t]+"));
  szName.remove(QRegExp("[ \t]+$"));
  m_szName=szName;
}

void CConfigSection::ConfigFilePtr(CConfigFile *pFile)
{
  m_pFile=pFile;
  for (int iPos=0; iPos<m_aItems.count(); iPos++)
    m_aItems[iPos]->ConfigSectionPtr(this);
}

//********************************************************************
//********************************************************************
// CConfigFile
//********************************************************************
//********************************************************************

// Standardkonstruktor
CConfigFile::CConfigFile(const QString& Filename)
  : QObject()
{
  m_szFilename=Filename;
  m_aSections.clear();
}

// Destruktor
CConfigFile::~CConfigFile()
{
  for (int iPos=0; iPos<m_aSections.count(); iPos++)
    delete m_aSections[iPos];
  m_aSections.clear();
}

// eine Referenz zu einer Sektion der Datei ermitteln
CConfigSection& CConfigFile::operator[](int Index) const
{
  if ((Index>=0) && (Index<m_aSections.count()))
    return *(m_aSections[Index]);
  else
  {
    static CConfigSection DummySection;
    return DummySection;
  }
}

// suche Index für vorhandene oder neu zu erstellende Sektion
bool CConfigFile::FindIndex(const QString& szName, int &iIndex) const
{
  int i;
  for (i=0; i<m_aSections.count(); ++i)
  {
    if (m_aSections[i]->GetName().compare(szName,Qt::CaseInsensitive)==0)
    {
      iIndex=i;
      return true; // gefunden
    }
  }
  iIndex=i;
  return false; // nicht gefunden
}

// eine Sektion zur Datei hinzufügen
int CConfigFile::AddSection(const CConfigSection &Section)
{
  int i;
  CConfigSection *pSection=new CConfigSection(Section);

  // wenn die Sektion hinten angefügt werden soll, finde die nächst spätere Position
  if (pSection->m_iPosition>=0x7FFFFFFF)
  {
    int iPosition=0;
    for (i=0; i<m_aSections.count(); ++i)
    {
      CConfigSection* pTmpSec=m_aSections[i];
      if (pTmpSec->m_iPosition>=iPosition) iPosition=pTmpSec->m_iPosition+1;
    }
    pSection->m_iPosition=iPosition;
  }
  m_aSections.append(pSection);
  for (i=0; i<m_aSections.count(); i++)
    m_aSections[i]->ConfigFilePtr(this);
  return i-1; // das ist der Index, des neuen Elements
}

int CConfigFile::FindSectionIndex(const QString& Name) const
{
  int iIndex;
  if (FindIndex(Name,iIndex)) return iIndex; // gefunden
  return -1; // nicht gefunden
}

// die Sektion löschen, auf die sich der Index bezieht
void CConfigFile::RemoveSection(int Index)
{
  if ((Index>=0) && (Index<m_aSections.count()))
  {
    delete m_aSections[Index];
    m_aSections.removeAt(Index);
  }
}

// Speichern der Daten in eine Konfigurationsdatei
void CConfigFile::SaveFile(QTextStream &ar)
{
  // Sektionen einzeln in der Reihenfolge speichern, wie sie gelesen wurden
  // dazu werden sie anhand der Position sortiert, die von GetPosition() zurückgeliefert wird
  QList<CConfigFile::ItemPosEntry> SectionPosition;
  int iSectionPos;
  for (iSectionPos=0; iSectionPos<m_aSections.count(); iSectionPos++)
  {
    CConfigSection *pSection=m_aSections[iSectionPos];

    ItemPosEntry i;
    i.iItemNum =iSectionPos;
    i.iPosition=pSection->GetPosition();

    int iMin=0, iMax=SectionPosition.count()-1, iIndex=0;
    while (iMin<=iMax) // binäre Suche nach dem geeigneten Platz innerhalb der Datei
    {
      iIndex=(iMin+iMax)>>1;
      if (SectionPosition[iIndex].iPosition>i.iPosition)
	iMax=iIndex-1;
      else
      {
	iMin=iIndex+1;
	iIndex=iMin;
      }
    }

    SectionPosition.insert(iIndex,i); // Position in der Datei merken
  }

  // Sektionen speichern
  for (iSectionPos=0; iSectionPos<m_aSections.count(); iSectionPos++)
  {
    CConfigSection *pSection=m_aSections[SectionPosition[iSectionPos].iItemNum];

    // wenn gefordert: Leerzeile vor die Sektion schreiben
    if ((iSectionPos>0) && (pSection->GetLineBefore()))
      ar << "\r\n";

    // Kommentarzeilen vor der Sektion schreiben
    for (int iPos=0; iPos<pSection->GetCommentCount(); iPos++)
    {
      QString szComment(pSection->GetComment(iPos)+"\r\n");
      QString szTmp(szComment);
      szTmp.remove(QRegExp("^[ \t]+"));
      if (szTmp.indexOf(QRegExp("[;#]"))!=0) szComment.insert(0,"# ");
      ar << szComment;
    }

    // Name der Sektion schreiben
    ar << "[" << pSection->GetName() << "]\r\n";

    // Werte einzeln in der Reihenfolge gespeichern, wie sie gelesen wurden
    // dazu werden sie anhand der Position sortiert, die von GetPosition() zurückgeliefert wird
    QList<CConfigFile::ItemPosEntry> ItemPosition;
    int iItemPos;
    for (iItemPos=0; iItemPos<pSection->GetItemCount(); iItemPos++)
    {
      CConfigItem *pItem=&pSection->GetItem(iItemPos);

      ItemPosEntry i;
      i.iItemNum =iItemPos;
      i.iPosition=pItem->GetPosition();

      int iMin=0, iMax=ItemPosition.count()-1, iIndex=0;
      while (iMin<=iMax) // binäre Suche nach dem geeigneten Platz innerhalb der Sektion
      {
	iIndex=(iMin+iMax)>>1;
	if (ItemPosition[iIndex].iPosition>i.iPosition)
	  iMax=iIndex-1;
	else
	{
	  iMin=iIndex+1;
	  iIndex=iMin;
	}
      }

      ItemPosition.insert(iIndex,i); // Position in der Sektion merken
    }

    // Werte speichern
    for (iItemPos=0; iItemPos<pSection->GetItemCount(); iItemPos++)
    {
      CConfigItem *pItem=&pSection->GetItem(ItemPosition[iItemPos].iItemNum);

      // wenn gefordert: Leerzeile vor das Element schreiben
      if (pItem->GetLineBefore()) ar << "\r\n";

      // Kommentarzeilen speichern
      for (int iPos=0; iPos<pItem->GetCommentCount(); iPos++)
      {
	QString szComment(pItem->GetComment(iPos)+"\r\n");
	QString szTmp(szComment);
	szTmp.remove(QRegExp("^[ \t]+"));
	if (!szTmp.contains(QRegExp("[;#]"))) szComment.insert(0,"# ");
	ar << szComment;
      }

      // Wert speichern
      QString szTemp;
      pItem->GetValue(szTemp);
      ar << pItem->GetName() << " = " << szTemp << "\r\n";
    }
  }
}

// Speichern der Daten in eine Konfigurationsdatei
bool CConfigFile::SaveFile(const QString& Filename)
{
  QString szFilename(Filename);
  if (szFilename.isEmpty()) szFilename=m_szFilename;

  QFile hFile(szFilename);
  if (!hFile.open(QIODevice::WriteOnly))
    return false;
  QTextStream hArchive(&hFile);

  // Konfigurationsdatei speichern
  SaveFile(hArchive);
  hArchive.flush();

  // Dateipuffer leeren und Datei schließen
  hFile.close();
  return true;
}

// Laden von Daten einer Konfigurationsdatei (interne Daten werden vorher gelöscht !!)
void CConfigFile::LoadFile(QTextStream &ar, LOADCOMMENTINFO LoadCommentInfo)
{
  QStringList Comments;
  int iSectionCount=0, iItemCount=0;
  CConfigSection *pCurrentSection=NULL;
  bool bHasLine=false;

  for (;;)
  {
    QString szLine(ar.readLine());
    if (szLine.isNull()) break;

    // entferne Leerzeichen, Tabs, die am Anfang stehen
    szLine.remove(QRegExp("^[ \t]+"));
    // entferne Leerzeichen, Tabs, Zeilenumbruchszeichen, die am Ende stehen
    szLine.remove(QRegExp("[ \t\r\n]+$"));

    if (!szLine.isEmpty()) // keine Leerzeilen
    {
      if (szLine.left(1)==QString(";")) // System-Kommentarzeile
      {
	switch (LoadCommentInfo)
	{
	  case CConfigFile::NO_COMMENTS:   // keine Kommentare
	  case CConfigFile::USER_COMMENTS: // nur Anwender-Kommentare
	    continue; // nächste Zeile einlesen
	  default: break;
	}
	if (szLine.at(1)!=' ') szLine.insert(1,' ');
	Comments.append(szLine);
      } else
      if (szLine.left(1)==QString("#")) // Anwender-Kommentarzeile
      {
	switch (LoadCommentInfo)
	{
	  case CConfigFile::NO_COMMENTS:     // keine Kommentare
	  case CConfigFile::SYSTEM_COMMENTS: // nur System-Kommentare
	    continue; // nächste Zeile einlesen
	  default: break;
	}
	if (szLine.at(1)!=' ') szLine.insert(1,' ');
	Comments.append(szLine);
      } else
      if (szLine.left(1)==QString("[")) // Sektionstrenner
      {
	int iPos=szLine.indexOf(']');
	if (iPos<0) iPos=szLine.count();

	CConfigSection Section;

	for (int iCount=0; iCount<Comments.count(); iCount++)
	  Section.AddComment(Comments[iCount]);

	Section.SetName(szLine.mid(1,iPos-1));
	Section.SetPosition(++iSectionCount);
	Section.SetLineBefore(bHasLine);
	iPos=AddSection(Section);
	pCurrentSection=m_aSections[iPos];

	Comments.clear();
	iItemCount=0;
	bHasLine=false;
      }
      else // Wert
      {
	if (pCurrentSection)
	{
	  int iPos;
	  if ((iPos=szLine.indexOf('='))>0)       // Begin eines Elements
	  {
	    CConfigItem Item;

	    for (int iCount=0; iCount<Comments.count(); iCount++)
	      Item.AddComment(Comments[iCount]);
	    Comments.clear();

	    if (iPos<0) iPos=szLine.count();
	    Item.SetName(szLine.left(iPos));
	    Item.SetPosition(++iItemCount);
	    Item.SetLineBefore(bHasLine);
	    bHasLine=false;

	    szLine=szLine.mid(iPos+1);
	    szLine.remove(QRegExp("^[ \t]+"));
	    Item.SetValue(szLine);
	    pCurrentSection->AddItem(Item);
	  }
	}

	Comments.clear();
	bHasLine=false;
      }
    }
    else
      bHasLine=true;
  }
}

// Laden von Daten einer Konfigurationsdatei (interne Daten werden vorher gelöscht !!)
bool CConfigFile::LoadFile(const QString& Filename, CConfigFile::LOADCOMMENTINFO LoadCommentInfo)
{
  QString szFilename(Filename);
  if (szFilename.isEmpty())
    szFilename=m_szFilename;
  else
    m_szFilename=szFilename;

  for (int iPos=0; iPos<m_aSections.count(); iPos++)
    delete m_aSections[iPos];
  m_aSections.clear();

  QFile hFile(szFilename);
  if (!hFile.open(QIODevice::ReadOnly))
    return false;
  QTextStream hArchive(&hFile);

  // Konfigurationsdatei laden
  LoadFile(hArchive,LoadCommentInfo);
  hFile.close();
  return true;
}
