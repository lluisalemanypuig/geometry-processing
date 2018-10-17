TEMPLATE = lib
TARGET = gpr

CONFIG += c++11
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += staticlib

QMAKE_CXXFLAGS += -fopenmp
QMAKE_CXXFLAGS_DEBUG += -DDEBUG
QMAKE_CXXFLAGS_RELEASE += -DNDEBUG

LIBS += -fopenmp

INCLUDEPATH += ..

HEADERS +=									\
    ply_reader.hpp							\
    triangle_mesh.hpp						\
    iterators/vertex_iterators.hpp			\
    iterators/mesh_iterator.hpp				\
    curvature/curvature.hpp

SOURCES +=									\
    ply_reader.cpp							\
    triangle_mesh.cpp						\
    iterators/vertex_vertex_iterator.cpp	\
    iterators/mesh_iterator.cpp				\
    curvature/mean.cpp						\
    curvature/Gauss.cpp
