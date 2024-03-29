#include "settings.h"


//----------------------------------------------------------------------------------------------------------------------
Settings::Settings(QWidget* widget_owner, const QString& organization, const QString& application) :
    owner(widget_owner), qsettings(organization, application), default_return(false)
{    
    vec_settings = {new Setting{QObject::tr("button_width"), kindset::appearance, WIDTH_BUT, QVariant(QVariant::Int), spin, 0,  nullptr},
                    new Setting{QObject::tr("button_height"), kindset::appearance, HEIGHT_BUT, QVariant(QVariant::Int), spin, 0, nullptr},
                    new Setting{QObject::tr("appview"), kindset::appearance, static_cast<int>(CalcView::Original), QVariant(QVariant::Int), combo,
                                sizeof(sViews) / sizeof(sViews[0]), sViews},

                    //new Setting{"posx", kindset::screen, 0, QVariant(QVariant::Int), none, 0, nullptr},
                    //new Setting{"posy", kindset::screen, 0, QVariant(QVariant::Int), none, 0, nullptr},
                    new Setting{"geometry", kindset::screen, 0, QVariant(QVariant::Int),none, false, nullptr},

                    new Setting{QObject::tr("Language"), kindset::appearance, static_cast<int>(Langs::Nop), static_cast<int>(Langs::Nop), combo,
                            sizeof(sLanguages) / sizeof(sLanguages[0]), sLanguages},

                    new Setting{QObject::tr("Loging"), kindset::misc, static_cast<int>(Loging::Disable), static_cast<int>(Loging::Disable), check,
                            0, nullptr},
                    new Setting{QObject::tr("logList"), kindset::misc, static_cast<int>(Loging::Disable), static_cast<int>(Loging::Disable), check,
                            0, nullptr},
                    new Setting{QObject::tr("log_rate"), kindset::misc, DEF_LOG_RATE, QVariant(QVariant::Int), spin,
                            0, nullptr},

                    new Setting{QObject::tr("memory"), kindset::state, "", QVariant(QVariant::String), none,
                            0, nullptr},
                    new Setting{QObject::tr("expression"), kindset::state, "", QVariant(QVariant::String), none,
                            0, nullptr}

                   };

    for(const auto& s: vec_settings) {mapset[s->title] = s;}
}


//----------------------------------------------------------------------------------------------------------------------
Settings::~Settings()
{
    for(auto s: vec_settings) {delete s;}
}



//----------------------------------------------------------------------------------------------------------------------
void Settings::loadSettingsByKind(kindset ks)
{
    qsettings.beginGroup("/" + sSettingKind[static_cast<int>(ks)]);
    for(Setting* s: vec_settings){
        if (s->kind == ks)
            s->value = qsettings.value("/" + s->title, s->default_value);
    }
    qsettings.endGroup();
}


//----------------------------------------------------------------------------------------------------------------------
void Settings::saveSettingsByKind(kindset ks)
{
    qsettings.beginGroup("/" + sSettingKind[static_cast<int>(ks)]);
    for(Setting* s: vec_settings){
        if (s->kind == ks)
            qsettings.setValue("/" + s->title, s->value);
    }
    qsettings.endGroup();
}



//----------------------------------------------------------------------------------------------------------------------
void Settings::loadSettingsScreen()
{
    if(owner)
    {
        loadSettingsByKind(kindset::screen);
        //owner->move(QPoint(getSetting("posx").toInt(), getSetting("posy").toInt()));
        owner->restoreGeometry(getSetting("geometry").toByteArray());
    }
}


//----------------------------------------------------------------------------------------------------------------------
void Settings::saveSettingsScreen()
{
    setSetting("geometry", owner->saveGeometry());
    saveSettingsByKind(kindset::screen);
}


//----------------------------------------------------------------------------------------------------------------------
const QVariant& Settings::getSetting(const QString& title)
{
    QMap<QString, Setting*>::iterator mit = mapset.find(title);
    if(mit != mapset.end())
    {
        Setting* s = *mit;
        if(s->value.isNull() || !s->value.isValid())
            return s->default_value;
        return  s->value;
    }
    return default_return;
}

bool Settings::isChanged(const QString &title)
{
    QMap<QString, Setting*>::iterator mit = mapset.find(title);
    if(mit != mapset.end())
    {
        Setting* s = *mit;
        return s->isChanged;
    }
    return false;
}


//----------------------------------------------------------------------------------------------------------------------
void Settings::setSetting(const QString& title, QVariant value){
    QMap<QString, Setting*>::iterator mit = mapset.find(title);
    if(mit != mapset.end())
    {
        Setting* s = *mit;
        s->isChanged = (s->value != value);
        s->value = value;
    }
}


//----------------------------------------------------------------------------------------------------------------------
const QString& Settings::getSettingsName(kindset ks)
{
    return sSettingKind[static_cast<int>(ks)];
}
