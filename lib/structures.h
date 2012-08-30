/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2012 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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

#include <QMetaType>
#include <QSharedDataPointer>

//! data packet structure as Share data object (multi threading preparation)
struct SD_MPD_PACKET_DATA : public QSharedData
{
	quint16 bufferLength;	//!< length of the buffer
	
	quint16 bufferType;	//!< the buffer type
	
	quint16 headerLength;	//!< the length of the buffer header
	
	quint16 bufferNumber;	//!< number of the packet 
	
	quint16 cmd;		//!< the command number
	
	quint8 	deviceStatus;	//!< the device state
	
	quint8 	deviceId;	//!< the id of the device
	
	quint16 time[3];	//!< device time
	
	quint16 headerChksum;	//!< check sum of the header
	
	quint16 data[750];	//!< the data, length of the data = length of the buffer - length of the header
};

Q_DECLARE_METATYPE(SD_MPD_PACKET_DATA);

//! command packet structure
struct MDP_PACKET
{
	quint16 bufferLength;	//!< length of the buffer
	
	quint16 bufferType;	//!< the buffer type
	
	quint16 headerLength;	//!< the length of the buffer header
	
	quint16 bufferNumber;	//!< number of the packet 
	
	quint16 cmd;		//!< the command number
	
	quint8 	deviceStatus;	//!< the device state
	
	quint8 	deviceId;	//!< the id of the device
	
	quint16 time[3];	//!< device time
	
	quint16 headerChksum;	//!< check sum of the header
	
	quint16 data[750];	//!< the data, length of the data = length of the buffer - length of the header
};
Q_DECLARE_METATYPE(MDP_PACKET)

//! data packet structure
struct DATA_PACKET
{
	quint16 bufferLength;	//!< length of the buffer
	
	quint16 bufferType;	//!< the buffer type
	
	quint16 headerLength;	//!< the length of the buffer header
	
	quint16 bufferNumber;	//!< number of the packet 
	
	quint16 runID; 		//!< the run ID
	
	quint8  deviceStatus;	//!< the device state
	
	quint8  deviceId;	//!< device time
	
	quint16 time[3];	//!< device time
	
	quint16 param[4][3];	//!< the values of the parameters (belong to the header)
	
	quint16 data[750];	//!< the events, length of the data = length of the buffer - length of the header
};
Q_DECLARE_METATYPE(DATA_PACKET)

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

#if 0
//! structure for MDLL settings
typedef struct _MDLL_SETTINGS 
{
	quint8 id;
//	bool master;
//	bool terminate;

	quint8 threshX;
	quint8 threshY;
	quint8 threshA;
	quint8 shiftX;
	quint8 shiftY;
	quint8 scaleX;
	quint8 scaleY;

#if 0
	quint8 mode;
	quint8 previewHistsize;
	quint16  previewHistrate;
	quint8 histSize;
	quint8 histType;
	quint8 slscOff;
	quint8 datareg;
    
	quint16 eventCounter0;
	quint16 eventCounter1;
	quint16 eventLimit0;
	quint16 eventLimit1;
#endif

	quint16 tsumXlo;
	quint16 tsumXhi;
	quint16 tsumYlo;
	quint16 tsumYhi;
    
	quint8 pulserOn;
	quint8 pulserAmpl;
	quint8 pulserPos;
    	
	quint8 energyLow;
	quint8 energyHi;

#if 0
	quint8 eScaleX;
	quint8 eScaleY;
#endif

	quint8 counterCell[3][2];
	quint16 auxTimer[4];
	quint8 paramSource[4];
} *P_MDLL_SETTING, MDLL_SETTING;
#endif

#if 0
//! structure for networksettings of MDLL
typedef struct _MDLL_ADDRESS_SET
{
	quint8 ip0;	//!<
	quint8 ip1;	//!<
	quint8 ip2;	//!<
	quint8 ip3;	//!<
    
	quint8 dip0;	//!<
	quint8 dip1;
	quint8 dip2;	//!<
	quint8 dip3;	//!<
    
	quint8 cudp;	//!<
	quint8 dudp;	//!<
    
	quint8 cip0;	//!<
	quint8 cip1;	//!<
	quint8 cip2;	//!<
	quint8 cip3;	//!<
} *P_MDLL_ADRESS_SET, MDLL_ADDRESS_SET;
#endif

