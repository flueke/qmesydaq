/***************************************************************************
 *   Copyright (C) 2002 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2008,2011 by Lutz Rossa <rossa@helmholtz-berlin.de>     *
 *   Copyright (C) 2009-2010 by Jens Krüger <jens.krueger@frm2.tum.de>     *
 *   Copyright (C) 2010 by Alexander Lenz <alexander.lenz@frm2.tum.de>     *
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
#ifndef CARESSLOOP_H
#define CARESSLOOP_H

#include <QStringList>
#include "LoopObject.h"

class QtInterface;

/*!
  \brief CARESS interface loop object
  \author Lutz Rossa <rossa@helmholtz-berlin.de>
 */
class CARESSLoop : public LoopObject
{
  Q_OBJECT
public:
  CARESSLoop(QStringList argList, QtInterface *interface = 0);

protected:
  void runLoop();

private slots:
  void shutdownLoop() { m_bDoLoop=false; }

private:
  bool        m_bDoLoop;     //!< flag to detect exit of QMesyDAQ
  QStringList m_asArguments; //!< command line arguments of QMesyDAQ
};

#endif // CARESSLOOP_H
