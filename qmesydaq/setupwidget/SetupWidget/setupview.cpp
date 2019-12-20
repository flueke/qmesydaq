/***************************************************************************
 *   Copyright (C) 2013-2019 by Stefan Rainow <stefan.rainow@frm2.tum.de>  *
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

#include "setupview.h"
#include "canvas.h"

#include <math.h>
#include <QWheelEvent>

#include <QDebug>

SetupView::SetupView(QWidget *parent) :
    QGraphicsView(parent)
{
        setResizeAnchor(QGraphicsView::AnchorUnderMouse);
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        this->setRenderHint(QPainter::Antialiasing, true);
        this->setRenderHint(QPainter::HighQualityAntialiasing, true);
        zoomfactor = 1;
        zoomable = true;
}

void SetupView::wheelEvent(QWheelEvent *event)
{
    if(zoomable)
    {
        double numDegrees = event->delta()/8.0;
        double numSteps = numDegrees /15.0;
        double factor = pow(1.125, numSteps);
        if (zoomfactor*factor < 0.25)
        {
            setZoom(0.25);
        }
        else if(zoomfactor*factor > 2)
        {
            setZoom(2);
        }
        else
        {
            zoomfactor *= factor;
            scale(factor,factor);
        }
        emit zoomChanged(zoomfactor);
    }

    QGraphicsView::wheelEvent(event);
}

void SetupView::resetZoom()
{
    scale(1/zoomfactor,1/zoomfactor);
    zoomfactor = 1;
    emit zoomChanged(zoomfactor);
}

void SetupView::setZoom(double zoom)
{
    if (zoomable)
    {
        resetZoom();
        scale(zoom,zoom);
        zoomfactor = zoom;
    }
    emit zoomChanged(zoomfactor);
}

void SetupView::changeZoomLock(bool state)
{
    zoomable = !state;
}

void SetupView::setZoomfactor(double zoom)
{
    zoomfactor = zoom;
    if (zoomfactor > 1)
        setZoom(1.0);
    else if (zoomfactor < 0.25)
        setZoom(0.25);
    else
        emit zoomChanged(zoomfactor);
}

bool SetupView::isZoomable()
{
    return zoomable;
}


