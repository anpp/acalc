#ifndef SETTINGS
#define SETTINGS

#include <QSettings>
#include <QWidget>
#include <QSize>
#include <QPoint>
#include <QVector>
#include <QMap>
#include "common.h"


enum kindset {appearance = 0, misc, screen};
enum typeset {integer = 0, string};

struct Setting
{
    QString title;
    kindset kind;
    typeset type;
    QString default_value;
    QString value;
};


class Settings {
    QWidget* owner;
    QSettings qsettings;
    QVector<Setting*> vec_settings;
    QMap<QString, Setting*> mapset;

public:
    Settings(QWidget* widget_owner, const QString &organization, const QString &application);

    void loadSettingsByKind(kindset ks);
    void saveSettingsByKind(kindset ks);

    void loadSettingsScreen();
    void saveSettingsScreen();

    int getSettingInt(const QString& title);
    void setSettingInt(const QString& title, int value);


    void loadSettings(){
        loadSettingsByKind(appearance);
        loadSettingsScreen();
    }

    void saveSettings(){
        saveSettingsByKind(appearance);
        saveSettingsScreen();
    }
};

#endif // SETTINGS

