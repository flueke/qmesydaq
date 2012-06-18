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

#include "mainwindow.h"

#include <QToolBar>
#include <QToolButton>

#include "plotwidget.h"
#include "plot.h"
#include "data.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, m_plot(NULL)
	, m_spectrumData(NULL)
	, m_histogramData(NULL)
{
	m_plotWidget = new PlotWidget(this);

	setCentralWidget(m_plotWidget);

	m_plot = m_plotWidget->m_plot;

// Create spectrum data
	m_spectrumData = new SpectrumData(::myspectrum, 100);
// Create histogram data
	m_histogramData = new HistogramData();

	m_plot->setSpectrumData(m_spectrumData);
	m_plot->setHistogramData(m_histogramData);
	m_plot->setDisplayMode(Plot::Histogram);

	connect(m_plotWidget->radioHistogram, SIGNAL(toggled(bool)), this, SLOT(showSpectrogram(bool)));
}

void MainWindow::showSpectrogram(bool val)
{
	if (val)
	{
		m_plot->setSpectrumData(m_spectrumData);
		m_plot->setDisplayMode(Plot::Histogram);
	}
	else
	{
		m_plot->setHistogramData(m_histogramData);
		m_plot->setDisplayMode(Plot::Spectrum);
	}
}
