Command Buffers
===============

Each command buffer consists of a buffer header (10 x 16 bit = 20 bytes)
and a trailing data block of variable length. The contents of the data
block depends on the individual commands.

The total length of a command buffer varies between 20 bytes (header only)
up to 1.500 bytes (limited by unfragmented Ethernet frame length)

(Padding bytes to keep Ethernet minimum buffer sizes must be added).


Buffer Structure:
-----------------
(in 16 bit words):


+---------------------------------+-------------------------+
| Buffer Length (in 16 bit words) | Word 0                  |
+---------------------------------+-------------------------+
| Buffer Type                                               |
+-----------------------------------------------------------+
| Header Length (in 16 bit words)                           |
+-----------------------------------------------------------+
| Buffer Number                                             |
+-----------------------------------------------------------+
| Cmd                                                       |
+---------------------------------+-------------------------+
| MCPD-ID                         | Status                  |
+---------------------------------+-------------------------+
| Header Timestamp Lo                                       |
+-----------------------------------------------------------+
| Header Timestamp Mid                                      |
+---------------------------------+-------------------------+
| Header Timestamp Hi             | Word 8                  |
+---------------------------------+-------------------------+
| Command Checksum                | Word 9                  |
+---------------------------------+-------------------------+
| Data 0                          | Word 10                 |
+---------------------------------+-------------------------+
| Data 1                                                    |
+-----------------------------------------------------------+
| Data 2                                                    |
+-----------------------------------------------------------+
| ...                                                       |
+-----------------------------------------------------------+
| Trailing data                                             |
+-----------------------------------------------------------+
| with                                                      |
+---------------------------------+-------------------------+
| Variable length                 | Word (buffer length –1) |
+---------------------------------+-------------------------+


Header data dictionary:
-----------------------

Use and meaning of header data varies depending on sender. While MCPD-8 fills
in all data, some of them may be left uninitialized when sending a data block
from a control pc to a MCPD-8:

:Buffer Type:       16 bit type descriptor

                    Bits 0 … 14 carry a version information and may be left
                    blank in buffers sent by control pc.

                    Bit 15 = 0: data buffer

                    *Bit 15 = 1:* *command buffer*
:Header Length:     Length of header information in 16 bit words
:Buffer Number:     Simple 16 bit counter to allow loss monitoring.
                    Separate counters for data and cmd buffers.
                    A control software could increment with each cmd issued.
                    MCPD-8 will increment its own counter with each cmd answered.
:Buffer Length:     In multiple of 16 bit words.
                    Spans from “Buffer Type” to last data word. Only counts
                    useful data words.
                    Padding bytes added to fulfill minimum Ethernet buffer size
                    will not be counted.
:CMD-ID:            16 bit value representing the command that is answered/issued
                    in this buffer.
                    Please see the following chapter for a detailed description
                    of the individual commands.
:MCPD-ID:           ID of the addressed/sending MCPD module.
                    To be assigned during hardware initialisation
:Status:            8 bit wide bit field for sync and start/stop status.
                    Can be left blank in buffers sent by control pc.
:Header Timestamp:  48 bits synchronized system timer (100 ns binning)
                    generated shortly before Ethernet transmission. It allows
                    to have a timed log of command communication.
                    Can be left blank in buffers sent by control pc.
:Command Checksum:  16 bit XOR checksum
                    Covering all words from “Buffer Length” (Word 0) to last
                    data word (Word buffer length –1).
                    Set checksum field to 0x0000 before calculation.

