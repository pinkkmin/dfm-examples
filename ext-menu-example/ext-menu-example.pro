# Still need the widgets module since MenuInterface need it.
QT       += widgets core

CONFIG   +=  plugin

TARGET = ext-menu-example
TEMPLATE = lib

DEFINES += DFMMOUNTERPLUGIN_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated.
DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += ../dfm_usr/include/dfm-extension/

SOURCES += \
    ext-menu-impl.cpp

DESTDIR = $$PWD/../out


RESOURCES += \
    res.qrc

HEADERS +=