#define CMDBUFTYPE		0x8000
#define TRIGGEREVENTTYPE	0x8000
#define CMDBUFLEN		1
#define CMDHEADLEN		10
#define STDBUFLEN		1

#endif

#ifndef MESYDEFS_H
#define MESYDEFS_H

// histogram size
const char XSIZE = 128;
const char YSIZE = 128;
const char TSIZE = 128;

// structures of upd/mdp communications
typedef struct _MDP_PACKET2
{
	quint8	sender;
	quint8	cmd;
	quint8	cpu;
	quint8	headerlength;
	quint16 packet;
	quint8	headerversion;
	quint8	hchksm;
	quint8	result;
	quint8	coll;
	quint8	bad;
	quint8	noini;
	quint16 star;
	quint16 isr;
	quint16 evct_hi;
	quint16 evct_lo;
	quint16 mon1_hi;
	quint16 mon1_lo;
	quint16 mon2_hi;
	quint16 mon2_lo;
	quint16 rback0;
	quint16 rback1;
	quint16 rback2;
	quint16 bufcount;
	quint16 timer_hi;
	quint16 timer_lo;
	quint8	ver_hi;
	quint8	ver_lo;
	quint8	data[1024];
} MDP_PACKET2, *PMDP_PACKET2;

typedef struct _PRESET_PACKET
{
	quint8	sender;
	quint8	cmd;
	quint8	cpu;
	quint8	headerlength;
	quint16	packet;
	quint8	headerversion;
	quint8	hchksm;
	quint16	rw;
	quint16	mon1hi;
	quint16	mon1lo;
	quint16	mon1active;
	quint16	mon2hi;
	quint16	mon2lo;
	quint16	mon2active;
	quint16	eventhi;
	quint16	eventlo;
	quint16	eventactive;
	quint16	timehi;
	quint16	timelo;
	quint16	timeactive;
} PRESET_PACKET, *PPRESET_PACKET;

// definitions for udp cmds:
#if 0
const quint8 DATAREQUEST = 0;
const quint8 DAQSTART = 1;
const quint8 DAQSTOP = 2;
const quint8 SETGAIN = 3;
const quint8 SETTHRESH = 4;
const quint8 SETPULSER = 5;
const quint8 SETMODE = 6;
const quint8 WRITEREG = 7;
const quint8 SETIRQMODE = 8;
const quint8 RESET = 9;
const quint8 SETIP = 10;
const quint8 SETCPUNUM = 11;
const quint8 PING = 12;
const quint8 REGHOST = 13;
const quint8 SETINIT = 14;
const quint8 GETPERIPH = 15;
const quint8 LEDON = 16;
const quint8 LEDOFF = 17;
const quint8 WRITESER = 18;
const quint8 READSER = 19;
const quint8 SETPORT = 20;
const quint8 CLRPORT = 21;
const quint8 DAQPAUSE = 22;
const quint8 DAQCONTINUE = 23;
const quint8 PRESET = 24;
const quint8 ENDCMD = 25;
#endif
 
// definitions for CPU register addresses
const quint8 STAW = 0;
const quint8 STAR = 2;
const quint8 IMR = 4;
const quint8 ISR = 6;
const quint8 DAT1 = 8;
const quint8 DAT2 = 10;
const quint8 BUF = 16;
const quint8 EVC0 = 20;
const quint8 M10 = 24;
const quint8 M20 = 28;
const quint8 MM10 = 32;
const quint8 MM20 = 36;
const quint8 COL = 40;
const quint8 BAD = 42;
const quint8 NI = 44;
const quint8 SENDH = 48;
const quint8 SENDD = 50;
const quint8 REC0 = 52;
const quint8 REC1 = 54;
const quint8 REC2 = 56;

// various definitions
const quint8 EVENTBUFS = 2;

// poti names/values
const quint8 PULSPOT = 0;
const quint8 THRESHPOT = 1;

// data length
const quint8 EIGHTBIT = 0;
const quint8 TENBIT = 1;
 
// pulser positions
#if 0
const quint8 LEFT = 0;
const quint8 RIGHT = 1;
const quint8 MIDDLE = 2;
#endif
 
