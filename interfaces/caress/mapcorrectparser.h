/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009 by Jens Krüger <jens.krueger@frm2.tum.de>          *
 *   Copyright (C) 2011 by Lutz Rossa <rossa@helmholtz-berlin.de>          *
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

////////////////////////////////////////////////////////////////////////////
// $$HeadURL$$
//
// last change:
// $$Author$$
// $$Date$$
// revision $$Rev$$
////////////////////////////////////////////////////////////////////////////

#ifndef __MAPCORRECTPARSER_H__8C42E71B_F6B6_49E9_B2B3_A53982C9165D__
#define __MAPCORRECTPARSER_H__8C42E71B_F6B6_49E9_B2B3_A53982C9165D__

#include "mapcorrect.h"

MapCorrection parseCaressMapCorrection(const char* pMapping, int iLength, int iSrcWidth, int iSrcHeight, int iDstWidth, int iDstHeight);
MapCorrection parseCaressMapCorrection(const QString& mapping, int iSrcWidth, int iSrcHeight, int iDstWidth, int iDstHeight);

#endif /*__MAPCORRECTPARSER_H__8C42E71B_F6B6_49E9_B2B3_A53982C9165D__*/
