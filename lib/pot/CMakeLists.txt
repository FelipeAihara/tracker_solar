add_library(pot_lib STATIC)
target_sources(pot_lib PRIVATE src/pot.cpp)
target_include_directories(pot_lib PUBLIC include
    ${ZEPHYR_BASE}/include
    ${CMAKE_BINARY_DIR}/zephyr/include/generated
    ${CMAKE_BINARY_DIR}/zephyr/include/generated/zephyr
)
target_link_libraries(zephyr_interface)

# FORÇA O CMAKE A ESPERAR OS ARQUIVOS GERADOS DO ZEPHYR ANTES DE COMPILAR O SERVO
add_dependencies(pot_lib zephyr_generated_headers)