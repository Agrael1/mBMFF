cmake_minimum_required(VERSION 3.20)
project(mbmff-test)

add_executable(mbmff-test entry_main.cpp)
set_target_properties(mbmff-test PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
target_link_libraries(mbmff-test PRIVATE mbmff)

add_executable(mbmff-page-test page_test.cpp)
set_target_properties(mbmff-page-test PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
target_link_libraries(mbmff-page-test PRIVATE mbmff)
