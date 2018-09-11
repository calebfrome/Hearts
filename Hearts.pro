#-------------------------------------------------
#
# Project created by QtCreator 2016-09-03T14:59:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Hearts
TEMPLATE = app
#CONFIG += console
#@CONFIG += shared@


SOURCES += main.cpp\
    mainwindow.cpp \
    card.cpp \
    hand.cpp \
    improperIndexException.cpp

HEADERS  += mainwindow.h \
    card.h \
    array.h \
    hand.h \
    improperIndexException.h

FORMS    += mainwindow.ui

OTHER_FILES +=

RESOURCES += \
    resource.qrc
