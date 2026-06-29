add_library(tracker_lib STATIC)
target_sources(tracker_lib PRIVATE src/tracker.cpp)
target_include_directories(tracker_lib PUBLIC include)
target_link_libraries(tracker_lib PUBLIC rtc_lib PUBLIC zephyr_interface)

# FORÇA O CMAKE A ESPERAR OS ARQUIVOS GERADOS DO ZEPHYR ANTES DE COMPILAR O SERVO
add_dependencies(tracker_lib zephyr_generated_headers)