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
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../geoproc-release/ -lgeoproc
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../geoproc-debug/ -lgeoproc
else:unix: LIBS += -L$$PWD/../geoproc-debug/ -lgeoproc

INCLUDEPATH += $$PWD/..
DEPENDPATH += $$PWD/..

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../geoproc-release/libgeoproc.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../geoproc-debug/libgeoproc.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../geoproc-release/geoproc.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../geoproc-debug/geoproc.lib
else:unix: PRE_TARGETDEPS += $$PWD/../geoproc-debug/libgeoproc.a
