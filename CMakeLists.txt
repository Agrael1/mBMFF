cmake_minimum_required (VERSION 3.22)
if (MSVC)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("mBMFF")

# Determine if this is the top-level project
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(MBMFF_TOP_LEVEL ON)
else()
    set(MBMFF_TOP_LEVEL OFF)
endif()

option(MBMFF_BUILD_TEST "Build tests" ${MBMFF_TOP_LEVEL})

add_library(mbmff INTERFACE)
target_include_directories(mbmff INTERFACE "include")
set_target_properties(mbmff PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

if (MBMFF_BUILD_TEST)
    add_subdirectory(test)
endif()