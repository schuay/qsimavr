set(rtc_SRCS
    rtcfactory.cpp
    rtclogic.cpp
)

add_library(RtcComponent SHARED
    ${rtc_SRCS}
)

target_link_libraries(RtcComponent
    TwiComponent
    ${QT_LIBRARIES}
    ${SIMAVR_LIBRARIES}
)

include_directories(
    ${CMAKE_SOURCE_DIR}/TwiComponent
    ${CMAKE_SOURCE_DIR}/QSimAVR
    ${QT_INCLUDES}
    ${SIMAVR_INCLUDE_DIRS}
)
