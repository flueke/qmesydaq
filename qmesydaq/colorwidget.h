/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
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

#ifndef _COLORWIDGET_H_
#define _COLORWIDGET_H_

#include <QWidget>

class MesydaqColorMap;
class QRadioButton;
class QButtonGroup;

/**
 * \short base class for all color maps used in qmesydaq
 *
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class MesydaqColorWidget : public QWidget
{
	Q_OBJECT
public:
	//! constructor
	MesydaqColorWidget(MesydaqColorMap *, const QString &, QWidget *parent = NULL);

public slots:
	/**
	 * This slot sets the status of the radio button to checked or not
	 *
	 * \param checked new checked state
	 */
	void setChecked(bool checked);

public:
	//! \returns the ckecked state of the radio button
	bool isChecked(void) const;

	/**
	 * Sets the radio button of this widget to a button group
	 *
	 * \param bg button group
	 */
	void setButtonGroup(QButtonGroup *bg);

private:
	MesydaqColorMap *m_map;

	QRadioButton	*m_button;
};

#endif
