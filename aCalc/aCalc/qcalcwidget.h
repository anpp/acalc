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
    explicit QExLabel(QWidget *parent = nullptr) : QLabel(parent){}
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
    QCalcWidget(QWidget *widget, int i, int j, QString key = "", QString value = "", int n_rows = 1, int n_cols = 1, bool bsoftsize = false);
    QCalcWidget(QWidget *widget, int i, int j, Pnl atype, QString key = "", QString value = "", int n_rows = 1, int n_cols = 1, bool bsoftsize = false);
    void SetTextColor(QColor *color);
    void SetSize(int w, int h);
    void SetAltTexts(QString altval, QString altkey);
    QString Value(void) {return sValueText.isEmpty() ? sKeyText: sValueText;}
    QString AltValue(void) {return sAltValueText.isEmpty() ? Value(): sAltValueText;}
    void SetHypMode(bool mode);
    void SetInvMode(bool mode);
    bool Hypable(void) {return hyp;}
    bool Invable(void) {return inv;}
    void SetText(const QString& text);
    void SetType(Pnl atype);
    Pnl GetType(void) {return type_btn;}
    Pnl GetSavedType(void) {return saved_type;}
    void SetNopable(bool anop) {nopable = anop;}
    bool IsNopable(void) {return nopable;}
    void SetReplaceValue(const QString& s) {sReplaceValue = s;}
    QString& GetReplaceValue(void) {return sReplaceValue;}
    bool IsReplace(void) {return wasReplace;}
    void Replace(QCalcWidget* prev_widget);
    void UnReplace();

    void LoadWhatIsText();

    int i;
    int j;
    int n_rows;
    int n_cols;
    int w;
    int h;
    QWidget *widget;
    bool hyp;
    bool inv;

private:
    void Init(QWidget *widget, int i, int j, int n_rows, int n_cols, QString key, QString value);

    bool bsoftsize;
    bool hyp_mode;
    bool inv_mode;
    Pnl type_btn;
    Pnl saved_type;
    bool nopable;
    QString sReplaceValue;
    bool wasReplace;
    int saved_i;
    int saved_j;

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
