Listmode Data Format
====================

General File Format
-------------------

Incoming data fom MCPD-8 modules will usually be written to disk directly in
listmode format. This is the native data format arriving from ethernet. It allows
an offline replay / reconstruction / processing of data once they are taken.

The listmode files may vary from setup to setup, but a general structure can be
given:

+-------------------------------------------------------------------------------+
| **File Header**                                                               |
|                                                                               |
| | ASCII format                                                                |
| | variable length                                                             |
| | length given in header                                                      |
| | carries administrationadata                                                 |
+-------------------------------------------------------------------------------+
| **Header Separator**                                                          |
|                                                                               |
| | binary format                                                               |
| | fixed length: 4 x 16 bit words                                              |
| | fixed signature: 0x0000, 0x5555, 0xAAAA, 0xFFFF                             |
| | may be used for integrity check, file reconstruction                        |
+-------------------------------------------------------------------------------+
| **First Data Block**                                                          |
|                                                                               |
| | binary format                                                               |
| | image of data buffer header plus trailing data like described               |
|   :ref:`above <header_data_dictionary>`                                       |
| | variable length                                                             |
| | length given in data header                                                 |
+-------------------------------------------------------------------------------+
| **Data Block Separator**                                                      |
|                                                                               |
| | binary format                                                               |
| | fixed length: 4 x 16 bit words                                              |
| | fixed signature: 0x0000, 0xFFFF, 0x5555, 0xAAAA                             |
| | may be used for integrity check, file reconstruction                        |
+-------------------------------------------------------------------------------+
| **Trailing Data Blocks**                                                      |
+-------------------------------------------------------------------------------+
| **Trailing Data Separators**                                                  |
+-------------------------------------------------------------------------------+
| **...**                                                                       |
+-------------------------------------------------------------------------------+
| **Closing signature**                                                         |
|                                                                               |
| | binary format                                                               |
| | fixed length: 4 x 16 bit words                                              |
| | fixed signature: 0xFFFF, 0xAAAA, 0x5555, 0x0000                             |
| | may be used for integrity check                                             |
+-------------------------------------------------------------------------------+


During data taking, only incoming event data will be written to file. Command
answers will be processed but not written to file.

File Header Format
------------------

Details of the File header will vary from system to system, depending on number
and contents of additional information that is to be written to the file.

In any case, the header will begin with ASCII line::

  mesytec psd listmode data

followed by a line::

  header length: n lines

where n is in simplest case 2.

This allows any data processing software to skip the variable ASCII header part of
the listmode file without knowing about the detailed structure.

Beginning with line nnnnn+1, the binary part of the listmode file starts with the
header separator.

Data Header Format
------------------

:ref:`Data Header <data_buffer_structure>` is in binary format.

Data Format
-----------

Each data block contains a variable number of :ref:`48 bit wide events <event_structure>`
in binary format.
