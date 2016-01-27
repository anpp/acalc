#include "pnlbutton.h"

PnlButton::PnlButton()
{
    panels = { {Pnl::Dig, 4, 3, new QColor(Qt::blue)},
               {Pnl::Op, 4, 3, new QColor(Qt::red)},
               {Pnl::Abc, 2, 3, new QColor(0, 64, 128)},
               {Pnl::Mem, 1, 3, new QColor(128, 0, 128)},
               {Pnl::Func, 3, 7, new QColor(Qt::black)},
               {Pnl::Scale, 1, 4, new QColor(Qt::black)},
               {Pnl::Drg, 1, 3, new QColor(Qt::black)},
               {Pnl::FuncModes, 1, 2, new QColor(Qt::black)},
               {Pnl::ServButtons, 1, 3, new QColor(Qt::black)} };
}


PnlButton::~PnlButton()
{
    for (PnlDesc pd: panels) delete pd.color;
}
