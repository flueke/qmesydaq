Trigger events (ID = 1)
^^^^^^^^^^^^^^^^^^^^^^^

Several trigger sources (counters, timers, digital inputs) can initiate a data
taking event. Triggerd by levels or defined overflows of the trigger sources,
data are taken from the data sources and written to an event structure. Possible
trigger and data sources are timers, counters, and ADC values.


MSB

+----------------+------------+------------+-----------+----------------+
| MSB            |            |            |           |            LSB |
+================+============+============+===========+================+
| ID (1) = **1** | TrigID (3) | DataID (4) | Data (21) | Timestamp (19) |
+----------------+------------+------------+-----------+----------------+

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
