/*************************************************************************
 * This code was developed on base of the discussion thread about a Qt   *
 * based IP input widget. The code was taken and adopted to the needed   *
 * requirements.                                                         *
 * http://stackoverflow.com/questions/9306335/an-ip-address-widget-for-qt-similar-to-mfcs-ip-address-control
 * Copyright (C) 2012 by jpo38, Tugo                                     *
 * Copyright (C) 2014 by Jens Krüger <jens.krueger@frm2.tum.de>          *
 *                                                                       *
 * This program is free software; you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation; either version 2 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program; if not, write to the                         *
 * Free Software Foundation, Inc.,                                       *
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *************************************************************************/

#include "ipaddresswidget.h"

#include <QRegExpValidator>
#include <QLineEdit>
#include <QFont>
#include <QLabel>
#include <QKeyEvent>

IPAddressWidget::IPAddressWidget(QWidget *parent)
	: QFrame(parent)
{
	init();
}

void IPAddressWidget::init(void)
{
	setupUi(this);

	QLabel *p_Label[QT_UTL_IP4_SIZE - 1];

	p_Label[0] = label1;
	p_Label[1] = label2;
	p_Label[2] = label3;

	for (unsigned int i = 0; i < QT_UTL_IP4_SIZE - 1; ++i)
	{
		QLabel *pLabel = p_Label[i];
		QFont font = pLabel->font();
		font.setStyleHint(QFont::Monospace);
		font.setFixedPitch(true);
		pLabel->setFont(font);
//		pLabel->setStyleSheet("background: white");
	}

	m_pLineEdit[0] = ip1;
	m_pLineEdit[1] = ip2;
	m_pLineEdit[2] = ip3;
	m_pLineEdit[3] = ip4;

	for (unsigned int i = 0; i != QT_UTL_IP4_SIZE; ++i)
	{
		QLineEdit *pEdit = m_pLineEdit[i];
		pEdit->installEventFilter(this);

		QFont font = pEdit->font();
		font.setStyleHint(QFont::Monospace);
		font.setFixedPitch(true);
		pEdit->setFont(font);

		QRegExp rx("^(0|[1-9]|[1-9][0-9]|1[0-9][0-9]|2([0-4][0-9]|5[0-5]))$");
		QValidator *validator = new QRegExpValidator(rx, pEdit);
		pEdit->setValidator(validator);
	}

	setMaximumWidth(30 * QT_UTL_IP4_SIZE);

	connect(this, SIGNAL(signalTextChanged(QLineEdit *)), this, SLOT(slotTextChanged(QLineEdit *)), Qt::QueuedConnection);
}

IPAddressWidget::IPAddressWidget(const QString &ip, QWidget *parent)
	: QFrame(parent)
{
	init();
	setAddress(ip);
}

IPAddressWidget::~IPAddressWidget()
{
}

void IPAddressWidget::slotTextChanged(QLineEdit *pEdit)
{
	for (unsigned int i = 0; i != QT_UTL_IP4_SIZE; ++i)
	{
		if (pEdit == m_pLineEdit[i])
		{
			if ((pEdit->text().size() == pEdit->maxLength() &&  pEdit->text().size() == pEdit->cursorPosition()) || (pEdit->text() == "0"))
			{
				// auto-move to next item
				if (i + 1 != QT_UTL_IP4_SIZE)
				{
					m_pLineEdit[i + 1]->setFocus();
					m_pLineEdit[i + 1]->selectAll();
				}
			}
			break;
		}
	}
}

bool IPAddressWidget::eventFilter(QObject *obj, QEvent *event)
{
	bool bRes = QFrame::eventFilter(obj, event);

	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent *pEvent = dynamic_cast<QKeyEvent *>(event);
		if (pEvent)
		{
			for (unsigned int i = 0; i != QT_UTL_IP4_SIZE; ++i)
			{
				QLineEdit *pEdit = m_pLineEdit[i];
				if (pEdit == obj)
				{
					switch (pEvent->key())
					{
						case Qt::Key_Left:
							if (pEdit->cursorPosition() == 0)
							{
								// user wants to move to previous item
								movePrevLineEdit(i);
							}
							break;
						case Qt::Key_Right:
							if (pEdit->text().isEmpty() || (pEdit->text().size() == pEdit->cursorPosition()))
							{
								// user wants to move to next item
								moveNextLineEdit(i);
							}
							break;
						case Qt::Key_0:
							if (pEdit->text().isEmpty() || pEdit->text() == "0")
							{
								pEdit->setText("0");
								// user wants to move to next item
								moveNextLineEdit(i);
							}
							emit signalTextChanged(pEdit);
							break;
						case Qt::Key_Backspace:
							if (pEdit->text().isEmpty() || pEdit->cursorPosition() == 0)
							{
								// user wants to move to previous item
								movePrevLineEdit(i);
							}
				                        break;
						case Qt::Key_Comma:
						case Qt::Key_Period:
							moveNextLineEdit(i);
							break;
						default:
							emit signalTextChanged(pEdit);
							break;
					}
					break;
				}
			}
		}
	}
	return bRes;
}

void IPAddressWidget::setAddress(const QString &addr)
{
	QStringList ipParts = addr.split(".");
	for (unsigned int i = ipParts.size(); i < QT_UTL_IP4_SIZE; ++i)
		ipParts << "0";

	for (unsigned int i = 0; i < QT_UTL_IP4_SIZE; ++i)
	{
		QLineEdit *pEdit = m_pLineEdit[i];
		int pos;
		if (pEdit->validator()->validate(ipParts[i], pos) != QValidator::Acceptable)
			ipParts[i] = "0";
		pEdit->setText(ipParts[i]);
	}
}

QString IPAddressWidget::getAddress(void)
{
	QStringList ret;
	for (unsigned int i = 0; i < QT_UTL_IP4_SIZE; ++i)
	{
		QString tmp = m_pLineEdit[i]->text();
		if (tmp.isEmpty())
			tmp = "0";
		ret << tmp;
	}
	return ret.join(".");
}

void IPAddressWidget::moveNextLineEdit(unsigned int i)
{
	if (i + 1 < QT_UTL_IP4_SIZE)
	{
		m_pLineEdit[i + 1]->setFocus();
		m_pLineEdit[i + 1]->setCursorPosition(0);
		m_pLineEdit[i + 1]->selectAll();
	}
}

void IPAddressWidget::movePrevLineEdit(int i)
{
	if (i > 0)
	{
		m_pLineEdit[i - 1]->setFocus();
		m_pLineEdit[i - 1]->setCursorPosition(m_pLineEdit[i - 1]->text().size());
		m_pLineEdit[i - 1]->selectAll();
	}
}
