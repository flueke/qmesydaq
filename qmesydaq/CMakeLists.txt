set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5 COMPONENTS Widgets Gui Network PrintSupport Svg REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem)

add_executable(qmesydaq
    caresssetup.cpp
    ChannelSpinBox.cpp
    colormaps.cpp
    colorwidget.cpp
    CommandEvent.cpp
    ConfigFileDialog.cpp
    data.cpp
    datetimelabel.cpp
    doublepresetwidget.cpp
    generalsetup.cpp
    histogramedittablewidget.cpp
    histogrammappingeditor.cpp
    ipaddresswidget.cpp
    LoopObject.cpp
    main.cpp
    mainwidget.cpp
    mainwindow.cpp
    mcpdsetup.cpp
    MCPDSpinBox.cpp
    mdllpulser.cpp
    mdllsetup.cpp
    mesydaqdata.cpp
    moduleidentificationpage.cpp
    modulemasterpage.cpp
    modulesetup.cpp
    ModuleSpinBox.cpp
    modulestatus.cpp
    modulewizard.cpp
    monitorpresetwidget.cpp
    mpsdpulser.cpp
    MultipleLoopApplication.cpp
    passworddialog.cpp
    plot.cpp
    presetwidget.cpp
    QMesydaqDetectorInterface.cpp
    QtInterface.cpp
    StatusBarEntry.cpp
    tacosetup.cpp
    tangosetup.cpp
    tcpsetup.cpp
    zoomer.cpp
    diskspace/diskspace.cpp

    additionaleventconfig.ui
    caresssetup.ui
    channelhistogramsetup.ui
    doublepresetwidget.ui
    generalsetup.ui
    histogrammappingeditor.ui
    ipaddresswidget.ui
    mainwidget.ui
    mainwindow.ui
    mcpdsetup.ui
    mdllpulser.ui
    mdllsetup.ui
    moduleidentificationpage.ui
    modulemasterpage.ui
    modulesetup.ui
    modulestatus.ui
    modulewizard.ui
    monitorpresetwidget.ui
    mpsdpulser.ui
    passworddialog.ui
    statusbarentry.ui
    tacosetup.ui
    tangosetup.ui
    tcpsetup.ui
    )

target_include_directories(qmesydaq
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/diskspace>
    )

target_link_libraries(qmesydaq
    PRIVATE Qt5::Widgets Qt5::Network Qt5::PrintSupport Qt5::Svg
    PRIVATE ${Boost_LIBRARIES}
    PRIVATE qled
    PRIVATE libqmesydaq
    PRIVATE ${QWT_LIBRARY})

install(TARGETS qmesydaq RUNTIME DESTINATION bin)
