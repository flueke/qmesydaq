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
 * file:            inifile.h
 * authors:         Lutz Rossa <rossa@helmholtz-berlin.de>
 *
 * last changed by: $$Author$$
 * last update:     $$Date$$
 * revision:        $$Rev$$
 ****************************************************************************/

#ifndef CONFIGFILE_H__6E844075_A70D_11D3_916C_0020AFAABCCB__INCLUDED_
#define CONFIGFILE_H__6E844075_A70D_11D3_916C_0020AFAABCCB__INCLUDED_

#include <QFile>
#include <QTextStream>
#include <QStringList>

//#define CONFIG_USE_C_SYNTAX

class CConfigItem;
class CConfigSection;
class CConfigFile;

////////////////////////////////////////////////////////////////////////////////////////////////
// CConfigItem: speichert ein Element einer Sektion einer Konfigurationsdatei
//              verwalten, hinzufügen, ändern, löschen von Werten eines
//              Elementes und verwalten, hinzufügen und löschen von Kommentaren
////////////////////////////////////////////////////////////////////////////////////////////////
// wenn
// #define CONFIG_USE_C_SYNTAX
// dann werden Sonderzeichen als C-Syntax \x?? gespeichert und geladen,
// wobei ?? den ASCII-Code in hexadezimaler Schreibweise darstellt
//
// ohne CONFIG_USE_C_SYNTAX
// können keine Sonderzeichen interpretiert oder geschrieben werden
////////////////////////////////////////////////////////////////////////////////////////////////
class CConfigItem : public QObject
{
  Q_OBJECT
  friend class CConfigSection;
public:
  /* Modus für Darstellung einer Zahl nach SetLongValue()
     dec                        = dezimal
     hex,hex2,hex4,hex6,hex8    = hexadezimal (mit mindestens 2, 4, 6 oder 8 Stellen)
     bin,bin8,bin16,bin24,bin32 = binär (mit mindestens 8, 16, 24 oder 32 Stellen)
     oct,oct3,oct6,oct9,oct12   = oktal (mit mindestens 3, 6, 9 oder 12 Stellen) */
  typedef enum
    { dec=0,
      hex,hex1,hex2,hex3,hex4,hex5,hex6,hex7,hex8,
      bin,bin1,bin2,bin3,bin4,bin5,bin6,bin7,bin8,bin12,bin16,bin20,bin24,bin28,bin32,
      oct,oct1,oct2,oct3,oct4,oct5,oct6,oct7,oct8,oct9,oct12 }
    LONGVALUEMODE;
  typedef enum { ILLEGALVALUE=0, SUCCESS=1, EMPTYVALUE=2 } STATUSINFO;

  //////////////////////////////////////////////////////////////////////////////////
  // Konstruktoren und überladene Operatoren
  //////////////////////////////////////////////////////////////////////////////////

  // Standardkonstruktor
  CConfigItem(const QString& Name=QString(), const QString& Value=QString(), int Position=0x7FFFFFFF, bool HasLineBefore=false);
  // Kopierkonstruktor
  CConfigItem(const CConfigItem &src);
  // Destruktor
  ~CConfigItem();
  // erlaubt Kopiervorgänge des C++-Datentyps "CConfigItem"
  CConfigItem& operator=(const CConfigItem &src);

  //////////////////////////////////////////////////////////////////////////////////
  // Verwaltung von Informationen des Elements
  //////////////////////////////////////////////////////////////////////////////////

  // Name des Elements lesen
  QString GetName() const                          { return m_szName; }
  // steht vor dem Element eine leere Zeile ?
  bool GetLineBefore() const                       { return m_bHasLineBefore; }
  // Position innerhalb der Sektion lesen
  int  GetPosition() const                         { return m_iPosition; }

  // Name des Elements setzen
  void SetName(const QString& Name);
  // soll vor dem Element eine leere Zeile stehen ?
  void SetLineBefore(bool Flag=true)               { m_bHasLineBefore=Flag; }
  // Position innerhalb der Sektion festlegen
  void SetPosition(int Position)                   { m_iPosition=Position; }

