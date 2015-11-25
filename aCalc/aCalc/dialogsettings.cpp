#include "dialogsettings.h"
#include "ui_dialogsettings.h"
#include "settings.h"

DialogSettings::DialogSettings(QWidget *parent) :
    QDialog(parent)
{

    dv = new QDeclarativeView(this);
    dv->setFocusPolicy(Qt::StrongFocus);

    dv->setSource(QUrl("qrc:/qml/settings.qml"));
    QDeclarativeContext* cntx = dv->rootContext();
    cntx->setContextProperty("window", this);
    dv->setResizeMode(QDeclarativeView::SizeViewToRootObject);


}

DialogSettings::~DialogSettings()
{
    delete dv;
}


