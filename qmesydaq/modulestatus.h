/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2011 by Jens Kr�ger <jens.krueger@frm2.tum.de>     *
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

class ModuleStatus : public QWidget, public Ui_ModuleStatus
{
	Q_OBJECT
public:
	ModuleStatus(QWidget * = 0);

	void update(const QString &, const float, const bool);
	
	void setId(const quint8);

signals:
	void clicked(quint8);

protected:
	void mouseDoubleClickEvent(QMouseEvent *);

	void setLabel(const QString &);

private:
	quint8	m_id;
	bool	m_online;
};
#endif