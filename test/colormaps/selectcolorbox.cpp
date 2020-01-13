/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2020 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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
#include <QButtonGroup>

#include "selectcolorbox.h"

#include <qwt_scale_widget.h>

#include "colormaps.h"
#include "colorwidget.h"

SelectScalingBox::SelectScalingBox(QWidget *parent)
	: QWidget(parent)
{
	QGridLayout *layout = new QGridLayout;
	QButtonGroup *bg = new QButtonGroup(this);

	MesydaqColorWidget *w = new MesydaqColorWidget(new StdColorMap(), tr("Std"));
	layout->addWidget(w, 0, 0);
	w->setButtonGroup(bg);
	w->setChecked(true);

	layout->addWidget(w = new MesydaqColorWidget(new JetColorMap(), tr("Jet")), 0, 1);
	w->setButtonGroup(bg);
	layout->addWidget(w = new MesydaqColorWidget(new HotColorMap(), tr("Hot")), 0, 2);
	w->setButtonGroup(bg);
	layout->addWidget(w = new MesydaqColorWidget(new CoolColorMap(), tr("Cool")), 0, 3);
	w->setButtonGroup(bg);
	layout->addWidget(w = new MesydaqColorWidget(new HsvColorMap(), tr("Hsv")), 0, 4);
	w->setButtonGroup(bg);
	layout->addWidget(w = new MesydaqColorWidget(new SpringColorMap(), tr("Spring")), 0, 5);
	w->setButtonGroup(bg);
	layout->addWidget(w = new MesydaqColorWidget(new SummerColorMap(), tr("Summer")), 0, 6);
	w->setButtonGroup(bg);
	layout->addWidget(w = new MesydaqColorWidget(new AutumnColorMap(), tr("Autumn")), 0, 7);
	w->setButtonGroup(bg);
	layout->addWidget(w = new MesydaqColorWidget(new WinterColorMap(), tr("Winter")), 0, 8);
	w->setButtonGroup(bg);
	layout->addWidget(w = new MesydaqColorWidget(new BoneColorMap(), tr("Bone")), 0, 9);
	w->setButtonGroup(bg);
	layout->addWidget(w = new MesydaqColorWidget(new CopperColorMap(), tr("Copper")), 0, 10);
	w->setButtonGroup(bg);
	layout->addWidget(w = new MesydaqColorWidget(new GrayColorMap(), tr("Gray")), 0, 11);
	w->setButtonGroup(bg);
	layout->addWidget(w = new MesydaqColorWidget(new SpectralColorMap(), tr("Spectral")), 0, 12);
	w->setButtonGroup(bg);

	setLayout(layout);
}

