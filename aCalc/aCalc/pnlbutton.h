#ifndef PNLBUTTON_H
#define PNLBUTTON_H

#include <QObject>
#include <QWidget>
#include <QFrame>
#include <QColor>
#include <QVector>
#include "common.h"

struct PnlDesc
{
    int rows;
    int cols;
    QColor* color;
};

class PnlButton
{
    std::map<Pnl, PnlDesc> panelsDesc;
    std::map<Pnl, QFrame*> panelsW;
    QWidget* parent;
public:
    PnlButton(QWidget* parent);
    ~PnlButton();

    const std::map<Pnl, PnlDesc>& panelsInfo() { return panelsDesc;}
    const std::map<Pnl, QFrame*>& panels() { return panelsW;}
    QFrame* getPanel(Pnl type);

};

#endif // PNLBUTTON_H
