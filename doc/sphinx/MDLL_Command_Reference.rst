Command Set MDLL
================

Command List (Numerical Order)
------------------------------

========= ========================================
**Cmd #** **Command**
========= ========================================
 0        :ref:`Reset <cmd0>`
 1        :ref:`Start DAQ <cmd1>`
 2        :ref:`Stop DAQ <cmd2>`
 3        :ref:`Continue DAQ <cmd3>`
 4        :ref:`Set MCPD-ID <cmd4>`
 5        :ref:`Set Protocol Parameters <cmd5>`
 6        :ref:`Set MCPD-8 Timing Setup <cmd6>`
 7        :ref:`Set Master Clock <cmd7>`
 8        :ref:`Set Run ID <cmd8>`
 9        :ref:`Set Counter / ADC cell <cmd9>`
 10       :ref:`Set Auxiliary Timer <cmd10>`
 11       :ref:`Set Parameter Source <cmd11>`
 12       :ref:`Get Parameters <cmd12>`
 18       :ref:`Send MCPD-8 Serial String <cmd18>`
 19       :ref:`Read MCPD-8 Serial String <cmd19>`
 51       :ref:`Retrieve MCPD-8 version information <cmd51>`
 60       :ref:`Set MDLL thresholds <cmd60>`
 61       :ref:`Set MDLL spectrum <cmd61>`
 65       :ref:`Set MDLL Test pulser <cmd65>`
 66       :ref:`Set MDLL dataset <cmd66>`
 67       :ref:`Set MDLL timing window <cmd67>`
 68       :ref:`Set MDLL energy window <cmd68>`
========= ========================================

.. note:: commands 60-68 are MDLL exclusive


**UDP command set for MDLL**

(extended command set for MDLL “detector properties”)

**Cmd=60 Set MDLL Thresholds**

.. table::
    :name: cmd60

    ======== ============================
    **Word** **Contents**
    ======== ============================
    10       Threshold X (0 … 255)
    11       Threshold Y (0 … 255)
    12       Threshold Anode (0 … 255)
    13       0xFFFF
    ======== ============================

Sets thresholds for the constant fraction discriminators. CFD signals start
(Anode) or stop the TAC measurement from which position calculation is derived.

**Cmd=61 Set MDLL Spectrum**

.. table::
    :name: cmd61

    ======== ============================
    **Word** **Contents**
    ======== ============================
    10       ShiftX (0 … 255)
    11       ShiftY (0 … 255)
    12       ScaleX (0 … 255)
    13       ScaleY (0 … 255)
    14       0xFFFF
    ======== ============================

Sets offset values for the calculated position (ShiftX, ShiftY), shifting the
position spectrum in given direction.

Sets scaling values (ScaleX, Scale Y) by defining the TAC max range between
100 ns and 500 ns.

**Cmd=66 Set MDLL TX dataset**

.. table::
    :name: cmd66

    ======== ============================
    **Word** **Contents**
    ======== ============================
    10       | 0 = E, X, Y
             | 1 = E, tsumX, tsumY
    11       0xFFFF
    ======== ============================

| Defines which data set will be transmitted:
| Standard is X, Y, and E (Anode amplitude).
| For setup purposes timingX, timingY and E can be chosen as data set.
  (allowing to define the borders for the software discrimination windows with
  :ref:`command #67 <cmd67>`) and :ref:`command #68 <cmd68>`) subsequently).

**Cmd=67 Set MDLL Timing window**

.. table::
    :name: cmd67

    ======== ============================
    **Word** **Contents**
    ======== ============================
    10       Not used
    11       Not used
    12       Tsum Limit X low (0 … 1024)
    13       Tsum Limit X high (0 … 1024)
    14       Tsum Limit Y low (0 … 1024)
    15       Tsum Limit Y high (0 … 1024)
    16       0xFFFF
    ======== ============================

| Defines a timing window for data acquisition.
| Only events matching these software window restrictions are copied into data
  buffer for transmission.
| This allows suppression of multiple hits leading to erroneous delay line timing.

**Cmd=68 Set MDLL Energy Window**

.. table::
    :name: cmd68

    ======== ============================
    **Word** **Contents**
    ======== ============================
    10       Lower threshold (0 … 255)
    11       Upper threshold (0 … 255)
    12       Not used
    13       Not used
    14       0xFFFF
    ======== ============================

| Defines an energy window for data acquisition.
| Only events matching this software window restriction are copied into data
  buffer for transmission.
| This allows suppression of events caused by unwanted particles with deviating
  energy deposition.

**Cmd=65 Set MDLL Test pulser**

.. table::
    :name: cmd65

    ======== ============================
    **Word** **Contents**
    ======== ============================
    10       On / Off (1 / 0)
    11       Pulser Amplitude (0, 1, 2, 3)
    12       Pulser Position (0, 1, 2)
    13       0xFFFF
    ======== ============================

| Sets MDLL test pulser to one of three possible positions (below left, middle,
  upper right) and selects one of three possible amplitudes.
| On / off switches pulser hardware on / off.
