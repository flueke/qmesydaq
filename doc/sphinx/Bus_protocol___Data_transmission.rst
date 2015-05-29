Bus protocol / Data transmission
================================


Central modules (MCPD-8) and peripheral devices (MPSD-8, MSTD-16, MPSD-8+) are
connected via mesytecs highspeed serial interface. A general description is
given in chapter “System Layout”.

In MCPD and MCPD-2, the interface uses a bus topology while MCPD-8 offers a
point-to-point connection.

Communication on the mesytec serial bus is split into two domains:

Control and Data transmission.


Control communication
---------------------

(transmission of control commands and answers in dialog mode)

Control communication is used to set up gains and thresholds and to read out
device data. Command/answer transmission between MCPD and MPSD/MSTD is done in
a proprietary protocol which is common among all flavours of central and
peripheral devices. Thus all MCPD versions are principally able to talk to all
MPSD/MSTD versions so far.

Issuing a correct UDP command like described above leads to a control communication
between central and peripheral device via mesytec bus.



Data transmission
-----------------

(fast transmission of measurement data from peripherals to central module)

Throughout the generations of the mesytec neutron detector readout system, data
transmission on the mesytec bus was modified and extended to allow for increasing
communication capabilities.

There are mainly 3 versions of data transmission format:

**POS (P):**

| Only Position or Amplitude information is transmitted, P/A has to be selected by
  mode command.
| This is the “legacy” transmission mode used by the first generation of MCPD and
  MPSD / MSTD. (In MSTD-16: always transmission of the amplitude information.)

**TOF-POS (TP):**

Position or Amplitude are transmitted together with an offset timestamp. P/A has
to be selected by mode command.

This is the transmission mode for the intermediate MPSD-8+ modules, which are
already capable generating the 100 ns precision timestamp.

**TOF-POS-AMP (TPA):**

| Position and Amplitude are transmitted simultaneously together with the offset
  timestamp.
| This is the transmission mode of the most recent MPSD-8+ modules.


**All three transmission modes only refer to the fast data transmission on the
mesytec bus!**

Ethernet data format (and of course command communication) is not influenced by
the type of bus communication!

**Capabilities of different peripheral modules:**

All peripheral modules with ID numbers < 105 are only capable of transmitting in
POS mode.

Modules with ID 105 and higher may be able to transmit also in higher modes (TP,
TPA) – they present a hardware register showing their transmit capabilities and
a register to set the desired transmit mode.

Capabilities are coded/set in an eight bit value like follows:

(Register 0)

+---+---+---+---+---+-----+----+---+
| 7 | 6 | 5 | 5 | 3 | 2   | 1  | 0 |
+---+---+---+---+---+-----+----+---+
| x | x | x | x | x | TPA | TP | P |
+---+---+---+---+---+-----+----+---+

Devices with ID 105 and higher are at least capable transmitting in TP mode
(corresponding with a register value of 3), devices with simultaneus calculationi
of position and amplitude present a register value of 7.

| The choice of the transmission mode influences the maximum continuous bus rate:
| In TP mode, a max. continuous bus rate of 800 kHz is expected, in TPA mode –
  due to the additional amplitude data – the max. continuous bus rate is 500 kHz.

**Capabilities of different MCPD-8 versions:**

As there are two versions of MCPD-8 with different communication capabilities.

There are also registers in MCPD-8 indicating / selecting the possible / desired
communication modes. The bitpattern is like described above for the MPSD-8+:

+---+---+---+---+---+-----+----+---+
| 7 | 6 | 5 | 5 | 3 | 2   | 1  | 0 |
+---+---+---+---+---+-----+----+---+
| x | x | x | x | x | TPA | TP | P |
+---+---+---+---+---+-----+----+---+

Bus communication mode setup
----------------------------

| By default / after power up, all devices are in the maximum possible transfer
  mode.
| After power up / reset, MCPD-8 scans all connected peripherals for their tx
  capabilities and sets the according tx mode.
