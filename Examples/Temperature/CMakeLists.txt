set(CMAKE_C_FLAGS "-mmcu=atmega1280 -Wall -gdwarf-2 -std=gnu99 -Wextra -pedantic -ffunction-sections -fdata-sections   -fno-inline-small-functions -DF_CPU=16000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -DOW_ONE_BUS")

add_executable(temperature
    crc8.c
    ds18x20.c
    main.c
    onewire.c
    uart.c
    uart_addon.c
)

include_directories(
    ${SIMAVR_INCLUDES}
)
