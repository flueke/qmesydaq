/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009 by Jens Krüger <jens.krueger@frm2.tum.de>          *
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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mesydaq2mainwidget.h"

class QwtPlotCurve;
class QTimer;
class MesydaqData;
class Mesydaq2;
class Measurement;
class CorbaThread;
class ControlInterface;

/**
	@author Gregor Montermann <g.montermann@mesytec.com>
*/
class MainWidget : public QWidget, public Ui_Mesydaq2MainWidget
{
Q_OBJECT
public:
	MainWidget(Mesydaq2 *, QWidget* parent = 0);

	~MainWidget();

	void 	update(void);
	QString buildTimestring(ulong timeval, bool nano);
	void 	setData(ulong * data, quint32 len, ulong max);
	void 	processDispData();
	void 	drawOpData();
	void 	dispFiledata(void);
	quint8 	getDispId(void);
	void 	updatePresets(void);
	void 	updateCaress(void);

public slots:
	void sendAuxSlot();
	void sendParamSlot();
	void sendCellSlot();
	void startStopSlot();
	void setTimingSlot();
	void resetTimerSlot();
	void clearChanSlot();
	void clearMpsdSlot();
	void clearMcpdSlot();
	void clearAllSlot();
	void setStreamSlot();
	void replayListfileSlot();
	void setRunIdSlot();
	void displayMpsdSlot(int = -1);
	void displayMcpdSlot(int = -1);

	void mpsdCheck(int);

	void setModeSlot(int);
	void scanPeriSlot();
	void restoreSetupSlot();
	void linlogSlot();
	void applyThreshSlot();
	void saveSetupSlot();
	void redrawSlot();
	void readRegisterSlot();
	void writeRegisterSlot();
	void selectConfigpathSlot();
	void selectHistpathSlot();
	void selectListpathSlot();
	void writeHistSlot();
	void m2PresetSlot(bool pr);
	void m1PresetSlot(bool pr);
	void tPresetSlot(bool pr);
	void ePresetSlot(bool pr);
	void m2ResetSlot();
	void m1ResetSlot();
	void eResetSlot();
	void tResetSlot();

	void saveConfigSlot();

	void draw(void);

protected slots:
	void selectListfileSlot();
	void setThresholdSlot();
	void setGainSlot();
	void setPulserSlot();
	void setIpUdpSlot();
	void setMcpdIdSlot();

private:
	Mesydaq2	*m_theApp;
	
	ulong 		*m_pDispBuffer;
	ulong 		m_dispMax;
	float		m_dispRange;
	ulong 		m_dispLen;

	quint16 	m_height;
	quint16 	m_width;
	quint16 	xstep;
	float		ystep;
	ulong 		dispLoThresh;
	ulong 		dispHiThresh;
	quint8 		scale;
	bool		dispLog;
	bool		dispThresh;
	quint8 		dispId;

	QwtPlotCurve	*m_curve;

	MesydaqData	*m_data;

	Measurement 	*m_meas;

	CorbaThread	*m_ct;

	ControlInterface *m_cInt;

	QTimer 		*m_dispTimer;
};	

#endif
