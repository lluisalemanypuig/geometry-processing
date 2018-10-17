TEMPLATE = lib
TARGET = gpr

CONFIG += c++11
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += staticlib

QMAKE_CXXFLAGS_DEBUG += -DDEBUG
QMAKE_CXXFLAGS_RELEASE += -DNDEBUG
INCLUDEPATH += ..

HEADERS += \
    ply_reader.hpp \
    triangle_mesh.hpp

SOURCES += \
    ply_reader.cpp \
    triangle_mesh.cpp
