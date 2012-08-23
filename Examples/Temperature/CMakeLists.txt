add_executable(temperature
    char_display.c
    one_wire.c
    temp_sensor.c
    temperature_control.c
    uart.c
    util.c
)

include_directories(
    ${SIMAVR_INCLUDES}
)
