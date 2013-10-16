/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2013 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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

#include "libqmesydaq_global.h"
#include "mdefines.h"
#include "logging.h"

/**
 * \class MPSD8
 * \short representation of MPSD-8 peripheral module 
 *
 * \author Gregor Montermann <g.montermann@mesytec.com>
 */
class LIBQMESYDAQ_EXPORT MPSD8 : public QObject
{
	Q_OBJECT
	Q_PROPERTY(quint8 m_busNum READ busNumber)

public:
	MPSD8(quint8, QObject * = 0);

	~MPSD8();

	static MPSD8 *create(int, int, QObject * = 0);

	virtual bool	active(void); 

	virtual bool	active(quint16);

	virtual void	setActive(bool act); 

	virtual void	setActive(quint16, bool);

	bool	histogram(void); 

	bool	histogram(quint16);

	void	setHistogram(bool hist); 

	void	setHistogram(quint16, bool);

	QList<quint16> getHistogramList(void);

	QList<quint16> getActiveList(void);

	//! \return the ID of the MPSD
	quint8 	getModuleId(void) {return m_mpsdId;}

	//! \return the type of the MPSD as string
	virtual QString getType(void) {return tr("MPSD-8");}

	//! \return the type of the MPSD as number
	virtual int type(void) {return TYPE_MPSD8;}

	//! \return is the module online or not
	virtual bool online(void);

// Pulser related methods
	virtual void	setPulser(quint8 chan, quint8 pos = 2, quint8 poti = 128, quint8 on = 0, bool preset = false);
	virtual void	setPulserPoti(quint8 chan, quint8 pos = 2, quint8 poti = 128, quint8 on = 0, bool preset = false);
	
	/**
	 * get the pulser position
	 *
         * \return pulser position (left, right, middle)
	 * \see getPulsAmp
	 * \see getPulsChan
	 * \see getPulsPoti
	 * \see setPulser
	 * \see setPulserPoti
	 */
	virtual quint8	getPulsPos(bool preset = false) {return m_pulsPos[preset];}
	
	/**
	 * get the pulser amplitude
	 *
         * \return pulser amplitude
	 * \see getPulsPos
	 * \see getPulsChan
	 * \see getPulsPoti
	 * \see setPulser
	 * \see setPulserPoti
	 */
	quint8	getPulsAmp(bool preset = false) {return m_pulsAmp[preset];}
	
	/**
	 * get the pulser channel
	 *
         * \return pulser channel 
	 * \see getPulsPos
	 * \see getPulsAmp
	 * \see getPulsPoti
	 * \see setPulser
	 * \see setPulserPoti
	 */
	quint8	getPulsChan(bool preset = false) {return m_pulsChan[preset];}
	
	/**
	 * get the pulser poti
	 *
         * \return pulser poti 
	 * \see getPulsPos
	 * \see getPulsAmp
	 * \see getPulsChan
	 * \see setPulser
	 * \see setPulserPoti
	 */
	quint8	getPulsPoti(bool preset = false) {return m_pulsPoti[preset];}

	//! \return is the pulser on
	bool	isPulserOn() {return m_pulser[0];}

// Threshold related methods
	virtual void	setThreshold(quint8 threshold, bool preset = false);
	virtual void 	setThreshpoti(quint8 thresh, bool preset = false);

	/**
 	 * gets the threshold
	 *
	 * \param preset ????
	 * \return threshold value
	 * \see setThreshold
	 * \see setThreshpoti
	 * \see getThreshpoti
	 */
	quint8	getThreshold(bool preset = false) {return m_threshVal[preset];}

	/**
 	 * gets the threshold poti value
	 *
	 * \param preset ????
	 * \return threshold poti value
	 * \see setThreshold
	 * \see setThreshpoti
	 * \see getThreshold
	 */
	quint8	getThreshpoti(bool preset = false) {return m_threshPoti[preset];}

// Gain related methods
	virtual void	setGain(quint8 channel, float gainv, bool preset = false);
	virtual void	setGain(quint8 channel, quint8 gain, bool preset = false);

