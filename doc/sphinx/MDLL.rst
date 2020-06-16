MDLL
****

**Readout module for area detectors with delayline readout**

**Principle of operation:**

MDLL is an integrated readout module for twodimensional detectors with delayline
readout. It is a compact combination of analog readout electronics and an MCPD-8
like network interface.

**Input Signals:**

Five analog detector signals are processed:

*   X0, X1
*   Y0, Y1
*   Anode

**Signal processing:**

CFDs on X and Y signals as well as on the Anode signal generate start/stop signals
for 4 TACs (Time to Analog Converters). These TACs are started by the Anode signal
and stop on X / Y signals. Positions are calculated unsing these TAC times.

CFD thresholds for X, Y and Anode are remotely controllable to adjust for optimum
noise reduction.

TAC ranges are remotely controllable between 100 and 500 ns – leading to a scaling
of positions within this range.

A remotely controllable offset can be applied to TAC data, allowing a shift in
position to adjust for geometrical/electronical variations of the detector.

A builtin test pulser (also remotely controllable) allows functional testing without
using a detector.

**Data processing:**

Before generating a valid neutron event, a (remotely controllable) software window
discriminator is applied on the timing value for each dimension (X, Y) as well as
on energy. This allows filtering of erroneous events caused by multiple hits as
well as by unwanted particles.

Each valid event generates a data set comprising either:

  | X- Position, Y-Position, Energy and Timestamp
  | (which is the “Standard” operating mode)

or:

  | X-Timing, Y-Timing, Energy and Timestamp
  | (which is useful for setup and control)

Data are output as UDP packages on an ethernet interface, data format is comparable
to data format of MCPD-8 / MPSD-8. Please see below for a detailed description.

**Overview Main Properties**

**MDLL, “detector part”**

*   direct inputs for 2 x X-cathode, 2 x Y-cathode, 1 x Anode
*   X and Y position calculation from delay time
*   Energy determination from anode signal
*   Adaptable to delays of 100 ns … 500 ns

**MDLL, “MCPD-8 part”**

*   same network interface like MCPD-8
*   similar digital interface like MCPD-8, but:
    *   only 3 digital (TTL-) Inputs for Chopper/Counter/Triggering
*   no analog In-/Outputs
*   Listmode data transmission, two data modes: [X, Y, E] or [tX, tY, E]
*   Data format identical to MCPD-8 / MPSD-8: 8 bit Energy, 2 x 10 bit position

**MDLL “detector part”: Logical Properties:**

*   ID: 35
*   3 thresholds: X, Y, Anode
*   2 shift factors: X, Y
*   2 scaling factors: X, Y
*   3 window settings: timing sum X, timing sum Y, Anode Amplitude
*   Test pulser settings: on/off, amplitude (4 steps), position (3 fixed positions)
*   Data selection: X, Y, E (standard mode), timing sum X, timing sum Y, E (for setup purposes)

.. toctree::
    MDLL_Databuffers
    MDLL_Command_Reference
