#include "dialogsettings.h"


//----------------------------------------------------------------------------------------------------------------------
DialogSettings::DialogSettings(Settings* app_settings, QWidget *parent) :
    QDialog(parent), btnOk("Ok", this), btnCancel("Cancel", this)
{
    this->settings = app_settings;

    loadSettingsGrids();

    for (DSGrid* dsg: vec_tbl) { vl.addWidget(&dsg->tblSettings);}

    vl.setSpacing(SPACING);
    vl.setMargin(SPACING);
    hl.addWidget(&btnOk);
    hl.addWidget(&btnCancel);
    this->setLayout(&vl);
    vl.addLayout(&hl);

    connect(&btnOk, SIGNAL(clicked()), SLOT(slotOk()));
    connect(&btnCancel, SIGNAL(clicked()), SLOT(reject()));
}


//----------------------------------------------------------------------------------------------------------------------
DialogSettings::~DialogSettings()
{
  for (DSGrid* dsg: vec_tbl) delete dsg;
}


//----------------------------------------------------------------------------------------------------------------------
void DialogSettings::loadSettingsGrids()
{
    if (!settings) return;

    DSGrid* dsg;
    QStringList sl_labels_vert, sl_labels_horz;
    int n_rows;

    for (Setting* s: settings->getListSettings()){ set_kindset << s->kind;}

    for (kindset ks: set_kindset)
    {
        if(ks == kindset::screen) continue;

        n_rows = 0;
        sl_labels_vert.clear();
        sl_labels_horz.clear();

        dsg = new DSGrid;
        dsg->kind = ks;
        dsg->tblSettings.setColumnCount(1);
        vec_tbl.append(dsg);
        for (Setting* s: settings->getListSettings())
        {
            if (ks == s->kind)
            {
                sl_labels_vert << QObject::tr(s->title.toStdString().c_str());
                dsg->tblSettings.setRowCount(++n_rows);
                setEditor(&dsg->tblSettings, s, dsg->tblSettings.rowCount() - 1);
            }
        }
        sl_labels_horz << QObject::tr(settings->getSettingsName(ks).toStdString().c_str());
        dsg->tblSettings.setVerticalHeaderLabels(sl_labels_vert);
        dsg->tblSettings.setHorizontalHeaderLabels(sl_labels_horz);
        dsg->tblSettings.verticalHeader()->setFixedWidth(100);
        dsg->tblSettings.horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
        dsg->tblSettings.setStyleSheet("QTableWidget{selection-background-color: transparent; background-color: transparent; border-top: 1px solid gray}");
        dsg->tblSettings.setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        //dsg->tblSettings.horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

        dsg->tblSettings.setFixedHeight(dsg->tblSettings.rowHeight(0) * dsg->tblSettings.rowCount() +
                                        dsg->tblSettings.horizontalHeader()->height() +
                                        dsg->tblSettings.contentsMargins().top() +
                                        dsg->tblSettings.contentsMargins().bottom());

    }
}


//----------------------------------------------------------------------------------------------------------------------
void DialogSettings::setEditor(QTableWidget* tblSettings, Setting* s, int row)
{
    if(!tblSettings) return;
    QSpinBox* spinbox = nullptr;
    QLineEdit* edit = nullptr;
    QComboBox* combobox = nullptr;
    switch (s->editor) {
    case spin:
        spinbox = new QSpinBox();
        tblSettings->setCellWidget(row, 0, spinbox);
        spinbox->setValue(settings->getSetting(s->title).toInt());
        mapSetControl[s->title] = spinbox;
        break;
    case text:
        edit = new QLineEdit();
        tblSettings->setCellWidget(row, 0, edit);
        edit->setText(settings->getSetting(s->title).toString());
        mapSetControl[s->title] = edit;
        break;
    case combo:
        combobox = new QComboBox();
        tblSettings->setCellWidget(row, 0, combobox);
        if(s->combovalues != nullptr)
        {
            for(auto i = 0; i < s->comboNum; ++i)
                combobox->addItem(QObject::tr(s->combovalues[i].toStdString().c_str()));
            combobox->setCurrentIndex(settings->getSetting(s->title).toInt());
        }
        mapSetControl[s->title] = combobox;

        break;
    }
}

void DialogSettings::resizeTable()
{
    for (DSGrid* dsg: vec_tbl) {dsg->tblSettings.setColumnWidth(0, dsg->tblSettings.width() - dsg->tblSettings.verticalHeader()->width() - SPACING); }
}

void DialogSettings::setLocaleTexts()
{

}

void DialogSettings::resizeEvent(QResizeEvent *event)
{
    this->QWidget::resizeEvent(event);
    resizeTable();
}

void DialogSettings::showEvent(QShowEvent *event)
{
    this->QWidget::showEvent(event);
    resizeTable();
}

//----------------------------------------------------------------------------------------------------------------------
void DialogSettings::slotOk()
{
    if(settings)
    {
        for (QString s: mapSetControl.keys())
        {
            QString classname = mapSetControl[s]->metaObject()->className();
            if(classname == "QSpinBox")
                settings->setSetting(s, static_cast<QSpinBox*>(mapSetControl[s])->value());
            if(classname == "QLineEdit")
                settings->setSetting(s, static_cast<QLineEdit*>(mapSetControl[s])->text().toInt());
            if(classname == "QComboBox")
                settings->setSetting(s, static_cast<QComboBox*>(mapSetControl[s])->currentIndex());
        }
    }
    accept();
}
