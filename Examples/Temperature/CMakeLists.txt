set(CMAKE_C_FLAGS "-mmcu=atmega16 -Os -std=gnu99")

add_executable(temperature
    char_display.c
    one_wire.c
    temp_sensor.c
    temperature_control.c
    uart.c
    util.c
)
