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

add_executable(mbmff-recursive-iterator-test recursive_iterator_tests.cpp)
set_target_properties(mbmff-recursive-iterator-test PROPERTIES
    CXX_STANDARD ${MBMFF_TEST_CXX_STANDARD}
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
target_link_libraries(mbmff-recursive-iterator-test PRIVATE mbmff)

add_test(NAME string-tests COMMAND mbmff-string-test)
add_test(NAME recursive-iterator COMMAND mbmff-recursive-iterator-test)

add_test(NAME page-test-heic COMMAND mbmff-test test/assets/sample.heic)
set_tests_properties(page-test-heic PROPERTIES WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

add_test(NAME page-test-avif COMMAND mbmff-test test/assets/avif_sample.avif)
set_tests_properties(page-test-avif PROPERTIES WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

if (MBMFF_ENABLE_CONSTEXPR_TEST)
    target_compile_definitions(mbmff-test PRIVATE MBMFF_ENABLE_CONSTEXPR_TEST)
endif()
