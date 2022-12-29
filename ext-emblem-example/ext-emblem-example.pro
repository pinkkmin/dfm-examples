# Still need the widgets module since MenuInterface need it.
QT       += widgets core

CONFIG   +=  plugin
QMAKE_CXXFLAGS += -std=c++11
TARGET = ext-emblem-demo
TEMPLATE = lib

DEFINES += DFMMOUNTERPLUGIN_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated.
DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += /usr/include/c++/8/
INCLUDEPATH += ../dfm_usr/include/dfm-extension

DESTDIR = $$PWD/../out

SOURCES += \
    ext-emblem-impl.cpp

unix {
    target.path = $$[QT_INSTALL_LIBS]/dde-file-manager/plugins/extensions/
    INSTALLS += target
}

RESOURCES += \
    res.qrc
