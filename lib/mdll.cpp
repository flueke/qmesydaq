/***************************************************************************
 *   Copyright (C) 2009-2013 by Gregor Montermann, mesytec GmbH & Co. KG   *
 *      g.montermann@mesytec.com                                           *
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
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <sys/types.h>
// #include <arpa/inet.h>

#include "mdll.h"
#include "mdefines.h"
#include "networkdevice.h"
#include "mcpd8.h"
#include "logging.h"

/*!
    constructor
    \param parent
 */
MDLL::MDLL(quint8 , QObject *parent)
    : QObject(parent)
    , m_mdllId(TYPE_MDLL)
{
    for(quint8 pre = 0; pre < 2; ++pre)
    {
        for(quint8 i = 0; i < 3; ++i)
        {
            m_thresh[i][pre] = 20;
        }
        for(quint8 i = 0; i < 2; ++i)
        {
            m_shift[i][pre] = 100;
            m_scale[i][pre] = 40;
        }
        m_timingWindow[0][pre] = 100;
        m_timingWindow[2][pre] = 100;
        m_timingWindow[1][pre] = 1000;
        m_timingWindow[3][pre] = 1000;

        m_pulser[pre] = false;
        m_pulsAmp[pre] = 3;
        m_pulsPos[pre] = 1;

        m_energyWindow[0][pre] = 20;
        m_energyWindow[1][pre] = 240;
    }
    m_dataset[0] = 1;
    m_dataset[1] = 1;
}

MDLL::~MDLL()
{
}

/*!
    \fn MDLL *MDLL::create(int bus, int typ, QObject *parent)

    factory to create a module

    \param bus
    \param typ
    \param parent

    \return pointer to new created module
 */
MDLL *MDLL::create(int bus, int typ, QObject *parent)
{
    Q_UNUSED(typ);
    return new MDLL(bus, parent);
}

/*!
    \fn MDLL::setThresholds(quint8 threshX, quint8 threshY, quint8 threshA, bool preset)

    \ sets the threshold values for X, Y, A CFDs
    \param threshX threshold for X CFD
    \param threshY threshold for Y CFD
    \param threshA threshold for A CFD
    \param preset ????
 */
void MDLL::setThresholds(quint8 threshX, quint8 threshY, quint8 threshA, bool preset)
{
    if(preset)
    {
        m_thresh[0][1] = threshX;
        m_thresh[1][1] = threshY;
        m_thresh[2][1] = threshA;
        MSG_NOTICE << tr("m_thresh presets%1, MDLL to: %2, %3, %4").arg(m_mcpdId).arg(m_thresh[0][preset]).arg(m_thresh[1][preset]).arg(m_thresh[3][preset]);
    }
    else
    {
        m_thresh[0][0] = threshX;
        m_thresh[1][0] = threshY;
        m_thresh[2][0] = threshA;
        MSG_NOTICE << tr("m_thresh presets%1, MDLL to: %2, %3, %4").arg(m_mcpdId).arg(m_thresh[0][preset]).arg(m_thresh[1][preset]).arg(m_thresh[3][preset]);
    }
}

/*!
    \fn MDLL::setSpectrum(quint8 shiftX, quint8 shiftY, quint8 scaleX, quint8 scaleY, bool preset)

    \ sets the shift (delay offstes) and scale (delay range) values for X, Y
    \param shiftX offset for X
    \param shiftY offset for Y
    \param scaleX delay range for X
    \param scaleY delay range for Y
    \param preset ????
 */
void MDLL::setSpectrum(quint8 shiftX, quint8 shiftY, quint8 scaleX, quint8 scaleY, bool preset)
{
    if(preset)
    {
        m_shift[0][1] = shiftX;
        m_shift[1][1] = shiftY;
        m_scale[0][1] = scaleX;
        m_scale[1][1] = scaleY;
        MSG_NOTICE << tr("m_shift/scale presets %1, MDLL to: %2, %3, %4, %5").
		arg(m_mcpdId).arg(m_shift[0][preset]).arg(m_shift[1][preset]).arg(m_scale[0][preset]).arg(m_scale[1][preset]);
    }
    else
    {
        m_shift[0][0] = shiftX;
        m_shift[1][0] = shiftY;
        m_scale[0][0] = scaleX;
        m_scale[1][0] = scaleY;
        MSG_NOTICE << tr("m_shift/scale presets %1, MDLL to: %2, %3, %4, %5").
		arg(m_mcpdId).arg(m_shift[0][preset]).arg(m_shift[1][preset]).arg(m_scale[0][preset]).arg(m_scale[1][preset]);
    }
}

