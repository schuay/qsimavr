set(twi_SRCS
    twicomponent.cpp
)

add_library(TwiComponent STATIC
    ${twi_SRCS}
)

target_link_libraries(TwiComponent
    ${QT_LIBRARIES}
    ${SIMAVR_LIBRARIES}
)

include_directories(
    ${CMAKE_SOURCE_DIR}/QSimAVR
    ${QT_INCLUDES}
    ${SIMAVR_INCLUDE_DIRS}
)

# vim:set ts=4 sw=4 et:
