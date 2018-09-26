TEMPLATE = app
TARGET = gpr

QT += gui opengl

CONFIG += c++11

INCLUDEPATH += .

FORMS += mainwindow.ui

HEADERS += \
    glwidget.hpp \
    mainwindow.hpp \
    plyreader.hpp \
    trianglemesh.hpp

SOURCES += \
    main.cpp \
    glwidget.cpp \
    mainwindow.cpp \
    trianglemesh.cpp \
    plyreader.cpp

DISTFILES += \
    shaders/simpleshader.vert \
    shaders/simpleshader.frag

RESOURCES += \
    resources.qrc
