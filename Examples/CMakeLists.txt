set(CMAKE_C_COMPILER avr-gcc)
set(CMAKE_C_FLAGS "-mmcu=atmega1280 -Os -std=gnu99 -g")
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")

add_subdirectory(Eeprom)
add_subdirectory(Glcd)
add_subdirectory(LcdLeds)
add_subdirectory(Temperature)
