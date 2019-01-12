TEMPLATE = app
TARGET = interface

QT += gui opengl
CONFIG += c++11
QMAKE_CXXFLAGS_DEBUG += -DDEBUG
QMAKE_CXXFLAGS_RELEASE += -DNDEBUG

FORMS += mainwindow.ui

HEADERS +=						\
	glwidget.hpp				\
	mainwindow.hpp				\
	twinglwidget.hpp			\
	render_triangle_mesh.hpp	\
	utils.hpp \
    err_war_helper.hpp \
    definitions.hpp

SOURCES +=						\
	main.cpp					\
	glwidget.cpp				\
	twinglwidget.cpp			\
	mainwindow.cpp				\
	render_triangle_mesh.cpp	\
	utils.cpp

DISTFILES +=					\
    shaders/vertex_shader.vert \
    shaders/fragment_shader.frag

RESOURCES +=					\
	resources.qrc

# OpenMP
LIBS += -fopenmp

# geoproc library
CONFIG(debug, debug|release) {
    LIBS += -L../geoproc-debug/ -lgeoproc
    PRE_TARGETDEPS += ../geoproc-debug/libgeoproc.a
}
CONFIG(release, debug|release) {
    LIBS += -L../geoproc-release/ -lgeoproc
    PRE_TARGETDEPS += ../geoproc-release/libgeoproc.a
}
INCLUDEPATH += ..
DEPENDPATH += ..
