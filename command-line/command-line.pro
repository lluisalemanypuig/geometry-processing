TEMPLATE = app
TARGET = command-line

CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -fopenmp
QMAKE_CXXFLAGS_DEBUG += -DDEBUG
QMAKE_CXXFLAGS_RELEASE += -DNDEBUG

LIBS += -fopenmp

SOURCES += main.cpp		\
    test_curvature.cpp	\
    test_iterate.cpp	\
	time.cpp			\
    test_smoothing.cpp

HEADERS +=				\
    test_algorithms.hpp \
    time.hpp

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../algorithms-release/ -lgpr
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../algorithms-debug/ -lgpr
else:unix: LIBS += -L$$PWD/../algorithms-debug/ -lgpr

INCLUDEPATH += $$PWD/../
DEPENDPATH += $$PWD/../

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../algorithms-release/libgpr.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../algorithms-debug/libgpr.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../algorithms-release/gpr.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../algorithms-debug/gpr.lib
else:unix: PRE_TARGETDEPS += $$PWD/../algorithms-debug/libgpr.a
