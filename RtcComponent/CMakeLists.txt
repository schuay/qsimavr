set(rtc_SRCS
    rtcfactory.cpp
    rtcgui.cpp
    rtclogic.cpp
)

qt4_wrap_cpp(rtc_MOC
    rtcgui.h
    rtclogic.h
)

qt4_wrap_ui(rtc_UI
    rtcgui.ui
)

add_library(RtcComponent SHARED
    ${rtc_SRCS}
    ${rtc_MOC}
    ${rtc_UI}
)

target_link_libraries(RtcComponent
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

install(TARGETS RtcComponent DESTINATION lib/qsimavr)
