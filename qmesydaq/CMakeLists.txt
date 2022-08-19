find_package(Qt5 COMPONENTS Widgets Gui PrintSupport Svg REQUIRED)

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
    plot.cpp
    presetwidget.cpp
    QMesydaqDetectorInterface.cpp
    QtInterface.cpp
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

#qt5_wrap_ui(qmesydaq passworddialog.ui)

target_include_directories(qmesydaq
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/diskspace>
    )

target_link_libraries(qmesydaq
    PRIVATE Qt5::Widgets Qt5::PrintSupport Qt5::Svg
    qled libqmesydaq ${QWT_LIBRARY})
