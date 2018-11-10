TEMPLATE = app
TARGET = command-line

CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -fopenmp
QMAKE_CXXFLAGS_DEBUG += -DDEBUG
QMAKE_CXXFLAGS_RELEASE += -DNDEBUG

LIBS += -fopenmp

SOURCES +=						\
    main.cpp					\
    test_curvature.cpp			\
    test_iterate.cpp			\
    time.cpp					\
    test_local_smoothing.cpp	\
    test_high_frequencies.cpp

HEADERS +=						\
    test_geoproc.hpp			\
    time.hpp

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
