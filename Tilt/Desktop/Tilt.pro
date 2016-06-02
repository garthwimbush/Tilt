#-------------------------------------------------
#
# Project created by QtCreator 2015-09-23T22:36:53
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Tilt
TEMPLATE = app


SOURCES += main.cpp\
        tilt.cpp

HEADERS  += tilt.h

FORMS    += tilt.ui

CONFIG += c++11
