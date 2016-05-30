.. |mus|  unicode:: U+003BC s

Command Reference
=================

General Command Format:
-----------------------

The structure of a command buffer is displayed here again. It’s layout is
identical for all commands.

+---------------------------------+--------------------------+
| Buffer Type                     | Word 0                   |
+---------------------------------+--------------------------+
| Header Length (in 16 bit words)                            |
+------------------------------------------------------------+
| Buffer Number                                              |
+------------------------------------------------------------+
| Buffer Length (in 16 bit words)                            |
+------------------------------------------------------------+
| Cmd                                                        |
+---------------------------------+--------------------------+
| MCPD-ID                         | Status                   |
+---------------------------------+--------------------------+
| Header Timestamp Lo                                        |
+------------------------------------------------------------+
| Header Timestamp Mid                                       |
+---------------------------------+--------------------------+
| Header Timestamp Hi             |                          |
+---------------------------------+--------------------------+
| Command Checksum                | Word 9                   |
+---------------------------------+--------------------------+
| Data 0                          | Word 10                  |
+---------------------------------+--------------------------+
| Data 1                                                     |
+------------------------------------------------------------+
| Data 2                                                     |
+------------------------------------------------------------+
| ...                                                        |
+------------------------------------------------------------+
| Trailing data                                              |
+------------------------------------------------------------+
| with                                                       |
+---------------------------------+--------------------------+
| Variable length                 | Word (buffer length – 1) |
+---------------------------------+--------------------------+

In the following, only data from Word 10 on (Data 0) are displayed. The given
command number has to be entered in the header field “Cmd” (Word 4)

Each command buffer has a trailing 0xFFFF as last word.

**Command answer:**

Every command buffer will be answered by MCPD-8. Set values instead of requested
values will be inserted into the appropriate fields. If a command fails in the
MCPD-8, the Cmd number will *have bit 15 set*.

Command Set MCPD-8:
-------------------

Command List (Numerical Order):
-------------------------------

========= ========================================
**Cmd #** **Command**
========= ========================================
0         :ref:`Reset <cmd0>`
1         :ref:`Start DAQ <cmd1>`
2         :ref:`Stop DAQ <cmd2>`
3         :ref:`Continue DAQ <cmd3>`
4         :ref:`Set MCPD ID# <cmd4>`
5         :ref:`Set Protocol Parameters <cmd5>`
6         :ref:`Set MCPD-8 Timing Setup <cmd6>`
7         :ref:`Set Master Clock <cmd7>`
8         :ref:`Set Run ID <cmd8>`
9         :ref:`Set Counter / ADC cell <cmd9>`
10        :ref:`Set Auxiliary Timer <cmd10>`
11        :ref:`Set Parameter Source <cmd11>`
12        :ref:`Get Parameters <cmd12>`
13        :ref:`Set MPSD-8 Gain <cmd13>`
14        :ref:`Set MPSD-8 Threshold <cmd14>`
15        :ref:`Set MPSD-8 Pulser <cmd15>`
16        :ref:`Set MPSD-8 Mode <cmd16>`
17        :ref:`Set MCPD-8 DAC <cmd17>`
18        :ref:`Send MCPD-8 Serial String <cmd18>`
19        :ref:`Read MCPD-8 Serial String <cmd19>`
21        :ref:`Set MCPD-8 TTL Outputs <cmd21>`
22        :ref:`Get MCPD-8 fast bus capabilities <cmd22>`
23        :ref:`Set MCPD-8 fast bus capabilities <cmd23>`
24        :ref:`Read MPSD-8+ parameters <cmd24>`
25        :ref:`Set MPSD-8+ fast tx protocol <cmd25>`
51        :ref:`Retrieve MCPD-8 version information <cmd51>`
========= ========================================

Command List (Functional Order):
--------------------------------

**Communication Settings:**

