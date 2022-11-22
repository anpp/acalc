#-------------------------------------------------
#
# Project created by QtCreator 2012-09-06T15:38:02
#
#-------------------------------------------------

QT       += core gui widgets

#CONFIG += static

CONFIG += c++14

QMAKE_CFLAGS += -std=c++14
QMAKE_CXXFLAGS += -std=c++14

#QMAKE_LFLAGS += -static -static-libgcc
#LIBS += -static-libgcc

TARGET = acalc
TEMPLATE = app


SOURCES += main.cpp\
    dialogvariables.cpp \
    loger.cpp \
    mainwidget.cpp \
    qcalcwidget.cpp \
    ../CalcIt/cvalue.cpp \
    ../CalcIt/token.cpp \
    ../CalcIt/variable.cpp \
    ../CalcIt/calcparser.cpp \
    dialogsettings.cpp \
    qlogcombobox.cpp \
    settings.cpp \
    ../CalcIt/function.cpp \
    pnlbutton.cpp \
    calcdisplay.cpp

HEADERS  += \
    common.h \
    dialogvariables.h \
    loger.h \
    mainwidget.h \
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
    qlogcombobox.h \
    settings.h \
    ../CalcIt/function.h \
    pnlbutton.h \
    calcdisplay.h

FORMS    +=

TRANSLATIONS = acalc_en.ts \
               acalc_ru.ts \
               popup_en.ts \
               popup_ru.ts \
               errors_en.ts \
               errors_ru.ts

RESOURCES += \
    aCalc.qrc

win32: RC_ICONS = $$PWD/Icons/calculator.ico

win32:{
    VERSION = 0.3.0
    QMAKE_TARGET_COMPANY =
    QMAKE_TARGET_PRODUCT = acalc
    QMAKE_TARGET_DESCRIPTION = Advanced calculator
    QMAKE_TARGET_COPYRIGHT = Andrey Ponomarenko
}


