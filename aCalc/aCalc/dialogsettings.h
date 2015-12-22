#ifndef DIALOGSETTINGS_H
#define DIALOGSETTINGS_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QSpinBox>
#include "settings.h"


class DialogSettings : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSettings(Settings* app_settings, QWidget *parent = 0);
    ~DialogSettings();



private:
    QVBoxLayout vl;
    QHBoxLayout hl;
    QPushButton btnOk;
    QPushButton btnCancel;
    QTableWidget tblSize;
    Settings* settings;

public slots:
    void slotOk();

};

#endif // DIALOGSETTINGS_H