========= ========================================
**Cmd #** **Command**
========= ========================================
4         :ref:`Set MCPD ID# <cmd4>`
5         :ref:`Set Protocol Parameters <cmd5>`
22        :ref:`Get MCPD-8 fast bus capabilities <cmd22>`
23        :ref:`Set MCPD-8 fast bus Capabilities <cmd23>`
========= ========================================

**General MCPD-8 Settings:**

========= ========================================
**Cmd #** **Command**
========= ========================================
6         :ref:`Set MCPD-8 Timing Setup <cmd6>`
7         :ref:`Set Master Clock <cmd7>`
8         :ref:`Set Run ID <cmd8>`
9         :ref:`Set Counter / ADC cell <cmd9>`
10        :ref:`Set Auxiliary Timer <cmd10>`
11        :ref:`Set Parameter Source <cmd11>`
12        :ref:`Get Parameters <cmd12>`
22        :ref:`Get MCPD-8 fast bus capabilities <cmd22>`
23        :ref:`Set MCPD-8 fast bus Capabilities <cmd23>`
51        :ref:`Retrieve MCPD-8 version information <cmd51>`
========= ========================================

**MPSD-8 Settings:**

========= ========================================
**Cmd #** **Command**
========= ========================================
13        :ref:`Set MPSD-8 Gain <cmd13>`
14        :ref:`Set MPSD-8 Threshold <cmd14>`
15        :ref:`Set MPSD-8 Pulser <cmd15>`
16        :ref:`Set MPSD-8 Mode <cmd16>`
24        :ref:`Read MPSD-8+ parameters <cmd24>`
25        :ref:`Set MPSD-8+ fast tx protocol <cmd25>`
========= ========================================

**DAQ Commands:**

========= ========================================
**Cmd #** **Command**
========= ========================================
0         :ref:`Reset <cmd0>`
1         :ref:`Start DAQ <cmd1>`
2         :ref:`Stop DAQ <cmd2>`
3         :ref:`Continue DAQ <cmd3>`
========= ========================================

**MCPD-8 Port Commands:**

========= ========================================
**Cmd #** **Command**
========= ========================================
17        :ref:`Set MCPD-8 DAC <cmd17>`
18        :ref:`Send MCPD-8 Serial String <cmd18>`
19        :ref:`Read MCPD-8 Serial String <cmd19>`
21        :ref:`Set MCPD-8 TTL Outputs <cmd21>`
========= ========================================

Command Descriptions:
---------------------

Communication Settings:
-----------------------

**Cmd=4 Set MCPD ID#**

.. table::
    :name: cmd4

    ======== =========================================
    **Word** **Contents**
    ======== =========================================
    10       ID (0 … 255)
    11       0xFFFF
    ======== =========================================

Each MCPD in a setup is given an individual ID number (8 bit). The ID is part of
the header of every data / cmd packet the MCPD emits. Thus data can be assigned
to a defined part of the readout system during data processing.

It is in the responsibility of the user (= frontend programmer) to keep IDs unique
throughout the readout system.

Answer buffer:

**Cmd=4 Set MCPD ID# (Answer)**

.. table::

    ======== =========================================
    **Word** **Contents**
    ======== =========================================
    10       ID
    11       0xFFFF
    ======== =========================================

-----

**Cmd=5 Set Protocol Parameters**

.. table::
    :name: cmd5

    ======== =========================================
    **Word** **Contents**
    ======== =========================================
    10       MCPD ip 0 (e.g. 192)
    11       MCPD ip 1 (e.g. 168)
    12       MCPD ip 2 (e.g. 168)
    13       MCPD ip 3 (e.g. 121)
    14       Data sink ip 0
    15       Data sink ip 1
    16       Data sink ip 2
    17       Data sink ip 3
    18       Cmd UPD Port
    19       Data UPD Port
    20       Cmd pc ip 0
    21       Cmd pc ip 1
    22       Cmd pc ip 2
    23       Cmd pc ip 3
    24       0xFFFF
    ======== =========================================

**MCPD ip:**

The IP address of the MCPD can be remotely changed. A hardware button on the CPU
module of the MCPD allows a reset to the factory address 192.168.168.121.
(This address is also mentioned as an example above to explain byte order)

