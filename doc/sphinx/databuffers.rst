Data Buffers
============

During data acquisition, the MCPD modules transmit a continuous stream of event
buffers.

Each event buffer consists of a buffer header (21 x 16 bit = 42 Bytes) and
variable number of events. Each event has a length of 48 bits = 6 Bytes.

The total length of an event buffer varies between 42 Bytes (header only) up
to 1500 Bytes (limited by unfragmented Ethernet frame length).

.. _data_buffer_structure:

Buffer Structure:
-----------------
(in 16 bit words):

+---------------------------------+-------------------------+
| Buffer Length (in 16 bit words) |                  Word 0 |
+---------------------------------+-------------------------+
| Buffer Type                                               |
+-----------------------------------------------------------+
| Header Length (in 16 bit words)                           |
+-----------------------------------------------------------+
| Buffer Number                                             |
+-----------------------------------------------------------+
| Run ID                                                    |
+---------------------------------+-------------------------+
| MCPD-ID                         | Status                  |
+---------------------------------+-------------------------+
| Header Timestamp Lo                                       |
+-----------------------------------------------------------+
| Header Timestamp Mid                                      |
+---------------------------------+-------------------------+
| Header Timestamp Hi             | Word 8                  |
+---------------------------------+-------------------------+
| Parameter 0 Lo                                            |
+-----------------------------------------------------------+
| Parameter 0 Mid                                           |
+---------------------------------+-------------------------+
| Parameter 0 Hi                  | Word 11                 |
+---------------------------------+-------------------------+
| Parameter 1 Lo                                            |
+-----------------------------------------------------------+
| Parameter 1 Mid                                           |
+---------------------------------+-------------------------+
| Parameter 1 Hi                  | Word 14                 |
+---------------------------------+-------------------------+
| Parameter 2 Lo                                            |
+-----------------------------------------------------------+
| Parameter 2 Mid                                           |
+---------------------------------+-------------------------+
| Parameter 2 Hi                  | Word 17                 |
+---------------------------------+-------------------------+
| Parameter 3 Lo                                            |
+-----------------------------------------------------------+
| Parameter 3 Mid                                           |
+---------------------------------+-------------------------+
| Parameter 3 Hi                  | Word 20                 |
+---------------------------------+-------------------------+
| Event 0 Lo                      | Word 21                 |
+---------------------------------+-------------------------+
| Event 0 Mid                                               |
+-----------------------------------------------------------+
| Event 0 Hi                                                |
+-----------------------------------------------------------+
| ...                                                       |
+-----------------------------------------------------------+
| Event n Lo                                                |
+-----------------------------------------------------------+
| Event n Mid                                               |
+---------------------------------+-------------------------+
| Event n Hi                      | Word 21 + 3 * n         |
+---------------------------------+-------------------------+

                                (must be identical with buffer length - 1)

.. _header_data_dictionary:

Header data dictionary:
-----------------------

:Buffer Type:       16 bit type descriptor

                    Bits 0 … 14 carry a version information and may be left
                    blank in buffers sent by control pc.

                    Bit 15 = 0: data buffer

                    (*Bit 15 = 1:* *command buffer*)
:Header Length:     Length of header information in 16 bit words
:Buffer Number:     Simple 16 bit counter for data buffers. Incremented
                    automatically by MCPD-8 to allow loss monitoring.
                    Separate counters for data and cmd buffers.
:Buffer Length:     Total length in multiple of 16 bit words, stretching from 
                    “Buffer Type” to last data word.
:Run ID:            Simple 16 bit run counter, set by software, to allow for
                    integrity control.
:MCPD-ID:           8 bit ID of the sending MCPD module, to be assigned during
                    hardware initialisation
:Status:            8 bit wide bit field for sync and start/stop status of the
                    sending MCPD-8.

                    Currently only bits 0, 1 used:

                    bit 0: 1 = DAQ running, 0 = DAQ stopped

                    bit 1: 1 = sync OK, 0 = sync error
:Header Timestamp:  48 bits current status of the synchronized system timer
                    (100 ns timing resolution). Represents the value of the
                    system timer at the moment of the buffer opening. All events
                    in a buffer carry a positive 19 bit offset information 
                    relative to this header timestamp.
:Parameter 0..3:    MCPD-8 allows the transmission of selected counter/input
                    value with each buffer. So parameter 0..3 represent the 
                    values of the selected counters/inputs at the moment of the
                    buffer opening (t = header timestamp). Also the digital 
                    input status and ADC values can mapped here.
:Event 0..n:        (n + 1) * 48 bit event information
                    Event structure is explained in detail below.

.. _event_structure:

