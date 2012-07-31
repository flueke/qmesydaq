/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2012 by Jens Kr�ger <jens.krueger@frm2.tum.de>     *
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

#include <QWidget>
#include <QColor>
#include <QTime>
#include "ui_mainwidget.h"
#include "plot.h"

class QTimer;

class Zoomer;
class MesydaqPlotSpectrogram;
class MesydaqSpectrumData;
class MesydaqHistogramData;
class Histogram;
class Mesydaq2;
class Measurement;
class CorbaThread;
class ControlInterface;

/**
    \short Main Widget in the application

    \author Gregor Montermann <g.montermann@mesytec.com>
*/
class MainWidget : public QWidget, public Ui_MainWidget
{
Q_OBJECT
public:
	//! The different access levels
	enum UserLevel 
	{
		User = 0, 	//!< normal user
		Expert,		//!< experient user (like instrument responsible)
		SuperUser,	//!< super user (she has access to all features)
	};

public:
	MainWidget(Mesydaq2 *, QWidget* parent = 0);

	~MainWidget();

	void selectUserMode(int);

	void 	closeEvent(QCloseEvent *);
signals:
	//! will be emitted in case of start of data acquisition
	void	started(bool);

	//! will be emitted to indicate that the data have do be redrawn
	void	redraw(void);

public slots:
	void	exportPDF();

	void	exportSVG();

	void 	printPlot();

	void	about();

	void	quitContinue();

	void	setupGeneral();

	void 	setupModule();

	void 	setupModule(quint8 id);

	void 	setupMdll();

	void 	setupMdll(quint8 id);

	void	setupMCPD();

	void 	addMCPD();

	//! load configuration file
	void	loadConfiguration(const QString &sFilename);

	void	tPresetSlot(bool pr);

	void	tResetSlot();

	void	ePresetSlot(bool pr);

	void	eResetSlot();

	void	m1PresetSlot(bool pr);

	void	m1ResetSlot();

	void	m2PresetSlot(bool pr);

	void	m2ResetSlot();

	void	m3PresetSlot(bool pr);

	void	m3ResetSlot();

	void	m4PresetSlot(bool pr);

	void	m4ResetSlot();

protected:
        void    customEvent(QEvent *);

//	void 	paintEvent(QPaintEvent *);

private:
#if QWT_VERSION < 0x060000
	void	print(QPrinter *, QwtPlotPrintFilter &);
#endif

	void 	updateDisplay(void);

	void 	drawOpData();

	void 	updatePresets(void);

private slots:
	void displayMpsdSlot(int = -1);

	void startStopSlot(bool = false);

	void clearChanSlot();

	void clearMpsdSlot();

	void clearMcpdSlot();

	void clearAllSlot();

	void setStreamSlot();

	void displayMcpdSlot(int = -1);

	void allPulserOff();

	void mpsdCheck(int);

	void scanPeriSlot(bool = true);

	void newSetupSlot(void);

// setup related methods
	void restoreSetupSlot();

	void saveSetupSlot();

// list mode files related methods
	void replayListfileSlot();

	void checkListfilename(bool);

	void applyThreshSlot();

	void writeHistSlot();

	void loadHistSlot();

	void loadCalibrationSlot();

	void draw(void);

	void moduleHistogramSlot(quint8, bool);

	void moduleActiveSlot(quint8, bool);

	void statusTabChanged(int);

	void setDisplayMode(int);

	void setHistogramType(int);

private:
	void 	dispFiledata(void);

	void 	timerEvent(QTimerEvent *event);

	QString buildTimestring(quint64 timeval, bool nano);

	void	init();

	QString selectListfile(void);

private:
	//! The MesyDaq object
	Mesydaq2		*m_theApp;
	
	
// using thresholds for display
	//! using thresholds ?
	bool			m_dispThresh;

	//! lower limit of the threshold
	ulong 			m_dispLoThresh;

	//! upper limit of the threshold
	ulong 			m_dispHiThresh;

	//! spectrum to be plotted
	MesydaqSpectrumData	*m_data;

	//! histogram to be plotted
	MesydaqHistogramData	*m_histData;

	//! measurement objct
	Measurement 		*m_meas;

	//! display timer ID
	int 			m_dispTimer;

	//! zoomer
	Zoomer			*m_zoomer;

	//! ???
	CorbaThread		*m_ct;

	//! the remote control interface
	ControlInterface 	*m_controlInt;

	//! the printer
	QPrinter		*m_printer;

	//! time object
	QTime			m_time;

	//! histogram object currently used
	Histogram		*m_histogram;

	Plot::Mode		m_mode;

	Plot 			*m_dataFrame;
};	

#endif
