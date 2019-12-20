/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2019 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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

	virtual QList<quint16> getHistogramList(void);

	virtual QList<quint16> getActiveList(void);

	//! \return the ID of the MPSD
	quint8 	getModuleId(void) const;

	//! \return the type of the MPSD as string
	virtual QString getType(void) const;

	//! \return the type of the MPSD as number
	virtual int type(void) const;

	//! \return is the module online or not
	virtual bool online(void) const;

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
	virtual quint8	getPulsPos(bool preset = false) const;

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
	quint8	getPulsAmp(bool preset = false) const;

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
	quint8	getPulsChan(bool preset = false) const;

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
	quint8	getPulsPoti(bool preset = false) const;

	//! \return is the pulser on
	bool	isPulserOn() const;

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
	quint8	getThreshold(bool preset = false) const;

	/**
 	 * gets the threshold poti value
	 *
	 * \param preset ????
	 * \return threshold poti value
	 * \see setThreshold
	 * \see setThreshpoti
	 * \see getThreshold
	 */
	quint8	getThreshpoti(bool preset = false) const;

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
	virtual quint8	getGainpoti(quint8 chan, bool preset = false);

	/**
	 * get the user value for the gain
	 *
	 * \return gain user value
	 * \see setGain
	 * \see getGainpoti
	 */
	virtual float	getGainval(quint8 chan, bool preset = false);

	//! \return use the same gain for all channels ?
	virtual bool	comGain() const;

// Mode related methods
	/**
	 * sets the mode amplitude/position
	 *
	 * \param amplitude true = amplitude, false = position
	 * \param preset ????
	 * \see getMode
	 */
	void	setMode(bool amplitude, bool preset = false);

	/**
	 * gets the mode amplitude/position
	 *
	 * \param preset ????
	 * \return amplitude true = amplitude, false = position
	 * \see setMode
	 */
	bool	getMode(bool preset = false) const;

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
	quint16	getInternalreg(quint8 reg, bool preset = false) const;

	virtual quint8	calcGainpoti(float fval);

        //! returns the number of bins per module
	virtual quint16 bins() const;

	//! returns the number of the bus
	quint8 busNumber(void) const;

// version related methods
	//! return version as float value major.minor
	float	version(void) const;

	//! sets the version as float value major.minor
	//! \param val
	void	setVersion(const float val);

// capabilities related methods
	//! return capabilities
	virtual quint16 capabilities(void) const;

	//! sets the capabilities
	//! \param val
	void	setCapabilities(const quint16 val);

	//! \return the number of available channels
	virtual quint8	getChannels() const;

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
	quint8 		m_gainPoti[17][2];

	//! Gain values
	float 		m_gainVal[17][2];

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

	//! amplitude mode
	bool		m_ampMode[2];

protected:
	//! the bus number ????
	quint8		m_busNum;

private:
	quint16 	m_internalReg[3][2];

protected:
	QVector<bool>	m_active;

	QVector<bool>	m_histogram;

private:
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
	NoModule(quint8 id, QObject *parent = 0);

	void	setActive(bool);

	void	setActive(quint16 chan, bool);

	//! \return the type of the MPSD as string
	virtual QString getType(void) const;

	//! \return the type of the MPSD as number
	virtual int type(void) const;

	//! \return is the module online or not
	virtual bool online(void) const;

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
	quint16 bins() const;

	void 	setGain(quint8 channel, float gainv, bool preset = false);
	void	setThreshold(quint8 threshold, bool preset = false);

	quint8	calcGainpoti(float fval);
	quint8	calcThreshpoti(quint8 tval);		// mainwidget.cpp

	//! \return the type of the MPSD as string
	QString getType(void) const;

	//! \return the type of the MPSD as number
	virtual int type(void) const;

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
	QString getType(void) const;

	//! \return the type of the MPSD as number
	virtual int type(void) const;

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
	QString getType(void) const;

	//! \return the type of the MPSD as number
	virtual int type(void) const;
};

#endif
