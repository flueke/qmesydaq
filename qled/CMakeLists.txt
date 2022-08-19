find_package(Qt5 COMPONENTS Widgets Gui Svg REQUIRED)
add_library(qled SHARED src/qled.cpp src/qled.qrc)
target_link_libraries(qled PRIVATE Qt5::Widgets Qt5::Svg)
target_include_directories(qled
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    )
