#include "settings.h"

QString sSettingKind[] = {QObject::tr("appearance"), QObject::tr("misc"), QObject::tr("screen")};


//----------------------------------------------------------------------------------------------------------------------
Settings::Settings(QWidget* widget_owner, const QString& organization, const QString& application) :
    owner(widget_owner), qsettings(organization, application), default_return(false)
{    
    vec_settings = {new Setting{QObject::tr("button_width"), kindset::appearance, WIDTH_BUT, QVariant(QVariant::Int), spin},
                    new Setting{QObject::tr("button_height"), kindset::appearance, HEIGHT_BUT, QVariant(QVariant::Int), spin},
                    new Setting{QObject::tr("appview"), kindset::appearance, static_cast<int>(CalcView::Original), QVariant(QVariant::Int), combo},

                    new Setting{QObject::tr("posx"), kindset::screen, 0, QVariant(QVariant::Int), text},
                    new Setting{QObject::tr("posy"), kindset::screen, 0, QVariant(QVariant::Int), text}};

    for(auto s: vec_settings) {mapset[s->title] = s;}
}


//----------------------------------------------------------------------------------------------------------------------
Settings::~Settings()
{
    for(auto s: vec_settings) {delete s;}
}



//----------------------------------------------------------------------------------------------------------------------
void Settings::loadSettingsByKind(kindset ks){
    qsettings.beginGroup("/" + sSettingKind[static_cast<int>(ks)]);
    for(Setting* s: vec_settings){
        if (s->kind == ks)
            s->value = qsettings.value("/" + s->title, s->default_value);
    }
    qsettings.endGroup();
}


//----------------------------------------------------------------------------------------------------------------------
void Settings::saveSettingsByKind(kindset ks){
    qsettings.beginGroup("/" + sSettingKind[static_cast<int>(ks)]);
    for (Setting* s: vec_settings){
        if (s->kind == ks)
            qsettings.setValue("/" + s->title, s->value);
    }
    qsettings.endGroup();
}



//----------------------------------------------------------------------------------------------------------------------
void Settings::loadSettingsScreen(){
    if(owner)
    {
        loadSettingsByKind(kindset::screen);
        owner->move(QPoint(getSetting("posx").toInt(), getSetting("posy").toInt()));
    }
}


//----------------------------------------------------------------------------------------------------------------------
void Settings::saveSettingsScreen(){
    if(owner)
    {
        setSetting("posx", owner->x());
        setSetting("posy", owner->y());
        saveSettingsByKind(kindset::screen);
    }
}


//----------------------------------------------------------------------------------------------------------------------
const QVariant& Settings::getSetting(const QString& title){
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


//----------------------------------------------------------------------------------------------------------------------
void Settings::setSetting(const QString& title, QVariant value){
    QMap<QString, Setting*>::iterator mit = mapset.find(title);
    if(mit != mapset.end())
    {
        Setting* s = *mit;
        s->value = value;
    }
}


//----------------------------------------------------------------------------------------------------------------------
const QString& Settings::getSettingsName(kindset ks)
{
    return sSettingKind[static_cast<int>(ks)];
}
