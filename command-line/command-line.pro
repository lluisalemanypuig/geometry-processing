TEMPLATE = app
TARGET = command-line

CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -fopenmp
QMAKE_CXXFLAGS_DEBUG += -DDEBUG
QMAKE_CXXFLAGS_RELEASE += -DNDEBUG

SOURCES +=						\
    main.cpp					\
    test_curvature.cpp			\
    test_iterate.cpp			\
    time.cpp					\
    test_band_frequencies.cpp \
    test_smoothing_local.cpp \
    test_smoothing_global.cpp

HEADERS +=						\
    test_geoproc.hpp			\
    time.hpp

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
