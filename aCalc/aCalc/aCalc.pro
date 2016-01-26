#-------------------------------------------------
#
# Project created by QtCreator 2012-09-06T15:38:02
#
#-------------------------------------------------

QT       += core gui widgets
DEFINES += HAVE_QT5


#CONFIG += static


QMAKE_CFLAGS += -std=c++11
QMAKE_CXXFLAGS += -std=c++11

#QMAKE_LFLAGS += -static -static-libgcc
#LIBS += -static-libgcc

TARGET = acalc
TEMPLATE = app


SOURCES += main.cpp\
        mainwidget.cpp \
    qcalcwidget.cpp \
    ../CalcIt/cvalue.cpp \
    ../CalcIt/token.cpp \
    ../CalcIt/variable.cpp \
    ../CalcIt/calcparser.cpp \
    dialogsettings.cpp \
    settings.cpp \
    ../CalcIt/function.cpp \
    pnlbutton.cpp

HEADERS  += mainwidget.h \
    common.h \
    qcalcwidget.h \
    ../CalcIt/cvalue.h \
    ../CalcIt/calcparser.h \
    ../CalcIt/variable.h \
    ../CalcIt/consts.h \
    ../CalcIt/token.h \
    ../CalcIt/def.h \
    buttexts.h \
    dialogsettings.h \
    functors.h \
    settings.h \
    ../CalcIt/function.h \
    pnlbutton.h

FORMS    += mainwidget.ui

TRANSLATIONS = acalc_en.ts \
               acalc_ru.ts \
               popup_en.ts \
               popup_ru.ts \
               errors_en.ts \
               errors_ru.ts

RESOURCES += \
    aCalc.qrc

DISTFILES +=
