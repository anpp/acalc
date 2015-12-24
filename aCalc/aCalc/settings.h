#ifndef SETTINGS
#define SETTINGS

#include <QSettings>
#include <QWidget>
#include <QSize>
#include <QPoint>
#include <QVector>
#include <QMap>
#include <QVariant>
#include "common.h"


enum kindset {appearance = 0, misc, screen};
enum editorset {text = 0, spin, combo};

struct Setting
{
    QString title;
    kindset kind;
    QVariant default_value;
    QVariant value;
    editorset editor;
};


class Settings {
    QWidget* owner;
    QSettings qsettings;
    QVector<Setting*> vec_settings;
    QMap<QString, Setting*> mapset;
    QVariant default_return;

public:
    Settings(QWidget* widget_owner, const QString &organization, const QString &application);

    void loadSettingsByKind(kindset ks);
    void saveSettingsByKind(kindset ks);

    void loadSettingsScreen();
    void saveSettingsScreen();

    const QVariant& getSetting(const QString& title);
    void setSetting(const QString& title, QVariant value);


    void loadSettings(){
        loadSettingsByKind(appearance);
        loadSettingsScreen();
    }

    void saveSettings(){
        saveSettingsByKind(appearance);
        saveSettingsScreen();
    }

    const QVector<Setting*>& getListSettings() {return vec_settings;}
    const QMap<QString, Setting*>& getMapSettings() {return mapset;}

    const QString& getSettingsName(kindset ks);
};

#endif // SETTINGS

