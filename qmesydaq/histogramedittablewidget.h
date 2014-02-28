/***************************************************************************
 *   Copyright (C) 2013-2014 by Lutz Rossa <rossa@helmholtz-berlin.de>,    *
 *                    Eric Faustmann <eric.faustmann@helmholtz-berlin.de>, *
 *                    Damian Rhein <damian.rhein@helmholtz-berlin.de>      *
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

#ifndef HISTOGRAMEDITTABLEWIDGET_H
#define HISTOGRAMEDITTABLEWIDGET_H

#include <QTableWidget>

/**
	\short Mapping editor table widget

	\author Eric Faustmann <eric.faustmann@helmholtz-berlin.de>, Damian Rhein <damian.rhein@helmholtz-berlin.de>, Lutz Rossa <rossa@helmholtz-berlin.de>
*/
class HistogramEditTableWidget : public QTableWidget
{
	Q_OBJECT
public:
	HistogramEditTableWidget(QWidget* pParent = NULL);
	HistogramEditTableWidget(int iRows, int iColumns, QWidget* pParent = NULL);
	~HistogramEditTableWidget();
	void keyPressEvent(QKeyEvent *pEvent);
	void mousePressEvent(QMouseEvent *pEvent);
};

#endif // HISTOGRAMEDITTABLEWIDGET_H