MCPD ip address will not be modified if MCPD ip 0 (Word 10) is set to zero.

**Data sink ip:**

Also the destination ip address for data packages can be set individually. (If
no address is set: the address of the cmd pc is used automatically)

Address will not be modified if Data sink ip 0 (Word 14) is set to zero.

If ip0 as well as ip1 are set to 0, the address of the pc sending this command
will be used automatically from out of the ip protocol. This allows to set the
address to the sending pc without knowing its address explicitly.

**Cmd pc ip:**

This allows to set a defined address for the pc that will send the cmds. No other
pc will then be able to take control over the system unless the new address is
published by the current cmd pc.

Address will not be modified if Cmd pc ip 0 (Word 20) is set to zero.

If Cmd pc ip0 as well as Cmd Pc ip1 are set to 0, the address of the pc sending
this command will be used automatically from out of the ip protocol. This allows
to set the address to the sending pc without knowing its address explicitly.

**UDP ports**

MCPD-8 is able to use individually set UPD port numbers, possibly different ones
for cmd and data. No change if fields are set to zero.

The following table gives an overview of the possible settings:

+---------------+-----------+-------------------------------------------------------------------------------+
| **Field(s)**  | **Value** | **Meaning**                                                                   |
+---------------+-----------+-------------------------------------------------------+--+--+--+--+--+--+--+--+
| MCPD ip0      | 0         | Do not change MCPD ip address                         |  |  |  |  |  |  |  |  |
+---------------+-----------+-------------------------------------------------------+--+--+--+--+--+--+--+--+
|               | > 0       | Set MCPD ip address to values in word 10 ...          |  |  |  |  |  |  |  |  |
|               |           | 13                                                    |  |  |  |  |  |  |  |  |
+---------------+-----------+-------------------------------------------------------+--+--+--+--+--+--+--+--+
|               |           |                                                       |  |  |  |  |  |  |  |  |
+---------------+-----------+-------------------------------------------------------+--+--+--+--+--+--+--+--+
| Data sink ip0 | 0         | Do not change Data sink ip address                    |  |  |  |  |  |  |  |  |
+---------------+-----------+-------------------------------------------------------+--+--+--+--+--+--+--+--+
|               | > 0       | Set data sink ip address to values in word 14 ... 17  |  |  |  |  |  |  |  |  |
+---------------+-----------+-------------------------------------------------------+--+--+--+--+--+--+--+--+
| Data sink ip0 | 0         | Set data sink ip address to address of cmd sending pc |  |  |  |  |  |  |  |  |
| *and*         |           |                                                       |  |  |  |  |  |  |  |  |
| Data sink ip1 |           |                                                       |  |  |  |  |  |  |  |  |
+---------------+-----------+-------------------------------------------------------+--+--+--+--+--+--+--+--+
|               |           |                                                       |  |  |  |  |  |  |  |  |
+---------------+-----------+-------------------------------------------------------+--+--+--+--+--+--+--+--+
| Cmd pc        | 0         | Do not change Data sink ip address                    |  |  |  |  |  |  |  |  |
| ip0           |           |                                                       |  |  |  |  |  |  |  |  |
+---------------+-----------+-------------------------------------------------------+--+--+--+--+--+--+--+--+
|               | > 0       | Set cmd pc ip address to values in word 20 ... 23     |  |  |  |  |  |  |  |  |
+---------------+-----------+-------------------------------------------------------+--+--+--+--+--+--+--+--+
| Cmd pc ip0    | 0         | Set cmd pc ip address to address of cmd sending pc    |  |  |  |  |  |  |  |  |
| *and*         |           |                                                       |  |  |  |  |  |  |  |  |
| Cmd pc ip1    |           |                                                       |  |  |  |  |  |  |  |  |
+---------------+-----------+-------------------------------------------------------+--+--+--+--+--+--+--+--+
|               |           |                                                       |  |  |  |  |  |  |  |  |
+---------------+-----------+-------------------------------------------------------+--+--+--+--+--+--+--+--+
| Udp port      | 0         | Do not modify                                         |  |  |  |  |  |  |  |  |
+---------------+-----------+-------------------------------------------------------+--+--+--+--+--+--+--+--+
|               | > 0       | Set to given value                                    |  |  |  |  |  |  |  |  |
+---------------+-----------+-------------------------------------------------------+--+--+--+--+--+--+--+--+

