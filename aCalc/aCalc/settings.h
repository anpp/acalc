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
    void loadSettingsAppearance(){
        qsettings.beginGroup("/appearance");
        button_size.setWidth(qsettings.value("/button_width", WIDTH_BUT).toInt());
        button_size.setHeight(qsettings.value("/button_height", HEIGHT_BUT).toInt());
        viewCalc = qsettings.value("/view", ORIGINAL).toInt();
        qsettings.endGroup();
    }

    void saveSettingsAppearance(){
        qsettings.beginGroup("/appearance");
        qsettings.setValue("/button_width", button_size.width());
        qsettings.setValue("/button_height", button_size.height());
        qsettings.setValue("/view", viewCalc);
        qsettings.endGroup();
    }

    void loadSettingsScreen(){
        qsettings.beginGroup("/screen");
        if(owner)
        {
            screen_position.setX(qsettings.value("/posx", screen_position.x()).toInt());
            screen_position.setY(qsettings.value("/posy", screen_position.y()).toInt());
            owner->move(screen_position);
        }
        qsettings.endGroup();
    }

    void saveSettingsScreen(){
        qsettings.beginGroup("/screen");
        if(owner)
        {
            qsettings.setValue("/posx", owner->geometry().x());
            qsettings.setValue("/posy", owner->geometry().y());
        }
        qsettings.endGroup();
    }

    void loadSettings(){
        loadSettingsAppearance();
        loadSettingsScreen();
    }

    void saveSettings(){
        saveSettingsAppearance();
        saveSettingsScreen();
    }

    QSize button_size;
    QPoint screen_position;
    int viewCalc;

private:
    QWidget* owner;
    QSettings qsettings;
};

#endif // SETTINGS

