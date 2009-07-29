/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann   *
 *   g.montermann@mesytec.com   *
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
#ifndef STRUCTURES_H
#define STRUCTURES_H

typedef struct _MDP_PACKET
{
	quint16 bufferLength;
	quint16 bufferType;
	quint16 headerLength;
	quint16 bufferNumber;
	quint16 cmd;
	quint8 	deviceStatus;
	quint8 	deviceId;
	quint16 time[3];
	quint16 headerChksum;
	quint16 data[750];
} MDP_PACKET, *PMDP_PACKET;

typedef struct _DATA_PACKET
{
	quint16 bufferLength;
	quint16 bufferType;
	quint16 headerLength;
	quint16 bufferNumber;
	quint16 cmd;
	quint8  deviceStatus;
	quint8  deviceId;
	quint16 time[3];
	quint16 param[4][3];
	quint16 data[750];
} DATA_PACKET, *PDATA_PACKET;

typedef struct TriggerEvent
{
	quint8	id 		: 1;
	quint8	trigId 		: 3;
	quint8	dataId 		: 4;
	quint32 data 		: 21;
	quint32 timestamp 	: 19;
} TRIGGER;

typedef struct NeutronEvent
{
	quint8 	id		: 1;
	quint8	modId		: 3;
	quint8	slotId		: 5;
	quint16	amp		: 10;
	quint16 pos		: 10;
	quint32	timestamp	: 19;
} NEUTRON;

#define BUFTYPE		0x8000
#define CMDBUFLEN	1
#define CMDHEADLEN	10
#define STDBUFLEN	1

#endif
