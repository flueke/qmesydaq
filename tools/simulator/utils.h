/***************************************************************************
 *   Copyright (C) 2013-2019 by Lutz Rossa <rossa@helmholtz-berlin.de>     *
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
#ifndef __MAIN_H__4BAC047E_3685_4F88_9ADF_3EE05A666E1C__
#define __MAIN_H__4BAC047E_3685_4F88_9ADF_3EE05A666E1C__

#include <QString>

class SimMCPD8;
void logmsg(SimMCPD8 *pMCPD8, const QString &szLine);
void logmsg(SimMCPD8 *pMCPD8, const char *szFormat, ...);

#endif /*__MAIN_H__4BAC047E_3685_4F88_9ADF_3EE05A666E1C__*/
