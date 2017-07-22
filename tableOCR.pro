#-------------------------------------------------
#
# Project created by QtCreator 2016-10-09T13:30:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tableOCR
TEMPLATE = app

CONFIG += link_pkgconfig
PKGCONFIG += opencv tesseract

SOURCES += main.cpp\
        mainwindow.cpp \
    imagetableedit.cpp \
    imagetableview.cpp

HEADERS  += mainwindow.h \
    imagetableedit.h \
    imagetableview.h

FORMS    += mainwindow.ui \
    imagetableedit.ui
