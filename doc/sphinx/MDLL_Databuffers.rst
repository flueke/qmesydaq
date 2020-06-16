MDLL Data
=========

MDLL has a slightly different data structure due to the fact that it has a fixed
number of position channels (960 x 960).

To signalize this different structure, field „Buffer Type“ in buffer header has a
value of 0x0002.

Buffer Structure
----------------

(in 16 bit words):

+---------------------------------+-----------------+
| Buffer Length (in 16 bit words) | Word 0          |
+---------------------------------+-----------------+
| **Buffer Type = 0x0002 for MDLL**                 |
+---------------------------------------------------+
| Header Length (in 16 bit words)                   |
+---------------------------------------------------+
| Buffer Number                                     |
+---------------------------------------------------+
| Run-ID                                            |
+---------------------------------+-----------------+
| MCPD-ID                         | Status          |
+---------------------------------+-----------------+
| Header Timestamp Lo                               |
+---------------------------------------------------+
| Header Timestamp Mid                              |
+---------------------------------------------------+
| Header Timestamp Hi                               |
+---------------------------------------------------+
| Parameter 0 Lo                                    |
+---------------------------------------------------+
| Parameter 0 Mid                                   |
+---------------------------------------------------+
| Parameter 0 Hi                                    |
+---------------------------------------------------+
| Parameter 1 Lo                                    |
+---------------------------------------------------+
| Parameter 1 Mid                                   |
+---------------------------------------------------+
| Parameter 1 Hi                                    |
+---------------------------------------------------+
| Parameter 2 Lo                                    |
+---------------------------------------------------+
| Parameter 2 Mid                                   |
+---------------------------------------------------+
| Parameter 2 Hi                                    |
+---------------------------------------------------+
| Parameter 3 Lo                                    |
+---------------------------------------------------+
| Parameter 3 Mid                                   |
+---------------------------------+-----------------+
| Parameter 3 Hi                  | Word 20         |
+---------------------------------+-----------------+
| Event 0 Lo                      | Word 21         |
+---------------------------------+-----------------+
| Event 0 Mid                                       |
+---------------------------------------------------+
| Event 0 Hi                                        |
+---------------------------------------------------+
| ...                                               |
+---------------------------------------------------+
| Event n Lo                                        |
+---------------------------------------------------+
| Event n Mid                                       |
+---------------------------------+-----------------+
| Event n Hi                      | Word 21 + 3 * n |
+---------------------------------+-----------------+

(must be identical with buffer length – 1)

Event structure MDLL
--------------------

Each event has a fixed 48 bit length. The contents differs according to the event
id.

As MDLL has – compared to MPSD systems – a fixed number of 960 x 960 channels,
transmitted data are slight ly different to MPSD data format:

:ID = 0:        Neutron data event

:ID = 1:        Trigger data event


Neutron data events (ID = 0)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

+------------+---------------+-----------------+-----------------+----------------+
| MSB        |               |                 |                 |         LSB    |
+------------+---------------+-----------------+-----------------+----------------+
| ID (1 bit) | Amplitude (8) | Y Position (10) | X Position (10) | Timestamp (19) |
| **= 0**    |               |                 |                 |                |
+------------+---------------+-----------------+-----------------+----------------+


:ID:            ID = 0 signalling a `neutron` event, resulting from a detector
                event at a peripheral module like MPSD-8.

                (Monitor counter events e.g., that of course also are “neutron events”
                are generated at the MCPD-8, don’t carry a position information and
                are therefore regarded as “other events” in this context.)

                1 bit

:Amplitude:     amplitude (energy) of the neutron event

                8 bit

:Y Position:    y position of the neutron event

                10 bit

:X Position:    x position of the neutron event

                10 bit

:Timestamp:     timing offset to the corresponding header timestamp

                event time = header timestamp + event timestamp

                19 bit
