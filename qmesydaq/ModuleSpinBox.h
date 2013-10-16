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

#ifndef __MODULE_SPIN_BOX_H
#define __MODULE_SPIN_BOX_H

#include <QSpinBox>

/**
 * \short Spinbox to select one of the connected MPSD modules 
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 * \version 0.1
 */
class ModuleSpinBox : public QSpinBox
{
Q_OBJECT
public:
	ModuleSpinBox(QWidget *parent = 0);

	void setModuleList(QList<int> modules);

	virtual void stepBy(int steps);

public slots:
	//! call back for steps up/down
	void steps(int steps)
	{
		stepBy(steps);
	}

signals:
	//! this signal will be emitted if the module number over/underruns
	void changeModule(int);

protected:
	virtual QValidator::State validate(QString & input, int &pos) const;

private:
        //! list of the existing modules
	QList<int>	m_modList;
};

#endif
