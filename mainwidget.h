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
class QwtPlotSpectrogram;
class QwtPlotZoomer;
class QTimer;
class MesydaqSpectrumData;
class MesydaqHistogramData;
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
	QString buildTimestring(quint64 timeval, bool nano);
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
	void startStopSlot(bool = false);
	void setTimingSlot();
	void resetTimerSlot();
	void clearChanSlot();
	void clearMpsdSlot();
	void clearMcpdSlot();
	void clearAllSlot();
	void setStreamSlot();
	void setRunIdSlot();
	void displayMpsdSlot(int = -1);
	void displayMcpdSlot(int = -1);
	void allPulserOff();

	void mpsdCheck(int);

	void setModeSlot(int);
	void scanPeriSlot();

// setup related methods
	void restoreSetupSlot();
	void saveSetupSlot();
	void saveConfigSlot();

// list mode files related methods
	void replayListfileSlot();

	void linlogSlot();
	void applyThreshSlot();
	void readRegisterSlot();
	void writeRegisterSlot();
	void selectConfigpathSlot();
	void selectHistpathSlot();
	void selectListpathSlot();
	void writeHistSlot();

	void tPresetSlot(bool pr);
	void tResetSlot();

	void ePresetSlot(bool pr);
	void eResetSlot();

	void m1PresetSlot(bool pr);
	void m1ResetSlot();

	void m2PresetSlot(bool pr);
	void m2ResetSlot();

	void m3PresetSlot(bool pr);
	void m3ResetSlot();

	void m4PresetSlot(bool pr);
	void m4ResetSlot();

	void draw(void);

protected slots:
	void selectListfileSlot();
	void setThresholdSlot();
	void setGainSlot();
	void setPulserSlot();
	void setIpUdpSlot();
	void setMcpdIdSlot();

	void zoomAreaSelected(const QwtDoubleRect&);
	void zoomed(const QwtDoubleRect&);

protected:
	void timerEvent(QTimerEvent *event);

private:
	Mesydaq2	*m_theApp;
	
	ulong 		*m_pDispBuffer;

	quint16 	m_width;

// using thresholds for display
	//! using thresholds ?
	bool		m_dispThresh;

	//! lower limit of the threshold
	ulong 		m_dispLoThresh;

	//! upper limit of the threshold
	ulong 		m_dispHiThresh;

	//! scale the Y axis logarithmic
	bool		m_dispLog;

	//! plot curve
	QwtPlotCurve	*m_curve;

	//! plot diffractogram
	QwtPlotSpectrogram	*m_histogram;

	//! spectrum to be plotted
	MesydaqSpectrumData	*m_data;

	//! histogram to be plotted
	MesydaqHistogramData	*m_histData;	

	//! measurement objct
	Measurement 	*m_meas;

	int 		m_dispTimer;

	QwtPlotZoomer	*m_zoomer;

	bool		m_zoomEnabled;

	CorbaThread	*m_ct;

	ControlInterface *m_cInt;
};	

#endif