/*!
    \fn void MDLL::setDataset(quint8 set, bool preset)

    \param set
    \param preset
 */
void MDLL::setDataset(quint8 set, bool preset)
{
    if(set > 1)
        set = 1;
    if(preset)
    {
        m_dataset[1] = set;
        MSG_NOTICE << tr("m_dataset preset %1, MDLL to: %2").arg(m_mcpdId).arg(m_dataset[preset]);
    }
    else
    {
        m_dataset[0] = set;
        MSG_NOTICE << tr("m_dataset preset %1, MDLL to: %2").arg(m_mcpdId).arg(m_dataset[preset]);
    }
}

/*!
    \fn void MDLL::setTimingWindow(quint16 xlo, quint16 xhi, quint16 ylo, quint16 yhi, bool preset)

    \param xlo
    \param xhi
    \param ylo
    \param yhi
    \param preset
 */
void MDLL::setTimingWindow(quint16 xlo, quint16 xhi, quint16 ylo, quint16 yhi, bool preset)
{
    if(preset)
    {
        m_timingWindow[0][1] = xlo;
        m_timingWindow[1][1] = xhi;
        m_timingWindow[2][1] = ylo;
        m_timingWindow[3][1] = yhi;
        MSG_NOTICE << tr("m_timingWindow presets %1, MDLL to: %2, %3, %4, %5").
	       arg(m_mcpdId).arg(m_timingWindow[0][preset]).arg(m_timingWindow[1][preset]).arg(m_timingWindow[2][preset]).arg(m_timingWindow[3][preset]);
    }
    else
    {
        m_timingWindow[0][0] = xlo;
        m_timingWindow[1][0] = xhi;
        m_timingWindow[2][0] = ylo;
        m_timingWindow[3][0] = yhi;
        MSG_NOTICE << tr("m_timingWindow presets %1, MDLL to: %2, %3, %4, %5").
	       arg(m_mcpdId).arg(m_timingWindow[0][preset]).arg(m_timingWindow[1][preset]).arg(m_timingWindow[2][preset]).arg(m_timingWindow[3][preset]);
    }
}

/*!
    \fn void MDLL::setEnergyWindow(quint8 elo, quint8 ehi, bool preset)

    \param elo
    \param ehi
    \param preset
 */
void MDLL::setEnergyWindow(quint8 elo, quint8 ehi, bool preset)
{
    m_energyWindow[0][preset] = elo;
    m_energyWindow[1][preset] = ehi;

    if(preset)
    {
        MSG_NOTICE << tr("m_energyWindow presets %1, MDLL to: %2, %3").arg(m_mcpdId).arg(m_energyWindow[0][preset]).arg(m_energyWindow[1][preset]);
    }
    else
    {
        MSG_NOTICE << tr("m_energyWindow presets %1, MDLL to: %2, %3").arg(m_mcpdId).arg(m_energyWindow[0][preset]).arg(m_energyWindow[1][preset]);
    }
}

/*!
    \fn void MDLL::setPulser(quint8 pos, quint8 amp, quint8 on, bool preset)

    \param pos
    \param amp
    \param on
    \param preset
 */
