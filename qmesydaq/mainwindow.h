/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
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

#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <QMainWindow>

#include "ui_mainwindow.h"

#include "structures.h"

class MainWidget;
class Mesydaq2;
class QCloseEvent;

/**
 * \short Application Main Window
 * \author Gregor Montermann <g.montermann@mesytec.com>
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 * \version 0.9
 */
class MainWindow : public QMainWindow, public Ui_MainWindow
{
	Q_OBJECT
public:
	/**
	 * Default Constructor
	 */
	MainWindow(QWidget *parent = 0);

	/**
	 * Destructor
	 */
	virtual ~MainWindow();

        /*! 
	 * wrapper method to emit signal to load a configuration file
	 *
	 * \param sFilename name of the file
	 */
	void	doLoadConfiguration(const QString &sFilename) { emit loadConfiguration(sFilename); }

signals:
	//! load configuration file
	void	loadConfiguration(const QString& sFilename);

private slots:
	//! callback if the normal user mode is selected
	void	selectUser(void);

	//! callback if the expert user mode is selected
	void 	selectExpert(void);

	//! callback if the super user mode is selected
	void	selectSuperuser(void);

protected:
	void closeEvent(QCloseEvent *event);

private:
	//! restore the last settings
	void restoreSettings(void);

	//! saves the current settings
	void saveSettings(void);

	bool checkPasswd(const QString &section);

private:
	//! the main widget
	MainWidget 	*m_main;

	//! the MesyDaq object
	Mesydaq2	*m_mesy;
};

#endif // _MAINWINDOW_H
