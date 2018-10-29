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
	shaders/curvature.vert		\
	shaders/curvature.frag		\
	shaders/simple.vert			\
	shaders/simple.frag

RESOURCES +=					\
	resources.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../geoproc-release/ -lgeoproc
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../geoproc-debug/ -lgeoproc
else:unix: LIBS += -L$$PWD/../geoproc-debug/ -lgeoproc

INCLUDEPATH += $$PWD/../
DEPENDPATH += $$PWD/../

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../geoproc-release/libgeoproc.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../geoproc-debug/libgeoproc.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../geoproc-release/geoproc.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../geoproc-debug/geoproc.lib
else:unix: PRE_TARGETDEPS += $$PWD/../geoproc-debug/libgeoproc.a
