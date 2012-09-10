set(lcd_SRCS
    hd44780.c
    lcdfactory.cpp
    lcdgui.cpp
    lcdlogic.cpp
)

qt4_wrap_cpp(lcd_MOC
    lcdgui.h
    lcdlogic.h
)

qt4_wrap_ui(lcd_UI
    lcdgui.ui
)

add_library(LcdComponent SHARED
    ${lcd_SRCS}
    ${lcd_MOC}
    ${lcd_UI}
)

target_link_libraries(LcdComponent
    ${QT_LIBRARIES}
    ${SIMAVR_LIBRARIES}
)

include_directories(
    ${CMAKE_SOURCE_DIR}/QSimAVR
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_INCLUDES}
    ${SIMAVR_INCLUDE_DIRS}
)

install(TARGETS LcdComponent DESTINATION lib/qsimavr)
