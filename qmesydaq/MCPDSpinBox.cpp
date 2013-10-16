/***************************************************************************
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
#include "MCPDSpinBox.h"

/*!
    \fn MCPDSpinBox::MCPDSpinBox(QList<int> modules, QWidget *parent)

    constructor

    \param parent Qt parents widget
 */
MCPDSpinBox::MCPDSpinBox(QWidget *parent)
	: QSpinBox(parent)
{
	setRange(0, 255);
	setWrapping(false);
}

/*!
    \fn MCPDSpinBox::setMCPDList(QList<int> modules)

    sets a new list of allowed module numbers

    \param modules list with allowed module numbers from 0 .. 7
 */
void MCPDSpinBox::setMCPDList(QList<int> modules)
{
	m_mcpdList = modules; 
	if (!m_mcpdList.empty())
	{
		setEnabled(true);
		qSort(m_mcpdList);
		setRange(m_mcpdList.first(), m_mcpdList.last());
		setValue(m_mcpdList.at(0));
	}
	else
		setDisabled(true);
}

/*!
    \overload MCPDSpinBox::stepBy(int steps)

    Virtual function that is called whenever the user triggers a step. The steps parameter 
    indicates how many steps were taken, e.g. Pressing Qt::Key_Down will trigger a call to 
    stepBy(-1), whereas pressing Qt::Key_Prior will trigger a call to stepBy(10).

    It will go to the next possible module and will go around if wrapping is enabled

    \param steps number of steps to go up or down
 */
void MCPDSpinBox::stepBy(int steps)
{
	if (m_mcpdList.isEmpty())
		return;
	int pos = value() + steps;
	if (m_mcpdList.contains(pos))
	{
		setValue(pos);
	}
	else if (steps > 0)
	{
		if (pos > maximum())
		{
			if (wrapping())
				setValue(minimum());
//			emit changeModule(steps);
		}
		else
		{
			QList<int>::const_iterator it = qUpperBound(m_mcpdList, pos);
			setValue(*it);
		}
	}
	else
	{
		if (pos < minimum())
		{
			if (wrapping())
				setValue(maximum());
//			emit changeModule(steps);
		}
		else
		{
			QList<int>::const_iterator it = qLowerBound(m_mcpdList, pos);
			setValue(*it);
		}
	}
}

/*!
    \overload QValidator::State MCPDSpinBox::validate(QString &input, int &pos) const

    This virtual function is called by the MCPDSpinBox to determine whether input is valid. 

    \param input string to validate
    \param pos indicates the position in the string. 

    \return the result of the validator
*/
QValidator::State MCPDSpinBox::validate(QString &input, int &) const
{
	return m_mcpdList.contains(input.toInt()) ? QValidator::Acceptable : QValidator::Invalid;
}