-----

**Cmd=22 Get MCPD-8 tx capabilites**

.. table::
    :name: cmd22

    ======== =========================================
    **Word** **Contents**
    ======== =========================================
    10       0xFFFF
    ======== =========================================

Reads possible eventbus data transmission formats of MCPD-8:

Answer buffer:

**Cmd=22 Get MCPD-8 tx capabilities (Answer)**

.. table::

    ======== =========================================
    **Word** **Contents**
    ======== =========================================
    10       Capabilites bitmap
    11       Current setting
    12       0xFFFF
    ======== =========================================

With bitmap:

+---+---+---+---+---+-------------+---------------+---------+
| 7 | 6 | 5 | 4 | 3 | 2           | 1             | 0       |
+---+---+---+---+---+-------------+---------------+---------+
| x | x | x | x | x | TOF+POS+AMP | TOF + Pos/Amp | Pos/Amp |
+---+---+---+---+---+-------------+---------------+---------+

Current setting = one of the possibilites selected:

  | 1 = Pos/Amp
  | 2 = TOF + Pos/Amp
  | 4 = TOF + Pos + Amp

-----

**Cmd=23 Set MCPD-8 tx capabilites**

.. table::
    :name: cmd23

    ======== =========================================
    **Word** **Contents**
    ======== =========================================
    10       Fast TX format
    11       0xFFFF
    ======== =========================================

Sets eventbus fast TX format for all MCPD-8 buses to:

  | 1 = Position / Amplitude
  | 2 = TOF + Position / Amplitude
  | 4 = TOF + Position + Amplitude

Answer buffer:

**Cmd=23 Set MCPD-8 tx capabilities (Answer)**

.. table::

    ======== =========================================
    **Word** **Contents**
    ======== =========================================
    10       Current setting
    11       0xFFFF
    ======== =========================================

General settings:
-----------------

**Cmd=6 Set MCPD-8 timing setup**

.. table::
    :name: cmd6

    ======== =========================================
    **Word** **Contents**
    ======== =========================================
    10       Timing / Sync master \
              | (0: MCPD is slave, \
              | 1: MCPD is master)
    11       Sync bus termination (0 = on, 1 = off)
    12       0xFFFF
    ======== =========================================

Sets timing properties:

- Please make sure that only one MCPD-8 is set as sync master!
- Sync bus has to be terminated at both ends – master is terminated automatically,
  last slave on bus has to be terminated.

-----

**Cmd=7 Set MCPD-8 master clock**

.. table::
    :name: cmd7

    ======== =========================================
    **Word** **Contents**
    ======== =========================================
    10       Master clock, bits 0 … 15
    11       Master clock, bits 16 … 31
    12       Master clock, bits 32 …47
    13       0xFFFF
    ======== =========================================

Master clock can be set to any value if desired. Normally, a reset is initiated
before a new run and all counters are set to zero during this reset automatically.

Only if another run start time than zero is desired, this registers must be set.

-----

**Cmd=8 Set Run Id**

.. table::
    :name: cmd8

    ======== =========================================
    **Word** **Contents**
    ======== =========================================
    10       Run Id
    11       0xFFFF
    ======== =========================================

Set value for the header field “Run ID” can be set to any desired value.

The master MCPD-8 distributes its Run ID over the sync bus. Thus it’s only
necessary to set the Run Id at the master module.

Counter, ADC, Timer and Parameter settings:
-------------------------------------------

**Cmd=9 Set Counter / ADC Cell**

