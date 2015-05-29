mesytec Protocol
================


Communication between daq / control computers and the central modules MCPD-8 is
based on the standardized UDP / IP protocol family.

There are two principle kinds of communications in a psd+ system:

* The MCPD-8 modules receive and answer command buffers. Usually commands are
  emitted by a single control PC. Each command is echoed to the commanding
  computer, delivering a command response and a status description as well as a
  variable amount of information.

* During data acquisition, the MCPD-8 modules will emit data packages autonomously.
  The address of the recipient is set up during the initialisation process of the
  system. Thus it is possible to send data packages and command answers to different
  recipients.

For experiments with high data rates, it might be useful to have data taking
distributed on more than one pc.

Both types of communication packets are wrapped in UDP packets.

Using UDP communication allows efficient data transmission without too much
protocol overhead. But it has to be remembered that there’s no intrinsic
protection against packet loss (like in TCP e. g.) Thus some simple control
mechanisms have been implemented into the mesytec protocol in order to achieve
sufficient data security.
