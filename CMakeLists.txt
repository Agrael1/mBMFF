cmake_minimum_required(VERSION 3.22)
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" MBMFF_VERSION_STRING)
string(STRIP "${MBMFF_VERSION_STRING}" MBMFF_VERSION_STRING)
project(mBMFF VERSION ${MBMFF_VERSION_STRING} LANGUAGES CXX)

include(GNUInstallDirs)

# --- Single-header amalgamation ---
find_package(Python3 REQUIRED COMPONENTS Interpreter)

file(GLOB MBMFF_SOURCE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mbmff/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mbmff/boxes/*.hpp
)

set(MBMFF_SINGLE_HEADER "${CMAKE_BINARY_DIR}/mbmff.hpp")

add_custom_command(
    OUTPUT ${MBMFF_SINGLE_HEADER}
    COMMAND "${Python3_EXECUTABLE}"
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/amalgamate.py"
        --input "${CMAKE_CURRENT_SOURCE_DIR}/include/mbmff"
        --output "${MBMFF_SINGLE_HEADER}"
    DEPENDS
        ${MBMFF_SOURCE_HEADERS}
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/amalgamate.py"
    COMMENT "Generating single-header mbmff.hpp"
)

add_custom_target(amalgamate DEPENDS ${MBMFF_SINGLE_HEADER})

# --- Library ---
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

# Install only the amalgamated single header for distribution
install(
    FILES ${MBMFF_SINGLE_HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mbmff
)

install(EXPORT mbmffTargets
    FILE mbmffTargets.cmake
    NAMESPACE mbmff::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mbmff
)

option(MBMFF_BUILD_TEST "Build tests" ${PROJECT_IS_TOP_LEVEL})
if (MBMFF_BUILD_TEST)
    add_subdirectory(test)
endif()

option(MBMFF_ENABLE_CONSTEXPR_TEST
    "Enable compile-time constexpr tests in headers"
    ${PROJECT_IS_TOP_LEVEL}
)
if (MBMFF_ENABLE_CONSTEXPR_TEST)
    target_compile_definitions(mbmff INTERFACE MBMFF_ENABLE_CONSTEXPR_TEST)
endif()