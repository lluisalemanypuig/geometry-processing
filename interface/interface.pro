TEMPLATE = app
TARGET = interface

QT += gui opengl
CONFIG += c++11
QMAKE_CXXFLAGS_DEBUG += -DDEBUG
QMAKE_CXXFLAGS_RELEASE += -DNDEBUG

LIBS += -fopenmp

FORMS += mainwindow.ui

HEADERS +=						\
	glwidget.hpp				\
	mainwindow.hpp				\
	twinglwidget.hpp			\
	render_triangle_mesh.hpp	\
	utils.hpp

SOURCES +=						\
	main.cpp					\
	glwidget.cpp				\
	twinglwidget.cpp			\
	mainwindow.cpp				\
	render_triangle_mesh.cpp	\
	utils.cpp

DISTFILES +=					\
    shaders/simple.vert			\
    shaders/simple.frag

RESOURCES +=					\
	resources.qrc

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
