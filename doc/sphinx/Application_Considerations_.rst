.. |mus|  unicode:: U+003BC s

Application Considerations:
===========================

The various functional units of the MCPD-8 allow building a highly flexible
system, delivering exactly the data needed by an instrument. Not only neutron
data, but also environmental parameters can be recorded in several ways.

As it is not really self explaining how to use the many flexible possibilities
the PSD system offers, here are some considerations and hints that might be
helpful adapting the readout system to an instrument.

Data Sources:
-------------

The PSD system offers three different kinds of data:

* | Neutron event data, generated in a MPSD-8 or MSTD-16 module:
  | Position and / or Energy information, together with a detector address and a
    precise event timestamp.

* | Counter / TTL event data, generated in MCPD-8:
  | Counter values are buffered immediately on triggering together with ID
    information and a precise timestamp, just like a neutron event.

  | Sources:
  | All six TTL inputs can each be used to drive a counter.

  | Triggers:
  | Counter event readout can be triggered from one of four trigger sources:

  * | One of four auxiliary timers:
    | This allows for repeated readout at fixed intervals (e.g. to get an easy
      monitor rate calculation.)

  * | One of two auxiliary TTL inputs:
    | This allows for a readout at a status that may be defined by any external
      device (e.g. a “ready” signal from sample environment)

  * | Increments over a defined bit position of the counter register.
    | An event will be triggered when this bit toggles from 0 to 1.
    | E.g. a “0” in compare register (means: “trigger, whenever bit 0 becomes
      “1”) generates a trigger every second count.
    | A “2” in compare register (means: “trigger, whenever bit 2 becomes “1”)
      will trigger every 8 counts, …
    | This allows reading the counter value at any binary power up to 20.

  * | Edge triggering of the TTL input:
    | Every rising edge of the TTL input triggers an event.
    | This might be useful e.g. to generate precisely timestamped chopper
      signal events, start or stop events correlated with sample environment, …

*   ADC event data:

    Sources:

        Two 12 bit ADCs with a selectable full range of +/- 4,5 or 0...9 V are
        read out continuously with a sampling rate of 25 kHz automatically. The
        corresponding registers can be read out and transmitted as timestamped
        ADC events.

    Triggers:

        ADC data readout can be triggered by nearly the same trigger sources as
        counters described above:

*   One of four auxiliary timers:

    This allows for repeated readout at fixed intervals (e.g. to get a
    continuous readout of changing environmental parameters like
    temperature, fields, pressure, ...)

