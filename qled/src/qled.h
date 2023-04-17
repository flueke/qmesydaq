/***************************************************************************
 *   Copyright (C) 2010 by P. Sereno                                       *
 *   http://www.sereno-online.com                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License           *
 *   version 2.1 as published by the Free Software Foundation              *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Lesser General Public License for more details.                   *
 *   http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.               *
 ***************************************************************************/

#ifndef QLED_H
#define QLED_H

#include <Qt>
#include <QWidget>
#include <QtUiPlugin/QDesignerExportWidget>

// My Qt designer widget plugin class

class QColor;
class QSvgRenderer;

class QDESIGNER_WIDGET_EXPORT QLed : public QWidget
{
Q_OBJECT
        Q_ENUMS(ledColor)
        Q_ENUMS(ledShape)

	Q_PROPERTY(bool value READ value WRITE setValue)
        Q_PROPERTY(ledColor onColor READ onColor WRITE setOnColor)
        Q_PROPERTY(ledColor offColor READ offColor WRITE setOffColor)
        Q_PROPERTY(ledShape shape READ shape WRITE setShape)
	Q_PROPERTY(bool blink READ blink WRITE setBlink)

public:
	QLed(QWidget *parent = 0);
	virtual ~QLed();
	bool value() const {return m_value;}
	enum ledColor {
		Red = 0,
		Green,
		Yellow,
		Grey,
		Orange,
		Purple,
		Blue
       	};
	enum ledShape {
		Circle = 0,
		Square,
		Triangle,
		Rounded
	};
	ledColor onColor() const {return m_onColor;}
	ledColor offColor() const {return m_offColor;}
	ledShape shape() const {return m_shape;}
	bool blink() const {return m_blink;}
 
public slots:
	void setValue(bool);
        void setOnColor(ledColor);
        void setOnColor(int);
        void setOffColor(ledColor);
        void setShape(ledShape);
        void setShape(int);
	void setBlink(bool);
	void toggleValue();

protected:
	void paintEvent(QPaintEvent *event);
	void timerEvent(QTimerEvent *event);
	
protected:
	bool		m_value;
	ledColor	m_onColor,
			m_offColor;
	int		id_Timer;
	ledShape	m_shape;
	QStringList	shapes;
	QStringList	colors;
	bool		m_blink;
private:
	bool		m_color;
	QSvgRenderer	*renderer ;
};

#endif