  // gesamten Inhalt des Items löschen
  void EmptyItem();
  // gesamten Wertestring lesen
  bool GetValue(QString &Value) const { return GetValue(&Value); }
  // gesamten Wertestring lesen
  bool GetValue(QString *Value) const;
  // gesamten Wertestring lesen
  bool GetValue(char *ValueBuffer, int Length) const;

  // einzelnen Teilwert als String lesen
  STATUSINFO GetStringValue(QString &Value, int Index=0) const { return GetStringValue(&Value,Index); }
  // einzelnen Teilwert als String lesen
  STATUSINFO GetStringValue(QString *Value, int Index=0) const;
  // einzelnen Teilwert als String lesen
  STATUSINFO GetStringValue(char *ValueBuffer, int Length, int Index=0) const;
  // einzelnen Teilwert als BOOLEAN lesen
  STATUSINFO GetBooleanValue(bool &Value, int Index=0) const { return GetBooleanValue(&Value,Index); }
  // einzelnen Teilwert als BOOLEAN lesen
  STATUSINFO GetBooleanValue(bool *Value, int Index=0) const;
  // einzelnen Teilwert als LONG lesen (wertet auch dec,hex,bin,oct aus)
  STATUSINFO GetLongValue(long &Value, int Index=0) const { return GetLongValue(&Value,Index); }
  // einzelnen Teilwert als LONG lesen (wertet auch dec,hex,bin,oct aus)
  STATUSINFO GetLongValue(long *Value, int Index=0) const;
  // einzelnen Teilwert als LONG lesen (wertet auch dec,hex,bin,oct aus)
  STATUSINFO GetULongValue(unsigned long &Value, int Index=0) const { return GetULongValue(&Value,Index); }
  // einzelnen Teilwert als LONG lesen (wertet auch dec,hex,bin,oct aus)
  STATUSINFO GetULongValue(unsigned long *Value, int Index=0) const;
  // einzelnen Teilwert als DOUBLE lesen (wertet auch dec,hex,bin,oct aus)
  STATUSINFO GetDoubleValue(double &Value, int Index=0) const { return GetDoubleValue(&Value,Index); }
  // einzelnen Teilwert als DOUBLE lesen (wertet auch dec,hex,bin,oct aus)
  STATUSINFO GetDoubleValue(double *Value, int Index=0) const;

  // einzelnen Teilwert als BOOLEAN lesen
  STATUSINFO GetValue(bool &Value, int Index) const      { return GetBooleanValue(&Value,Index); }
  // einzelnen Teilwert als BOOLEAN lesen
  STATUSINFO GetValue(bool *Value, int Index) const      { return GetBooleanValue(Value,Index); }
  // einzelnen Teilwert als LONG lesen (wertet auch dec,hex,bin,oct aus)
  STATUSINFO GetValue(long &Value, int Index) const      { return GetLongValue(&Value,Index); }
  // einzelnen Teilwert als LONG lesen (wertet auch dec,hex,bin,oct aus)
  STATUSINFO GetValue(long *Value, int Index) const      { return GetLongValue(Value,Index); }
  // einzelnen Teilwert als U_LONG lesen (wertet auch dec,hex,bin,oct aus)
  STATUSINFO GetValue(unsigned long &Value, int Index) const { return GetULongValue(&Value,Index); }
  // einzelnen Teilwert als U_LONG lesen (wertet auch dec,hex,bin,oct aus)
  STATUSINFO GetValue(unsigned long *Value, int Index) const { return GetULongValue(Value,Index); }
  // einzelnen Teilwert als DOUBLE lesen (wertet auch dec,hex,bin,oct aus)
  STATUSINFO GetValue(double &Value, int Index) const    { return GetDoubleValue(&Value,Index); }
  // einzelnen Teilwert als DOUBLE lesen (wertet auch dec,hex,bin,oct aus)
  STATUSINFO GetValue(double *Value, int Index) const    { return GetDoubleValue(Value,Index); }
  // einzelnen Teilwert als String lesen
  STATUSINFO GetValue(QString &Value, int Index) const   { return GetStringValue(&Value,Index); }
  // einzelnen Teilwert als String lesen
  STATUSINFO GetValue(QString *Value, int Index) const   { return GetStringValue(Value,Index); }