Event structure:
~~~~~~~~~~~~~~~~

Each event has a fixed 48 bit length. The content differs according to the event
ID.

:ID = 0: Neutron data event
:ID = 1: Trigger data event

Neutron data events (ID = 0):
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

+------------------+------------+----------+--------------+-------------+--------------+
| MSB              |            |          |              |             |         LSB  |
+------------------+------------+----------+--------------+-------------+--------------+
|ID (1 bit) = **0**|ModID(3 bit)|SlotID (5)|Amplitude (10)|Position (10)|Timestamp (19)|
+------------------+------------+----------+--------------+-------------+--------------+

:ID:            ID = 0 signalling a `neutron` event, resulting from a detector
                event at a peripheral module like MPSD-8.

                (Monitor counter events e.g., that of course also are `neutron`
                events are generated at the MCPD-8, don't carry a position
                information and are therefore regarded as `other events` in this
                context.

                1 bit

:ModID:         Module ID of the MPSD-8, determined by serial bus number (bus input
                at MCDP-8)

                3 bit

:SlotID:        channel (slot) number in the MPSD module:

                [0..7] for the MPSD-8 and MSTD-16

                5 bit (but the 2 MSB's not used, only [2..0] are valid!)

:Amplitude:     amplitude (energy) of the neutron event if protocol TPA is selected,
                otherwise = 0

                :MSPD8+:        10 valid bits
                :MPSD8:         8 valid bits (bits 0, 1 = 0!)
                :MSTD-16+:      9 valid bits [8..0]

                10 bit

:Position:      position of the neutron event

                10 bit

:Timestamp:     timing offset to the corresponding header timestamp

                event time = header timestamp + event timestamp

                19 bit

 
Address reconstruction:
"""""""""""""""""""""""

The complete, two dimensional address of a neutron event consists of max.
16 + 10 bit and is composed by the following partial informations:

**Channel (= individual detector tube):**

:MCPD-ID: MCPD-branch, if multiple              8 bit
:ModID:   Bus number on identified MCPD-8       3 bit
:SlotID:  Subchannel within identified MPSD-8   5 bit

+-------------+------------+------------+
| Bit 15 .. 8 | Bit 7 .. 5 | Bit 4 .. 0 |
+-------------+------------+------------+
| MCPD-ID     | ModID      | SlotID     |
+-------------+------------+------------+

A system using only MPSD-8 can reduce the address length further
        - ModID has only 3 valid bits
        - MCPD-ID normally doesn't use the full 8 bit range (but is due to
          users definition!)

So a reduced calculated address format for a system using 4 MCPD-8 and a total
maximum of 4 (MCPD-8) x 8 (MPSD-8) x 8 (Detectors/MPSD-8) = 256 detectors could
look like this:


+------------+------------+------------+
| Bit 7 .. 6 | Bit 5 .. 3 | Bit 2 .. 0 |
+------------+------------+------------+
| MCPD-ID    | ModID      | SlotID     |
+------------+------------+------------+


**Position (= event position on identified tube):**

:Data:  10 bit position data along detector tube

        No calculation needed (but possibly calibration against physical data ...)


Trigger events (ID = 1):
^^^^^^^^^^^^^^^^^^^^^^^^

Several trigger sources (counters, timers, digital inputs) can initiate a data
taking event. Triggerd by levels or defined overflows of the trigger sources,
data are taken from the data sources and written to an event structure. Possible
trigger and dats sources are timers, counters, and ADC values.


MSB

+------------------+-------------+----------+--------------+--------------+
|ID (1 bit) = **1**|TrigID(3 bit)|DataID (4)|Data (21 bit) |Timestamp (19)|
+------------------+-------------+----------+--------------+--------------+

:ID:    ID = 1, signalling a 'not neutron' event (= generated centrally in MCPD-8)
        possible trigger and data sources are:
        - Counter
        - Timers
        - Digital inputs

        1 bit

:TrigID: Trigger ID characterizing the event trigger source
         :1..4: Timer 1 .. 4
         :5,6:  rear TTL inputs 1,2
         :7:    compare register

         3 bit

:DataID: DataID characterizing the data source.

         Data taking was initiated by the trigger source indentified in TrigID,
         at the time ``header timestamp + event timestamp``.

         :0..3: Monitor/Chopper input 1..4
         :4,5:  rear TTL inputs 1,2
         :6,7:  ADC 1,2

         4 bit

:Data:  Counter, timer, or ADC value of the identified data source
        
        21 bit (depending on source not necessarily all valid)
        
:Timestamp:     timing offset to the corresponding header timestamp
               
                event time = header timestamp + event timestamp

                19 bit