.. table::
    :name: cmd9

    ======== =========================================
    **Word** **Contents**
    ======== =========================================
    10       Cell #: \
              | 0 … 3: monitor / chopper inputs 1…4 \
              | 4, 5: dig. backpanel inputs 1, 2 \
              | 6, 7: ADC 1, 2
    11       Trigger source: \
              | 0 = no trigger \
              | 1 … 4: trigger on aux timer 1… 4 \
              | 5, 6: trigger on rising edge at rear input 1, 2 \
              | 7: trigger from compare register \
              | (7 only for counter cells)
    12       Compare register, numerical value n:\
              | 0 … 20: trigger on bit n = 1 \
              | 21: trigger on counter overflow \
              | 22: trigger on rising edge of input \
              | (can be left blank for ADC cells)
    13       0xFFFF
    ======== =========================================


This command configures the given counter cell:

One of six possible counter cells and two possible ADC cells is addressed. The
value of the according 21 bit counter is transmitted as a trigger event when
triggered.

Trigger source can be one of the digital inputs, one of the four auxiliary timers
or a special compare register.

Please note that the compare register does not do a full compare, but checks for
a ‘1’ at the given bit position, allowing for triggers at multiples of 2.

Counter cells are intended to generate repeated trigger events. They can be used
e.g. for a continuous monitoring of counter values and ADC inputs.

Choosing the rising signal edge as trigger source enables to generate a (fully
timestamped) event e.g. for each chopper signal and allows precise chopper timing
calculation.

-----

**Cmd=10 Set Auxiliary Timer**

.. table::
    :name: cmd10

    ======== =========================================
    **Word** **Contents**
    ======== =========================================
    10       Timer #: (0 … 3)
    11       Capture register: (0 … 65.536) Time base \
             is 10 |mus|, allowing for intervals from \
             10 |mus| to 655,36 ms
    12       0xFFFF
    ======== =========================================

Auxiliary timer compare register is set to the given value.

An identical compare generates a trigger signal (that might be used in one of the
counter / ADC cells) and resets the counter to zero. Thus four independent triggers
with periods between 10 |mus| and 655,36 ms are possible.

-----

**Cmd=11 Set Parameter Source**

.. table::
    :name: cmd11

    ======== =========================================
    **Word** **Contents**
    ======== =========================================
    10       Parameter: (0 … 3)
    11       Source: \
              | 0 … 3: Monitor/Chopper inputs 1…4 \
              | 4, 5: backpanel TTL inputs 1, 2 \
              | 6: combination of all digital inputs, \
                and both ADC values \
              | 7: event counter \
              | 8: master clock
    12       0xFFFF
    ======== =========================================

Defines the counter source for the given parameter.

While 0 … 5 are real counters, 6 delivers a combination of the current status
of all defined inputs and 7, 8 get copies of the current value of event counter
or master clock.

All four Parameter values are transmitted with every data buffer, delivering a
continuous monitoring information.

-----

**Cmd=12 Get All Parameters**

.. table::
    :name: cmd12

    ======== =========================================
    **Word** **Contents**
    ======== =========================================
    10       0xFFFF
    ======== =========================================

Requests all available parameter information.

Answer buffer:

**Cmd=12 Get all Parameters (Answer)**

.. table::

    ======== =========================================
    **Word** **Contents**
    ======== =========================================
    10       ADC 1 (12 valid bits)
    11       ADC 2 (12 valid bits)
    12       DAC 1 (12 bits)
    13       DAC 2 (12 bits)
    14       TTL outputs (2 bits)
    15       TTL inputs (6 bits)
    16       Event counter Lo
    17       Event counter Mid
    18       Event counter Hi
    19       Parameter 0 Lo
    20       Parameter 0 Mid
    21       Parameter 0 Hi
    22       Parameter 1 Lo
    23       Parameter 1 Mid
    24       Parameter 1 Hi
    25       Parameter 2 Lo
    26       Parameter 2 Mid
    27       Parameter 2 Hi
    28       Parameter 3 Lo
    29       Parameter 3 Mid
    30       Parameter 3 Hi
    31       0xFFFF
    ======== =========================================

