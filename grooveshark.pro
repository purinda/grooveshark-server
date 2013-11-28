#-------------------------------------------------
#
# Project created by QtCreator 2013-11-09T00:40:27
#
#-------------------------------------------------

QT       += core network multimedia

QT       -= gui

TARGET = grooveshark
CONFIG   += console
CONFIG   += c++11
CONFIG   -= app_bundle
TEMPLATE = app


SOURCES += main.cpp \
    lib/grooveshark/qsearch.cpp \
    lib/grooveshark/qplayer.cpp \
    lib/grooveshark/qgroovesession.cpp \
    lib/grooveshark/test.cpp \
    lib/grooveshark/qrequest.cpp

HEADERS += \
    lib/grooveshark/qsearch.h \
    lib/grooveshark/qplayer.h \
    lib/grooveshark/qgroovesession.h \
    lib/grooveshark/test.h \
    lib/grooveshark/qrequest.h
