#-------------------------------------------------
#
# Project created by QtCreator 2018-03-19T14:36:30
#
#-------------------------------------------------

QT       += core gui svg

DEFINES += DATADIR

INCLUDEPATH += $$PWD/../SRC

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ThemeTest
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    ../src/kcolorutils.cpp \
    ../src/kcolorspaces.cpp \
    ../src/pbscolorscheme.cpp \
    ../src/pbscolorconfig.cpp \
    ../src/animation.cpp \
    ../src/blurhelper.cpp \
    ../src/Kvantum.cpp \
    ../src/pbsskin.cpp \
    ../src/shortcuthandler.cpp \
    ../src/ThemeConfig.cpp \
    ../src/windowmanager.cpp \
    ../src/x11wmmove.cpp

HEADERS += \
        mainwindow.h \
    ../src/kcolorutils.h \
    ../src/kguiaddons_export.h \
    ../src/kcolorspaces_p.h \
    ../src/kguiaddons_colorhelpers_p.h \
    ../src/pbscolorscheme.h \
    ../src/pbscolorconfig.h \
    ../src/animation.h \
    ../src/blurhelper.h \
    ../src/Kvantum.h \
    ../src/pbsskin.h \
    ../src/shortcuthandler.h \
    ../src/specs.h \
    ../src/ThemeConfig.h \
    ../src/windowmanager.h \
    ../src/x11wmmove.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    ThemeTest.qrc