Gathers the given information.

MCPD-8 port commands
--------------------

**Cmd=17 Set MCPD-8 DAC**

.. table::
    :name: cmd17

    ======== =========================================
    **Word** **Contents**
    ======== =========================================
    10       DAC 0 (12 valid bits)
    11       DAC 1 (12 valid bits)
    12       0xFFFF
    ======== =========================================

| MCPD-8 offers two DAC ports that can be set in a 12 bit range.
| Full range output voltage is +/-3 V or 0…6 V, according to jumper setting
  in MCPD-8.

-----

**Cmd=18 Send MCPD-8 serial string**

.. table::
    :name: cmd18

    ============ =========================================
    **Word**     **Contents**
    ============ =========================================
    10           Total length of string to send \
                 (including carriage return, linefeed, …)
    11           First character to send
    10 + len     Last character to send
    10 + len + 1 0xFFFF
    ============ =========================================

| MCPD-8 offers a serial RS-232 interface that can be used to control other devices.
| Port settings are 9600 Bd, 8N1 by default.
| The given string is sent to the serial interface. Answers are collected in a
  buffer that can be retrieved with the following command.

-----

**Cmd=19 Read MCPD-8 serial string**

.. table::
    :name: cmd19

    ======== =========================================
    **Word** **Contents**
    ======== =========================================
    10       0xFFFF
    ======== =========================================

Requests a readout of the serial buffer.

Answer buffers look like follows:

**Cmd=19 Read MCPD-8 serial string (Answer)**

.. table::

    ============ =========================================
    **Word**     **Contents**
    ============ =========================================
    10           Total length of string to send \
                 (including carriage return, linefeed, …)
    11           First character in serial buffer
    10 + len     Last character in serial buffer
    10 + len + 1 0xFFFF
    ============ =========================================

Returns version information of MCPD-8 microcontroller and FPGA firmware.

-----

**Cmd=21 Set TTL Output**

.. table::
    :name: cmd21

    ======== =========================================
    **Word** **Contents**
    ======== =========================================
    10       bit 0 TTL port 1, bit 1 TTL port 2
    11       0xFFFF
    ======== =========================================

MCPD-8 offers two TTL output ports that can be set.

-----

**Cmd=51 Read MCPD-8 software versions**

.. table::
    :name: cmd51

    ======== =========================================
    **Word** **Contents**
    ======== =========================================
    10       0xFFFF
    ======== =========================================

Answer buffers look like follows:

**Cmd=51 Read MCPD-8 version status (Answer)**

.. table::

    ======== =========================================
    **Word** **Contents**
    ======== =========================================
    10       Major CPU software version
    11       Minor CPU software version
    12       Maj. FPGA ver.  | Min. FPGA ver.
    13       0xFFFF
    ======== =========================================

MPSD-8 commands
---------------

**Cmd=13 Set MPSD Gain**

.. table::
    :name: cmd13

    ======== =========================================
    **Word** **Contents**
    ======== =========================================
    10       MPSD device number (0 … 7)
    11       Channel within MPSD (0 … 7, 8 = all)
    12       Gain value (0 … 255)
    13       0xFFFF
    ======== =========================================

Each channel gain can be set individually. To facilitate a quick setup, using
channel number 8 will write the same gain value to all channels of the addressed
MPSD-8 module.

-----

**Cmd=14 Set MPSD Threshold**

.. table::
    :name: cmd14

    ======== =========================================
    **Word** **Contents**
    ======== =========================================
    10       MPSD device number (0 … 7)
    11       Threshold value (0 … 255)
    12       0xFFFF
    ======== =========================================

Each peripheral module MPSD-8 has one common lower threshold for its window
discriminator. An 8 bit value is used to set the lower discriminator threshold.

-----

**Cmd=15 Set MPSD Pulser**

.. table::
    :name: cmd15

    ======== =========================================
    **Word** **Contents**
    ======== =========================================
    10       MPSD device number (0 … 7)
    11       Channel within MPSD (0 … 7)
    12       Position within channel \
             (0 = left, 1 = right, 2 = middle)
    13       Pulser amplitude (0 … 255)
    14       Pulser on/off (0 = off, 1 = on)
    15       0xFFFF
    ======== =========================================


