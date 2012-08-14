set(eeprom_SRCS
    eepromfactory.cpp
    eepromlogic.cpp
)

add_library(EepromComponent SHARED
    ${eeprom_SRCS}
)

target_link_libraries(EepromComponent
    TwiComponent
    ${QT_LIBRARIES}
    ${SIMAVR_LIBRARIES}
)

include_directories(
    ${CMAKE_SOURCE_DIR}/TwiComponent
    ${CMAKE_SOURCE_DIR}/QSimAVR
    ${QT_INCLUDES}
    ${SIMAVR_INCLUDES}
)
