**qmesydaq - GUI for mesytec MPSD devices**

This is a mesytec-maintained fork of the TUM/FRM2 version of
[qmesydaq](https://forge.frm2.tum.de/cgit/cgit.cgi/frm2/general/qmesydaq.git/).
This version has been ported to Qt5+CMake and has support for the new
``MCPD-8_v2``, including support for hostname based connections.

QMesyDAQ application is based in part on the work of the Qwt project
(http://qwt.sf.net).

Build and install using CMake:

```shell
    mkdir qmesydaq/build
    cd qmesydaq/build
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/qmesydaq ..
    make -j12 install

```

Note: The TACO and CARESS interfaces, the docs, tools and tests are currently
not built by the CMake based build system. If you need one of these use the
FRM2 hosted version linked above or add the respective CMake code.

Binaries are available here: https://mesytec.com/downloads/qmesydaq/
The source code is hosted on github: https://github.com/flueke/qmesydaq/
