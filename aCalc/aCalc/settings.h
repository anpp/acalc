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

static QString sViews[] = {QObject::tr("Original"), QObject::tr("Simple"), QObject::tr("Programmable")};
static QString sLanguages[] = {QObject::tr("Default"), QObject::tr("English"), QObject::tr("Russian")};
static QString sShortLanguages[] = {"none", "en", "ru"};
static QString sSettingKind[] = {QObject::tr("appearance"), QObject::tr("misc"), QObject::tr("screen"), QObject::tr("state"), QObject::tr("variables")};


enum kindset: int {appearance = 0, misc, screen, state};
enum editorset {none = 0, text, spin, combo, check};

struct Setting
{
    QString title;
    kindset kind;
    QVariant default_value;
    QVariant value;
    editorset editor;
    int comboNum;
    QString* combovalues;
    bool isChanged = false;
};


class Settings {
    QWidget* owner;
    QSettings qsettings;
    std::vector<Setting*> vec_settings;
    QMap<QString, Setting*> mapset;
    QVariant default_return;

public:
    Settings(QWidget* widget_owner, const QString &organization, const QString &application);
    ~Settings();

    void loadSettingsByKind(kindset ks);
    void saveSettingsByKind(kindset ks);

    void loadSettingsScreen();
    void saveSettingsScreen();

    const QVariant& getSetting(const QString& title);
    bool isChanged(const QString& title);
    void setSetting(const QString& title, QVariant value);


    void loadSettings(){
        loadSettingsByKind(kindset::appearance);
        loadSettingsByKind(kindset::misc);
        loadSettingsByKind(kindset::state);
        loadSettingsScreen();
    }

    void saveSettings(){
        saveSettingsByKind(kindset::appearance);
        saveSettingsByKind(kindset::misc);
        saveSettingsByKind(kindset::state);
        saveSettingsScreen();
    }

    const std::vector<Setting*>& getListSettings() {return vec_settings;}
    const QMap<QString, Setting*>& getMapSettings() {return mapset;}

    const QString& getSettingsName(kindset ks);
};

#endif // SETTINGS

