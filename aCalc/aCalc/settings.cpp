#include "settings.h"

QString sSettingKind[] = {"appearance", "misc", "screen"};

Setting ini_settings[] = {{QObject::tr("button_width"), appearance, integer, QString::number(WIDTH_BUT), ""},
                          {QObject::tr("button_height"), appearance, integer, QString::number(HEIGHT_BUT), ""},
                          {QObject::tr("appview"), appearance, integer, QString::number(ORIGINAL), ""},

                          {QObject::tr("posx"), screen, integer, "0", ""},
                          {QObject::tr("posy"), screen, integer, "0", ""}};




//----------------------------------------------------------------------------------------------------------------------
Settings::Settings(QWidget* widget_owner, const QString &organization, const QString &application) :
    owner(widget_owner), qsettings(organization, application)
{
    for(unsigned int i = 0; i < sizeof(ini_settings) / sizeof(Setting); ++i)
        vec_settings.append(&ini_settings[i]);

    foreach (Setting* s, vec_settings) {mapset[s->title] = s;}
}


//----------------------------------------------------------------------------------------------------------------------
void Settings::loadSettingsByKind(kindset ks){
    qsettings.beginGroup("/" + sSettingKind[ks]);
    foreach (Setting* s, vec_settings){
        if (s->kind == ks)
            s->value = qsettings.value("/" + s->title, s->default_value).toString();
    }
    qsettings.endGroup();
}


//----------------------------------------------------------------------------------------------------------------------
void Settings::saveSettingsByKind(kindset ks){
    qsettings.beginGroup("/" + sSettingKind[ks]);
    foreach (Setting* s, vec_settings){
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
        owner->move(QPoint(getSettingInt("posx"), getSettingInt("posy")));
    }
}


//----------------------------------------------------------------------------------------------------------------------
void Settings::saveSettingsScreen(){
    if(owner)
    {
        setSettingInt("posx", owner->geometry().x());
        setSettingInt("posy", owner->geometry().y());
        saveSettingsByKind(screen);
    }
}


//----------------------------------------------------------------------------------------------------------------------
int Settings::getSettingInt(const QString& title){
    QMap<QString, Setting*>::iterator mit = mapset.find(title);
    if(mit != mapset.end() && ((Setting*)*mit)->type == integer)
    {
        Setting* s = *mit;
        if(s->value.isEmpty() || s->value == "")
            return s->default_value.toInt();
        return  s->value.toInt();
    }
        return -1;
}


//----------------------------------------------------------------------------------------------------------------------
void Settings::setSettingInt(const QString& title, int value){
    QMap<QString, Setting*>::iterator mit = mapset.find(title);
    if(mit != mapset.end() && ((Setting*)*mit)->type == integer)
    {
        Setting* s = *mit;
        s->value = QString::number(value);
    }
}
