set(eeprom_SRCS
    eepromfactory.cpp
    eepromgui.cpp
    eepromlogic.cpp
)

qt4_wrap_cpp(eeprom_MOC
    eepromgui.h
    eepromlogic.h
)

qt4_wrap_ui(eeprom_UI
    eepromgui.ui
)

add_library(EepromComponent SHARED
    ${eeprom_SRCS}
    ${eeprom_MOC}
    ${eeprom_UI}
)

target_link_libraries(EepromComponent
    TwiComponent
    ${QT_LIBRARIES}
    ${SIMAVR_LIBRARIES}
    QHexEdit2
)

include_directories(
    ${CMAKE_SOURCE_DIR}/QHexEdit2
    ${CMAKE_SOURCE_DIR}/TwiComponent
    ${CMAKE_SOURCE_DIR}/QSimAVR
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_INCLUDES}
    ${SIMAVR_INCLUDE_DIRS}
)

install(TARGETS EepromComponent DESTINATION lib/qsimavr)
