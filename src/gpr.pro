TEMPLATE = app
TARGET = gpr

QT += gui opengl

CONFIG += c++11

INCLUDEPATH += .

FORMS += mainwindow.ui

HEADERS += \
    glwidget.hpp \
    mainwindow.hpp \
    ply_reader.hpp \
    triangle_mesh.hpp

SOURCES += \
    main.cpp \
    glwidget.cpp \
    mainwindow.cpp \
    ply_reader.cpp \
    triangle_mesh.cpp

DISTFILES += \
    shaders/simpleshader.vert \
    shaders/simpleshader.frag

RESOURCES += \
    resources.qrc