  // Anzahl der Teilwerte ermitteln
  int  GetValueCount() const                             { return m_aszValues.count(); }

  // gesamten Wertestring schreiben
  void SetValue(const QString& Value);
  // einzelnen Teilwert als String schreiben, Index=-1: neuen Teilwert anhängen
  STATUSINFO SetStringValue(const QString& Value, int Index=0);
  // einzelnen Teilwert als BOOLEAN schreiben (dec,hex,bin,oct)
  // Index=-1: neuen Teilwert anhängen
  STATUSINFO SetBooleanValue(bool Value, int Index=0);
  // einzelnen Teilwert als LONG schreiben (dec,hex,bin,oct)
  // Index=-1: neuen Teilwert anhängen
  STATUSINFO SetLongValue(long Value, LONGVALUEMODE Mode=dec, int Index=0);
  // einzelnen Teilwert als DOUBLE schreiben (ggf. technische Schreibweise)
  // Index=-1: neuen Teilwert anhängen
  STATUSINFO SetDoubleValue(double Value, bool bTechnical=false, int Index=0);
  // Anzahl der Teilwerte festlegen
  void SetValueCount(int Count);

  //////////////////////////////////////////////////////////////////////////////////
  // Verwaltung der Kommentare
  //////////////////////////////////////////////////////////////////////////////////

  // eine Kommentarzeile anfügen, die vor das Element gesetzt wird
  int AddComment(const QString& Comment);
  // eine Kommentarzeile lesen
  QString& GetComment(int Index);
  // Anzahl der Kommentarzeilen ermitteln
  int  GetCommentCount() const                     { return m_aszComments.count(); }
  // eine Kommentarzeile löschen
  void RemoveComment(int Index);

protected:
  QString      m_szName;             // Name des Elements
  QStringList  m_aszValues;          // Feld der Teilwerte des Elements
  int          m_iPosition;          // Position des Elements innerhalb der Sektion
  bool         m_bHasLineBefore;     // steht vor dem Element eine leere Zeile ?
  QStringList  m_aszComments;        // Feld der Kommentarzeilen des Elements

  // interne Funktionen, damit Berechnung mathematischer Ausdrücke möglich wird
  static QString Dbl2Str(double fValue, int iExponent);
  CConfigItem(CConfigSection *pSection);
  void ConfigSectionPtr(CConfigSection *pSection);
  CConfigSection* ConfigSectionPtr()               { return m_pSection; }

  void InUse(bool bInUse)                          { m_bInUse=bInUse; }
  bool InUse()                                     { return m_bInUse; }

private:
  CConfigSection *m_pSection;      // für GetLongValue/GetDoubleValue mit arithm. Ausdrücken
  bool            m_bInUse;
};

////////////////////////////////////////////////////////////////////////////////////////////////
// CConfigSection: speichert eine Sektion einer Konfigurationsdatei
//                 verwalten, suchen, hinzufügen, ändern, löschen
//                 von Elementen innerhalb der Sektion und
//                 verwalten, hinzufügen und löschen von Kommentaren
////////////////////////////////////////////////////////////////////////////////////////////////
class CConfigSection : public QObject
{
  Q_OBJECT
  friend class CConfigFile;
  friend class CConfigItem;
public:
  //////////////////////////////////////////////////////////////////////////////////
  // Konstruktoren und überladene Operatoren
  //////////////////////////////////////////////////////////////////////////////////

  // Standardkonstruktor
  CConfigSection(const QString& Name=QString(), int Position=0x7FFFFFFF);
  // Kopierkonstruktor
  CConfigSection(const CConfigSection &src);
  // Destruktor
  ~CConfigSection();
  // erlaubt Kopiervorgänge des C++-Datentyps "CConfigSection"
  CConfigSection& operator=(const CConfigSection &src);
  // eine Referenz zu einem Element der Sektion ermitteln
  CConfigItem& operator[](int Index) const;
  // eine Referenz zu einem Element der Sektion ermitteln
  CConfigItem& operator()(int Index) const  { return operator[](Index); }

