set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5 COMPONENTS Widgets Gui Network REQUIRED)

add_library(libqmesydaq
    calibration.cpp
    counter.cpp
    datarepeater.cpp
    detector.cpp
    editormemory.cpp
    histogram.cpp
    linearmapcorrect.cpp
    m2d.cpp
    m2dfactory.cpp
    mapcorrect.cpp
    mappeddetector.cpp
    mappedhistogram.cpp
    mcpd2.cpp
    mcpd8.cpp
    mcpd.cpp
    mdllcorrect.cpp
    mdll.cpp
    measurement.cpp
    mpsd8.cpp
    mpsd8old.cpp
    mpsd8p.cpp
    mpsd8sadc.cpp
    mpsdfactory.cpp
    mstd16.cpp
    mwpchr.cpp
    networkdevice.cpp
    nomodule.cpp
    pulsertest.cpp
    qmlogging.cpp
    spectrum.cpp
    stdafx.cpp
    streamwriter.cpp
    usermapcorrect.cpp
    )

target_link_libraries(libqmesydaq PRIVATE Qt5::Widgets Qt5::Network)
target_include_directories(libqmesydaq
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )
set_target_properties(libqmesydaq PROPERTIES OUTPUT_NAME mesydaq)
