/***************************************************************************
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

#include "ModuleSpinBox.h"

/*!
    \fn ModuleSpinBox::ModuleSpinBox(QList<int> modules, QWidget *parent)

    constructor

    \param parent Qt parents widget
 */
ModuleSpinBox::ModuleSpinBox(QWidget *parent)
	: QSpinBox(parent)
{
	setMinimum(0);
	setMaximum(7);
}

/*!
    \fn ModuleSpinBox::setModuleList(QList<int> modules)

    sets a new list of allowed module numbers

    \param modules list with allowed module numbers from 0 .. 7
 */
void ModuleSpinBox::setModuleList(QList<int> modules)
{
	m_modList = modules; 
	if (!m_modList.empty())
		setValue(m_modList.at(0));
}

/*!
    \overload ModuleSpinBox::stepBy(int steps)

    Virtual function that is called whenever the user triggers a step. The steps parameter 
    indicates how many steps were taken, e.g. Pressing Qt::Key_Down will trigger a call to 
    stepBy(-1), whereas pressing Qt::Key_Prior will trigger a call to stepBy(10).

    It will go to the next possible module and will go around if wrapping is enabled

    \param steps number of steps to go up or down
 */
void ModuleSpinBox::stepBy(int steps)
{
	int pos = value() + steps;
	if (m_modList.contains(pos))
		setValue(pos);
	else if (steps > 0)
	{
		for (; pos <= maximum(); ++pos)
			if (m_modList.contains(pos))
			{
				setValue(pos);
				return;
			}
		if (wrapping())
			for (pos = minimum(); pos < value(); ++pos)
				if (m_modList.contains(pos))
				{
					setValue(pos);
					return;
				}
	}
	else
	{
		for (; pos >= minimum(); --pos)
			if (m_modList.contains(pos))
			{
				setValue(pos);
				return;
			}
		if (wrapping())
			for (pos = maximum(); pos > value(); --pos)
				if (m_modList.contains(pos))
				{
					setValue(pos);
					return;
				}
	}
}

/*!
    \overload QValidator::State ModuleSpinBox::validate(QString &input, int &pos) const

    This virtual function is called by the ModuleSpinBox to determine whether input is valid. 

    \param input string to validate
    \param pos indicates the position in the string. 
*/
QValidator::State ModuleSpinBox::validate(QString &input, int &) const
{
	return m_modList.contains(input.toInt()) ? QValidator::Acceptable : QValidator::Invalid;
}

