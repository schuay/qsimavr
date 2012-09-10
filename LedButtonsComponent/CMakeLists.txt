set(ledbuttons_SRCS
    ledbuttonfactory.cpp
    ledbuttonsgui.cpp
    ledbuttonslogic.cpp
)

qt4_wrap_cpp(ledbuttons_MOC
    ledbuttonsgui.h
    ledbuttonslogic.h
)

qt4_wrap_ui(ledbuttons_UI
    ledbuttonsgui.ui
)

add_library(LedButtonsComponent SHARED
    ${ledbuttons_SRCS}
    ${ledbuttons_MOC}
    ${ledbuttons_UI}
)

target_link_libraries(LedButtonsComponent
    ${QT_LIBRARIES}
    ${SIMAVR_LIBRARIES}
)

include_directories(
    ${CMAKE_SOURCE_DIR}/QSimAVR
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_INCLUDES}
    ${SIMAVR_INCLUDE_DIRS}
)
