project(mbmff-test)

add_executable(mbmff-test entry_main.cpp)
set_target_properties(mbmff-test PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
target_link_libraries(mbmff-test PRIVATE mbmff)

# Copy file to output directory
add_custom_command(TARGET mbmff-test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:mbmff-test>/assets"
)