// command bytes for FPGA:
// SPODI type
const quint8 PERIPOT = 0;
const quint8 CENTPOT = 1;
const quint8 PULSPOS = 2;
const quint8 READTYPE = 3;
// SANS type
const quint8 SETXY = 1;
const quint8 SETXA = 2;
const quint8 SETYA = 3;
const quint8 SETDIAG = 4;
const quint8 RESETMCU = 5;
#if 0
const quint8 READID = 6;
#endif
const quint8 SIMSTART = 7;
const quint8 SIMSTOP = 8;
const quint8 MASKX0 = 16;
const quint8 MASKX1 = 17;
const quint8 MASKY0 = 18;
const quint8 MASKY1 = 19;
const quint8 POTIS = 24;
const quint8 AMPX = 32;
const quint8 AMPY = 33;
const quint8 AMPXY = 34;
const quint8 AMPEXT = 35;
const quint8 TEMP = 36;
const quint8 VOLT = 37;
const quint8 PULSON = 40;
const quint8 PULSOFF = 41;
const quint8 GAINMEAN = 42;
const quint8 GAINPOT = 43;
const quint8 INILEDON = 48;
const quint8 INILEDOFF = 49;
const quint8 TRIGAMP = 56;
const quint8 TRIGX = 57;
const quint8 TRIGY = 58;
 // MPET type
const quint8 PERIPOTA = 0;
const quint8 PERIPOTB = 1;
const quint8 PETCENTPOT = 2;

// operating modes for SANS  type
const quint8 XY = 0;
const quint8 XA = 1;
const quint8 YA = 2;
const quint8 DIAG = 3;

// trigger modes for SANS type
const quint8 TRIG_A = 0;
const quint8 TRIG_X = 1;
const quint8 TRIG_Y = 2;

// amplitide sources for SANS type
const quint8 AMP_X = 0;
const quint8 AMP_Y = 1;
const quint8 AMP_XY= 2;
const quint8 AMP_EXT = 3;

// display modes
const quint8 XYFLAT = 0;
const quint8 YXFLAT = 1;
const quint8 XAFLAT = 2;
const quint8 YAFLAT = 3;
const quint8 TFLAT = 4;
const quint8 XY2D = 5;
const quint8 XA2D = 6;
const quint8 YA2D = 7;
const quint8 XT2D = 8;
const quint8 YT2D = 9;

// error codes
const quint8 BX0 = 0x02;
const quint8 BX1 = 0x04;
const quint8 BY0 = 0x08;
const quint8 BY1 = 0x10;
const quint8 MX00 = 0x20;
const quint8 MX01 = 0x40;
const quint8 MX10 = 0x80;
const quint8 MX11 = 0x04;
const quint8 MY00 = 0x08;
const quint8 MY01 = 0x10;
const quint8 MY10 = 0x20;
const quint8 MY11 = 0x40;
const quint8 ERRX = 0x80;

// and error numbers (for histogram sorting)
const quint8 E_BX0 = 0;
const quint8 E_BX1 = 1;
const quint8 E_BY0 = 2;
const quint8 E_BY1 = 3;
const quint8 E_MX00 = 4;
const quint8 E_MX01 = 5;
const quint8 E_MX10 = 6;
const quint8 E_MX11 = 7;
const quint8 E_MY00 = 8;
const quint8 E_MY01 = 9;
const quint8 E_MY10 = 10;
const quint8 E_MY11 = 11;
const quint8 E_ERRX = 12;

// stop condition sources:
const quint8 CLEAR_COND = 0;
const quint8 MON1_COND = 1;
const quint8 MON2_COND = 2;
const quint8 TIMER_COND = 3;
const quint8 EVENT_COND = 4;

// several definitions
const quint8 MASKOFF = 99;
const quint8 MASKALL = 40;
const quint8 MASKTEST = 50;
const quint16 SYNCON = 0x0080;
const quint16 SYNCOFF = 0xFF7F;
const quint8 ALED = 0;
const quint8 YLED = 1;
const quint8 DAQLED = 2;
const quint8 FIFOLED = 3;
const float TEMPFACT = 0.186;
const float TEMPOFFS = 0;
const float VOLTFACT = 0.052;
const float VOLTOFFS = 0;

// conversion factor for MCPD timing:
const float TIMEFACT = 19531.25;

// file types
#if 0
const quint8 NOFILE = 0;
#endif
const quint8 HISTOGRAM = 1;
const quint8 LISTFILE = 2;

// additional SPODI CARESS interface codes
const quint8 PREPARE = 27;
const quint8 TERMINATE = 28;
const quint8 NEXTSTEP = 29;
const quint8 READPORT = 30;
const quint8 READHISTO = 31;
const quint8 STARTANGLE = 32;

#endif 
