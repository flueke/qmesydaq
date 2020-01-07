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
#ifndef MODULE_STATUS_H
#define MODULE_STATUS_H

#include <QWidget>
#include "ui_modulestatus.h"

class QMouseEvent;

/*!
    \class ModuleStatus

    \short This class handles displaying of the status of connected MPSD modules

    \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class ModuleStatus : public QWidget, public Ui_ModuleStatus
{
	Q_OBJECT
public:
	ModuleStatus(QWidget * = 0);

	void update(const QString &, const float, const bool, const bool = true, const bool = true);
	
	void setId(const quint8);

signals:
	//! is emitted if a double clicked was made on this widget
	void clicked(quint8);

	//! is emitted if the histogram check box changes
	void histogram(quint8, bool);

	//! is emitted if the active check box changes
	void active(quint8, bool);

protected:
	void mouseDoubleClickEvent(QMouseEvent *);

	void setLabel(const QString &);

private slots:
	void histogramSlot(bool);

	void activeSlot(bool);

private:
	//! number of the module
	quint8	m_id;

	//! module online or not
	bool	m_online;
};
#endif
