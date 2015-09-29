#ifndef QCALCWIDGET_H
#define QCALCWIDGET_H

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QLabel>
#include <QBoxLayout>
#include <QMouseEvent>
#include <QLayout>
#include "common.h"


class QExLabel : public QLabel
{
    Q_OBJECT
public:
    explicit QExLabel(QWidget *parent = 0) : QLabel(parent){}
signals:
    void clicked(QObject *sender);
protected:

    void mouseReleaseEvent(QMouseEvent *e)
    {
        e->accept();
        QApplication::sendEvent(this->parentWidget(), e);
    }

    void mousePressEvent(QMouseEvent *e)
    {
        e->accept();
        QApplication::sendEvent(this->parentWidget(), e);
    }

    void mouseMoveEvent(QMouseEvent *e)
    {
        e->accept();
        QApplication::sendEvent(this->parentWidget(), e);
    }

};



class QCalcWidget: public QObject
{
    Q_OBJECT
public:
    QCalcWidget(QWidget *widget, unsigned i, unsigned j, QString key = "", QString value = "", unsigned n_rows = 1, unsigned n_cols = 1, bool bsoftsize = false);
    void SetTextColor(QColor *color);
    void SetSize(unsigned w, unsigned h);
    void SetAltTexts(QString altval, QString altkey);
    QString Value(void) {return sValueText.isEmpty() ? sKeyText: sValueText;}
    QString AltValue(void) {return sAltValueText.isEmpty() ? Value(): sAltValueText;}
    void SetHypMode(bool mode);
    void SetInvMode(bool mode);
    bool Hypable(void) {return hyp;}
    bool Invable(void) {return inv;}
    void SetText(const QString& text);

    unsigned i;
    unsigned j;
    unsigned n_rows;
    unsigned n_cols;
    unsigned w;
    unsigned h;
    QWidget *widget;
    bool hyp;
    bool inv;

private:
    void Init(QWidget *widget, unsigned i, unsigned j, unsigned n_rows, unsigned n_cols, QString key, QString value);

    bool bsoftsize;
    bool hyp_mode;
    bool inv_mode;

    QString sValueText;
    QString sKeyText;
    QString sAltValueText;
    QString sAltKeyText;

    QExLabel *lblW;


signals:
    void ClickButton(const QString&);
    void ClickServButton(const QString&);

public slots:
    void sendSignalClick()
    {
        QString val = Value();

        if((hyp && hyp_mode) || inv_mode)
            val = AltValue();
        emit ClickButton(val);
    }

    void sendSignalServClick()
    {
        emit ClickServButton(sKeyText);
    }

};

#endif // QCALCWIDGET_H
