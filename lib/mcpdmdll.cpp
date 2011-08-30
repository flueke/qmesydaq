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
#include <QHostAddress>
#include <QApplication>
#include <QTimer>

#include "networkdevice.h"
#include "mpsd8.h"
#include "mcpd8.h"
#include "mstd16.h"
#include "mdll.h"
#include "mdefines.h"

/*!
    \fn bool MCPDMDLL::setThreshold(quint8 x, quint8 y, quint8 amp)

    sets the thresholds

    \param x
    \param y
    \param amp
    \return true if operation was succesful or not
    \see getId
 */
bool MCPDMDLL::setThreshold(quint8 x, quint8 y, quint8 amp)
{
    	protocol(tr("Set threshholds to x=%1, y=%2, and amp=%3.").arg(x).arg(y).arg(amp), NOTICE);
	initCmdBuffer(SETDLLTHRESHS);
	setBuffer(0, x);
	setBuffer(1, y);
	setBuffer(2, amp);
	finishCmdBuffer(3);
	if (sendCommand())
	{
    		m_threshX = x;
    		m_threshY = y;
    		m_threshAmp = amp;
		return true;
	}
	return false;
}

/*!
    \fn bool MCPDMDLL::setSpectrum(quint8 shiftX, quint8 shiftY, quint8 scaleX, quint8 scaleY)

    sets the spectrum

    \param shiftX
    \param shiftY
    \param scaleX
    \param scaleY
    \return true if operation was succesful or not
    \see getId
 */
bool MCPDMDLL::setSpectrum(quint8 shiftX, quint8 shiftY, quint8 scaleX, quint8 scaleY)
{
    	protocol(tr("Set spectrum to shift(%1, %2) and scale(%3, %4).").arg(shiftX).arg(shiftY).arg(scaleX).arg(scaleY), NOTICE);
	initCmdBuffer(SETDLLSPECTRUM);
	setBuffer(0, shiftX);
	setBuffer(1, shiftY);
	setBuffer(2, scaleX);
	setBuffer(3, scaleY);
	finishCmdBuffer(4);
	if (sendCommand())
	{
    		m_shiftX = shiftX;
    		m_shiftY = shiftY;
    		m_scaleX = scaleX;
    		m_scaleY = scaleY;
		return true;
	}
	return false;
}

/*!
    \fn bool MCPDMDLL::setHistogram(quint8 previewSize, quint16 previewRate, quint8 size, quint8 type)

    \param previewSize
    \param previewRate
    \param size
    \param type
    \return true if operation was succesful or not
 */
bool MCPDMDLL::setHistogram(quint8 previewSize, quint16 previewRate, quint8 size, quint8 type)
{
    	protocol(tr("Set histgram to preview size = %1, previewRate = %2, size = %3, and type = %4).").arg(previewSize).arg(previewRate).arg(size).arg(type), NOTICE);
	initCmdBuffer(SETDLLHIST);
        setBuffer(0, previewSize);
        setBuffer(1, previewRate);
        setBuffer(2, size);
        setBuffer(3, type);
	finishCmdBuffer(4);
#if 0
	if (sendCommand())
#endif
	{
		m_previewHistSize = previewSize;
		m_previewHistRate = previewRate;
		m_histSize = size;
		m_histType = type;
		return true;
	}
	return false;
}

/*!
    \fn bool MCPDMDLL::setMode(quint8 mode)

    sets the mode 

    \param mode mode parameter
    \return true if operation was succesful or not
    \see getMode
*/
bool MCPDMDLL::setMode(quint8 mode)
{
	initCmdBuffer(SETDLLMODE);
	setBuffer(0, mode);
	finishCmdBuffer(1);
#if 0
	return sendCommand();
#else
	return true;
#endif
}

/*!
    \fn bool MCPDMDLL::setSlide(quint8 slscOff)

    set the slide ???? 

    \param slscOff 
    \return true if operation was succesful or not
    \see getMode
*/
bool MCPDMDLL::setSlide(quint8 slscOff)
{
	initCmdBuffer(SETDLLSLSC);
	setBuffer(0, slscOff);
	finishCmdBuffer(1);
#if 0
	return sendCommand();
#else
	return true;
#endif
}

/*!
    \fn bool MCPDMDLL::setDataReg(quint8 datareg)

    set the data ???? 

    \param datareg 
    \return true if operation was succesful or not
    \see getMode
*/
bool MCPDMDLL::setDataReg(quint8 datareg)
{
	initCmdBuffer(SETDLLTESTREG);
	setBuffer(0, datareg);
	finishCmdBuffer(1);
#if 0
	return sendCommand();
#else
	return true;
#endif
}

/*!
    \fn bool MCPDMDLL::setAcqset(quint32 eventLimit, quint32 tSumX, quint32 tSumY)
    
    setup the data acquisition setup ???

    \param eventLimit
    \param tSumX
    \param tSumY
    \return true if operation was succesful or not
 */
bool MCPDMDLL::setAcqset(quint32 eventLimit, quint32 tSumX, quint32 tSumY)
{
	initCmdBuffer(SETDLLACQSET);
	setBuffer(0, quint16(eventLimit & 0xFFFF));
	setBuffer(1, quint16(eventLimit >> 16));
	setBuffer(2, quint16(tSumX & 0xFFFF));
	setBuffer(3, quint16(tSumX >> 16));
	setBuffer(4, quint16(tSumY & 0xFFFF));
	setBuffer(5, quint16(tSumY >> 16));
	finishCmdBuffer(6);
	return sendCommand();
}

/*!
    \fn bool MCPDMDLL::setEnergy(quint8 energyLow, quint8 energyHi, quint8 eScaleX, quint8 eScaleY)

    sets up the engery ????
 
    \param energyLow
    \param energyHi
    \param eScaleX
    \param eScaleY
    \return true if operation was succesful or not
 */
bool MCPDMDLL::setEnergy(quint8 energyLow, quint8 energyHi, quint8 eScaleX, quint8 eScaleY)
{
	initCmdBuffer(SETDLLENERGY);
	setBuffer(0, energyLow);
	setBuffer(1, energyHi);
	setBuffer(3, eScaleX);
	setBuffer(4, eScaleY);
	finishCmdBuffer(4);
	return sendCommand();
}

/*!
    \fn bool MCPDMDLL::setPulser(quint8 on, quint8 ampl, quint8 pos)

    sets the pulser

    \param on
    \param ampl
    \param pos
 
    \return true if operation was succesful or not
 */
bool MCPDMDLL::setPulser(quint8 on, quint8 ampl, quint8 pos)
{
	initCmdBuffer(SETDLLPULSER);
	setBuffer(0, on);
	setBuffer(1, ampl);
	setBuffer(2, pos);
	finishCmdBuffer(3);
	return sendCommand();
}
