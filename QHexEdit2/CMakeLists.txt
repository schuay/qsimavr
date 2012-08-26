set(qhexedit2_SRCS
    commands.cpp
    qhexedit.cpp
    qhexedit_p.cpp
    xbytearray.cpp
)

qt4_wrap_cpp(qhexedit2_MOC
    qhexedit.h
    qhexedit_p.h
    xbytearray.h
)

add_library(QHexEdit2 SHARED
    ${qhexedit2_SRCS}
    ${qhexedit2_MOC}
    ${qhexedit2_UI}
)

target_link_libraries(QHexEdit2
    ${QT_LIBRARIES}
)
