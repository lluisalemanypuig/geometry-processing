TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS_DEBUG += -DDEBUG
QMAKE_CXXFLAGS_RELEASE += -DNDEBUG
INCLUDEPATH += ..

CONFIG(release, debug|release) {
    LIBS += -L../gpr-algorithms-release/ -lgpr
}
CONFIG(debug, debug|release) {
    LIBS += -L../gpr-algorithms-debug/ -lgpr
}

SOURCES += main.cpp \
    test_curvature.cpp

HEADERS += \
    test_algorithms.hpp
