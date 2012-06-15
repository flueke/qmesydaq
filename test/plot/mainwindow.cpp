#include "mainwindow.h"

#include <QMainWindow>
#include <QToolBar>
#include <QToolButton>

#include "plot.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, m_plot(NULL)
{
	m_plot = new Plot(this);

	setCentralWidget(m_plot);

	QToolBar *toolBar = new QToolBar(this);

	QToolButton *btnSpectrogram = new QToolButton(toolBar);
//	QToolButton *btnContour = new QToolButton(toolBar);
//	QToolButton *btnPrint = new QToolButton(toolBar);

	btnSpectrogram->setText("Spectrogram");
//	btnSpectrogram->setIcon(QIcon());
	btnSpectrogram->setCheckable(true);
	btnSpectrogram->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	toolBar->addWidget(btnSpectrogram);

//	btnContour->setText("Contour");
//	btnContour->setIcon(QIcon());
//	btnContour->setCheckable(true);
//	btnContour->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
//	toolBar->addWidget(btnContour);

//	btnPrint->setText("Print");
//	btnPrint->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
//	toolBar->addWidget(btnPrint);

	addToolBar(toolBar);

	connect(btnSpectrogram, SIGNAL(toggled(bool)), this, SLOT(showSpectrogram(bool)));
//	connect(btnContour, SIGNAL(toggled(bool)), d_plot, SLOT(showContour(bool)));
//	connect(btnPrint, SIGNAL(clicked()), d_plot, SLOT(printPlot()) );

	btnSpectrogram->setChecked(true);
//	btnContour->setChecked(false);
}

void MainWindow::showSpectrogram(bool val)
{
	if (val)
		m_plot->setDisplayMode(Plot::Histogram);
	else
		m_plot->setDisplayMode(Plot::Spectrum);
}
