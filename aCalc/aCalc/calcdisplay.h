#ifndef CALCDISPLAY_H
#define CALCDISPLAY_H

#include <QWidget>
#include <QLabel>
#include <QTextEdit>
#include <QGridLayout>

#include "common.h"

class CalcDisplay : public QFrame
{
    Q_OBJECT
public:
    explicit CalcDisplay(QWidget *parent = nullptr);

    void Layout();
    const QString result() {return wResult->text();}
    void setExpression(const QString& expression);
    void setResult(const QString& result);
    void deleteLayout();
    const QWidget& getTextEdit(){return *wExpression;}
private:
    QTextEdit *wExpression;
    QLabel *wResult;
signals:

public slots:
};

#endif // CALCDISPLAY_H
