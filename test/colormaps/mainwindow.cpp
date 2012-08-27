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

#include <QGridLayout>
#include <QRadioButton>

#include "mainwindow.h"

#include <qwt_scale_widget.h>

#include "colormaps.h"

MainWindow::MainWindow(QWidget *parent)
	: QWidget(parent)
{
	QGridLayout *layout = new QGridLayout;

	m_stdScale = new QwtScaleWidget(this);
	m_stdScale->setColorBarEnabled(true);
	m_stdScale->setColorMap(QwtDoubleInterval(0.0, 1.0), StdColorMap());
	layout->addWidget(m_stdScale, 0, 0);
	layout->addWidget(new QRadioButton(tr("Std")), 1, 0);

	m_jetScale = new QwtScaleWidget(this);
	m_jetScale->setColorBarEnabled(true);
	m_jetScale->setColorMap(QwtDoubleInterval(0.0, 1.0), JetColorMap());
	layout->addWidget(m_jetScale, 0, 1);
	layout->addWidget(new QRadioButton(tr("Jet")), 1, 1);

	m_hotScale = new QwtScaleWidget(this);
	m_hotScale->setColorBarEnabled(true);
	m_hotScale->setColorMap(QwtDoubleInterval(0.0, 1.0), HotColorMap());
	layout->addWidget(m_hotScale, 0, 2);
	layout->addWidget(new QRadioButton(tr("Hot")), 1, 2);

	m_hsvScale = new QwtScaleWidget(this);
	m_hsvScale->setColorBarEnabled(true);
	m_hsvScale->setColorMap(QwtDoubleInterval(0.0, 1.0), HsvColorMap());
	layout->addWidget(m_hsvScale, 0, 3);
	layout->addWidget(new QRadioButton(tr("Hsv")), 1, 3);

	m_springScale = new QwtScaleWidget(this);
	m_springScale->setColorBarEnabled(true);
	m_springScale->setColorMap(QwtDoubleInterval(0.0, 1.0), SpringColorMap());
	layout->addWidget(m_springScale, 0, 4);
	layout->addWidget(new QRadioButton(tr("Spring")), 1, 4);

	m_summerScale = new QwtScaleWidget(this);
	m_summerScale->setColorBarEnabled(true);
	m_summerScale->setColorMap(QwtDoubleInterval(0.0, 1.0), SummerColorMap());
	layout->addWidget(m_summerScale, 0, 5);
	layout->addWidget(new QRadioButton(tr("Summer")), 1, 5);

	m_winterScale = new QwtScaleWidget(this);
	m_winterScale->setColorBarEnabled(true);
	m_winterScale->setColorMap(QwtDoubleInterval(0.0, 1.0), WinterColorMap());
	layout->addWidget(m_winterScale, 0, 6);
	layout->addWidget(new QRadioButton(tr("Winter")), 1, 6);

	setLayout(layout);
}

