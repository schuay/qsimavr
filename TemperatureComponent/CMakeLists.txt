set(temperature_SRCS
    temperaturefactory.cpp
    temperaturegui.cpp
    temperaturelogic.cpp
)

qt4_wrap_cpp(temperature_MOC
    temperaturegui.h
    temperaturelogic.h
)

qt4_wrap_ui(temperature_UI
    temperaturegui.ui
)

add_library(TemperatureComponent SHARED
    ${temperature_SRCS}
    ${temperature_MOC}
    ${temperature_UI}
)

target_link_libraries(TemperatureComponent
    ${QT_LIBRARIES}
    ${SIMAVR_LIBRARIES}
)

include_directories(
    ${CMAKE_SOURCE_DIR}/QSimAVR
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_INCLUDES}
    ${SIMAVR_INCLUDES}
)
