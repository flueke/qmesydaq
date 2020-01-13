/***************************************************************************
 *   Copyright (C) 2014-2020 by Jens Krüger <jens.krueger@frm2.tum.de>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Module Public License as published by   *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Module Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU Module Public License      *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "website.h"


WebsiteTool::WebsiteTool(const QString &site, QWidget *parent)
	: QDialog(parent)
{
	setupUi(this);
//	site = settings.get('url', '');
	if (!site.isEmpty())
		webView->load(QUrl(site));
}

void WebsiteTool::closeEvent(QCloseEvent *event)
{
	hide();
	event->ignore();
}
