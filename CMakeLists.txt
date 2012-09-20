cmake_minimum_required(VERSION 2.8.0)
project(qsimavr)

set(VERSION 1)

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Debug)
endif()

find_package(Qt4 REQUIRED)

include(CPack)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A graphical frontend to simavr with periphery simulation")
set(CPACK_PACKAGE_NAME "qsimavr")
set(CPACK_PACKAGE_VERSION "${VERSION}")
set(CPACK_RPM_PACKAGE_REQUIRES "simavr")

include(FindPkgConfig)
pkg_search_module(SIMAVR REQUIRED simavr)

set(LIBELF_LIBRARIES
    elf
)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-fPIC)
    set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -std=gnu99)
endif(CMAKE_COMPILER_IS_GNUCXX)

add_subdirectory(EepromComponent)
add_subdirectory(Examples)
add_subdirectory(GlcdComponent)
add_subdirectory(LcdComponent)
add_subdirectory(LedButtonsComponent)
add_subdirectory(QHexEdit2)
add_subdirectory(QSimAVR)
add_subdirectory(RtcComponent)
add_subdirectory(TemperatureComponent)
add_subdirectory(TwiComponent)

# vim:set ts=4 sw=4 et:
