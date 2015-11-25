#-------------------------------------------------
#
# Project created by QtCreator 2012-09-06T15:38:02
#
#-------------------------------------------------

QT       += core gui widgets declarative
DEFINES += HAVE_QT5

TARGET = aCalc
TEMPLATE = app


SOURCES += main.cpp\
        mainwidget.cpp \
    qcalcwidget.cpp \
    ../CalcIt/cvalue.cpp \
    ../CalcIt/token.cpp \
    ../CalcIt/variable.cpp \
    ../CalcIt/calcparser.cpp \
    settings.cpp \
    dialogsettings.cpp

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
    settings.h \
    dialogsettings.h

FORMS    += mainwidget.ui

TRANSLATIONS = acalc_en.ts \
               acalc_ru.ts \
               popup_en.ts \
               popup_ru.ts \
               errors_en.ts \
               errors_ru.ts

#LIBS += "../../gettext/gettext-runtime/lib/intl.lib"

RESOURCES += \
    aCalc.qrc

DISTFILES +=
