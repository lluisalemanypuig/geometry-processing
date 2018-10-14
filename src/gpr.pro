TEMPLATE = app
TARGET = gpr

QT += gui opengl
CONFIG += c++11
QMAKE_CXXFLAGS_DEBUG += -DDEBUG
QMAKE_CXXFLAGS_RELEASE += -DNDEBUG

INCLUDEPATH += .

FORMS += mainwindow.ui

HEADERS += \
    glwidget.hpp \
    mainwindow.hpp \
    ply_reader.hpp \
    triangle_mesh.hpp

SOURCES += \
    main.cpp \
    glwidget.cpp \
    mainwindow.cpp \
    ply_reader.cpp \
    triangle_mesh.cpp

DISTFILES += \
    shaders/shader_vert.vert \
    shaders/shader_frag.frag \
    shaders/vertex_col.vert \
    shaders/vertex_col.frag

RESOURCES += \
    resources.qrc
