/***************************************************************************
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

#include "ChannelSpinBox.h"

/*!
    \fn ChannelSpinBox::ChannelSpinBox(QList<int> modules, QWidget *parent)

    constructor

    \param parent Qt parents widget
 */
ChannelSpinBox::ChannelSpinBox(QWidget *parent)
	: QSpinBox(parent)
{
	setRange(0, 7);
	setSingleStep(1);
}

/*!
    \overload ChannelSpinBox::stepBy(int steps)

    Virtual function that is called whenever the user triggers a step. The steps parameter
    indicates how many steps were taken, e.g. Pressing Qt::Key_Down will trigger a call to
    stepBy(-1), whereas pressing Qt::Key_Prior will trigger a call to stepBy(10).

    It will go to the next possible module and will go around if wrapping is enabled

    \param steps number of steps to go up or down
 */
void ChannelSpinBox::stepBy(int steps)
{
	int pos = value() + steps;
	if (steps > 0)
	{
		if (pos > maximum())
		{
			setValue(minimum());
			emit changeModule(1);
			return;
		}
	}
	else
	{
		if (pos < minimum())
		{
			setValue(maximum());
			emit changeModule(-1);
			return;
		}
	}
	setValue(pos);
}

