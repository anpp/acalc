#ifndef DIALOGSETTINGS_H
#define DIALOGSETTINGS_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QSpinBox>
#include <QComboBox>
#include <QVector>
#include <QLineEdit>
#include <QCheckBox>
#include <QTabWidget>
#include "settings.h"

#define WIDTH_VHEADER 160
#define WIDTH_COLUMN 200
#define OFFSET 50


struct DSGrid
{
    kindset kind;
    QTableWidget tblSettings;
};


class DialogSettings : public QDialog
{
    Q_OBJECT
public:
    explicit DialogSettings(Settings* app_settings, QWidget *parent = nullptr);
    ~DialogSettings();

private:
    void loadSettingsGrids();
    void setEditor(QTableWidget* tblSettings, Setting* s, int row);
    void resizeTable();

    QTabWidget twSettings;
    QVBoxLayout vl;
    QHBoxLayout hl;
    QPushButton btnOk;
    QPushButton btnCancel;    
    Settings* settings;

    QVector<DSGrid*> vec_tbl;
    QVector<kindset> vec_kindset;
    QMap<QString, QWidget*> mapSetControl;

protected:
    void resizeEvent (QResizeEvent *event);
    void showEvent(QShowEvent *event);

public slots:
    void slotOk();
    void slotResize();

};

#endif // DIALOGSETTINGS_H
