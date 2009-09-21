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
#ifndef MPSD8_H
#define MPSD8_H

#include <QString>
#include "mesydaqobject.h"

/**
 * \short representation of MPSD-8 peripheral module 
 *
 * \author Gregor Montermann <g.montermann@mesytec.com>
 */
class MPSD_8 : public MesydaqObject
{
Q_OBJECT
public:
	MPSD_8(quint8, QObject * = 0);

	~MPSD_8();

	void	setMpsdId(quint8, quint8, bool = true);
	quint8 	getMpsdId(void) {return m_mpsdId;}

// Pulser related methods
	void	setPulser(quint8 chan, quint8 pos = 2, quint8 poti = 128, quint8 on = 0, bool preset = false);
	void	setPulserPoti(quint8 chan, quint8 pos = 2, quint8 poti = 128, quint8 on = 0, bool preset = false);
	quint8	getPulsPos(bool preset = false) {return m_pulsPos[preset];}
	quint8	getPulsAmp(bool preset = false) {return m_pulsAmp[preset];}
	quint8	getPulsChan(bool preset = false) {return m_pulsChan[preset];}
	quint8	getPulsPoti(bool preset = false) {return m_pulsPoti[preset];}
	bool	isPulserOn() {return m_pulser[0];}

// Threshold related methods
	void	setThreshold(quint8 threshold, bool preset = false);
	quint8	getThreshold(bool preset = false) {return m_threshVal[preset];}

	void 	setThreshpoti(quint8 thresh, bool preset = false);
	quint8	getThreshpoti(bool preset = false) {return m_threshPoti[preset];}

// Gain related methods
	virtual void	setGain(quint8 channel, float gainv, bool preset = 0);
	virtual void	setGain(quint8 channel, quint8 gain, bool preset = 0);
	quint8		getGainpoti(quint8 chan, bool preset = 0) {return m_gainPoti[chan][preset];}
	float		getGainval(quint8 chan, bool preset = 0) {return m_gainVal[chan][preset];}
	bool		comGain() {return m_comgain;}

// Mode related methods
	void	setMode(bool amplitude, bool preset = 0) {m_ampMode[preset] = amplitude;}
	bool	getMode(bool preset = 0) {return m_ampMode[preset];}

// Internal registers related methods
	void	setInternalreg(quint8 reg, quint16 val, bool preset = 0);
	quint16	getInternalreg(quint8 reg, bool preset = 0) {return m_internalReg[reg][preset];}
	
	virtual quint8	calcGainpoti(float fval);
protected:
	virtual float	calcGainval(quint8 ga);
	virtual quint8	calcPulsPoti(quint8 val, float gv);
	virtual quint8	calcPulsAmp(quint8 val, float gv);
	virtual quint8	calcThreshval(quint8 thr);
public:
	virtual quint8	calcThreshpoti(quint8 tval);		// mainwidget.cpp

private:
	//! MCPD-8 id
	quint8 		m_mcpdId;
	
	//! MPSD-8 id
	quint8 		m_mpsdId;

protected:
	//! Gain poti values 
	quint8 		m_gainPoti[9][2];
	
	//! Gain values
	float 		m_gainVal[9][2];

	//! Common gain
	bool 		m_comgain;

private:
	//! Threshold poti values
	quint8 		m_threshPoti[2];

	//! Treshold values
	quint8 		m_threshVal[2];

	//! Pulser poti values
	quint8 		m_pulsPoti[2];

	//! Pulser position
	quint8 		m_pulsPos[2];

	//! Pulser amplitude
	float		m_pulsAmp[2];

	//! Pulser channel
	quint8 		m_pulsChan[2];

	//! Pulser 
	quint8 		m_pulser[2];

	//! amplitude mode
	bool		m_ampMode[2]; 
protected:
	quint8		m_busNum;

private:
	quint16 	m_internalReg[3][2];
};

/**
 * \short representation of MPSD-8+ peripheral module 
 *
 * \author Gregor Montermann <g.montermann@mesytec.com>
 */
class MPSD_8p : public MPSD_8
{
Q_OBJECT
public:
	MPSD_8p(quint8 id, QObject *parent = 0);

	~MPSD_8p() {}

	virtual void 	setGain(quint8 channel, float gainv, bool preset = 0);
	virtual void	setThreshold(quint8 threshold, bool preset = 0);

	virtual quint8	calcGainpoti(float fval);
	virtual quint8	calcThreshpoti(quint8 tval);		// mainwidget.cpp

protected:
	virtual float	calcGainval(quint8 ga);
	virtual quint8	calcPulsPoti(quint8 val, float gv);
	virtual quint8	calcPulsAmp(quint8 val, float gv);
	virtual quint8	calcThreshval(quint8 thr);

private:
	float 		m_g1;
	float 		m_g2;
	float 		m_t1;
	float 		m_t2;
	float 		m_p1;
	float 		m_p2;
};

#endif
