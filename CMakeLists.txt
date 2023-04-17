cmake_minimum_required(VERSION 3.12)

project(qmesydaq
    DESCRIPTION "qmesydaq - DAQ software for mesytec MCPD and MDLL")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INSTALL_RPATH "$ORIGIN;$ORIGIN/../lib")

# Add the local cmake directory module path.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tools/cmake")

# Pull information from git:
# GIT_SHA1 contains the complete git hash
#
# GIT_VERSION contains the name of the latest git tag, the number of commits
# since that tag was created and the start of the git hash if there have been
# changes since the tag was created.
# Example GIT_VERSION: 0.7-371-g12d9678

# GIT_VERSION_SHORT contains the same as GIT_VERSION but without the start of
# the last commits hash.
# Example GIT_VERSION_SHORT: 0.7-371

# GIT_VERSION_TAG contains only the tag part of the version
# Example GIT_VERSION_TAG: 0.7
include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)
git_describe(GIT_VERSION "--always")
string(REGEX REPLACE "^v" "" GIT_VERSION ${GIT_VERSION})
string(REGEX MATCH "^[0-9.]+(-beta[0-9]*|-rc[0-9]*|-dev[0-9]*)?(-[0-9]+)?" GIT_VERSION_SHORT ${GIT_VERSION})
string(REGEX MATCH "^[0-9.]+" GIT_VERSION_TAG ${GIT_VERSION})

execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE)

message("-- GIT_VERSION=${GIT_VERSION}")
message("-- GIT_VERSION_SHORT=${GIT_VERSION_SHORT}")
message("-- GIT_VERSION_TAG=${GIT_VERSION_TAG}")
message("-- GIT_BRANCH=${GIT_BRANCH}")

set(not-msvc $<NOT:$<CXX_COMPILER_ID:MSVC>>)

# Create our binaries in the root of the build directory. It's just more convenient.
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Look for qwt
find_path(QWT_INCLUDE_DIR
    NAMES qwt_plot.h
    PATH_SUFFIXES qwt qwt-qt5 qwt-6.2.0 qwt-6.1.3 qt5/qwt6
    PATHS /usr/local/lib/qwt.framework/Headers)

include_directories(${QWT_INCLUDE_DIR})

find_library(QWT_LIBRARY_RELEASE NAMES qwt qwt-qt5 qwt6)
find_library(QWT_LIBRARY_DEBUG   NAMES qwtd qwt-qt5d qwt6d)

if (WIN32 OR QWT_LIBRARY_DEBUG)
    if (QWT_LIBRARY_DEBUG)
        set(QWT_LIBRARY
            optimized ${QWT_LIBRARY_RELEASE}
            debug ${QWT_LIBRARY_DEBUG}
            )
        #set(QWT_LIBRARY ${QWT_LIBRARY_DEBUG})
    else()
        set(QWT_LIBRARY
            optimized ${QWT_LIBRARY_RELEASE}
            debug ${QWT_LIBRARY_RELEASE}
            )
        #set(QWT_LIBRARY ${QWT_LIBRARY_RELEASE})
    endif()
else()
    set(QWT_LIBRARY ${QWT_LIBRARY_RELEASE})
endif()

message("-- Using QWT_INCLUDE_DIR=${QWT_INCLUDE_DIR}")
message("-- Using QWT_LIBRARY=${QWT_LIBRARY}")

add_definitions(-DVERSION="${GIT_VERSION}")

add_subdirectory(qled)
add_subdirectory(lib)
add_subdirectory(qmesydaq)

#find_package(Doxygen)
#
#if (DOXYGEN_FOUND)
#add_custom_target(doc_doxygen
#    COMMAND Doxygen::doxygen ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
#    COMMENT "Running doxygen")
#endif (DOXYGEN_FOUND)
#
#add_custom_target(doc ALL)
#add_dependencies(doc doc_doxygen)
