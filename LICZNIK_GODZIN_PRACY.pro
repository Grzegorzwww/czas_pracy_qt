#-------------------------------------------------
#
# Project created by QtCreator 2015-02-04T14:59:41
#
#-------------------------------------------------

QT       += core gui

QT += gui

QT += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LICZNIK_GODZIN_PRACY
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    singleapplication.cpp

HEADERS  += mainwindow.h \
    singleapplication.h \
    singleapplication_p.h

FORMS    += mainwindow.ui



RESOURCES += \
    syrena.qrc
