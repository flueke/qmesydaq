set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5 COMPONENTS Widgets Gui Svg REQUIRED)
qt5_add_resources(LIBQLED_RES_RC_OUT src/qled.qrc)
add_library(qled STATIC src/qled.cpp ${LIBQLED_RES_RC_OUT})
target_link_libraries(qled PUBLIC Qt5::Widgets Qt5::Svg)
target_include_directories(qled
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/>
    )
install(TARGETS qled RUNTIME DESTINATION lib)
include(GenerateExportHeader)
generate_export_header(qled)
