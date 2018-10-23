TEMPLATE = app
TARGET = interface

QT += gui opengl
CONFIG += c++11
QMAKE_CXXFLAGS_DEBUG += -DDEBUG
QMAKE_CXXFLAGS_RELEASE += -DNDEBUG

INCLUDEPATH += ..
LIBS += -fopenmp

CONFIG(release, debug|release) {
    LIBS += -L../algorithms-release/ -lgpr
}
CONFIG(debug, debug|release) {
    LIBS += -L../algorithms-debug/ -lgpr
}

FORMS += mainwindow.ui

HEADERS +=						\
    glwidget.hpp				\
    mainwindow.hpp				\
    render_triangle_mesh.hpp \
    utils.hpp

SOURCES +=						\
    main.cpp					\
    glwidget.cpp				\
    mainwindow.cpp				\
    render_triangle_mesh.cpp \
    utils.cpp

DISTFILES +=					\
    shaders/curvature.vert		\
    shaders/curvature.frag		\
    shaders/simple.vert			\
    shaders/simple.frag

RESOURCES +=					\
    resources.qrc