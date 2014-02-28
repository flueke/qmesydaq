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

#include "histogrammappingeditor.h"
#include "ui_histogrammappingeditor.h"
#include <QMenuBar>
#include <QKeyEvent>
#include <QToolTip>
#include <QMessageBox>
#include "measurement.h"
#include "mappedhistogram.h"
#include "usermapcorrect.h"
#include "data.h"
#include "mappeddetector.h"
#include "editormemory.h"
#include "mappeddetector.h"
#include "logging.h"

static const QString g_sUndefined("#undef.");
const int g_iColumnCount = 6;

HistogramMappingEditor::HistogramMappingEditor(Measurement *pMeasurement, QWidget *pParent) :
	QDialog(pParent),
	ui(new Ui::HistogramMappingEditor),
	m_pMeasurement(pMeasurement),
	m_pEditor(NULL),
	m_iDefaultTubes(30),
	m_bChanged(false),
	m_bClose(false)
{
	ui->setupUi(this);

	ui->TableWidget->setColumnWidth(0, 0);
	QGridLayout* pLayout = dynamic_cast<QGridLayout*>(layout());
	if (pLayout != NULL)
	{
		ui->toolBar->setMovable(false);
		ui->toolBar->setVisible(true);

		QMargins margins(pLayout->contentsMargins());
		margins.setTop(0);
		pLayout->setContentsMargins(margins);
		pLayout->setRowMinimumHeight(0, ui->toolBar->sizeHint().height());
		pLayout->setRowStretch(0, 0);
		for (int i = 1; i < pLayout->rowCount(); ++i)
			pLayout->setRowStretch(i, 10);
	}

	int iDefault(-1);
	if (pMeasurement->hist(Measurement::PositionHistogram) != NULL)
		iDefault = pMeasurement->hist(Measurement::PositionHistogram)->width();
	if (iDefault <= 0 && pMeasurement->posHistMapCorrection() != NULL)
		iDefault = pMeasurement->posHistMapCorrection()->getSourceSize().width();
	if (iDefault > 0)
		m_iDefaultTubes = iDefault;
	ui->ChannelNumber->setText(QString().setNum(m_iDefaultTubes));
	ui->ChannelNumber->setEnabled(iDefault <= 0); // en-/disable editing of channel number

	UserMapCorrection* pUserMapCorrection(dynamic_cast<UserMapCorrection*>(m_pMeasurement->posHistMapCorrection()));
	Q_ASSERT(pUserMapCorrection != NULL);
	EditorMemory* pSourceEditor(pUserMapCorrection->getMESFData());
	if (pSourceEditor == NULL)
	{
		pSourceEditor = new EditorMemory(this, m_iDefaultTubes);
		pUserMapCorrection->setMESFData(pSourceEditor);
	}
	m_pEditor = new EditorMemory(this, pSourceEditor);
	if (m_pEditor->getChannelCount() < 1)
		m_pEditor->tubesResize(m_iDefaultTubes);
	show();

	QPalette palette = QToolTip::palette();
	palette.setColor(QPalette::ToolTipText, Qt::black);
	QToolTip::setPalette(palette);
}

HistogramMappingEditor::~HistogramMappingEditor()
{
	delete m_pEditor;
	delete ui;
}

void HistogramMappingEditor::show()
{
	QStringList asLabels;
	int iRowCount(m_pEditor->getChannelCount());
	asLabels.reserve(iRowCount);
	for (int i = 0; i < iRowCount; ++i)
		asLabels << QString().setNum(i);
	ui->TableWidget->setRowCount(iRowCount);
	ui->TableWidget->setVerticalHeaderLabels(asLabels);

	const QVector<MappedDetector>& aTubes = m_pEditor->getTubes();
	for (int i = 0; i < iRowCount; ++i)
	{
		const MappedDetector &md = aTubes[i];
		ui->TableWidget->setItem(i, 0, new QTableWidgetItem(QString().setNum(i)));
		ui->TableWidget->setItem(i, 1, new QTableWidgetItem(md.getStartInput()  < 0 ? g_sUndefined : QString().setNum(md.getStartInput())));
		ui->TableWidget->setItem(i, 2, new QTableWidgetItem(md.getEndInput()    < 0 ? g_sUndefined : QString().setNum(md.getEndInput())));
		ui->TableWidget->setItem(i, 3, new QTableWidgetItem(md.getStartOutput() < 0 ? g_sUndefined : QString().setNum(md.getStartOutput())));
		ui->TableWidget->setItem(i, 4, new QTableWidgetItem(md.getEndOutput()   < 0 ? g_sUndefined : QString().setNum(md.getEndOutput())));
		ui->TableWidget->setItem(i, 5, new QTableWidgetItem(md.getFactor()   <= 0.0 ? g_sUndefined : QString().setNum(md.getFactor())));
	}
	ui->ChannelNumber->setText(QString().setNum(iRowCount));
	ui->Orientation->setCurrentIndex(m_pEditor->getOrientation());
	selectionChanged(NULL);
}

bool HistogramMappingEditor::keyPress(QKeyEvent* pEvent)
{
	switch (pEvent->modifiers())
	{
		case Qt::ControlModifier:
			switch (pEvent->key())
			{
				case Qt::Key_A:
					ui->TableWidget->selectAll();
					selectionChanged(NULL);
					return true;
				case Qt::Key_C:
					copyTubesClick();
					return true;
				case Qt::Key_V:
					pasteTubesClick();
					return true;
				case Qt::Key_Up:
					shiftTubesUp();
					return true;
				case Qt::Key_Down:
					shiftTubesDown();
					return true;
				default:
					break;
			};
			break;
		case Qt::NoModifier:
			switch (pEvent->key())
			{
				case Qt::Key_Delete:
					clearClick();
					return true;
				case Qt::Key_Help:
				case Qt::Key_F1:
					helpClick();
					return true;
				case Qt::Key_Return:
				case Qt::Key_Enter: // ignore
					return true;
				default:
					break;
			};
			break;
		default:
			break;
	}
	return false;
}

void HistogramMappingEditor::textChanged(const QString &sNewInput)
{
	QString sChangedInput;
	int i(0);
	int iPos(ui->Channel->cursorPosition());

	while (i < sNewInput.length() && !sNewInput[i].isDigit())
		++i;
	while (i < sNewInput.length() && sNewInput[i].isDigit())
		sChangedInput.append(sNewInput[i++]);

	if (!sChangedInput.isEmpty())
	{
		int iNum1(sChangedInput.toInt());
		int iNum2(0);
		QString sMinus;

		sChangedInput.clear();
		while (i < sNewInput.length() && !sNewInput[i].isDigit())
		{
			++i;
			sMinus = "-";
		}
		while (i < sNewInput.length() && sNewInput[i].isDigit())
			sChangedInput.append(sNewInput[i++]);
		if (!sChangedInput.isEmpty())
		{
			iNum2 = sChangedInput.toInt();
			sChangedInput = QString().setNum(iNum1) + sMinus + QString().setNum(iNum2);
		}
		else
		{
			sChangedInput = QString().setNum(iNum1) + sMinus;
			iNum2 = iNum1;
		}

		ui->TableWidget->blockSignals(true);
		ui->TableWidget->selectRow(1);
		ui->TableWidget->selectRow(0);
		ui->TableWidget->selectionModel()->clearSelection();

		int iHeight(ui->TableWidget->rowHeight(0));
		ui->TableWidget->mousePressEvent(new QMouseEvent(QEvent::MouseButtonPress, QPoint(100, iNum1 * iHeight), Qt::LeftButton, Qt::NoButton, Qt::NoModifier));
		ui->TableWidget->mousePressEvent(new QMouseEvent(QEvent::MouseButtonPress, QPoint(100, iNum2 * iHeight), Qt::LeftButton, Qt::NoButton, Qt::ShiftModifier));
		ui->TableWidget->blockSignals(false);

		QList<QTableWidgetItem*> aSelectedItems(getSelectedItems());
		selectionChanged(&aSelectedItems);
		getRowData(aSelectedItems.size() == g_iColumnCount ? aSelectedItems[0]->row()  : -1);
	}
	ui->Channel->blockSignals(true);
	ui->Channel->setText(sChangedInput);
	ui->Channel->setCursorPosition(iPos);
	ui->Channel->blockSignals(false);
}

void HistogramMappingEditor::itemSelectionChanged()
{
	QList<QTableWidgetItem*> aSelectedItems(getSelectedItems());
	int iSelectedRows(aSelectedItems.size() / g_iColumnCount);
	QString sSelection;
	if (iSelectedRows == 1)
	{
		int iRow(aSelectedItems[0]->row());
		getRowData(iRow);
		sSelection.setNum(iRow);
	}
	else if (iSelectedRows > 0)
	{
		int iLow(m_iDefaultTubes), iHigh(-1), iLast(-10);
		for (int i = 0;; ++i)
		{
			int iRow(-1);
			if (i < iSelectedRows)
				iRow = aSelectedItems[g_iColumnCount * i]->row();
			if ((iLast + 1) != iRow || i >= iSelectedRows)
			{
				if (iLow == iHigh)
					sSelection += QString(",%1").arg(iLow);
				else if (iLow < iHigh)
					sSelection += QString(",%1-%2").arg(iLow).arg(iHigh);
				iLow  = m_iDefaultTubes;
				iHigh = -1;
				if (i >= iSelectedRows)
					break;
			}
			if (iLow > iRow) iLow = iRow;
			if (iHigh < iRow) iHigh = iRow;
			iLast = iRow;
		}
		sSelection.remove(0,1);
	}
	ui->Channel->blockSignals(true);
	ui->Channel->setText(sSelection);
	ui->Channel->blockSignals(false);

	selectionChanged(&aSelectedItems);
}

void HistogramMappingEditor::selectionChanged(QList<QTableWidgetItem*> *paSelectedItems)
{
	int iSelectedRows(0);
	if (paSelectedItems != NULL)
		iSelectedRows = paSelectedItems->size();
	else
		iSelectedRows = getSelectedItems().size();
	iSelectedRows /= g_iColumnCount;

	QString sText("Appl&y to Row");
	if (iSelectedRows != 1)
		sText += 's';
	ui->ApplyData->setText(sText);
	sText = "Clea&r Row";
	if (iSelectedRows != 1)
		sText += 's';
	ui->ClearData->setText(sText);

	bool bEnable(iSelectedRows > 0);
	ui->LabelStartInput ->setEnabled(bEnable);
	ui->LabelEndInput   ->setEnabled(bEnable);
	ui->LabelStartOutput->setEnabled(bEnable);
	ui->LabelEndOutput  ->setEnabled(bEnable);
	ui->LabelFactor     ->setEnabled(bEnable);
	ui->StartInput      ->setEnabled(bEnable);
	ui->EndInput        ->setEnabled(bEnable);
	ui->StartOutput     ->setEnabled(bEnable);
	ui->EndOutput       ->setEnabled(bEnable);
	ui->Factor          ->setEnabled(bEnable);
	ui->ApplyData       ->setEnabled(bEnable);
	ui->ClearData       ->setEnabled(bEnable);
	ui->actionCopy      ->setEnabled(bEnable);
	ui->actionApplyData ->setEnabled(bEnable);
	ui->actionClearData ->setEnabled(bEnable);
	ui->actionMoveDown  ->setEnabled(bEnable);
	ui->actionMoveUp    ->setEnabled(bEnable);
	ui->actionPaste     ->setEnabled(bEnable && m_aClipboard.size() > 0);
}

void HistogramMappingEditor::getRowData(int iRow)
{
	const QVector<MappedDetector>& aTubes(m_pEditor->getTubes());
	if (iRow >= 0 && iRow < aTubes.size())
	{
		const MappedDetector &md = aTubes[iRow];
		ui->StartInput ->setText((md.getStartInput()  >= 0) ? QString().setNum(md.getStartInput())  : "");
		ui->EndInput   ->setText((md.getEndInput()    >= 0) ? QString().setNum(md.getEndInput())    : "");
		ui->StartOutput->setText((md.getStartOutput() >= 0) ? QString().setNum(md.getStartOutput()) : "");
		ui->EndOutput  ->setText((md.getEndOutput()   >= 0) ? QString().setNum(md.getEndOutput())   : "");
		ui->Factor     ->setText((md.getFactor()     > 0.0) ? QString().setNum(md.getFactor())      : "");
	}
	else
	{
		ui->StartInput ->clear();
		ui->EndInput   ->clear();
		ui->StartOutput->clear();
		ui->EndOutput  ->clear();
		ui->Factor     ->clear();
	}
}

bool HistogramMappingEditor::myLessThanTW(const QTableWidgetItem *a, const QTableWidgetItem *b)
{
	register int iRowA(a->row());
	register int iRowB(b->row());
	return iRowA < iRowB || (iRowA == iRowB && a->column() < b->column());
}

QList<QTableWidgetItem*> HistogramMappingEditor::getSelectedItems()
{
	QList<QTableWidgetItem*> aList(ui->TableWidget->selectedItems());
	qSort(aList.begin(), aList.end(), HistogramMappingEditor::myLessThanTW);
	return aList;
}

void HistogramMappingEditor::saveClick()
{
	m_pEditor->save();
}

void HistogramMappingEditor::loadClick()
{
	m_pEditor->wantToSave(this, m_iDefaultTubes);
	m_pEditor->load();
	show();
}

void HistogramMappingEditor::copyTubesClick()
{
	copyTubes(m_aClipboard);
	selectionChanged(NULL);
}

void HistogramMappingEditor::copyTubes(QVector<ListedMappedDetector> &cv)
{
	QList<QTableWidgetItem*> aList(getSelectedItems());
	int iLength(aList.size() / g_iColumnCount);
	cv.clear();
	for (int i = 0; i < iLength; ++i)
	{
		MappedDetector md(
			(aList[i * g_iColumnCount + 1]->text() != g_sUndefined) ? aList[i * g_iColumnCount + 1]->text().toInt()   : -1,
			(aList[i * g_iColumnCount + 2]->text() != g_sUndefined) ? aList[i * g_iColumnCount + 2]->text().toInt()   : -1,
			(aList[i * g_iColumnCount + 3]->text() != g_sUndefined) ? aList[i * g_iColumnCount + 3]->text().toInt()   : -1,
			(aList[i * g_iColumnCount + 4]->text() != g_sUndefined) ? aList[i * g_iColumnCount + 4]->text().toInt()   : -1,
			(aList[i * g_iColumnCount + 5]->text() != g_sUndefined) ? aList[i * g_iColumnCount + 5]->text().toFloat() : 0.0);
		cv.append(ListedMappedDetector(md, (aList[i * g_iColumnCount]->text() != g_sUndefined) ? aList[i * g_iColumnCount]->text().toInt() : -1));
	}
}

void HistogramMappingEditor::pasteTubesClick()
{
	QList<QTableWidgetItem*> aSelectedItems(getSelectedItems());
	QVector<MappedDetector> &aTubes = m_pEditor->getTubes();

	int iStartRow(aSelectedItems[0]->row());
	int iLength(aSelectedItems.size() / g_iColumnCount);

	for (int i = 1; i < iLength; ++i)
		if (iStartRow > aSelectedItems[g_iColumnCount * i]->row())
			iStartRow = aSelectedItems[g_iColumnCount * i]->row();

	for (int i = 0; i < m_aClipboard.size(); ++i)
	{
		if ((iStartRow + i) >= aTubes.size())
			break;
		aTubes[iStartRow + i].set(m_aClipboard[i]);
		m_pEditor->setChanged();
	}
	if (iLength == 1)
		getRowData(iStartRow);
	show();
}

bool HistogramMappingEditor::myLessThanEV(const ListedMappedDetector &a, const ListedMappedDetector &b)
{
	return (a.getChannelNumber() < b.getChannelNumber());
}

void HistogramMappingEditor::shiftTubesUp()
{
	shiftTubes(-1);
}

void HistogramMappingEditor::shiftTubesDown()
{
	shiftTubes(1);
}

void HistogramMappingEditor::shiftTubes(int iShiftRange)
{
	int iLength(m_pEditor->getTubes().size());
	if (iLength < 1 || iShiftRange == 0)
		return;

	QVector<ListedMappedDetector> aExtractionVec;
	copyTubes(aExtractionVec);

	ui->TableWidget->selectionModel()->clearSelection();
	ui->TableWidget->setSelectionMode(QAbstractItemView::MultiSelection);
	if (aExtractionVec.size() > 0)
	{
		for (int i = aExtractionVec.size() - 1; i >= 0; --i)
		{
			int iRow(aExtractionVec[i].getChannelNumber());
			m_pEditor->getTubes().remove(iRow);
			iRow += iShiftRange;
			while (iRow < 0)
				iRow += iLength;
			aExtractionVec[i].setChannelNumber(iRow % iLength);
		}

		qSort(aExtractionVec.begin(), aExtractionVec.end(), HistogramMappingEditor::myLessThanEV);

		for (int i = 0; i < aExtractionVec.size(); ++i)
		{
			int iRow(aExtractionVec[i].getChannelNumber());
			m_pEditor->getTubes().insert(iRow, aExtractionVec[i]);
			ui->TableWidget->selectRow(iRow);
		}
		show();
		ui->TableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
		m_pEditor->setChanged();
	}
}