* | One of two auxiliary TTL inputs:
  | This allows for a readout at a status that may be defined by any external
    device (e.g. a “ready” signal from sample environment)
  | (There's no self-triggering with a compare register for the ADCs.)


Examples:
---------

Chopper Signals:
~~~~~~~~~~~~~~~~

Chopper signals need precise timing, so it's a good application for a self
triggering TTL input:

*   Connect the TTL signal from copper electronics to one of the four
    Monitor/Chopper inputs or to one of the backpanel TTL inputs.

*   Configure the according counter cell using :ref:`command #9 <cmd9>` with:

    *   cell#: 0-3 for frontpanel, 4,5 for backpanel

    *   trigger source = 7 (trigger on compare register)

    *   compare register value = 22 (trigger on rising edge of input)

Now each time the TTL input rises, a trigger event is generated, timestamped and
buffered with the following data :

  | TrigID = 7 (compare register)
  | DataID = n (n=address of input)
  | Data = counter value (may perhaps be discarded in data processing for chopper
    pulses)

If a divider for chopper pulses is required, the compare register can be set to
values other than 22: a „0“ will lead to triggering every second rising edge,
a „1“ every fourth“, ...


Monitor Counters:
~~~~~~~~~~~~~~~~~

Monitor counters normally don't need precise timing with a 100 ns timebase. So
there are several possibilites for monitor counter readout – depending on
instrumental needs:

Connect the monitor counting output (TTL)
to one of the four Monitor/Chopper inputs or to one of the backpanel TTL inputs.

Monitor counter as header parameter:
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Define the appropriate counter (0...3: frontpanel, 4,5: backpanel) as source of
one of the four parameter fields that will be transmitted with every data buffer
header.

Use :ref:`command #11 <cmd11>` with:

  | Parameter# = 0...3
  | Source = 0..3 for frontpanel inputs, 4,5 for backpanel inputs

Now each time a new data buffer is generated, the defined counters will be read
into their assigned parameter fields. Timestamp is the header timestamp.

Buffers will be generated and transmitted whenever a buffer is full or at least
every 25 ms (40 Hz minimum buffer frequency).

**Monitor counter as timer triggered event:**

Using one of the auxiliary timers as trigger source, monitor counter values can
be transmitted with a constant frequency: timebase for the 16 bit timers are
10 |mus|, the maximum interval is therefore 655,36 ms.

*   Configure one of the four auxiliary timers with the appropriate trigger
    interval, using :ref:`command #10 <cmd10>`:

    *   timer# = 0...3

    *   capture register 0... 65.536 (e.g. 10.000 for 100 ms intervals)

*   Configure the counter cell using :ref:`command #9 <cmd9>`:

    *   cell# = 0..3 for frontpanel inputs, 4,5 for backpanel inputs

    *   trigger source = 1...4 for aux timer 0...3

    *   compare register: will not be used

Now each time the auxiliary timer reaches the capture value, a trigger event
will be generated, timestamped and buffered with the following data:

  | TrigID = 1...4 (timer#)
  | DataID = n (n=address of input)
  | Data = monitor counter value


**Monitor counter as counter triggered event:**

Configure the according counter cell using :ref:`command #9 <cmd9>` with:

    *   cell#: 0-3 for frontpanel, 4,5 for backpanel

    *   trigger source = 7 (trigger on compare register)

    *   compare register value = 0...21

Now each time the bit position defined in compare register value becomes „1“,
a trigger event is generated, timestamped and buffered with the following data:

  | TrigID = 7 (compare register)
  | DataID = n (n=address of input)
  | Data = counter value (may perhaps be discarded in data processing for chopper
    pulses)

This allows reading out monitor counters at any power of from :math:`2^1` up
to :math:`2^22`

**Monitor counter as edge triggered event:**

Identical to chopper signals described above.

External analog data, timer triggered:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Many instruments need information from other components of the setup: temperature,
pressure, fields, …

Often these values can be read out by an analog voltage signal. MCPD-8 allows to
digitize such signals and fill the according values into the data stream.

Just like described above for monitor counters, also the ADC readout can be
triggered by auxiliary timers:

*   Configure one of the four auxiliary timers with the appropriate trigger
    interval, using :ref:`command #10 <cmd10>`:

    *   timer# = 0...3

    *   capture register 0... 65.536 (e.g. 10.000 for 100 ms intervals)

*   Configure the ADC cell using :ref:`command #9 <cmd9>`:

    *   cell# = 6,7 for ADC input 1, 2

    *   trigger source = 1...4 for aux timer 0...3

    *   compare register: will not be used

Now each time the auxiliary timer reaches the capture value, a trigger event will
be generated, timestamped and buffered with the following data:

  | TrigID = 1...4 (timer#)
  | DataID = n (n=address of ADC input)
  | Data = ADC value (12 bit valid data, 9 bit leading zeroes)

External analog data, TTL triggered:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ADC readout can also be triggered by an axternal TTL signal, which can be an end
point switch, an external frequency source, …

Configure the ADC cell using :ref:`command #9 <cmd9>`:

    *   cell# = 6,7 for ADC input 1, 2

    *   trigger source = 5,6 for backpanel TTL inputs 1,2

    *   compare register: will not be used

Now on each rising edge of the assigned backpanel TTL input a trigger event will
be generated, timestamped and buffered with the following data :

  | TrigID = 5, 6 (TTL input)
  | DataID = n (n=address of ADC input)
  | Data = ADC value (12 bit valid data, 9 bit leading zeroes)

External analog data, continuously transmitted with buffer header:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If external data just have to be monitored, but precise timing is not the issue,
data transmission in one of the four parameter fields of the buffer header may
be a solution:

Use :ref:`command #11 <cmd11>` to set up the selected parameter field for transmission of the
ADC values: Source = 6 will transmit a combination of both TTL input levels and
both ADC values in the selected parameter field.

Buffers are transmitted whenever they are full with events or latest every 40 ms.
Values and timestamps are generated at the moment of buffer generation.

Combination of timers, triggering, counting and header parameters:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For a sophisticated setup, most of the readout possiblities can be combined.

For example monitor counters:

It may be convenient to have edge triggered, timestamped monitor counter events
in the data stream to allow a count-precise offline data processing from listmode
files.

But it is time consuming to evaluate every event online to search for a stop
condition for data taking. For this purpose it's mostly sufficient to have the
monitor counter as one of the four header parameters to generate a stop condition.


Same for external parameters:

It might be necessary to have a temperature/field/pressure profile with a
resolution of some hundred microseconds in offline data analysis. So a timer
triggered event generation with the necessary frequency is a good idea.

But for online status monitoring, the ADC values as one of the header parameters,
transmitted every 40 ms latest, should be more than enough in most cases.

So just configure the data source for triggering as well as for parameter
transmission:

e.g. the monitor counter:

* use :ref:`command #9 <cmd9>` to set up the appropriate counter cell to trigger an event on
  every rising edge of the input

* use :ref:`command #11 <cmd11>` to define the same counter as source for one of the parameter
  fields

Thus the monitor counter value will be transmitted as an event on each rising
edge (to be used in offline data processing if necessary) AND it will be
transmitted with every buffer header (e.g. for economical generation of stop
conditions).