	/**
	 * get the poti value for the gain
	 *
	 * \return gain poti value
	 * \see setGain
	 * \see getGainval
	 */
	quint8	getGainpoti(quint8 chan, bool preset = false) {return m_gainPoti[chan][preset];}

	/**
	 * get the user value for the gain
	 *
	 * \return gain user value
	 * \see setGain
	 * \see getGainpoti
	 */
	float	getGainval(quint8 chan, bool preset = false) 
	{
		MSG_DEBUG << "gain val " << chan << ' ' << m_gainVal[chan][preset];
		return m_gainVal[chan][preset];
	}

	//! \return use the same gain for all channels ?
	bool	comGain() {return m_comgain;}

// Mode related methods
	/**
	 * sets the mode amplitude/position
	 *
	 * \param amplitude true = amplitude, false = position
	 * \param preset ????
	 * \see getMode
	 */
	void	setMode(bool amplitude, bool preset = false) {m_ampMode[preset] = amplitude;}

	/**
	 * gets the mode amplitude/position
	 *
	 * \param preset ????
	 * \return amplitude true = amplitude, false = position
	 * \see setMode
	 */
	bool	getMode(bool preset = false) {return m_ampMode[preset];}

// Internal registers related methods
	virtual void	setInternalreg(quint8 reg, quint16 val, bool preset = false);

	/**
	 * get the value of the internal registers 
	 *
	 * \param reg register number
	 * \param preset ????
	 * \return value of the register
	 * \see setInternalreg
	 */
	quint16	getInternalreg(quint8 reg, bool preset = false) {return m_internalReg[reg][preset];}
	
	virtual quint8	calcGainpoti(float fval);

        //! returns the number of bins per module
	virtual quint16 bins() {return 960;}

	//! returns the number of the bus 
	quint8 busNumber(void) {return m_busNum;} 

// version related methods
        //! return version as float value major.minor
	float	version(void) const {return m_version;}

        //! sets the version as float value major.minor
	//! \param val
        void	setVersion(const float val) {m_version = val;}

// capabilities related methods
        //! return capabilities 
	quint16	capabilities(void) const {return m_capabilities;}

        //! sets the capabilities 
	//! \param val
        void	setCapabilities(const quint16 val) {m_capabilities = val;}

protected:
	virtual float	calcGainval(quint8 ga);
	virtual quint8	calcPulsPoti(quint8 val, float gv);
	virtual quint8	calcPulsAmp(quint8 val, float gv);
	virtual quint8	calcThreshval(quint8 thr);
public:
	virtual quint8	calcThreshpoti(quint8 tval);		// mainwidget.cpp

protected:
	//! MCPD-8 id
	quint8 		m_mcpdId;
	
	//! MPSD-8 id
	quint8 		m_mpsdId;

	//! Gain poti values 
	quint8 		m_gainPoti[9][2];
	
	//! Gain values
	float 		m_gainVal[9][2];

	//! Common gain
	bool 		m_comgain;

	//! Threshold poti values
	quint8 		m_threshPoti[2];

	//! Treshold values
	quint8 		m_threshVal[2];

	//! Pulser poti values
	quint8 		m_pulsPoti[2];

	//! Pulser amplitude
	float		m_pulsAmp[2];

	//! Pulser position
	quint8 		m_pulsPos[2];

	//! Pulser channel
	quint8 		m_pulsChan[2];

	//! Pulser 
	quint8 		m_pulser[2];

private:
	//! amplitude mode
	bool		m_ampMode[2]; 

protected:
	//! the bus number ????
	quint8		m_busNum;

private:
	quint16 	m_internalReg[3][2];

	bool		m_active[8];

	bool		m_histogram[8];

        float		m_version;

	quint16		m_capabilities;
};

/**
 * \class NoModule
 * \short representation of not extisting module
 * 
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class LIBQMESYDAQ_EXPORT NoModule : public MPSD8
{
	Q_OBJECT
public:
	/*! 
	   constructor

	   /param id
	   /param parent
	 */
	NoModule(quint8 id, QObject *parent = 0) 
		: MPSD8(id, parent) 
	{
		m_mpsdId = 0;
		setHistogram(false);
	}

	void	setActive(bool) {MPSD8::setActive(false);}

	void	setActive(quint16 chan, bool) {MPSD8::setActive(chan, false);}

	//! \return the type of the MPSD as string
	virtual QString getType(void) {return tr("-");}

	//! \return the type of the MPSD as number
	virtual int type(void) {return TYPE_NOMODULE;}

	//! \return is the module online or not
	virtual bool online(void) {return false;}

