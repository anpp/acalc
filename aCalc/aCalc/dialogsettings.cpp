#include "dialogsettings.h"

DialogSettings::DialogSettings(Settings* app_settings, QWidget *parent) :
    QDialog(parent), btnOk("Ok", this), btnCancel("Cancel", this), tblSize(2, 1)
{
    this->settings = app_settings;
    QStringList header_labels_vert, header_labels_horz;
    header_labels_vert <<"Width"<<"Height";
    header_labels_horz <<"";

    tblSize.setHorizontalHeaderLabels(header_labels_horz);
    tblSize.setVerticalHeaderLabels(header_labels_vert);

    tblSize.setCellWidget(0, 0, new QSpinBox(this));
    tblSize.setCellWidget(1, 0, new QSpinBox(this));
    if(settings)
    {
        ((QSpinBox*)tblSize.cellWidget(0, 0))->setValue(settings->getSettingInt("button_width"));
        ((QSpinBox*)tblSize.cellWidget(1, 0))->setValue(settings->getSettingInt("button_height"));
    }

    tblSize.horizontalHeader()->resizeContentsPrecision();

    hl.addWidget(&btnOk);
    hl.addWidget(&btnCancel);
    this->setLayout(&vl);
    vl.addWidget(&tblSize);
    vl.addLayout(&hl);

    connect(&btnOk, SIGNAL(clicked()), SLOT(slotOk()));
    connect(&btnCancel, SIGNAL(clicked()), SLOT(reject()));
}

DialogSettings::~DialogSettings()
{

}

void DialogSettings::slotOk()
{
    if(settings)
    {
        settings->setSettingInt("button_width",((QSpinBox*)tblSize.cellWidget(0, 0))->value());
        settings->setSettingInt("button_height",((QSpinBox*)tblSize.cellWidget(1, 0))->value());
    }
    accept();
}
