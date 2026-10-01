QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets opengl openglwidgets
include(3rdparty/qtgifimage/src/gifimage/qtgifimage.pri)

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Choose if you want to use static lib or build from source files
DEFINES += USE_LIBS

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    openglwidget.cpp

HEADERS += \
    mainwindow.h \
    openglwidget.h

FORMS += \
    mainwindow.ui

contains(DEFINES, USE_LIBS) {
LIBS += \
    ../libs/3d_viewer.a
} else {
SOURCES += \
    ../modules/obj_parser.c \
    ../modules/data_mapper.c
HEADERS += \
    ../modules/obj_parser.h \
    ../modules/data_mapper.h \
    ../3DViewer_v.1.0.h
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