  //////////////////////////////////////////////////////////////////////////////////
  // Verwaltung von Informationen der Sektion
  //////////////////////////////////////////////////////////////////////////////////

  // Name der Sektion lesen
  QString GetName() const                   { return m_szName; }
  // steht vor dem Element eine leere Zeile ?
  bool GetLineBefore() const                { return m_bHasLineBefore; }
  // Position innerhalb der Datei lesen
  int  GetPosition() const                  { return m_iPosition; }

  // Name der Sektion setzen
  void SetName(const QString& Name);
  // soll vor dem Element eine leere Zeile stehen ?
  void SetLineBefore(bool Flag=true)        { m_bHasLineBefore=Flag; }
  // Position innerhalb der Datei festlegen
  void SetPosition(int Position)            { m_iPosition=Position; }

  //////////////////////////////////////////////////////////////////////////////////
  // Verwaltung der einzelnen Elemente
  //////////////////////////////////////////////////////////////////////////////////

  // ein Element zur Sektion hinzufügen
  int  AddItem(const CConfigItem &Item);
  // ein Element der Sektion anhand des Namens suchen
  int  FindItemIndex(const QString& Name) const;
  // ein Element der Sektion anhand des Namens suchen
  int  FindItemIndex(const CConfigItem &Item) const { return FindItemIndex(Item.GetName()); }
  // eine Referenz zu einem Element der Sektion ermitteln
  CConfigItem& GetItem(int Index) const       { return operator[](Index); }
  // Anzahl der Elemente der Sektion lesen
  int  GetItemCount() const                   { return m_aItems.count(); }
  // das Element löschen, auf das sich der Index bezieht
  void RemoveItem(int Index);
  // das Element löschen, das durch den Namen angegeben wird
  void RemoveItem(const QString &Name)        { RemoveItem(FindItemIndex(Name)); }
  // das Element löschen, das durch den Namen angegeben wird
  void RemoveItem(const CConfigItem &Item)    { RemoveItem(FindItemIndex(Item.GetName())); }

  //////////////////////////////////////////////////////////////////////////////////
  // Verwaltung der Kommentare
  //////////////////////////////////////////////////////////////////////////////////

  // eine Kommentarzeile anfügen, die vor den Sektionsnamen gesetzt wird
  int AddComment(const QString& Comment);
  // eine Kommentarzeile lesen
  QString& GetComment(int Index);
  // Anzahl der Kommentarzeilen ermitteln
  int  GetCommentCount() const              { return m_aszComments.count(); }
  // eine Kommentarzeile löschen
  void RemoveComment(int Index);

protected:
  QString      m_szName;                          // Name der Sektion
  QList<CConfigItem*> m_aItems;                   // Feld der Elemente der Sektion
  int          m_iPosition;                       // Position der Sektion innerhalb der Datei
  bool         m_bHasLineBefore;                  // steht vor der Sektion eine leere Zeile ?
  QStringList  m_aszComments;                     // Feld der Kommentarzeilen

  // interne Funktionen, damit Berechnung mathematischer Ausdrücke möglich wird
  CConfigSection(CConfigFile *pFile);
  void ConfigFilePtr(CConfigFile *pFile);
  CConfigFile* ConfigFilePtr()               { return m_pFile; }

  // suche Index für vorhandenes oder neu zu erstellendes Item
  bool FindIndex(const QString& szName, int &iIndex) const;

private:
  CConfigFile *m_pFile;      // für CConfigItem::GetLongValue/CConfigItem::GetDoubleValue mit arithm. Ausdrücken
};

////////////////////////////////////////////////////////////////////////////////////////////////
// CConfigFile: laden, speichern von Konfigurationsdateien im Format von .INI-Dateien
//              verwalten, hinzufügen, ändern, löschen von Sektionen
////////////////////////////////////////////////////////////////////////////////////////////////
class CConfigFile : public QObject
{
  Q_OBJECT
  friend class CConfigSection;
public:
  /* Modus für das Laden einer Konfigurationsdatei mit LoadFile()
     NO_COMMENTS     = keine Kommentarzeilen einlesen
     USER_COMMENTS   = nur Anwenderkommentare einlesen
     SYSTEM_COMMENTS = nur Systemkommentare einlesen
     ALL_COMMENTS    = alle Kommentarzeilen einlesen */
  typedef enum { NO_COMMENTS, USER_COMMENTS, SYSTEM_COMMENTS, ALL_COMMENTS } LOADCOMMENTINFO;

