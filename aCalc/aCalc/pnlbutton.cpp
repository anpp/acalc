#include "pnlbutton.h"


//----------------------------------------------------------------------------------------------------------------------
PnlButton::PnlButton(QWidget* parent)
{
    this->parent = parent;
    panelsDesc = {
                std::make_pair(Pnl::Dig, PnlDesc{4, 3, new QColor(Qt::blue)}),
                std::make_pair(Pnl::Op, PnlDesc{4, 3, new QColor(Qt::red)}),
                std::make_pair(Pnl::Abc, PnlDesc{2, 3, new QColor(0, 64, 128)}),
                std::make_pair(Pnl::Mem, PnlDesc{1, 3, new QColor(128, 0, 128)}),
                std::make_pair(Pnl::MemDisp, PnlDesc{1, 1, new QColor(Qt::black)}),
                std::make_pair(Pnl::Func, PnlDesc{3, 7, new QColor(Qt::black)}),
                std::make_pair(Pnl::Scale, PnlDesc{1, 4, new QColor(Qt::black)}),
                std::make_pair(Pnl::Drg, PnlDesc{1, 3, new QColor(Qt::black)}),
                std::make_pair(Pnl::FuncModes, PnlDesc{1, 2, new QColor(Qt::black)}),
                std::make_pair(Pnl::ServButtons, PnlDesc{1, 3, new QColor(Qt::black)})
                };
    QFrame* w;
    for(auto pd: panelsDesc)
    {
        w = new QFrame(this->parent);
        w->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
        panelsW[pd.first] = w;
    }
}


//----------------------------------------------------------------------------------------------------------------------
PnlButton::~PnlButton()
{
    for (auto pd: panelsDesc) delete pd.second.color;
}



//----------------------------------------------------------------------------------------------------------------------
QFrame* PnlButton::getPanel(Pnl type)
{
    auto i = panelsW.find(type);
    return (i == panelsW.end()? nullptr: i->second);
}
