#ifndef SETTINGS
#define SETTINGS

#include <QSettings>
#include <QWidget>
#include <QSize>
#include <QPoint>
#include "common.h"

struct Settings {
    Settings(QWidget* widget_owner, const QString &organization, const QString &application) :
        owner(widget_owner), qsettings(organization, application)
    {
        button_size.setWidth(WIDTH_BUT);
        button_size.setHeight(HEIGHT_BUT);
        viewCalc = ORIGINAL;
    }
    void loadSettings(const QString& type_settings){
        qsettings.beginGroup(type_settings);
        button_size.setWidth(qsettings.value("/button_width", WIDTH_BUT).toInt());
        button_size.setHeight(qsettings.value("/button_height", HEIGHT_BUT).toInt());
        if(owner)
        {
            screen_position.setX(qsettings.value("/posx", screen_position.x()).toInt());
            screen_position.setY(qsettings.value("/posy", screen_position.y()).toInt());
            owner->move(screen_position);
        }
        viewCalc = qsettings.value("/view", ORIGINAL).toInt();
        qsettings.endGroup();
    }

    void saveSettings(const QString& type_settings){
        qsettings.beginGroup(type_settings);
        qsettings.setValue("/button_width", button_size.width());
        qsettings.setValue("/button_height", button_size.height());
        if(owner)
        {
            qsettings.setValue("/posx", owner->geometry().x());
            qsettings.setValue("/posy", owner->geometry().y());
        }
        qsettings.setValue("/view", viewCalc);
        qsettings.endGroup();
    }

    QSize button_size;
    QPoint screen_position;
    int viewCalc;

private:
    QWidget* owner;
    QSettings qsettings;
};

#endif // SETTINGS

