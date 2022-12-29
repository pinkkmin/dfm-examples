QT += core gui network widgets

CONFIG += c++11 link_pkgconfig

INCLUDEPATH += ../dfm_usr/include/dde-file-manager

DESTDIR = $$PWD/../out

TARGET = emblem-example-plugin
CONFIG += plugin

TEMPLATE = lib

SOURCES += main.cpp \
    dfmgenericpluginobject.cpp

HEADERS += \
    dfmgenericpluginobject.h

DISTFILES += generic.json

target.path = /usr/lib/$${QMAKE_HOST.arch}-linux-gnu/dde-file-manager/plugins/generics

INSTALLS += target

RESOURCES += \
    res.qrc