  //////////////////////////////////////////////////////////////////////////////////
  // Konstruktoren und überladene Operatoren
  //////////////////////////////////////////////////////////////////////////////////

  // Standardkonstruktor
  CConfigFile(const QString& Filename=QString());
  // Destruktor
  ~CConfigFile();
  // eine Referenz zu einer Sektion der Datei ermitteln
  CConfigSection& operator[](int Index) const;
  // eine Referenz zu einer Sektion der Datei ermitteln
  CConfigSection& operator()(int Index) const   { return operator[](Index); }

  //////////////////////////////////////////////////////////////////////////////////
  // Verwaltung von Informationen der Konfigurationsdatei
  //////////////////////////////////////////////////////////////////////////////////

  // Dateiname lesen (nur für LoadFile und SaveFile ohne Parameter)
  QString GetFileName() const                   { return m_szFilename; }
  // Dateiname schreiben (nur für LoadFile und SaveFile ohne Parameter)
//  void SetFileName(const QString& Filename)   { m_szFilename=Filename; }

  //////////////////////////////////////////////////////////////////////////////////
  // Verwaltung der einzelnen Sektionen
  //////////////////////////////////////////////////////////////////////////////////

  // eine Sektion zur Datei hinzufügen
  int AddSection(const CConfigSection &Section);
  // eine Sektion der Datei anhand des Namens suchen
  int FindSectionIndex(const QString& Name) const;
  // eine Sektion der Datei anhand des Namens suchen
  int FindSectionIndex(const CConfigSection &Section) const { return FindSectionIndex(Section.GetName()); }
  // eine Referenz zu einer Sektion der Datei ermitteln
  CConfigSection& GetSection(int Index) const         { return operator[](Index); }
  // Anzahl der Sektionen der Datei lesen
  int  GetSectionCount() const                        { return m_aSections.count(); }
  // die Sektion löschen, auf die sich der Index bezieht
  void RemoveSection(int Index);
  // die Sektion löschen, die durch den Namen angegeben wird
  void RemoveSection(const QString& Name)             { RemoveSection(FindSectionIndex(Name)); }
  // die Sektion löschen, die durch den Namen angegeben wird
  void RemoveSection(const CConfigSection &Section)   { RemoveSection(FindSectionIndex(Section.GetName())); }

  //////////////////////////////////////////////////////////////////////////////////
  // Laden und Speichern
  //////////////////////////////////////////////////////////////////////////////////
  // erzeuge leere Konfigurationsdatei
  void EmptyFile(const QString& Filename=QString());
  // Laden von Daten einer Konfigurationsdatei (interne Daten werden vorher gelöscht !!)
  bool LoadFile(const QString& Filename=QString(), LOADCOMMENTINFO LoadCommentInfo=ALL_COMMENTS);
  // Laden von Daten einer Konfigurationsdatei (interne Daten werden vorher gelöscht !!)
  void LoadFile(QTextStream &ar, LOADCOMMENTINFO LoadCommentInfo=ALL_COMMENTS);
  // Speichern der Daten in eine Konfigurationsdatei
  bool SaveFile(const QString& Filename=QString());
  // Speichern der Daten in eine Konfigurationsdatei
  void SaveFile(QTextStream &ar);

protected:
  QString  m_szFilename;                                // Dateiname
  QList<CConfigSection*> m_aSections; // Feld der Sektionen

  // interner Datentyp, der bei SaveFile benutzt wird
  typedef struct
  {
    int iItemNum;  // Index des Elements/der Sektion innerhalb des Feld
    int iPosition; // Kopie des Wertes der Funktion GetPosition()
  } ItemPosEntry;

  // suche Index für vorhandene oder neu zu erstellende Sektion
  bool FindIndex(const QString& szName, int &iIndex) const;
};

#endif // CONFIGFILE_H__6E844075_A70D_11D3_916C_0020AFAABCCB__INCLUDED_
