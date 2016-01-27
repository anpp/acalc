#ifndef PNLBUTTON_H
#define PNLBUTTON_H

#include <QObject>
#include <QWidget>
#include <QColor>
#include <QVector>
#include "common.h"

struct PnlDesc
{
    Pnl type;
    int rows;
    int cols;
    QColor* color;
};

class PnlButton
{
    QVector<PnlDesc> panels;
public:
    PnlButton();
    ~PnlButton();

};

#endif // PNLBUTTON_H
