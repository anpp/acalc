#ifndef DIALOGSETTINGS_H
#define DIALOGSETTINGS_H

#include <QDialog>
#include <QDeclarativeView>
#include <QtDeclarative/QDeclarativeContext>



class DialogSettings : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSettings(QWidget *parent = 0);
    ~DialogSettings();



private:
    QDeclarativeView *dv;
};

#endif // DIALOGSETTINGS_H