| These defaults can be overwritten by software commands:
| The registers indicating and controlling the bus protocol can be read and set
  via MCPD-8 using the following commands


**MPSD-8:**

| Read peripheral register (Cmd ID = 52)
| Write peripheral register (Cmd ID = 53)

Read TX capabilities of connected MPSD-8:

**Cmd=52 Read Peripheral Register**

.. table::
    :name: Read Peripheral Register

    ======== =====================================
    **Word** **Contents**
    ======== =====================================
    10       MPSD device number (0 … 7)
    11       Register number (0 for tx capabilities)
    12       0xFFFF
    ======== =====================================

The answer packet will look like follows:

**Cmd=52 Read Peripheral Register (Answer)**

.. table::

    ======== =====================================
    **Word** **Contents**
    ======== =====================================
    10       MPSD device number (0 … 7)
    11       Register number (= 0 for tx capabilities)
    12       Capability bitmap
    13       0xFFFF
    ======== =====================================

Set desired TX mode:

**Cmd=53 Write Peripheral Register**

.. table::
    :name: Write Peripheral Register

    ======== =====================================
    **Word** **Contents**
    ======== =====================================
    10       MPSD device number (0 … 7)
    11       Register number (= 1 for tx capabilities)
    12       TX mode (1 = P, 2 = TP, 4 = TPA)
    13       0xFFFF
    ======== =====================================

**MCPD-8:**

| Read Register (Cmd ID = 32)
| Write Register (Cmd ID = 31)

The read and write register commands for MCPD-8 are slightly different to those
of MPSD-8.

In MCPD-8 it is possible to read / write a block of registers. Thus the
read/write commands are extended by a length parameter.

**Cmd=32 Read Register**

.. table::
    :name: Read Register

    ======== =====================================
    **Word** **Contents**
    ======== =====================================
    10       Number of Registers to read (=1)
    11       Register address (102 for tx capabilities)
    12       Capability bitmap
    13       0xFFFF
    ======== =====================================

**Cmd=31 Write Register**

.. table::
    :name: Write Register

    ======== =====================================
    **Word** **Contents**
    ======== =====================================
    10       Number of Registers to write (=1)
    11       Register address (103 for tx mode)
    12       TX mode (1 = P, 2 = TP, 4 = TPA)
    13       0xFFFF
    ======== =====================================


Examples for bus setups
-----------------------

*   | MCPD-8 and MPSD-8 („SPODI Type“):
    | MPSD-ID is 1.
    | Only possible protocol is „P“ (Position / Amplitude only).
    | MPSD-8 („SPODI-Type“) is in „P“ mode by default.
    | MCPD-8 has to be set to mode „P“ (Value 1 in register 103)

*   | MCPD-8 and MSTD-16:
    | MSTD-ID is 104.
    | Only possible protocol is „P“ (Position / Amplitude only).
    | MSTD-16 is in „P“ mode by default.
    | MCPD-8 has to be set to mode „P“ (Value 1 in register 103)

*   | MCPD-8 and MPSD-8+ („DNS Type“):
    | MPSD-ID is 103.
    | Possible protocols are „P“ (Position / Amplitude only) and „TP“ (TOF timing plus Position / Amplitude)
    | MPSD-8 („DNS-Type“) is in „TP“ mode by default.
    | MCPD-8 has to be set to mode „TP“ (Value 2 in register 103)

*   | MCPD-8 and MPSD-8+ („SANS Type“):
    | MPSD-ID is 105.
    | Possible protocols are „P“ (Position / Amplitude only), „TP“ (TOF timing plus Position / Amplitude)
      and „TPA“ (TOF timing plus Postion plus Amplitude)
    | MPSD-8 („SANS-Type“) is in „TPA“ mode by default.
    | MCPD-8 can be left unchanged.


Both devices have to be set to mode „TP“ (Value 2 in register 103) if for reasons
of highest data rate capabilities only one of the parameters Amplitude / Position
shall be transmitted.
