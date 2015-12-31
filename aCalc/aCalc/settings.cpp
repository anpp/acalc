#include "settings.h"

QString sSettingKind[] = {QObject::tr("appearance"), QObject::tr("misc"), QObject::tr("screen")};

Setting ini_settings[] = {{QObject::tr("button_width"), appearance, WIDTH_BUT, QVariant(QVariant::Int), spin},
                          {QObject::tr("button_height"), appearance, HEIGHT_BUT, QVariant(QVariant::Int), spin},
                          {QObject::tr("appview"), appearance, static_cast<int>(CalcView::Original), QVariant(QVariant::Int), combo},

                          {QObject::tr("posx"), screen, 0, QVariant(QVariant::Int), text},
                          {QObject::tr("posy"), screen, 0, QVariant(QVariant::Int), text}};


//----------------------------------------------------------------------------------------------------------------------
Settings::Settings(QWidget* widget_owner, const QString& organization, const QString& application) :
    owner(widget_owner), qsettings(organization, application), default_return(false)
{
    for(auto i = std::begin(ini_settings); i < std::end(ini_settings); ++i)
        vec_settings.append(&(*i));

    for(auto s: vec_settings) {mapset[s->title] = s;}
}


//----------------------------------------------------------------------------------------------------------------------
void Settings::loadSettingsByKind(kindset ks){
    qsettings.beginGroup("/" + sSettingKind[ks]);
    for(Setting* s: vec_settings){
        if (s->kind == ks)
            s->value = qsettings.value("/" + s->title, s->default_value);
    }
    qsettings.endGroup();
}


//----------------------------------------------------------------------------------------------------------------------
void Settings::saveSettingsByKind(kindset ks){
    qsettings.beginGroup("/" + sSettingKind[ks]);
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
        loadSettingsByKind(screen);
        owner->move(QPoint(getSetting("posx").toInt(), getSetting("posy").toInt()));
    }
}


//----------------------------------------------------------------------------------------------------------------------
void Settings::saveSettingsScreen(){
    if(owner)
    {
        setSetting("posx", owner->x());
        setSetting("posy", owner->y());
        saveSettingsByKind(screen);
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
    return sSettingKind[ks];
}
