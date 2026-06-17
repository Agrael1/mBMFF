project(mbmff-test)

set(MBMFF_TEST_CXX_STANDARD 23 CACHE STRING "C++ standard for the test executable")

add_executable(mbmff-test entry_main.cpp)
set_target_properties(mbmff-test PROPERTIES
    CXX_STANDARD ${MBMFF_TEST_CXX_STANDARD}
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
target_link_libraries(mbmff-test PRIVATE mbmff)

add_executable(mbmff-string-test string_tests.cpp)
set_target_properties(mbmff-string-test PROPERTIES
    CXX_STANDARD ${MBMFF_TEST_CXX_STANDARD}
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
target_link_libraries(mbmff-string-test PRIVATE mbmff)

if (MBMFF_ENABLE_CONSTEXPR_TEST)
    target_compile_definitions(mbmff-test PRIVATE MBMFF_ENABLE_CONSTEXPR_TEST)
endif()
