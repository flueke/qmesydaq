/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2014 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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

#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <QMainWindow>

class Plot;
class SpectrumData;
class HistogramData;
class DiffractogramData;
class PlotWidget;

class MainWindow: public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget * = NULL);

private slots:
	void setDisplayMode(int);

private:
	PlotWidget		*m_plotWidget;

	Plot 			*m_plot;

	SpectrumData		*m_spectrumData;

	HistogramData		*m_histogramData;

	DiffractogramData	*m_diffData;
};

#endif
