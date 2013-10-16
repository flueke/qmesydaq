/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2013 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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

#include <QWidget>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QButtonGroup>

#include <qwt_scale_widget.h>

#include "colormaps.h"

#include "colorwidget.h"

MesydaqColorWidget::MesydaqColorWidget(MesydaqColorMap *map, const QString &text, QWidget *parent)
	: QWidget(parent)
	, m_map(map)
	, m_button(new QRadioButton(text))
{
	QVBoxLayout 	*layout = new QVBoxLayout;
	QwtScaleWidget 	*scale = new QwtScaleWidget(this);
	scale->setColorBarEnabled(true);
	scale->setColorMap(QwtDoubleInterval(0.0, 1.0), *m_map);
	layout->addWidget(scale);
	layout->addWidget(m_button);
	setLayout(layout);
}
	
void MesydaqColorWidget::setChecked(bool val)
{
	m_button->setChecked(val);
}

bool MesydaqColorWidget::isChecked(void) const
{
	return m_button->isChecked();
}

void MesydaqColorWidget::setButtonGroup(QButtonGroup *bg)
{
	bg->addButton(m_button);
}
