#ifndef PNLBUTTON_H
#define PNLBUTTON_H

#include <QObject>
#include <QWidget>
#include <QFrame>
#include <QColor>
#include <QVector>
#include <map>
#include "common.h"

struct PnlDesc
{
    int rows;
    int cols;
    QColor* color;
    int spacing;
};

class PnlButton
{
    std::map<Pnl, PnlDesc> panelsDesc
    {
        std::make_pair(Pnl::Dig, PnlDesc{4, 3, new QColor(Qt::blue), SPACING}),
        std::make_pair(Pnl::Op, PnlDesc{4, 3, new QColor(Qt::red), SPACING}),
        std::make_pair(Pnl::Abc, PnlDesc{2, 3, new QColor(0, 64, 128), SPACING}),
        std::make_pair(Pnl::Mem, PnlDesc{1, 3, new QColor(128, 0, 128), SPACING}),
        std::make_pair(Pnl::MemDisp, PnlDesc{1, 1, new QColor(Qt::black), SPACING}),
        std::make_pair(Pnl::Func, PnlDesc{3, 7, new QColor(Qt::black), SPACING}),
        std::make_pair(Pnl::Scale, PnlDesc{1, 4, new QColor(Qt::black), 0}),
        std::make_pair(Pnl::Drg, PnlDesc{1, 3, new QColor(Qt::black), 0}),
        std::make_pair(Pnl::FuncModes, PnlDesc{1, 2, new QColor(Qt::black), 0}),
        std::make_pair(Pnl::ServButtons, PnlDesc{1, 3, new QColor(10, 30, 150), SPACING})
    };

    std::map<Pnl, QFrame*> panelsW;
    QWidget* parent;
public:
    PnlButton(QWidget* parent);
    ~PnlButton();

    const std::map<Pnl, PnlDesc>& panelsInfo() { return panelsDesc;}
    const std::map<Pnl, QFrame*>& panels() { return panelsW;}
    QFrame* getPanel(Pnl type);
    PnlDesc* getPanelInfo(Pnl type);
    QColor* getColor(Pnl type);
    int getRows(Pnl type);
    int getCols(Pnl type);
    void setSizeButton(Pnl type, int button_w, int button_h, int subw = 0, int subh = 0);

};

#endif // PNLBUTTON_H
