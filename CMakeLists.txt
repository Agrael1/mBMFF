cmake_minimum_required(VERSION 3.22)
project(mBMFF VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)

add_library(mbmff INTERFACE)
add_library(mbmff::mbmff ALIAS mbmff)

target_include_directories(mbmff INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(TARGETS mbmff
    EXPORT mbmffTargets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT mbmffTargets
    FILE mbmffTargets.cmake
    NAMESPACE mbmff::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mbmff
)

# Determine if this is the top-level project
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(MBMFF_TOP_LEVEL ON)
else()
    set(MBMFF_TOP_LEVEL OFF)
endif()

option(MBMFF_BUILD_TEST "Build tests" ${MBMFF_TOP_LEVEL})
if (MBMFF_BUILD_TEST)
    add_subdirectory(test)
endif()