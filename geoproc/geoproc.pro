TEMPLATE = lib
TARGET = geoproc

CONFIG += c++11
CONFIG += staticlib
CONFIG -= app_bundle
CONFIG -= qt

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
    curvature/curvature.hpp					\
    smoothing/smoothing.hpp					\
    smoothing/smoothing_defs.hpp			\
    smoothing/iterative_local.hpp			\
    smoothing/iterative_private.hpp			\
    high-frequencies/highfrequencies.hpp

SOURCES +=									\
    ply_reader.cpp							\
    triangle_mesh.cpp						\
    iterators/mesh_iterator.cpp				\
    iterators/vertex_iterators.cpp			\
    curvature/mean.cpp						\
    curvature/Gauss.cpp						\
    smoothing/iterative_local.cpp			\
    smoothing/smoothing_private.cpp			\
    high-frequencies/highfrequencies.cpp
