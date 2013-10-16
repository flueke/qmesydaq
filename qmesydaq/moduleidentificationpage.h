/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2013 by Jens Krüger <jens.krueger@frm2.tum.de>     *
 *   Copyright (C) 2013 by Lutz Rossa <rossa@helmholtz-berlin.de>          *
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
#ifndef MODULE_IDENTIFICATION_PAGE_H
#define MODULE_IDENTIFICATION_PAGE_H

#include <QMutex>
#include <QWizardPage>
#include <QThread>
#include <QWaitCondition>
#include "ui_moduleidentificationpage.h"

class QMouseEvent;
class ModuleIdentificationPageThread;

/*!
    \class ModuleIdentificationPage

    \short This class handles the wizard page for identifying the given data for the MCPD

    \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>, Lutz Rossa <rossa@helmholtz-berlin.de>
 */
class ModuleIdentificationPage : public QWizardPage, public Ui_ModuleIdentificationPage
{
	friend class ModuleIdentificationPageThread;
	Q_OBJECT
public:
	ModuleIdentificationPage(QWidget * = 0);
	virtual ~ModuleIdentificationPage();

	void initialize(const QString &ip = "192.168.168.121", const quint16 id = 0);

	bool isComplete() const;

private slots:
	void valueChanged();
	void testTimeout();
	void updateTimeout();

protected:
	ModuleIdentificationPageThread *m_pThread;
	QMutex		*m_pThreadMutex; //! thread mutex
	QTimer		*m_pTestTimer;   //! single shot timer for MCPD test
	QTimer		*m_pUpdateTimer; //! GUI update timer
	volatile bool	m_bValid;       //! flag, if MCPD is valid
	bool		m_bOldValid;    //! old copy of m_bValid
};

/**
 * \short worker thread class for ModuleIdentificationPage wizard
 *
 * \author Lutz Rossa <rossa@helmholtz-berlin.de>
 */
class ModuleIdentificationPageThread : public QThread
{
	friend class ModuleIdentificationPage;
	Q_OBJECT
	Q_DISABLE_COPY(ModuleIdentificationPageThread)
protected:
	ModuleIdentificationPageThread(ModuleIdentificationPage *pWizard);
	virtual ~ModuleIdentificationPageThread();
	virtual void run();

	//! reference to wizard
	ModuleIdentificationPage *m_pWizard;

	enum Command {
		NONE,
		WORK,
		QUIT,
	} m_iCommand;

	//! wake thread up
	QWaitCondition m_ThreadCondition;

	//! MCPD ip address
	QString m_szMcpdIp;

	//! MCPD id
	quint8 m_byMcpdId;
};
#endif