//	virtual void	setPulser(quint8 chan, quint8 pos = 2, quint8 poti = 128, quint8 on = 0, bool preset = false);
//	virtual void	setPulserPoti(quint8 chan, quint8 pos = 2, quint8 poti = 128, quint8 on = 0, bool preset = false);

//	virtual void	setThreshold(quint8 threshold, bool preset = false);
//	virtual void 	setThreshpoti(quint8 thresh, bool preset = false);

//	virtual void	setGain(quint8 channel, float gainv, bool preset = false);
//	virtual void	setGain(quint8 channel, quint8 gain, bool preset = false);

//	virtual void	setInternalreg(quint8 reg, quint16 val, bool preset = false);
};
 

/**
 * \class MPSD8old
 * \short representation of old MPSD-8 peripheral module 
 *
 * \author Gregor Montermann <g.montermann@mesytec.com>
 */
class LIBQMESYDAQ_EXPORT MPSD8old : public MPSD8
{
	Q_OBJECT
public:
	MPSD8old(quint8 id, QObject *parent = 0);

        //! returns the number of bins per module
	quint16 bins() {return 255;}

	void 	setGain(quint8 channel, float gainv, bool preset = false);
	void	setThreshold(quint8 threshold, bool preset = false);

	quint8	calcGainpoti(float fval);
	quint8	calcThreshpoti(quint8 tval);		// mainwidget.cpp

	//! \return the type of the MPSD as string
	QString getType(void) {return tr("MPSD-8 (old)");}

	//! \return the type of the MPSD as number
	virtual int type(void) {return TYPE_MPSD8OLD;}

protected:
	float	calcGainval(quint8 ga);
	quint8	calcThreshval(quint8 thr);
//	quint8	calcPulsPoti(quint8 val, float gv);
//	quint8	calcPulsAmp(quint8 val, float gv);

private:
	float 		m_g1;
	float 		m_g2;
	float 		m_t1;
	float 		m_t2;
	float 		m_p1;
	float 		m_p2;
};

/**
 * \class MPSD8plus
 * \short representation of MPSD-8+ peripheral module 
 *
 * \author Gregor Montermann <g.montermann@mesytec.com>
 */
class LIBQMESYDAQ_EXPORT MPSD8plus : public MPSD8
{
	Q_OBJECT
public:
	MPSD8plus(quint8 id, QObject *parent = 0);

	//! \return the type of the MPSD as string
	QString getType(void) {return tr("MPSD-8+");}

	//! \return the type of the MPSD as number
	virtual int type(void) {return TYPE_MPSD8P;}

	void 	setGain(quint8 channel, float gainv, bool preset = false);
	void	setThreshold(quint8 threshold, bool preset = false);

	quint8	calcGainpoti(float fval);
	quint8	calcThreshpoti(quint8 tval);		// mainwidget.cpp

protected:
	float	calcGainval(quint8 ga);
	quint8	calcThreshval(quint8 thr);
	quint8	calcPulsPoti(quint8 val, float gv);
	quint8	calcPulsAmp(quint8 val, float gv);

private:
	float 		m_g1;
	float 		m_g2;
	float 		m_t1;
	float 		m_t2;
	float 		m_p1;
	float 		m_p2;
};

/** 
 * \class MPSD8SingleADC
 * \short representation of the MPSD-8 pheripheral module with a single ADC like at DNS
 *
 * \author Jens Krueger <jens.krueger@frm2.tum.de>
 */ 
class LIBQMESYDAQ_EXPORT MPSD8SingleADC : public MPSD8
{
	Q_OBJECT
public:
	MPSD8SingleADC(quint8 id, QObject *parent = 0);

	//! \return the type of the MPSD as string
	QString getType(void) {return tr("MPSD-8 (single ADC)");}

	//! \return the type of the MPSD as number
	virtual int type(void) {return TYPE_MPSD8SADC;}
};

#endif
