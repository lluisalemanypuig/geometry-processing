TEMPLATE = app
TARGET = command-line

CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -fopenmp
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

SOURCES += main.cpp		\
    test_curvature.cpp	\
    test_iterate.cpp	\
    time.cpp

HEADERS +=				\
    test_algorithms.hpp \
    time.hpp
