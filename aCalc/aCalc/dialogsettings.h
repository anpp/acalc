#ifndef DIALOGSETTINGS_H
#define DIALOGSETTINGS_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QSpinBox>
#include <QVector>
#include <QSet>
#include <QLineEdit>
#include "settings.h"



struct DSGrid
{
    kindset kind;
    QTableWidget tblSettings;
};


class DialogSettings : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSettings(Settings* app_settings, QWidget *parent = 0);
    ~DialogSettings();



private:
    void loadSettingsGrids();
    void setEditor(QTableWidget* tblSettings, Setting* s, int row);

    QVBoxLayout vl;
    QHBoxLayout hl;
    QPushButton btnOk;
    QPushButton btnCancel;    
    Settings* settings;

    QVector<DSGrid*> vec_tbl;
    QSet<kindset> set_kindset;

public slots:
    void slotOk();

};

#endif // DIALOGSETTINGS_H
