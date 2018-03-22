#-------------------------------------------------
#
# Project created by QtCreator 2018-02-11T14:35:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = demoRMR
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    rplidar.cpp \
    CKobuki.cpp \
    mapping.cpp

HEADERS  += mainwindow.h \
    rplidar.h \
    CKobuki.h \
    mapping.h \
    hehe.h

FORMS    += mainwindow.ui

CONFIG += warn_off
