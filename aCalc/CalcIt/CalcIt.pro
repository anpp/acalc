#-------------------------------------------------
#
# Project created by QtCreator 2012-08-09T14:28:55
#
#-------------------------------------------------

#QT       += core

QT       -= gui

TARGET = CalcIt
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    calcparser.cpp \
    token.cpp \
    variable.cpp \
    cvalue.cpp

HEADERS += \
    calcparser.h \
    consts.h \
    def.h \
    token.h \
    variable.h \
    cvalue.h

#ifdef _UNIX
#--LIBS += "/usr/local/lib/libintl.so"
#else
LIBS += "../../gettext/gettext-runtime/lib/intl.lib"
#endif

