/***************************************************************************
 *   Copyright (C) 2013-2014 by Lutz Rossa <rossa@helmholtz-berlin.de>,    *
 *                    Eric Faustmann <eric.faustmann@helmholtz-berlin.de>, *
 *                    Damian Rhein <damian.rhein@helmholtz-berlin.de>      *
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

#ifndef HISTOGRAMMAPPINGEDITOR_H
#define HISTOGRAMMAPPINGEDITOR_H

#include <QDialog>
#include "mappeddetector.h"

namespace Ui {
	class HistogramMappingEditor;
}

class QAbstractButton;
class QTableWidgetItem;
class Measurement;
class MapCorrection;
class UserMapCorrection;
class EditorMemory;

/**
	\short Mapping editor

	\author Eric Faustmann <eric.faustmann@helmholtz-berlin.de>, Damian Rhein <damian.rhein@helmholtz-berlin.de>, Lutz Rossa <rossa@helmholtz-berlin.de>
*/
class HistogramMappingEditor : public QDialog
{
	Q_OBJECT

public:
	//! constructor
	explicit HistogramMappingEditor(Measurement *pMeasurement, QWidget *pParent = NULL);

	//! destructor
	~HistogramMappingEditor();

	//! keyboard accelerator handler
	bool keyPress(QKeyEvent *pEvent);

	//! flag, if content was changed before close
	bool hasChanged() const { return m_bChanged; }

signals:
	//! emitted, when user clicks the "Apply" button
	void applyedData();

private slots:
	//! handle standard buttons
	void on_buttonBox_clicked(QAbstractButton* pButton);

	//! handle channel text box for selection
	void textChanged(const QString &sNewInput);

	//! selection of table widget changed
	void itemSelectionChanged();

	//! handle button "Apply Data"
	void applyClick();

	//! handle button "Clear Data"
	void clearClick();

	//! handle menu "Save ..."
	void saveClick();

	//! handle menu "Load ..."
	void loadClick();

	//! handle button "shift down"
	void shiftTubesDown();

	//! handle button "shift up"
	void shiftTubesUp();

	//! handle button "Copy" and "CTRL-C" key sequence
	void copyTubesClick();

	//! handle button "Paste" and "CTRL-V" key sequence
	void pasteTubesClick();

	//! handle button "Help" and menu "Help"
	void helpClick();

	//! handle button "Restore Defaults" and menu "Clear ..."
	void setDefaultsClick();

private:
	//! the Qt user interface
	Ui::HistogramMappingEditor      *ui;

	//! pointer to data layer
	Measurement                     *m_pMeasurement;

	//! start up values of editor / values after "Apply" button
//	UserMapCorrection               *m_pUserMapCorrection;

	//! current content of editor
	EditorMemory                    *m_pEditor;

	//! clipboard data
	QVector<ListedMappedDetector>   m_aClipboard;

	//! number of available tubes
	int                             m_iDefaultTubes;

	//! flag, if content was changed before close
	bool                            m_bChanged;

	//! flag, if editor will be closed by "Close" button
	bool                            m_bClose;

	//! show editor contents
	void show();

	//! moves selected tube content
	void shiftTubes(int iShiftRange);

	//! copy selected tube content into vector
	void copyTubes(QVector<ListedMappedDetector> &cv);

	//! apply changed data
	void applyValues();

	//! get list of selected elements
	QList<QTableWidgetItem*> getSelectedItems();

	//! called due changed selections
	void selectionChanged(QList<QTableWidgetItem *> *paSelectedItems);

	//! read selected row from table widget
	void getRowData(int iRow);

	//! handle buttons of the dialog button box
	void handleStandardButton(int iButton);

	//! called by pressing "ESC"
	virtual void reject();

	//! qsort helper for QTableWidgetItem
	static bool myLessThanTW(const QTableWidgetItem *a, const QTableWidgetItem *b);

	//! qsort helper for ListedMappedDetector
	static bool myLessThanEV(const ListedMappedDetector &a, const ListedMappedDetector &b);
};

#endif // HISTOGRAMMAPPINGEDITOR_H
