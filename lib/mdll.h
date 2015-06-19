/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2015 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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
#ifndef MDLL_H
#define MDLL_H

#include <QString>
#include "mdefines.h"
#include "logging.h"
#include "mpsd8.h"

class M2D : public QObject
{
	Q_OBJECT

public:
	M2D(quint8, QObject *parent = 0);

	~M2D();

	static M2D *create(int, int, QObject * = 0);

	//! \return the ID of the MDLL
	virtual quint8 	getModuleId(void) const = 0;

	//! \return the type of the module as number
	virtual int type(void) const = 0;

	//! \return the type of the MDLL as string
	virtual QString getType(void) const = 0;

	virtual bool	active(void) const;

	virtual void	setActive(bool);

	// bool	active(quint16);

	// void	setActive(quint16, bool);

	virtual bool	histogram(void) const;

	virtual void	setHistogram(bool);

	// bool	histogram(quint16);

	// void	setHistogram(quint16, bool);

	virtual QList<quint16> getHistogramList(void) = 0;

	virtual QList<quint16> getActiveList(void) = 0;

	//! returns the number of bins
	virtual quint16 bins() const;

	//! returns the number of channels
	virtual quint16 channels() const;

	//! returns the number of the bus
	quint8 busNumber(void) const;

	virtual quint16 capabilities(void) const;

protected:
	//! MCPD-8 id
	quint8 		m_mcpdId;
};

/**
 * \class MDLL
 * \short representation of MDLL module
 *
 * \author Gregor Montermann <g.montermann@mesytec.com>
 */
class MDLL : public M2D
{
	Q_OBJECT

public:
	MDLL(quint8, QObject * = 0);

	~MDLL();

	//! \return the ID of the MDLL
	quint8 	getModuleId(void) const;

	//! \return the type of the MDLL as string
	QString getType(void) const;

	//! \return the type of the MPSD as number
	int type(void) const;

	virtual QList<quint16> getHistogramList(void);

	virtual QList<quint16> getActiveList(void);

	//! \return is the module online or not
	bool online(void) const;

	void setSpectrum(quint8 shiftX, quint8 shiftY, quint8 scaleX, quint8 scaleY, bool preset);

	quint8 getSpectrum(quint8 val);

	void setThresholds(quint8 thresh_x, quint8 thresh_y, quint8 thresh_a, bool preset = false);

	/*!
         * \param val
         * \return the threshold
	*/
	quint8 getThreshold(quint8 val) const;

	void setTimingWindow(quint16 xlo, quint16 xhi, quint16 ylo, quint16 yhi, bool preset);

	/*!
	 * \param val
	 * \return the timing window
	 */
	quint16 getTimingWindow(quint8 val) const;

	void setEnergyWindow(quint8 elo, quint8 ehi, bool preset);

	/*!
	 * \param val
	 * \return the energy window
	 */
	quint8 getEnergyWindow(quint8 val) const;

	void setDataset(quint8 set, bool preset = false);

	/*!
	 * \return the data set type
	 */
	quint8 getDataset(void) const;

	// Pulser related methods
	void setPulser(quint8 pos = 2, quint8 ampl = 3, quint8 on = 0, bool preset = false);

	//! return the pulser position
	quint8 getPulsPos(bool preset = false) const;

	//! return the pulser amplitude
	quint8 getPulsAmp(bool preset = false) const;

	//! \return is the pulser on
	bool isPulserOn(bool preset = false) const;

	//! initialises the MDLL
	void init(void);


// Mode related methods
	/**
	 * sets the mode E_x_y / E_tx_ty
	 *
	 * \param timing true = E_tx_ty, false = E_x_y
	 * \param preset ????
	 * \see getMode
	 */
	void	setMode(bool timing, bool preset = false);

	/**
	 * gets the mode E_x_y / E_tx_ty
	 *
	 * \param preset ????
	 * \return timing true = E_tx_ty, false = E_x_y
	 * \see setMode
	 */
	bool	getMode(bool preset = false) const;

#if 0
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
#endif

protected:
	// quint8	calcThreshval(quint8 thr);

public:
	// quint8	calcThreshpoti(quint8 tval);

private:
	//! timing mode
	bool	m_timingMode[2];

	// bool	m_active;

	// bool	m_histogram;

	//! MDLL specific parameters:

	//! thresholds for x, y and anode CFD
	quint8	m_thresh[3][2];

	//! delay offsets for x and y
	quint8	m_shift[2][2];
	//! delay range (scaling) for x and y
	quint8	m_scale[2][2];

	//! software windows for timing sum [xlo, xhi], [ylo, yhi]
	quint16	m_timingWindow[4][2];
	//! software window for energy
	quint8	m_energyWindow[2][2];

	//! data set to be transmitted: 0={X, Y, E}, 1={tsumX, tsumY, E}
	quint8	m_dataset[2];

	//! Pulser amplitude
	quint8	m_pulsAmp[2];
	//! Pulser position
	quint8	m_pulsPos[2];
	//! Pulser
	quint8	m_pulser[2];
};

/**
 * \class MWPCHR
 * \short representation of MWPCHR module
 *
 * \author Jens Krüger <jens.krueger@frm2.tum.de>
 */
class MWPCHR : public M2D
{
	Q_OBJECT

public:
	MWPCHR(quint8, QObject * = 0);

	~MWPCHR();

	//! \return the ID of the MDLL
	quint8 	getModuleId(void) const;

	//! \return the type of the MDLL as string
	QString getType(void) const;

	//! \return the type of the MPSD as number
	int type(void) const;

	//! \return is the module online or not
	bool online(void);

	bool isPulserOn(bool = false) const;

	//! initialises the MWPCHR
	void init(void);

	QList<quint16> getHistogramList(void);

	QList<quint16> getActiveList(void);

	//! returns the number of bins
	quint16 bins() const;

	//! returns the number of channels
	quint16 channels() const;
};

#endif
