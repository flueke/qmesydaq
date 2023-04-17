set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5 COMPONENTS Widgets Gui Svg REQUIRED)
add_library(qled SHARED src/qled.cpp src/qled.qrc)
target_link_libraries(qled PRIVATE Qt5::Widgets Qt5::Svg)
target_include_directories(qled
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    )
