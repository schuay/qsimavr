set(glcd_SRCS
    glcdfactory.cpp
    glcdgraphicsscene.cpp
    glcdgui.cpp
    glcdlogic.cpp
    nt7108.cpp
)

qt4_wrap_cpp(glcd_MOC
    glcdgui.h
    glcdlogic.h
    nt7108.h
)

qt4_wrap_ui(glcd_UI
    glcdgui.ui
)

add_library(GlcdComponent SHARED
    ${glcd_SRCS}
    ${glcd_MOC}
    ${glcd_UI}
)

target_link_libraries(GlcdComponent
    ${QT_LIBRARIES}
    ${SIMAVR_LIBRARIES}
)

include_directories(
    ${CMAKE_SOURCE_DIR}/QSimAVR
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_INCLUDES}
    ${SIMAVR_INCLUDES}
)