void HistogramMappingEditor::applyValues()
{
	QList<QTableWidgetItem*> list = getSelectedItems();
	int length = list.size() / g_iColumnCount;

	int   iStartInput ((!ui->StartInput ->text().isEmpty()) ? ui->StartInput ->text().toInt()   : -1);
	int   iEndInput   ((!ui->EndInput   ->text().isEmpty()) ? ui->EndInput   ->text().toInt()   : -1);
	int   iStartOutput((!ui->StartOutput->text().isEmpty()) ? ui->StartOutput->text().toInt()   : -1);
	int   iEndOutput  ((!ui->EndOutput  ->text().isEmpty()) ? ui->EndOutput  ->text().toInt()   : -1);
	float fFactor     ((!ui->Factor     ->text().isEmpty()) ? ui->Factor     ->text().toFloat() : 0.0);

	for (int i = 0; i < length; ++i)
	{
		MappedDetector &md = m_pEditor->getTubes()[list[i * g_iColumnCount]->row()];
		md.set(iStartInput, iEndInput, iStartOutput, iEndOutput, fFactor);
	}
	if (length > 0)
		m_pEditor->setChanged();
}

void HistogramMappingEditor::applyClick()
{
	MapCorrection::Orientation iOldOrientation(m_pEditor->getOrientation());
	MapCorrection::Orientation iNewOrientation((MapCorrection::Orientation)ui->Orientation->currentIndex());
	if (iOldOrientation != iNewOrientation)
	{
		m_pEditor->setOrientation(iNewOrientation);
		m_pEditor->setChanged();
	}
	applyValues();
	if (!ui->ChannelNumber->isReadOnly())
	{
		int iOldCount(m_pEditor->getTubes().size());
		int iNewCount(ui->ChannelNumber->text().toInt());
		if (iNewCount >= 0 && iOldCount != iNewCount)
		{
			m_pEditor->tubesResize(iNewCount);
			m_pEditor->setChanged();
		}
	}
	ui->ChannelNumber->setText(QString().setNum(m_pEditor->getTubes().size()));
	show();
}

void HistogramMappingEditor::clearClick()
{
	QList<QTableWidgetItem*> aSelectedItems(getSelectedItems());
	int iLength(aSelectedItems.size() / g_iColumnCount);
	if (iLength > 0)
	{
		for (int i = 0; i < iLength; ++i)
			m_pEditor->getTubes()[aSelectedItems[i * g_iColumnCount]->row()].clr();
		m_pEditor->setChanged();
		show();
	}
}

void HistogramMappingEditor::helpClick()
{
	QString sHelp("<b>This editor helps you to create/edit a mapping for the corrected position histogram.</b>");
	QMessageBox msgbox(QMessageBox::Information, QString("Histogram mapping editor help"), sHelp, QMessageBox::Close, this);
	if (ui->ChannelNumber->isReadOnly())
		sHelp = "<b>Info:</b> the <b><i>\"Channel Number\"</i></b> is fixed, <b>QMesyDAQ</b> does know this number (in the other case you had to provide this).";
	else
		sHelp = "First set/change the number of tubes of your hardware into <b><i>&quot;Channel Number&quot;</i></b>, because QMesyDAQ does not know this number.";
	sHelp += " The <b><i>&quot;Orientation&quot;</i></b> is used for displaying this data, not for mapping. This values are possible:<ul>" \
			 "<li><u>up:</u> <i>unrotated</i> - up=<i>up</i>, down=<i>down</i>, left=<i>left</i>, right=<i>right</i></li>" \
			 "<li><u>down:</u> rotated by 180deg</li><li><u>left:</u> rotated by 90deg ccw</li><li><u>right:</u> rotated by 90deg cw</li>" \
			 "<li><u>uprev:</u> up=<i>up</i>, down=<i>down</i>, but left and right are mirrored</li><li><u>downrev:</u> up and down are " \
			 "mirrored, but left=<i>left</i>, right=<i>right</i></li><li><u>leftrev:</u> rotated by 90deg ccw, but up and down are mirrored</li>" \
			 "<li><u>rightrev:</u> rotated by 90deg cw, but up and down are mirrored</li></ul><b>How to use this editor:</b><ol>" \
			 "<li>Select one or more lines in the table for editing. If you select one line only, the row data will be copied" \
			 " to the text input fields. You can also select lines by typing the line number into <b><i>&quot;Channel Selection&quot;</i></b>.</li>" \
			 "<li>Input or change the start- and end of data (inputs: <b><i>Start Input</i></b>, <b><i>End Input</i></b>)" \
			 " and position (corrected outputs: <b><i>Start Output</i></b>, <b><i>End Output</i></b>).</li>" \
			 "<li>Press the <b><i>&quot;Apply Data&quot;</i></b> button and the data will be copied back to the selected rows in the table.</li>" \
			 "</ol>With the button <b><i>&quot;Clear Data&quot;</i></b> you can clear the selected rows and the string <b><i>&quot;";
	sHelp += g_sUndefined;
	sHelp += "&quot;</i></b> will appear. Cleared rows or rows with <b><i>&quot;";
	sHelp += g_sUndefined;
	sHelp += "&quot;</i></b> items will be skipped in the corrected output histogram and a complete empty mapping will result in a default mapping" \
			 "(which is the same like the button <b><i>&quot;Restore Defaults&quot;</i></b>).";
	msgbox.setInformativeText(sHelp);
	msgbox.exec();
}

