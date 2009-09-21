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
#ifndef STRUCTURES_H
#define STRUCTURES_H

//! command packet structure
typedef struct _MDP_PACKET
{
	//! length of the buffer
	quint16 bufferLength;
	//! the buffer type
	quint16 bufferType;
	//! the length of the buffer header
	quint16 headerLength;
	//! number of the packet 
	quint16 bufferNumber;
	//! the command number
	quint16 cmd;
	//! the device state
	quint8 	deviceStatus;
	//! the id of the device
	quint8 	deviceId;
	//! device time
	quint16 time[3];
	//! check sum of the header
	quint16 headerChksum;
	//! the data, length of the data = length of the buffer - length of the header
	quint16 data[750];
} MDP_PACKET, *PMDP_PACKET;

//! data packet structure
typedef struct _DATA_PACKET
{
	//! length of the buffer
	quint16 bufferLength;
	//! the buffer type
	quint16 bufferType;
	//! the length of the buffer header
	quint16 headerLength;
	//! number of the packet 
	quint16 bufferNumber;
	//! the command number
	quint16 cmd;
	//! the device state
	quint8  deviceStatus;
	//! device time
	quint8  deviceId;
	//! device time
	quint16 time[3];
	//! the values of the parameters (belong to the header)
	quint16 param[4][3];
	//! the events, length of the data = length of the buffer - length of the header
	quint16 data[750];
} DATA_PACKET, *PDATA_PACKET;

//! trigger event structure
typedef struct TriggerEvent
{
	//! flag to indicate a trigger event
	quint8	id 		: 1;
	//! number of the trigger
	quint8	trigId 		: 3;
	//! number of the data source
	quint8	dataId 		: 4;
	//! data itself
	quint32 data 		: 21;
	//! timestamp of the trigger event
	quint32 timestamp 	: 19;
} TRIGGER;

//! neutron event structure
typedef struct NeutronEvent
{
	//! flag to indicate a neutron event
	quint16 id		: 1;
	//! number of the MPSD generating this event
	quint16	modId		: 3;
	//! number of the slot inside the MPSD
	quint16	slotId		: 5;
	//! amplitude value of the neutron event
	quint16	amp		: 10;
	//! position of the neutron event
	quint16 pos		: 10;
	//! timestamp of the neutron event
	quint32	timestamp	: 19;
} NEUTRON;

#define BUFTYPE		0x8000
#define CMDBUFLEN	1
#define CMDHEADLEN	10
#define STDBUFLEN	1

#endif
