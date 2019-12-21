/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2019 by Jens Krüger <jens.krueger@frm2.tum.de>     *
 *   Copyright (C) 2013-2019 by Lutz Rossa <rossa@helmholtz-berlin.de>     *
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
#include <QMouseEvent>
#include <QAbstractButton>
#include <QTimer>
#include "networkdevice.h"

#include "moduleidentificationpage.h"
#include "mcpd8.h"
#include "qmlogging.h"
#include "stdafx.h"

#define WAITTIME 2000 //! (ms) wait time after last input before testing new MCPD

/*!
    constructor
 */
ModuleIdentificationPage::ModuleIdentificationPage(QWidget *parent)
	: QWizardPage(parent)
	, m_pThread(NULL)
	, m_pThreadMutex(NULL)
	, m_pTestTimer(NULL)
	, m_bValid(false)
	, m_bOldValid(false)
{
	setupUi(this);
	registerField("ipaddress*", moduleIPInput, "address", "signalTextChanged(QLineEdit *)");
	registerField("moduleid*", moduleIDInput);
	initialize();
	m_pThreadMutex = new QMutex();
	m_pThread = new ModuleIdentificationPageThread(this);
	m_pThread->start(QThread::LowPriority);
	m_pTestTimer = new QTimer();
	m_pTestTimer->setSingleShot(true);
	m_pTestTimer->start(WAITTIME);
	m_pUpdateTimer = new QTimer();

	connect(moduleIPInput, SIGNAL(textChanged(const QString &)), this, SLOT(valueChanged()));
	connect(moduleIPInput, SIGNAL(textEdited(const QString &)), this, SLOT(valueChanged()));
	connect(moduleIDInput, SIGNAL(valueChanged(int)), this, SLOT(valueChanged()));
	connect(m_pTestTimer, SIGNAL(timeout()), this, SLOT(testTimeout()));
	connect(m_pUpdateTimer, SIGNAL(timeout()), this, SLOT(updateTimeout()));

	m_pUpdateTimer->start(200);
}

/** \fn ModuleIdentificationPage::~ModuleIdentificationPage()
 *
 *  destructor
 */
ModuleIdentificationPage::~ModuleIdentificationPage()
{
	delete m_pThread;
	delete m_pThreadMutex;
	delete m_pUpdateTimer;
	delete m_pTestTimer;
}

/*!
    \fn void ModuleIdentificationPage::initialize(const QString &ip, const quint16 id)

    initializes the input fields for IP address and module ID

    \param ip IP address
    \param id Module ID
 */
void ModuleIdentificationPage::initialize(const QString &ip, const quint16 id)
{
	moduleIPInput->setAddress(ip);
	moduleIDInput->setValue(id);
}

/*!
    \fn bool ModuleIdentificationPage::isComplete() const

    checks for completeness of input. In our case it tries to connect to the MCPD and
    tries to read the connected modules. If found a MCPD it will return true.

    \return true in case of found a MCPD otherwise false
 */
bool ModuleIdentificationPage::isComplete() const
{
	return m_bValid;
}

/*!
    \fn void ModuleIdentificationPage::valueChanged()

    sets the Next button true if a value was changed
 */
void ModuleIdentificationPage::valueChanged()
{
        enum QWizard::WizardButton bt = QWizard::NextButton;
	if (isFinalPage())
		bt = QWizard::FinishButton;
	wizard()->button(bt)->setEnabled(m_bValid = false);
	m_pTestTimer->start(WAITTIME);
}

/*!
    \fn void ModuleIdentificationPage::testTimeout()

    Checks user input, tries to read MCPD version and
    enables finish button, if successful.
 */
void ModuleIdentificationPage::testTimeout()
{
	for (;;)
	{
        	m_pThreadMutex->lock();
        	if (m_pThread->m_iCommand != ModuleIdentificationPageThread::WORK)
			break;
        	m_pThreadMutex->unlock();
        	usleep(1000);
	}

	if (m_pThread->m_iCommand == ModuleIdentificationPageThread::NONE)
	{
        	m_pThread->m_szMcpdIp = moduleIPInput->getAddress(); // 'text()' not feasible, returns "127...2" instead of "127.0.0.2"
        	m_pThread->m_byMcpdId = moduleIDInput->value();
        	m_pThread->m_iCommand = ModuleIdentificationPageThread::WORK;
        	m_pThread->m_ThreadCondition.wakeOne();
	}
	m_pThreadMutex->unlock();
}

/*!
    \fn void ModuleIdentificationPage::updateTimeout()

    Checks user input, tries to read MCPD version and
    enables finish button, if successful.
 */
void ModuleIdentificationPage::updateTimeout()
{
	if (m_bOldValid != m_bValid)
	{
		enum QWizard::WizardButton bt = QWizard::NextButton;
		if (isFinalPage())
			bt = QWizard::FinishButton;
		wizard()->button(bt)->setEnabled(m_bValid);
		m_bOldValid = m_bValid;
	}
}

/** \fn ModuleIdentificationPageThread::ModuleIdentificationPageThread(ModuleIdentificationPage* pWizard)
 *
 *  constructor
 *
 *  \param pWizard pointer to matching ModuleIdentificationPage instance
 */
ModuleIdentificationPageThread::ModuleIdentificationPageThread(ModuleIdentificationPage* pWizard)
	: m_pWizard(pWizard)
	, m_iCommand(NONE)
	, m_byMcpdId(0)
{
}

/** \fn ModuleIdentificationPageThread::~ModuleIdentificationPageThread()
 *
 *  destructor
 */
ModuleIdentificationPageThread::~ModuleIdentificationPageThread()
{
	m_pWizard->m_pThreadMutex->lock();
	m_iCommand = QUIT;
	m_ThreadCondition.wakeOne();
	m_pWizard->m_pThreadMutex->unlock();
	wait();
}

/** \fn void ModuleIdentificationPageThread::run()
 *
 *  worker thread for wizard background test
 */
void ModuleIdentificationPageThread::run()
{
	for (;;)
	{
		msleep(1);
		m_pWizard->m_pThreadMutex->lock();
		switch (m_iCommand)
		{
			case QUIT:
				m_pWizard->m_pThreadMutex->unlock();
				return;
			case NONE:
				m_ThreadCondition.wait(m_pWizard->m_pThreadMutex);
				m_pWizard->m_pThreadMutex->unlock();
				continue;
			default:
				break;
		}

		m_iCommand = NONE;
		MCPD8 *mcpd = new MCPD8(m_byMcpdId, m_szMcpdIp, 54321, QString::null, true);
		m_pWizard->m_bValid = (mcpd->version() > 0.0);
		delete mcpd;
		m_pWizard->m_pThreadMutex->unlock();
	}
}
