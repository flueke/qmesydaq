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

#include "ModuleSpinBox.h"

/*!
    \fn ModuleSpinBox::ModuleSpinBox(QList<int> modules, QWidget *parent)

    constructor

    \param parent Qt parents widget
 */
ModuleSpinBox::ModuleSpinBox(QWidget *parent)
	: QSpinBox(parent)
	, m_iChangeSteps(0)
{
	setRange(0, 7);
	setWrapping(false);
}

/*!
    \fn ModuleSpinBox::setModuleList(QList<int> modules)

    sets a new list of allowed module numbers

    \param modules list with allowed module numbers from 0 .. 7
 */
void ModuleSpinBox::setModuleList(QList<int> modules)
{
	m_modList = modules;
	if (m_modList.empty())
		setDisabled(true);
	else
	{
		if (!isEnabled())
			setEnabled(true);
		if (m_iChangeSteps < 0)
			setValue(m_modList.last());
		else
			setValue(m_modList.first());
		qSort(m_modList);
		setRange(m_modList.first(), m_modList.last());
		setValue(m_modList.first());
	}
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
	if (m_modList.isEmpty())
		return;
	int pos = value() + steps;
	if (m_modList.contains(pos))
	{
		setValue(pos);
	}
	else if (steps > 0)
	{
		if (pos > maximum())
		{
			setValue(minimum());
			m_iChangeSteps=steps;
			emit changeModule(steps);
			m_iChangeSteps = 0;
		}
		else
		{
			QList<int>::const_iterator it = qUpperBound(m_modList, pos);
			setValue(*it);
		}
	}
	else
	{
		if (pos < minimum())
		{
			setValue(maximum());
			m_iChangeSteps = steps;
			emit changeModule(steps);
			m_iChangeSteps = 0;
		}
		else
		{
			QList<int>::const_iterator it = qLowerBound(m_modList, pos);
			setValue(*it);
		}
	}
}

/*!
    \overload QValidator::State ModuleSpinBox::validate(QString &input, int &pos) const

    This virtual function is called by the ModuleSpinBox to determine whether input is valid.

    \param input string to validate
    \param pos indicates the position in the string.

    \return the result of the validator
*/
QValidator::State ModuleSpinBox::validate(QString &input, int &) const
{
	return m_modList.contains(input.toInt()) ? QValidator::Acceptable : QValidator::Invalid;
}

