/***************************************************************************
 *   Copyright (C) 2009-2020 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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

#include "mdll.h"
#include "mdefines.h"
#include "networkdevice.h"
#include "mcpd8.h"
#include "qmlogging.h"

/*!
    constructor
    \param parent
 */
MWPCHR::MWPCHR(quint8 bus, QObject *parent)
    : M2D(bus, parent)
{
}

MWPCHR::~MWPCHR()
{
}

/*!
    \fn QList<quint16> MCPD8::getHistogramList(void)

    return the list of channels used in histograms

    \return the list of channels used in histograms
 */
QList<quint16> MWPCHR::getHistogramList(void)
{
    QList<quint16> result;

// MWPCHR has fixed channels
    for(quint16 i = 0; i < 1024; i++)
        result << i;
    return result;
}

/*!
    \fn QList<quint16> MCPD8::getHistogramList(void)

    return the list of channels used in histograms

    \return the list of channels used in histograms
 */
QList<quint16> MWPCHR::getActiveList(void)
{
    QList<quint16> result;

// MDLL has fixed channels
    for(quint16 i = 0; i < 1024; i++)
        result << i;
    return result;
}

#if 0
/*!
    \fn void MDll::serialize(QFile * fi)

    \param fi
 */
void MWPCHR::serialize(QFile *fi)
{
        quint8 c;
        QTextStream t( fi );        // use a text stream

        t << "[MWPCHR]";
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

quint8 MWPCHR::getModuleId(void) const
{
	return TYPE_MWPCHR;
}

QString MWPCHR::getType(void) const
{
	return tr("MWPCHR");
}

int MWPCHR::type(void) const
{
	return TYPE_MWPCHR;
}

bool MWPCHR::isPulserOn(bool) const
{
	return false;
}

quint16 MWPCHR::bins() const
{
	return 1024;
}

quint16 MWPCHR::channels() const
{
	return 1024;
}
