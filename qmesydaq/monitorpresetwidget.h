/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2014 by Jens Kr�ger <jens.krueger@frm2.tum.de>     *
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
#ifndef MONITOR_PRESET_WIDGET_H
#define MONITOR_PRESET_WIDGET_H

#include <QWidget>
#include "ui_monitorpresetwidget.h"

class QMouseEvent;

/*!
    \class MonitorPresetWidget

    \short This class handles displaying and handling of the presets

    \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class MonitorPresetWidget : public QWidget, protected Ui_MonitorPresetWidget
{
	Q_OBJECT
public:
	MonitorPresetWidget(QWidget * = 0);

	void setId(const int id);

	void setLabel(const QString &);

	bool isChecked(void);

	void setChecked(const bool);

	void setPresetValue(const quint64 = 0);

	void setValue(const quint64 = 0);

	void setRate(const quint64 = 0);

	quint64 presetValue(void);

	void setConfigureMode(const bool);

	bool isInConfigureMode(void)
	{
		return mcpdSpinBox->isEnabled();
	}

	void configureMapping(const int, const int);

	void setMCPDList(QList<int> modules);

signals:
	//! this signal will be emitted if the reset button is pressed
	void resetClicked(int);

	//! this signal will be emitted if the check button for the master is changed
	void presetClicked(int, bool);

	void mappingChanged(int, quint16, qint8, qint8);

private slots:
	void presetCheckClicked(bool);

	void resetButtonClicked(void);

	void mcpdChanged(int);

	void inputChanged(int);

private:
	void emitMappingChanged(void);

private:
	int m_id;
};
#endif
