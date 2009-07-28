/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann   *
 *   g.montermann@mesytec.com   *
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

class Mesydaq2;
class QwtPlotCurve;
class MesydaqData;

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
	void 	draw(void);
	void 	processDispData();
	void 	drawOpData();
	void 	dispFiledata(void);
	quint8 	getDispId(void);
	void 	updatePresets(void);
	void 	updateCaress(void);

public slots:
	virtual void sendAuxSlot();
	virtual void sendParamSlot();
	virtual void sendCellSlot();
	virtual void startStopSlot();
	virtual void setTimingSlot();
	virtual void resetTimerSlot();
	virtual void clearChanSlot();
	virtual void clearMpsdSlot();
	virtual void clearMcpdSlot();
	virtual void clearAllSlot();
	virtual void setStreamSlot();
	virtual void replayListfileSlot();
	virtual void setRunIdSlot();
	virtual void displayMpsdSlot(int = -1);
	virtual void displayMcpdSlot(int = -1);

	void mpsdCheck(int);

	virtual void setModeSlot(int);
	virtual void scanPeriSlot();
	virtual void restoreSetupSlot();
	virtual void linlogSlot();
	virtual void applyThreshSlot();
	virtual void saveSetupSlot();
	virtual void redrawSlot();
	virtual void readRegisterSlot();
	virtual void writeRegisterSlot();
	virtual void selectConfigpathSlot();
	virtual void selectHistpathSlot();
	virtual void selectListpathSlot();
	virtual void writeHistSlot();
	virtual void m2PresetSlot(bool pr);
	virtual void m1PresetSlot(bool pr);
	virtual void tPresetSlot(bool pr);
	virtual void ePresetSlot(bool pr);
	virtual void m2ResetSlot();
	virtual void m1ResetSlot();
	virtual void eResetSlot();
	virtual void tResetSlot();

	virtual void	saveConfigSlot();

protected slots:
	virtual void	selectListfileSlot();
	virtual void	setThresholdSlot();
	virtual void	setGainSlot();
	virtual void	setPulserSlot();
	virtual void	setIpUdpSlot();
	virtual void	setMcpdIdSlot();

private:
	quint16 	m_cmdBuffer[20];
	quint16 	*m_pBuffer;
	Mesydaq2	*m_theApp;
	QString		m_pstring;
	
	ulong 		*m_pDispBuffer;
	ulong 		m_dispMax;
	float		m_dispRange;
	ulong 		m_dispLen;

	quint16 	m_height;
	quint16 	width;
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
};	

#endif
