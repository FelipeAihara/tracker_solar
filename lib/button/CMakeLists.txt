add_library(button_lib STATIC)
target_sources(button_lib PRIVATE src/button.cpp)
target_include_directories(button_lib PUBLIC include)
target_link_libraries(button_lib PUBLIC zephyr_interface)

add_dependencies(zephyr_generated_headers)