void MDLL::setPulser(quint8 pos, quint8 amp, quint8 on, bool preset)
{
    if(preset)
    {
        m_pulser[1] = on;
        if(pos > 2)
            pos = 2;
        m_pulsPos[1] = pos;

        if(amp > 3)
            amp = 3;
        m_pulsAmp[1] = amp;
        MSG_NOTICE << tr("Pulser presets %1, MDLL to: %2, %3, %4").arg(m_mcpdId).arg(m_pulser[preset]).arg(m_pulsPos[preset]).arg(m_pulsAmp[preset]);
    }
    else
    {
        m_pulser[0] = on;
        if(pos > 2)
            pos = 2;
        m_pulsPos[0] = pos;

        if(amp > 3)
            amp = 3;
        m_pulsAmp[0] = amp;
        MSG_NOTICE << tr("Pulser presets %1, MDLL to: %2, %3, %4").arg(m_mcpdId).arg(m_pulser[preset]).arg(m_pulsPos[preset]).arg(m_pulsAmp[preset]);
    }
}

/*!
    \fn quint8 MDLL::getSpectrum(quint8 val)

    \param val 
    \return the spectrum
 */
quint8 MDLL::getSpectrum(quint8 val)
{
    if(val < 2)
        return m_shift[val][1];
    else
        return m_scale[val-2][1];

}

#if 0
/*!
    \fn void MDll::initMdll(void)

    initialize settings
 */
void MDLL::initMdll(void)
{
    m_mcpd->setMdllSpectrum(m_shift[0][1], m_shift[1][1], m_scale[0][1], m_scale[1][1]);
    m_mcpd->setMdllAcqset(0, 0, m_timingWindow[0][1], m_timingWindow[1][1], m_timingWindow[2][1], m_timingWindow[3][1]);
    m_mcpd->setMdllEnergyWindow(m_energyWindow[0][1], m_energyWindow[1][1], 0, 0);
    m_mcpd->setMdllThresholds(m_thresh[0], m_thresh[1], m_thresh[2]);
}

#endif

#if 0
/*!
    \fn void MDll::serialize(QFile * fi)

    \param fi
 */
void MDLL::serialize(QFile *fi)
{
        quint8 c;
        QTextStream t( fi );        // use a text stream

        t << "[MDLL]";
        t << '\r' << '\n';
        t << "id = " << 0;
        t << '\r' << '\n';
#if defined(_MSC_VER)
#	pragma message("ToDo")
#else
#	warning TODO
#endif
#if 0
        t << "ipAddress = " << theApp->netDev->getOwnAddress(0);
#endif
        t << '\r' << '\n';
        t << "master = ";
        if(master)
                t << 1;
        else
                t << 0;
        t << '\r' << '\n';
        t << "terminate = ";
        if(terminate)
                t << 1;
        else
                t << 0;
        t << '\r' << '\n';
        for(c=0;c<3;c++){
                t << "counterCell"<< c << " = " << m_mdllSet.counterCell[c][0] << " " << m_mdllSet.counterCell[c][1];
                t << '\r' << '\n';
        }
        for(c=0;c<4;c++){
                t << "auxTimer"<< c << " = " << m_mdllSet.auxTimer[c];
                t << '\r' << '\n';
        }
        t << '\r' << '\n';
        t << "threshX = " << m_mdllSet.threshX;
        t << '\r' << '\n';
        t << "threshY = " << m_mdllSet.threshY;
        t << '\r' << '\n';
        t << "threshA = " << m_mdllSet.threshA;
        t << '\r' << '\n';
        t << "shiftX = " << m_mdllSet.shiftX;
        t << '\r' << '\n';
        t << "shiftY = " << m_mdllSet.shiftY;
        t << '\r' << '\n';
        t << "scaleX = " << m_mdllSet.scaleX;
        t << '\r' << '\n';
        t << "scaleY = " << m_mdllSet.scaleY;
        t << '\r' << '\n';
        t << "tsumXlo = " << m_mdllSet.tsumXlo;
        t << '\r' << '\n';
        t << "tsumXhi = " << m_mdllSet.tsumXhi;
        t << '\r' << '\n';
        t << "tsumYlo = " << m_mdllSet.tsumYlo;
        t << '\r' << '\n';
        t << "tsumYhi = " << m_mdllSet.tsumYhi;
        t << '\r' << '\n';
        t << "energyLo = " << m_mdllSet.energyLow;
        t << '\r' << '\n';
        t << "energyHi = " << m_mdllSet.energyHi;
        t << '\r' << '\n';
}
#endif
