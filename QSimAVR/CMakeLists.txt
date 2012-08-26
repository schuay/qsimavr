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

add_executable(qsimavr
    ${qsimavr_SRCS}
    ${qsimavr_UI}
    ${qsimavr_MOC}
)

target_link_libraries(qsimavr
    ${QT_LIBRARIES}
    ${SIMAVR_LIBRARIES}
    ${LIBELF_LIBRARIES}
)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_INCLUDES}
    ${SIMAVR_INCLUDES}
)

# vim:set ts=4 sw=4 et:
