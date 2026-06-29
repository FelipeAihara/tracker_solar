add_library(lcd_lib STATIC)
target_sources(lcd_lib PRIVATE src/lcd.cpp)
target_include_directories(lcd_lib PUBLIC include)
target_link_libraries(lcd_lib PUBLIC zephyr_interface)

# FORÇA O CMAKE A ESPERAR OS ARQUIVOS GERADOS DO ZEPHYR ANTES DE COMPILAR O SERVO
add_dependencies(servo_lib zephyr_generated_headers)