void HistogramMappingEditor::setDefaultsClick()
{
	if (QMessageBox::Yes != QMessageBox::question(this, "Reset to default histogram",
			"<b>Do you want to reset the content to defaults?</b><br>Note: you also have to &quot;Apply&quot; the default mapping!", QMessageBox::Yes, QMessageBox::No))
		return;
	m_pEditor->wantToSave(this, m_iDefaultTubes);
	delete m_pEditor;
	m_pEditor = new EditorMemory(this, m_iDefaultTubes);
	show();
}

void HistogramMappingEditor::handleStandardButton(int iButton)
{
	switch (iButton)
	{
		case QDialogButtonBox::QDialogButtonBox::Reset: // reset to defaults
		case QDialogButtonBox::QDialogButtonBox::RestoreDefaults:
			setDefaultsClick();
			break;
		case QDialogButtonBox::Help: // help
			helpClick();
			break;
		case QDialogButtonBox::Close: // close
			m_bClose = true;
			if (!m_pEditor->needsToSave())
				break;
			if (QMessageBox::Yes != QMessageBox::question(this, "Apply changes", "Do you want to apply the changes, you made?", QMessageBox::No, QMessageBox::Yes))
				break;
			/*no break*/
		case QDialogButtonBox::Apply: // apply data
		case QDialogButtonBox::Ok:
		{
			MapCorrection* &pCorrection(m_pMeasurement->posHistMapCorrection());
			UserMapCorrection* pUserMapCorrection(dynamic_cast<UserMapCorrection*>(pCorrection));
			Histogram* pHistogram(m_pMeasurement->hist(Measurement::PositionHistogram));

			if (pUserMapCorrection == NULL)
			{
				// destroy default mapping
				delete pCorrection;
				pCorrection = pUserMapCorrection = new UserMapCorrection(QSize(pHistogram->width(), pHistogram->height()),
					m_pEditor->getOrientation(), MapCorrection::CorrectSourcePixel);
				Q_ASSERT(pUserMapCorrection != NULL);
			}
			m_pEditor->setOrientation((MapCorrection::Orientation)ui->Orientation->currentIndex());

			// copy mapping into UserMapCorrection
			EditorMemory* pSourceEditor(pUserMapCorrection->getMESFData());
			*pSourceEditor = *m_pEditor;
			pSourceEditor->setChanged(m_pEditor->needsToSave());
			if (pUserMapCorrection->setMapCorrection(*m_pEditor))
				m_pMeasurement->setCalibrationfilename(m_pEditor->getPath());
			else
			{
				// create default mapping
				delete pCorrection;
				pCorrection = NULL;
				m_pMeasurement->readCalibration(QString(""), true);
				pCorrection = m_pMeasurement->posHistMapCorrection();
			}

			// apply mapping to mapped histogram
			MappedHistogram *pMappedHistogram(dynamic_cast<MappedHistogram*>(m_pMeasurement->hist(Measurement::CorrectedPositionHistogram)));
			if (pMappedHistogram != NULL)
				pMappedHistogram->setMapCorrection(pCorrection, m_pMeasurement->hist(Measurement::PositionHistogram));

			emit applyedData();
			m_bChanged = true;
			break;
		}
		case QDialogButtonBox::Cancel: // cancel
			m_bClose = true;
			break;
		default: // Yes, No and others are ignored
			break;
	}
}

void HistogramMappingEditor::reject()
{
	if (!m_bClose)
		handleStandardButton(QDialogButtonBox::Close);
	QDialog::reject();
}

void HistogramMappingEditor::on_buttonBox_clicked(QAbstractButton* pButton)
{
	if (pButton != NULL)
		handleStandardButton(ui->buttonBox->standardButton(pButton));
}
