set(VERSION 1)
if(NOT PLUGIN_PATH)
    set(PLUGIN_PATH ${CMAKE_INSTALL_PREFIX}/lib/qsimavr)
endif()

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/config.h"
)

set(qsimavr_SRCS
    QsDebugOutput.cpp
    QsLog.cpp
    QsLogDest.cpp
    aboutdialog.cpp
    component.cpp
    main.cpp
    mainwindow.cpp
    pluginmanager.cpp
    pluginswindow.cpp
    plugintablemodel.cpp
    recentfiles.cpp
    simavr.cpp
)

qt4_wrap_cpp(qsimavr_MOC
    aboutdialog.h
    mainwindow.h
    pluginmanager.h
    pluginswindow.h
    simavr.h
)

qt4_wrap_ui(qsimavr_UI
    aboutdialog.ui
    mainwindow.ui
    pluginswindow.ui
)

qt4_add_resources(qsimavr_RCCS
    qsimavr.qrc
)

add_executable(qsimavr
    ${qsimavr_SRCS}
    ${qsimavr_UI}
    ${qsimavr_MOC}
    ${qsimavr_RCCS}
)

target_link_libraries(qsimavr
    ${QT_LIBRARIES}
    ${SIMAVR_LIBRARIES}
    ${LIBELF_LIBRARIES}
)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_INCLUDES}
    ${SIMAVR_INCLUDE_DIRS}
)

install(TARGETS qsimavr DESTINATION bin)
install(FILES component.h DESTINATION include/qsimavr)

# vim:set ts=4 sw=4 et:
