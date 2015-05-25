#-------------------------------------------------
#
# Project created by QtCreator 2015-05-24T11:00:03
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qlaunch
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    expando.cpp

HEADERS  += mainwindow.h \
    expando.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    menu.ini \
    menu2.ini \
    qlaunch.css \
    README.md \
    COPYING

RESOURCES += \
    qlaunch.qrc