A builtin test pulser is useful to check electronics performance without the
need of “real” neutron events.

The pulser can be set to 3 positions (left, middle, right) in a psd channel.
Furthermore, the pulser amplitude can be controlled and pulser function can
be switched on/off.

.. warning:: Be sure to switch all pulsers off before starting neutron recording!

-----

**Cmd=16 Set MPSD-8 Mode**

.. table::
    :name: cmd16

    ======== ==========================================
    **Word** **Contents**
    ======== ==========================================
    10       MPSD device (=bus) number (0 … 7, 8 = all)
    11       Mode (0 = position, 1 = amplitude)
    12       0xFFFF
    ======== ==========================================

MPSD-8 can be run in two modes:

- Position mode transmits a 10 bit position information.

- Amplitude (Energy) mode transmits a 10 bit signal amplitude information.

New versions of MPSD-8+ are capable transmitting position and amplitude date
simultaneously.

-----

**Cmd=24 Get MPSD Parameters**

.. table::
    :name: cmd24

    ======== ==========================================
    **Word** **Contents**
    ======== ==========================================
    10       MPSD device number (0…7)
    11       0xFFFF
    ======== ==========================================

Retrieves contents of MPSD-8 parameter registers.

Answer buffer:

**Cmd=24 Get MPSD Parameters (Answer)**

.. table::

    ======== ==========================================
    **Word** **Contents**
    ======== ==========================================
    10       MPSD device number (0…7)
    11       Eventbus transmit capabilities
    12       Current eventbus fast tx format setting
    13       Firmware revision
    14       0xFFF
    ======== ==========================================

**Cmd=25 Set MPSD-8+ fast tx protocol**

.. table::
    :name: cmd25

    ======== ==========================================
    **Word** **Contents**
    ======== ==========================================
    ======== ==========================================

.. warning:: Missing documentation

DAQ commands
------------

**Cmd=1 Start DAQ**

.. table::
    :name: cmd1

    ======== ==========================================
    **Word** **Contents**
    ======== ==========================================
    10       0xFFFF
    ======== ==========================================

| Start DAQ starts the data acquisition system.
| All timers (master timer + auxiliary timers) start / continue running.
| Neutron and trigger events will be filled into data buffers.
| Start signal is propagated over the sync line. Thus it is not necessary to send
  a start signal to each individual MCPD-8.
| MCPD-8 not set as master will refuse command.

-----

**Cmd=2 Stop DAQ**

.. table::
    :name: cmd2

    ======== ==========================================
    **Word** **Contents**
    ======== ==========================================
    10       0xFFFF
    ======== ==========================================

| Stop DAQ stops the data acquisition system.
| All timers (master timer + auxiliary timers) stop running.
| Stop signal is propagated over the sync line. Thus it is not necessary to send a
  stop signal to each individual MCPD-8.
| MCPD-8 not set as master will refuse command.

-----

**Cmd=3 Continue DAQ**

.. table::
    :name: cmd3

    ======== ==========================================
    **Word** **Contents**
    ======== ==========================================
    10       0xFFFF
    ======== ==========================================

| Continue DAQ restarts the data acquisition system.
| All timers (master timer + auxiliary timers) will continue running.
| Continue signal is propagated over the sync line. Thus it is not necessary to
  send a continue signal to each individual MCPD-8.
| MCPD-8 not set as master will refuse command.

-----

**Cmd=0 Reset**

.. table::
    :name: cmd0

    ======== ==========================================
    **Word** **Contents**
    ======== ==========================================
    10       0xFFFF
    ======== ==========================================

| Running DAQ will be stopped
| All counters and timers will be reset to 0.
| Reset signal is propagated over the sync line. Thus it is not necessary to send
  a reset signal to each individual MCPD-8.
| MCPD-8 not set as master will refuse command.
